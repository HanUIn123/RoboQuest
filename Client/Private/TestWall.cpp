#include "pch.h"
#include "TestWall.h"
#include "GameInstance.h"

CTestWall::CTestWall(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CTestBuilding{ _pDevice, _pContext }
{
}

CTestWall::CTestWall(const CTestWall& _Prototype)
    :CTestBuilding(_Prototype)
{
}

HRESULT CTestWall::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CTestWall::Initialize(void* _pArg)
{
    if (nullptr != _pArg)
    {
        TEST_WALL_DESC* pDesc = static_cast<TEST_WALL_DESC*>(_pArg);

        if (FAILED(__super::Initialize(pDesc)))
            return E_FAIL;

        if (FAILED(Ready_Component(pDesc->eWallType)))
            return E_FAIL;

        m_fPosition = pDesc->vBigWallPos;
        m_fRotation = pDesc->vBigWallRotation;
        m_fScale = pDesc->vBigWallScale;

        _vector  vBigWallPos = XMLoadFloat3(&m_fPosition);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vBigWallPos, 1.0f));

        _matrix matRotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&pDesc->vBigWallRotation));
        m_pTransformCom->Set_State(CTransform::STATE_RIGHT, matRotation.r[0]);
        m_pTransformCom->Set_State(CTransform::STATE_UP, matRotation.r[1]);
        m_pTransformCom->Set_State(CTransform::STATE_LOOK, matRotation.r[2]);

        m_pTransformCom->Scaling(pDesc->vBigWallScale);
    }
    return S_OK;
}

void CTestWall::Priority_Update(_float _fTimeDelta)
{
}

void CTestWall::Update(_float _fTimeDelta)
{
    //if (true == m_pModelCom->Play_Animation(_fTimeDelta))
    //    int a = 10;
}

void CTestWall::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CTestWall::Render()
{
    if (FAILED(Bind_ShaderResource()))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();


    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture", 0)))
            return E_FAIL;
        //if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
        //    return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }


    return S_OK;
}

HRESULT CTestWall::Ready_Component(WALL_TYPE _eType)
{
    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    switch (_eType)
    {
    case WALL_TYPE::WT_BASIC:
    {
        if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_TestBigWall"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
    }
    break;
    case WALL_TYPE::WT_BIG:
    {
        if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_TestBigBigWall"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
    }
    break;
    case WALL_TYPE::WT_PILLAR:
    {
        if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_TestPillar"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
    }
    break;

    case WALL_TYPE::WT_CONCRETE:
    {
        if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_TestConcrete"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
    }
    break;
    }

    return S_OK;
}

HRESULT CTestWall::Bind_ShaderResource()
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

CTestWall* CTestWall::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTestWall* pInstance = new CTestWall(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CTestWall");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CTestWall::Clone(void* _pArg)
{
    CTestWall* pInstance = new CTestWall(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CTestWall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTestWall::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
