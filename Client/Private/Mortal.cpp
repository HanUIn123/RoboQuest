#include "pch.h"
#include "Mortal.h"
#include "GameInstance.h"

CMortal::CMortal(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CPartObject{_pDevice, _pContext}
{
}

CMortal::CMortal(const CMortal& _Prototype)
    :CPartObject(_Prototype)
{
}

HRESULT CMortal::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMortal::Initialize(void* _pArg)
{
    MORTAL_DESC* pDesc = static_cast<MORTAL_DESC*>(_pArg);

    m_pSocketMatrix = pDesc->pSocketMatrix;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CMortal::Priority_Update(_float _fTimeDelta)
{
    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (size_t i = 0; i < 3; ++i)
    {
        // 행 3개를 길이 1로 만듬.
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
    }
    
    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));
}

void CMortal::Update(_float _fTimeDelta)
{
    m_pColliderCom->Update(XMLoadFloat4x4(&m_CombinedWorldMatrix));
}

void CMortal::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CMortal::Render()
{
    //m_pColliderCom->Render();
    return S_OK;
}

HRESULT CMortal::Ready_Components()
{
    /* Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.3f, -0.3f, 0.3f);
    ColliderDesc.vCenter = _float3(0.f, 0.0f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

CMortal* CMortal::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CMortal* pInstance = new CMortal(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CMortal");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMortal::Clone(void* _pArg)
{
    CMortal* pInstance = new CMortal(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CMortal");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMortal::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom);
}
