#include "pch.h"
#include "BossTerrain.h"
#include "GameInstance.h"

CBossTerrain::CBossTerrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGameObject{_pDevice, _pContext}
{
}

CBossTerrain::CBossTerrain(const CBossTerrain& _Prototype)
    :CGameObject(_Prototype)
{
}

HRESULT CBossTerrain::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBossTerrain::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    return S_OK;
}

void CBossTerrain::Priority_Update(_float _fTimeDelta)
{
    m_pNavigationCom->SetUp_WorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));
}

void CBossTerrain::Update(_float _fTimeDelta)
{
}

void CBossTerrain::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CBossTerrain::Render()
{
    Bind_ShaderResource();

    m_pShaderCom->Begin(0);

    m_pVITerrainBufferCom->Bind_InputAssembler();

    m_pVITerrainBufferCom->Render();

#ifdef _DEBUG
    m_pNavigationCom->Render();
#endif

    return S_OK;
}

HRESULT CBossTerrain::Ready_Component()
{
    // Com_Diffuse
    if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Texture_BossTerrain"), TEXT("Com_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom[BTT_DIFFUSE]))))
        return E_FAIL;
    //if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BasicTerrain"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
    //    return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Shader_VtxNorTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_VIBuffer_BossTerrain"), TEXT("Com_VITerrainBuffer_BossTerrain"), reinterpret_cast<CComponent**>(&m_pVITerrainBufferCom))))
        return E_FAIL;

    /* Com_Navigation */
    if (FAILED(__super::Add_Component(LEVEL_BOSS, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBossTerrain::Bind_ShaderResource()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    _float4x4        ViewMatrix, ProjMatrix;
    ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
    ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom[BTT_DIFFUSE]->Bind_ShaderResource_Multi(m_pShaderCom, "g_DiffuseTexture")))
        return E_FAIL;

    //if (FAILED(m_pTextureCom[BT_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
    //    return E_FAIL;

    return S_OK;
}

_float CBossTerrain::Compute_Height(const XMFLOAT3& _vPos)
{
    if (m_pVITerrainBufferCom)
        return m_pVITerrainBufferCom->Get_Height(_vPos);

    return 0.0f;
}

CBossTerrain* CBossTerrain::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBossTerrain* pInstance = new CBossTerrain(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CBossTerrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBossTerrain::Clone(void* _pArg)
{
    CBossTerrain* pInstance = new CBossTerrain(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CBossTerrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBossTerrain::Free()
{
    __super::Free();

    for (auto& pTexture : m_pTextureCom)
        Safe_Release(pTexture);

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pShaderCom);

    Safe_Release(m_pVITerrainBufferCom);
}
