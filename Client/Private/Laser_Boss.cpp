#include "pch.h"
#include "Laser_Boss.h"
#include "GameInstance.h"

CLaser_Boss::CLaser_Boss(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CMissile{ _pDevice, _pContext }
{
}

CLaser_Boss::CLaser_Boss(const CLaser_Boss& _Prototype)
    :CMissile(_Prototype)
{
}

HRESULT CLaser_Boss::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLaser_Boss::Initialize(void* _pArg)
{
    LASER_BOSS_DESC* pDesc = static_cast<LASER_BOSS_DESC*>(_pArg);

    pDesc->fSpeedPerSec = 13.f;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    m_pTransformCom->Scaling(_float3(0.5f, 0.5f, 5.0f));

    m_vInitialMissilePos = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vInitialMissilePos);

    m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, TEXT("Layer_Player")));

    return S_OK;
}

void CLaser_Boss::Priority_Update(_float _fTimeDelta)
{
}

void CLaser_Boss::Update(_float _fTimeDelta)
{
    if (!m_bActive)
        return;

    m_fTimer += _fTimeDelta;

    __super::Update(_fTimeDelta);

    _matrix WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    m_pColliderCom->Update(WorldMatrix);

    Check_Collision_With_Player();

    //if (m_bScalingUp)
    //{
    //    // Lerp (선형 보간)으로 부드럽게 확대
    //    m_vCurrentScale.x = min(m_vTargetScale.x, m_vCurrentScale.x + m_fScaleSpeed * _fTimeDelta);
    //    m_vCurrentScale.y = min(m_vTargetScale.y, m_vCurrentScale.y + m_fScaleSpeed * _fTimeDelta);
    //    m_vCurrentScale.z = min(m_vTargetScale.z, m_vCurrentScale.z + m_fScaleSpeed * _fTimeDelta);

    //    m_pTransformCom->Scaling(m_vCurrentScale);

    //    // 목표 스케일에 도달하면 애니메이션 종료
    //    if (m_vCurrentScale.x >= m_vTargetScale.x &&
    //        m_vCurrentScale.y >= m_vTargetScale.y &&
    //        m_vCurrentScale.z >= m_vTargetScale.z)
    //    {
    //        m_bScalingUp = false;
    //    }
    //}
}

void CLaser_Boss::Late_Update(_float _fTimeDelta)
{
    m_pTargetCollider = static_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), 0));

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_GLOW, this);
}

HRESULT CLaser_Boss::Render()
{
#ifdef _DEBUG
    //m_pColliderCom->Render();
#endif

    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(Bind_ShaderResource()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture", 0)))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CLaser_Boss::Render_Glow()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    if (FAILED(Bind_ShaderResource()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture", 0)))
            return E_FAIL;

        m_pShaderCom->Begin(1);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CLaser_Boss::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxLaser"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LaserBoss"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
    ColliderDesc.vExtents = _float3(1.5f, 1.5f, 1.5f);
    ColliderDesc.vCenter = _float3(0.f, 0.0f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLaser_Boss::Bind_ShaderResource()
{
    // 월드 행렬 바인딩. 
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    // 뷰 행렬 바인딩. 
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    // 투영 행렬 바인딩. 
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_Timer", &m_fTimer, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCameraPosition", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
        return E_FAIL;

    return S_OK;
}

void CLaser_Boss::Activate_Laser(const _vector& _vPosition, const _vector& _vMuzzleLook)
{
    __super::Activate_Missile();
    
    m_vInitialMissilePos = _vPosition;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPosition);

    _vector vDown = XMVectorSet(0.f, -0.3f, 0.f, 0.f);
    _vector vAdjustedLook = XMVector3Normalize(_vMuzzleLook + vDown);

    m_pTransformCom->LookAt(_vPosition + vAdjustedLook);

    //m_vCurrentScale = _float3(0.1f, 0.1f, 0.1f); // 작게 시작
    //m_pTransformCom->Scaling(m_vCurrentScale);
    //m_bScalingUp = true; // 스케일 증가 시작
}

void CLaser_Boss::DeActivate_Missile()
{
    m_bActive = false;

    _vector vMissilePos = XMVectorSet(9999.0f, 9999.0f, 9999.0f, 1.0f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMissilePos);
}

void CLaser_Boss::Check_Collision_With_Player()
{
    if (!m_pColliderCom || !m_pTargetCollider)
        return;

    if (m_pColliderCom->Intersect(m_pTargetCollider))
    {
        Hit_Target();
    }
}

void CLaser_Boss::Hit_Target()
{
    DeActivate_Missile();
}

void CLaser_Boss::Show_Laser(_float _fTimeDelta)
{
    if (!m_bActive)
        return;
}

CLaser_Boss* CLaser_Boss::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CLaser_Boss* pInstance = new CLaser_Boss(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created: CLaser_Boss");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLaser_Boss::Clone(void* _pArg)
{
    CLaser_Boss* pInstance = new CLaser_Boss(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Created: CLaser_Boss");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLaser_Boss::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
}
