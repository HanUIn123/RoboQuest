#include "pch.h"
#include "Body_Box.h"
#include "GameInstance.h"
#include "Monster.h"

CBody_Box::CBody_Box(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CPartObject{ _pDevice, _pContext }
{
}

CBody_Box::CBody_Box(const CBody_Box& _Prototype)
    :CPartObject(_Prototype)
{
}

HRESULT CBody_Box::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Box::Initialize(void* _pArg)
{
    CBody_Box::BODY_BOX_DESC* pDesc = static_cast<CBody_Box::BODY_BOX_DESC*>(_pArg);

    m_pParentState = pDesc->pParentState;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CBody_Box::Priority_Update(_float _fTimeDelta)
{
    if (true == m_pModelCom->Play_Animation(_fTimeDelta))
        int a = 10;

    if (*m_pParentState != m_iPreAnimationState)
    {
        if (*m_pParentState == 1)
        {
            m_pModelCom->SetUp_Animation(0, true, true);
        }
        if (*m_pParentState == 2)
        {
            m_pModelCom->SetUp_Animation(1, true, true);
        }
        if (*m_pParentState == 3)
        {
            m_pModelCom->SetUp_Animation(2, false, true);
        }

        m_iPreAnimationState = *m_pParentState;

        //if (*m_pParentState == 1)
        //    cout << "박스 애니메이션 상태 : " << "감지함" << endl;
        //if (*m_pParentState == 2)
        //    cout << "박스 애니메이션 상태 : " << "가만있음" << endl;
        //if (*m_pParentState == 3)
        //    cout << "박스 애니메이션 상태 : " << "열어버림" << endl;
    }

    //m_pModelCom->Play_Animation(_fTimeDelta);

    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentWorldMatrix));
}

void CBody_Box::Update(_float _fTimeDelta)
{
}

void CBody_Box::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CBody_Box::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
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

_bool CBody_Box::IsAnimationFinished() const
{
    return m_pModelCom->IsAnimOnceDone();
}

HRESULT CBody_Box::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ItemBox"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    {
        //m_iBoxNumber = 1;
        return E_FAIL;
    }

    //if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ItemBox2"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    //{
    //    m_iBoxNumber = 2;
    //    return E_FAIL;
    //}




    return S_OK;
}

HRESULT CBody_Box::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    return S_OK;
}

CBody_Box* CBody_Box::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBody_Box* pInstance = new CBody_Box(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CBody_Box");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CBody_Box::Clone(void* _pArg)
{
    CBody_Box* pInstance = new CBody_Box(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CBody_Box");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_Box::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
