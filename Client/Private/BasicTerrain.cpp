#include "pch.h"
#include "../Public/BasicTerrain.h"
#include "GameInstance.h"

CBasicTerrain::CBasicTerrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CGameObject{ _pDevice, _pContext }
{
}

CBasicTerrain::CBasicTerrain(const CBasicTerrain& _Prototype)
    :CGameObject(_Prototype)
{
}

HRESULT CBasicTerrain::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBasicTerrain::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;


    //// 이 BackGround 객체를 만들 때, 그냥 Component도 같이 붙여버리겠다~.
    if (FAILED(Ready_Component()))
        return E_FAIL;

    // load함수.
    //Load_Terrain();

    return S_OK;
}

void CBasicTerrain::Priority_Update(_float _fTimeDelta)
{
    m_pNavigationCom->SetUp_WorldMatrix(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));
}

void CBasicTerrain::Update(_float _fTimeDelta)
{

}

void CBasicTerrain::Late_Update(_float _fTimeDelta)
{
    // 배경이라는 뜻의 RG_PRIORITY로 Render Group에 추가.
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CBasicTerrain::Render()
{
    Bind_ShaderResource();
    // Shader 컴객체의 Begin 함수로, 0 번 째 pass 호출.
    m_pShaderCom->Begin(0);

    m_pVITerrainBufferCom->Bind_InputAssembler();

    m_pVITerrainBufferCom->Render();

#ifdef _DEBUG
    //m_pNavigationCom->Render();
#endif

    return S_OK;
}

HRESULT CBasicTerrain::Ready_Component()
{
    // Com_Diffuse
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BasicTerrain"), TEXT("Com_Diffuse"), reinterpret_cast<CComponent**>(&m_pTextureCom[BT_DIFFUSE]))))
        return E_FAIL;
    //if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BasicTerrain"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
    //    return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"), TEXT("Com_VITerrainBuffer_BasicTerrain"), reinterpret_cast<CComponent**>(&m_pVITerrainBufferCom))))
        return E_FAIL;

    /* Com_Navigation */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBasicTerrain::Bind_ShaderResource()
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

    if (FAILED(m_pTextureCom[BT_DIFFUSE]->Bind_ShaderResource_Multi(m_pShaderCom, "g_DiffuseTexture")))
        return E_FAIL;

    //if (FAILED(m_pTextureCom[BT_MASK]->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
    //    return E_FAIL;

    return S_OK;
}

_float CBasicTerrain::Compute_Height(const XMFLOAT3& _vPos)
{
    if (m_pVITerrainBufferCom)
        return m_pVITerrainBufferCom->Get_Height(_vPos);

    return 0.0f;
}

CBasicTerrain* CBasicTerrain::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
     CBasicTerrain* pInstance = new CBasicTerrain(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CBasicTerrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBasicTerrain::Clone(void* _pArg)
{
    CBasicTerrain* pInstance = new CBasicTerrain(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CBasicTerrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBasicTerrain::Free()
{
    __super::Free();

    for (auto& pTexture : m_pTextureCom)
        Safe_Release(pTexture);

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pShaderCom);

    Safe_Release(m_pVITerrainBufferCom);
}
