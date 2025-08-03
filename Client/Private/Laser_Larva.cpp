#include "pch.h"
#include "Laser_Larva.h"
#include "GameInstance.h"

CLaser_Larva::CLaser_Larva(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CMissile{_pDevice, _pContext}
{
}

CLaser_Larva::CLaser_Larva(const CLaser_Larva& _Prototype)
    :CMissile(_Prototype)
{
}

HRESULT CLaser_Larva::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLaser_Larva::Initialize(void* _pArg)
{
    LASER_DESC* pDesc = static_cast<LASER_DESC*>(_pArg);

    pDesc->fSpeedPerSec = 10.f;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    m_pTransformCom->Scaling(_float3(0.3f, 0.3f, 0.3f));

    m_vInitialMissilePos = XMVectorSet(1.0f, 0.0f, 1.0f, 1.0f);
    m_fSpeed = 10.0f;
    m_fFlightTime = 0.0f;
    m_fMaxRange = 50.0f;

    m_fCurveWidth = 0.1f;
    m_fCurveHeight = 0.2f;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vInitialMissilePos);

    return S_OK;
}

void CLaser_Larva::Priority_Update(_float _fTimeDelta)
{
}

void CLaser_Larva::Update(_float _fTimeDelta)
{
    if (!m_bActive)
    {
        return;
    }

    static const _float fMaxDistance = 15.0f;
    _vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _float fDistance = XMVectorGetX(XMVector3Length(vCurrentPos - m_vInitialMissilePos));

    if (fDistance > fMaxDistance)
    {
        DeActivate_Missile();
        return;
    }

    Curve_Laser(_fTimeDelta);

    _matrix WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    m_pColliderCom->Update(WorldMatrix);


    Check_Collision_With_Player();
}

void CLaser_Larva::Late_Update(_float _fTimeDelta)
{
    m_pTargetCollider = static_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), 0));


    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CLaser_Larva::Render()  
{
    if (FAILED(Bind_ShaderResource()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_InputAssembler();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CLaser_Larva::Activate_Laser(const _vector& _vPosition, const _vector& _vDirection)
{
    __super::Activate_Missile();
    
    m_vInitialMissilePos = _vPosition;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPosition);
    m_pTransformCom->LookAt(_vPosition + _vDirection);

    m_fFlightTime = 0.0f;
}

void CLaser_Larva::Check_Collision_With_Player()
{
    if (!m_pColliderCom || !m_pTargetCollider)
        return;

    if (m_pColliderCom->Intersect(m_pTargetCollider))
    {
        Hit_Target();
    }
}

HRESULT CLaser_Larva::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCube"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_RectangleCube"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Laser"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;


    /* Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
    ColliderDesc.vExtents = _float3(1.5f, 1.5f, 1.5f);
    ColliderDesc.vCenter = _float3(0.f, 0.0f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CLaser_Larva::Bind_ShaderResource()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    return S_OK;
}

void CLaser_Larva::Curve_Laser(_float _fTimeDelta)
{
    //_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    //
    //_vector vDirection = m_pTransformCom->Get_State(CTransform::STATE_LOOK); 
    //_vector vUpdatedPos = vCurrentPos + vDirection * m_fSpeed * _fTimeDelta;
    //
    //m_fFlightTime += _fTimeDelta; 
    //
    //_float fCurveOffset = cosf(m_fFlightTime * m_fCurveWidth) * m_fCurveHeight; 
    //_float fNewYPos = XMVectorGetY(vUpdatedPos) + fCurveOffset;
    //
    //vUpdatedPos = XMVectorSetY(vUpdatedPos, fNewYPos);
    //
    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, vUpdatedPos);

    m_pTransformCom->Go_Straight(_fTimeDelta);
}

CLaser_Larva* CLaser_Larva::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CLaser_Larva* pInstance = new CLaser_Larva(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CLaser_Larva");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLaser_Larva::Clone(void* _pArg)
{
    CLaser_Larva* pInstance = new CLaser_Larva(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CLaser_Larva");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLaser_Larva::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pColliderCom);
}
