#include "pch.h"
#include "TempCollider.h"
#include "GameInstance.h"


CTempCollider::CTempCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGameObject{ _pDevice, _pContext }
{
}

CTempCollider::CTempCollider(const CTempCollider& _Prototype)
    :CGameObject(_Prototype)
{
}

HRESULT CTempCollider::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CTempCollider::Initialize(void* _pArg)
{
    TEMP_COLLIDER_DESC* pDesc = static_cast<TEMP_COLLIDER_DESC*>(_pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    _float3 fPosition = pDesc->vTempColliderpos;
    _float3 fScale = pDesc->vTempColliderScale;

    _vector vTempColliderPos = XMLoadFloat3(&fPosition);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vTempColliderPos, 1.0f));

    return S_OK;
}

void CTempCollider::Priority_Update(_float _fTimeDelta)
{
}

void CTempCollider::Update(_float _fTimeDelta)
{
}

void CTempCollider::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CTempCollider::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_InputAssembler();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CTempCollider::Ready_Component()
{
    /* Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxCube"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CTempCollider::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    _float4x4   ViewMatrix, ProjMatrix;
    ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
    ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;

    //if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 2)))
    //    return E_FAIL;
    return S_OK;
}

CTempCollider* CTempCollider::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTempCollider* pInstance = new CTempCollider(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CTempCollider");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CTempCollider::Clone(void* _pArg)
{
    CTempCollider* pInstance = new CTempCollider(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CTempCollider");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTempCollider::Free()
{
    __super::Free();

    //Safe_Release(m_pColliderCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);

}
