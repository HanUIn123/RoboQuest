#include "pch.h"
#include "TestDoor.h"
#include "GameInstance.h"

CTestDoor::CTestDoor(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CMapToolMonster(_pDevice, _pContext)
{
}

CTestDoor::CTestDoor(const CTestDoor& _Prototype)
    :CMapToolMonster(_Prototype)
{
}

HRESULT CTestDoor::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CTestDoor::Initialize(void* _pArg)
{
    if (nullptr != _pArg)
    {
        TESTDOOR_DESC* pDesc = static_cast<TESTDOOR_DESC*>(_pArg);

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

void CTestDoor::Priority_Update(_float _fTimeDelta)
{
}

void CTestDoor::Update(_float _fTimeDelta)
{
    if (true == m_pModelCom->Play_Animation(_fTimeDelta))
        int a = 10;

    _vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
}

void CTestDoor::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CTestDoor::Render()
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

    return S_OK;
}

HRESULT CTestDoor::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_TestDoor"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CTestDoor::Bind_ShaderResource()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    return S_OK;
}

CTestDoor* CTestDoor::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTestDoor* pInstance = new CTestDoor(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CTestDoor");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CTestDoor::Clone(void* _pArg)
{
    CTestDoor* pInstance = new CTestDoor(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CTestDoor");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTestDoor::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
