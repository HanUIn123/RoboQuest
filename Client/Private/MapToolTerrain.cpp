#include "pch.h"
#include "MapToolTerrain.h"
#include "GameInstance.h"

CMapToolTerrain::CMapToolTerrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject{ _pDevice, _pContext }
{
}

CMapToolTerrain::CMapToolTerrain(const CMapToolTerrain& _Prototype)
    :CGameObject(_Prototype)
{
}

HRESULT CMapToolTerrain::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMapToolTerrain::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    // 이 BackGround 객체를 만들 때, 그냥 Component도 같이 붙여버리겠다~.
    if (FAILED(Ready_Component()))
        return E_FAIL;

    return S_OK;
}

void CMapToolTerrain::Priority_Update(_float _fTimeDelta)
{
    m_pNavigationCom->SetUp_WorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));
}

void CMapToolTerrain::Update(_float _fTimeDelta)
{

}

void CMapToolTerrain::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CMapToolTerrain::Render()
{
    Bind_ShaderResource();
    // Shader 컴객체의 Begin 함수로, 0 번 째 pass 호출.
    m_pShaderCom->Begin(0);

    m_pVITerrainBufferCom->Bind_InputAssembler();

    CImGui_Manager::GetInstance()->SwitchFrameMode();

    m_pVITerrainBufferCom->Render();

#ifdef _DEBUG
    m_pNavigationCom->Render();
#endif
    return S_OK;
}

HRESULT CMapToolTerrain::Ready_Component()
{
    // Com_Diffuse
    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_MapToolTerrain"), TEXT("Com_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom[TT_DIFFUSE]))))
        return E_FAIL;
    // Com_Mask
    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_MapToolTerrain_Mask"), TEXT("Com_Mask"), reinterpret_cast<CComponent**>(&m_pTextureCom[TT_MASK]))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_MapToolTerrain_Mouse"), TEXT("Com_Mouse"), reinterpret_cast<CComponent**>(&m_pTextureCom[TT_MOUSE]))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxNorTex_MapToolTerrain"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_VIBuffer_MapToolTerrain"), TEXT("Com_VITerrainBuffer_MapTool"), reinterpret_cast<CComponent**>(&m_pVITerrainBufferCom))))
        return E_FAIL;




    /* Com_Navigation */
    //CNavigation::NAVIGATION_DESC		NaviDesc{};

    //NaviDesc.iCurrentCellIndex = 0;

    /* Com_Navigation */
    //if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Navigation_FromLoad"), TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
        return E_FAIL;
    return S_OK;
}

HRESULT CMapToolTerrain::Bind_ShaderResource()
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

    if (FAILED(m_pTextureCom[TT_DIFFUSE]->Bind_ShaderResource_Multi(m_pShaderCom, "g_DiffuseTexture")))
        return E_FAIL;

    //if (FAILED(m_pTextureCom[TT_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", m_iDiffuseImageIndex)))
    //    return E_FAIL;

    if (FAILED(m_pTextureCom[TT_MOUSE]->Bind_ShaderResource(m_pShaderCom, "g_MouseTexture", 0)))
        return E_FAIL;

    if (FAILED(m_pTextureCom[TT_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", m_iMaskImageIndex)))
        return E_FAIL;
    // 피킹된 좌표를 쉐이더에 던져주기 
    if (FAILED(m_pShaderCom->Bind_RawValue("g_PickedCoord", &m_fPickPos, sizeof(_float3))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_BrushRange", &m_fRange, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_Brushed", &m_bBrushed, sizeof(_bool))))
        return E_FAIL;

    //const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    //if (nullptr == pLightDesc)
    //    return E_FAIL;

    //// Diffuse 용 빛 정보 던지기
    //if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
    //    return E_FAIL;

    //// Diffuse 용 빛 방향 벡터 던지기 
    //if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
    //    return E_FAIL;

    //if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
    //    return E_FAIL;

    //if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
    //    return E_FAIL;


    //if (FAILED(m_pShaderCom->Bind_RawValue("g_vCameraPosition", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
    //    return E_FAIL;

    return S_OK;
}

HRESULT CMapToolTerrain::Bind_Terrain_Texture(_float _fX, _float _fY)
{
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageTypeX", &_fX, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fImageTypeY", &_fY, sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

HRESULT CMapToolTerrain::Bind_DiffuseTextureIndex(_uint _iFirstIndex, _uint _iSecondIndex)
{
    if (FAILED(m_pShaderCom->Bind_RawValue("g_DiffuseTextureFirstIndex", &_iFirstIndex, sizeof(_uint))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_DiffuseTextureSecondIndex", &_iSecondIndex, sizeof(_uint))))
        return E_FAIL;

    return S_OK;
}

CMapToolTerrain* CMapToolTerrain::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CMapToolTerrain* pInstance = new CMapToolTerrain(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CMapToolTerrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMapToolTerrain::Clone(void* _pArg)
{
    CMapToolTerrain* pInstance = new CMapToolTerrain(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CMapToolTerrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMapToolTerrain::Free()
{
    __super::Free();

    for (auto& pTexture : m_pTextureCom)
        Safe_Release(pTexture);


    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVITerrainBufferCom);

    Safe_Release(m_pNavigationCom);
}
