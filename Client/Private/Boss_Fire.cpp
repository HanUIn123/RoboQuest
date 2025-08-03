#include "pch.h"
#include "Boss_Fire.h"
#include "GameInstance.h"

CBoss_Fire::CBoss_Fire(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CPartObject{ _pDevice, _pContext }
{
}

CBoss_Fire::CBoss_Fire(const CBoss_Fire& _Prototype)
    :CPartObject(_Prototype)
{
}

HRESULT CBoss_Fire::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBoss_Fire::Initialize(void* _pArg)
{
    BOSS_FIRE_DESC* pDesc = static_cast<BOSS_FIRE_DESC*>(_pArg);

    m_pSocketMatrix = pDesc->pSocketMatrix;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CBoss_Fire::Priority_Update(_float _fTimeDelta)
{
    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (size_t i = 0; i < 3; ++i)
    {
        // 행 3개를 길이 1로 만듬.
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
    }

    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));
}

void CBoss_Fire::Update(_float _fTimeDelta)
{
    m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CBoss_Fire::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CBoss_Fire::Render()
{
#ifdef _DEBUG
    //m_pColliderCom->Render();
#endif
    return S_OK;
}

HRESULT CBoss_Fire::Ready_Components()
{
    /* Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
    ColliderDesc.vExtents = _float3(2.5f, 2.5f, 2.5f);
    ColliderDesc.vCenter = _float3(0.f, 0.0f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

CBoss_Fire* CBoss_Fire::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBoss_Fire* pInstance = new CBoss_Fire(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CBoss_Fire");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBoss_Fire::Clone(void* _pArg)
{
    CBoss_Fire* pInstance = new CBoss_Fire(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CBoss_Fire");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_Fire::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);
}
