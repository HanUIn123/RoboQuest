#include "pch.h"
#include "Body_Robo_Boom.h"
#include "GameInstance.h"

#include "Monster.h"

CBody_Robo_Boom::CBody_Robo_Boom(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CPartObject{ _pDevice, _pContext }
{
}

CBody_Robo_Boom::CBody_Robo_Boom(const CBody_Robo_Boom& _Prototype)
    :CPartObject(_Prototype)
{
}

HRESULT CBody_Robo_Boom::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Robo_Boom::Initialize(void* _pArg)
{
    CBody_Robo_Boom::BODY_BOOM_DESC* pDesc = static_cast<CBody_Robo_Boom::BODY_BOOM_DESC*>(_pArg);

    m_pParentState = pDesc->pParentState;


    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //m_pModelCom->SetUp_Animation(0, true, true);
    m_bIsEffectiveObject = true;

    return S_OK;
}

void CBody_Robo_Boom::Priority_Update(_float _fTimeDelta)
{
    if (true == m_pModelCom->Play_Animation(_fTimeDelta))
        int a = 10;

    if (*m_pParentState != m_iPreAnimationState)
    {
        if (*m_pParentState == 1)
        {
            //idle
            m_pModelCom->SetUp_Animation(0, true, true);
        }
        if (*m_pParentState == 2)
        {
            //run
            m_pModelCom->SetUp_Animation(3, true, true);
        }
        if (*m_pParentState == 3)
        {
            //shoot
            m_pModelCom->SetUp_Animation(8, true, true);
        }
        if (*m_pParentState == 4)
        {
            //stun
            m_pModelCom->SetUp_Animation(10, false, true);
            m_pModelCom->Set_AnimationSpeed(2.0f, 11);
        }

    
        m_iPreAnimationState = *m_pParentState;

        //if (*m_pParentState == 1)
        //    cout << "몬스터 애니메이션 상태 : " << "가만있음" << endl;
        //if (*m_pParentState == 2)
        //    cout << "몬스터 애니메이션 상태 : " << "달리기" << endl;
        //if (*m_pParentState == 3)
        //    cout << "몬스터 애니메이션 상태 : " << "쏘기" << endl;
        //if (*m_pParentState == 4)
        //    cout << "몬스터 애니메이션 상태 : " << "스턴" << endl;
    }


    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentWorldMatrix));
}

void CBody_Robo_Boom::Update(_float _fTimeDelta)
{
    if (m_fHitEffectValue > 0.0f)
    {
        m_fHitEffectValue -= _fTimeDelta * 1.5f;
        m_fHitEffectValue = max(0.0f, m_fHitEffectValue);
    }

    if (m_bDissolving)
    {
        m_fDissolveTime += _fTimeDelta * 0.9f;
        m_fDissolveTime = min(m_fDissolveTime, 1.0f);
    }
}

void CBody_Robo_Boom::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
    //m_pGameInstance->Add_RenderGroup(CRenderer::RG_SHADOW, this);
}

HRESULT CBody_Robo_Boom::Render()
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

//HRESULT CBody_Robo_Boom::Render_Shadow()
//{
//    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
//        return E_FAIL;
//
//    if (FAILED(m_pGameInstance->Bind_Shadow_Matrices(m_pShaderCom, "g_ViewMatrix", "g_ProjMatrix")))
//        return E_FAIL;
//
//    _uint			iNumMeshes = m_pModelCom->Get_NumMeshes();
//
//    for (_uint i = 0; i < iNumMeshes; i++)
//    {
//        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
//            return E_FAIL;
//
//        m_pShaderCom->Begin(1);
//        m_pModelCom->Render(i);
//    }
//
//    return S_OK;
//}

_bool CBody_Robo_Boom::IsAnimationFinished() const
{
    return m_pModelCom->IsAnimOnceDone();
}

void CBody_Robo_Boom::Activate_HittedEffect()
{
    m_fHitEffectValue = 0.85f;
}

void CBody_Robo_Boom::Activate_DissolveEffect()
{
    m_fDissolveTime = 0.0f;
    m_bDissolving = true;
}

HRESULT CBody_Robo_Boom::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BoomDissolve"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;


    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RoboBoom"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Robo_Boom::Bind_ShaderResources()
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

    if (FAILED(m_pShaderCom->Bind_RawValue("g_EffectiveObject", &m_bIsEffectiveObject, sizeof(_bool))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_HitEffectValue", &m_fHitEffectValue, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_DissolveValue", &m_fDissolveTime, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 0)))
        return E_FAIL;

    return S_OK;
}

CBody_Robo_Boom* CBody_Robo_Boom::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBody_Robo_Boom* pInstance = new CBody_Robo_Boom(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CBody_Robo_Boom");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CBody_Robo_Boom::Clone(void* _pArg)
{
    CBody_Robo_Boom* pInstance = new CBody_Robo_Boom(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CBody_Robo_Boom");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_Robo_Boom::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pTextureCom);
}
