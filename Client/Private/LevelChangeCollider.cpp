#include "pch.h"
#include "LevelChangeCollider.h"
#include "GameInstance.h"

CLevelChangeCollider::CLevelChangeCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGameObject{_pDevice, _pContext}
{
}

CLevelChangeCollider::CLevelChangeCollider(const CLevelChangeCollider& _Prototype)
    :CGameObject(_Prototype)
{
}

HRESULT CLevelChangeCollider::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevelChangeCollider::Initialize(void* _pArg)
{
    LC_DESC* pDesc = static_cast<LC_DESC*>(_pArg);

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //_vector vLColliderPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(10.0f, 0.0f, 10.0f, 1.0f));
        
    return S_OK;
}

void CLevelChangeCollider::Priority_Update(_float _fTimeDelta)
{
}

void CLevelChangeCollider::Update(_float _fTimeDelta)
{
    m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));
}

void CLevelChangeCollider::Late_Update(_float _fTimeDelta)
{
    //CCollider* pTargetCollider = static_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Part_Body"), TEXT("Com_Collider"), 0));
    //_bool   bIsCollided = m_pColliderCom[]



    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CLevelChangeCollider::Render()
{
    //m_pColliderCom->Render();
    return S_OK;
}

HRESULT CLevelChangeCollider::Ready_Components()
{
    /* Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.3f, -0.3f, 0.3f);
    ColliderDesc.vCenter = _float3(0.f, 1.0f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

_bool CLevelChangeCollider::Check_Collision_With_Player()
{
    if (nullptr == m_pColliderCom)
        return false;

    CCollider* pTargetCollider = static_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), 0));

    return m_pColliderCom->Intersect(pTargetCollider);
}

CLevelChangeCollider* CLevelChangeCollider::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CLevelChangeCollider* pInstance = new CLevelChangeCollider(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CLevelChangeCollider");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CLevelChangeCollider::Clone(void* _pArg)
{
    CLevelChangeCollider* pInstance = new CLevelChangeCollider(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CLevelChangeCollider");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevelChangeCollider::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);
}
