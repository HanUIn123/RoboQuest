#include "pch.h"
#include "Missile_Boom.h"
#include "GameInstance.h"

CMissile_Boom::CMissile_Boom(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CMissile{ _pDevice, _pContext }
{
}

CMissile_Boom::CMissile_Boom(const CMissile_Boom& _Prototype)
    :CMissile(_Prototype)
{
}

HRESULT CMissile_Boom::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMissile_Boom::Initialize(void* _pArg)
{
    MISSILE_BOOM_DESC* pDesc = static_cast<MISSILE_BOOM_DESC*>(_pArg);

    pDesc->fSpeedPerSec = 10.f;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    m_pTransformCom->Scaling(_float3(0.0001f, 0.0001f, 0.0001f));

    m_vInitialMissilePos = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vInitialMissilePos);

    return S_OK;
}

void CMissile_Boom::Priority_Update(_float _fTimeDelta)
{
   
}

void CMissile_Boom::Update(_float _fTimeDelta)
{
    if (!m_bActive)
    {
        return;
    }

    static const _float fMaxDistance = 10.0f;
    _vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _float fDistance = XMVectorGetX(XMVector3Length(vCurrentPos - m_vInitialMissilePos));

    if (fDistance > fMaxDistance)
    {
        DeActivate_Missile();
        return;
    }

    Missile_Fly(_fTimeDelta);

    _matrix WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    m_pColliderCom->Update(WorldMatrix);

    Check_Collision_With_Player();
}

void CMissile_Boom::Late_Update(_float _fTimeDelta)
{
    m_pTargetCollider = static_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), 0));


    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CMissile_Boom::Render()
{
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


HRESULT CMissile_Boom::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Missile"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
    ColliderDesc.vExtents = _float3(1.5f, 1.5f, 1.5f);
    ColliderDesc.vCenter = _float3(0.f, 0.0f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMissile_Boom::Bind_ShaderResource()
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

    return S_OK;
}

void CMissile_Boom::Activate_Missile(const _vector& _vPosition, const _vector& _vGunMuzzleLook)
{
    __super::Activate_Missile();

   
    m_vInitialMissilePos = _vPosition;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPosition);
    m_pTransformCom->LookAt(_vPosition + _vGunMuzzleLook);
}

void CMissile_Boom::DeActivate_Missile()
{
    m_bActive = false;

    _vector vMissilePos = XMVectorSet(9999.0f, 9999.0f, 9999.0f, 1.0f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMissilePos);
}

void CMissile_Boom::Missile_Fly(_float _fTimeDelta)
{
    if (!m_bActive)
        return;

    m_pTransformCom->Go_Straight(_fTimeDelta);
}

void CMissile_Boom::Check_Collision_With_Player()
{
    if (!m_pColliderCom || !m_pTargetCollider)
        return;

    if (m_pColliderCom->Intersect(m_pTargetCollider))
    {
        Hit_Target();
    }
}

CMissile_Boom* CMissile_Boom::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CMissile_Boom* pInstance = new CMissile_Boom(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created: CMissile_Boom");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMissile_Boom::Clone(void* _pArg)
{
    CMissile_Boom* pInstance = new CMissile_Boom(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Created: CMissile_Boom");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMissile_Boom::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
}
