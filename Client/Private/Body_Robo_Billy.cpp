#include "pch.h"
#include "Body_Robo_Billy.h"
#include "GameInstance.h"
#include "Monster.h"

CBody_Robo_Billy::CBody_Robo_Billy(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CPartObject{ _pDevice, _pContext }
{
}

CBody_Robo_Billy::CBody_Robo_Billy(const CBody_Robo_Billy& _Prototype)
    :CPartObject(_Prototype)
{
}

HRESULT CBody_Robo_Billy::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Robo_Billy::Initialize(void* _pArg)
{
    CBody_Robo_Billy::BODY_BILLY_DESC* pDesc = static_cast<CBody_Robo_Billy::BODY_BILLY_DESC*>(_pArg);

    m_pParentState = pDesc->pParentState;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //m_pModelCom->SetUp_Animation(3, true, true);

    return S_OK;
}

void CBody_Robo_Billy::Priority_Update(_float _fTimeDelta)
{
    //if (true == m_pModelCom->Play_Animation(_fTimeDelta))
    //    int a = 10;

    //if (*m_pParentState != m_iPreAnimationState)
    //{
    //    if (*m_pParentState == 1)
    //    {
    //        m_pModelCom->SetUp_Animation(3, true, true);
    //    }
    //    if (*m_pParentState == 2)
    //    {
    //        m_pModelCom->SetUp_Animation(4, true, true);
    //    }
    //    if (*m_pParentState == 3)
    //    {
    //        m_pModelCom->SetUp_Animation(5, true, true);
    //    }
    //    if (*m_pParentState == 4)
    //    {
    //        m_pModelCom->SetUp_Animation(1, true, true);
    //        m_pModelCom->Set_AnimationSpeed(2.0f, 2);
    //    }
    //    if (*m_pParentState == 5)
    //    {
    //        m_pModelCom->SetUp_Animation(0, false, true);
    //        m_pModelCom->Set_AnimationSpeed(2.0f, 1);
    //    }

    //    m_iPreAnimationState = *m_pParentState;

    //    cout << " 애니메이션 상태 : " << *m_pParentState << endl;
    //}

    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentWorldMatrix));
}

void CBody_Robo_Billy::Update(_float _fTimeDelta)
{
}

void CBody_Robo_Billy::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CBody_Robo_Billy::Render()
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

_bool CBody_Robo_Billy::IsAnimationFinished() const
{
    return m_pModelCom->IsAnimOnceDone();
}

HRESULT CBody_Robo_Billy::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RoboBilly"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Robo_Billy::Bind_ShaderResources()
{
    //if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
     //	return E_FAIL;
     // 이제 이 BodyPlayer의 행렬은, 부모의 행렬까지 곱한 최종 상태의 행렬을 쉐이더에 던져줄껏이다.. 
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    //const LIGHT_DESC* pLightDesc = m_pGameInstance->Get_LightDesc(0);
    //if (nullptr == pLightDesc)
    //    return E_FAIL;

   /* if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCameraPosition", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
        return E_FAIL;*/

    return S_OK;
}

CBody_Robo_Billy* CBody_Robo_Billy::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBody_Robo_Billy* pInstance = new CBody_Robo_Billy(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CBody_Robo_Billy");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CBody_Robo_Billy::Clone(void* _pArg)
{
    CBody_Robo_Billy* pInstance = new CBody_Robo_Billy(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CBody_Robo_Billy");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_Robo_Billy::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
