#include "pch.h"
#include "TestNorRobo.h"
#include "GameInstance.h"

CTestNorRobo::CTestNorRobo(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CMapToolMonster{ _pDevice, _pContext }
{
}

CTestNorRobo::CTestNorRobo(const CTestNorRobo& _Prototype)
    :CMapToolMonster(_Prototype)
{
}

HRESULT CTestNorRobo::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CTestNorRobo::Initialize(void* _pArg)
{
    if (nullptr != _pArg)
    {
        TEST_NOR_ROBO* pDesc = static_cast<TEST_NOR_ROBO*>(_pArg);

        if (FAILED(__super::Initialize(pDesc)))
            return E_FAIL;

        if (FAILED(Ready_Component()))
            return E_FAIL;

        m_fPosition = pDesc->vMonsterpos;
        m_fRotation = pDesc->vMonsterRotation;
        m_fScale = pDesc->vMonsterScale;

        _vector  vMonsterPos = XMLoadFloat3(&m_fPosition);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vMonsterPos, 1.0f));
    }
    return S_OK;
}

void CTestNorRobo::Priority_Update(_float _fTimeDelta)
{
}

void CTestNorRobo::Update(_float _fTimeDelta)
{
    if (true == m_pModelCom->Play_Animation(_fTimeDelta))
        int a = 10;

    _vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}

void CTestNorRobo::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CTestNorRobo::Render()
{
    if (FAILED(Bind_ShaderResource()))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture", 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }

    //m_pNavigationCom->Render();

    return S_OK;
}


HRESULT CTestNorRobo::Ready_Component()
{

    //CNavigation::NAVIGATION_DESC		NaviDesc{};
    //
    //NaviDesc.iCurrentCellIndex = 0;

    //if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
    //    return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_BoomMonster"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CTestNorRobo::Bind_ShaderResource()
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

CTestNorRobo* CTestNorRobo::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTestNorRobo* pInstance = new CTestNorRobo(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CTestNorRobo");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CTestNorRobo::Clone(void* _pArg)
{
    CTestNorRobo* pInstance = new CTestNorRobo(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CTestNorRobo");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTestNorRobo::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
