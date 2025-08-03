#include "pch.h"
#include "Body_Robo_Small.h"
#include "GameInstance.h"

#include "Monster.h"



CBody_Robo_Small::CBody_Robo_Small(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CPartObject{ _pDevice, _pContext }
{
}

CBody_Robo_Small::CBody_Robo_Small(const CBody_Robo_Small& _Prototype)
    :CPartObject(_Prototype)
{
}

HRESULT CBody_Robo_Small::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Robo_Small::Initialize(void* _pArg)
{
    CBody_Robo_Small::BODY_SMALL_DESC* pDesc = static_cast<CBody_Robo_Small::BODY_SMALL_DESC*>(_pArg);

    m_pParentState = pDesc->pParentState;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;


    m_bIsEffectiveObject = true;

    return S_OK;
}

void CBody_Robo_Small::Priority_Update(_float _fTimeDelta)
{
    if (true == m_pModelCom->Play_Animation(_fTimeDelta))
        int a = 10;

    if (*m_pParentState != m_iPreAnimationState)
    {
        // idle
        if (*m_pParentState == 1)
            m_pModelCom->SetUp_Animation(0, true, true);
        // shoot
        if (*m_pParentState == 2)
            m_pModelCom->SetUp_Animation(1, true, true);
        // shake
        if (*m_pParentState == 3)
        {
            m_pModelCom->SetUp_Animation(2, true, true);
            m_pModelCom->Set_AnimationSpeed(9.0f, 3);
        }
        // run
        if (*m_pParentState == 4)
            m_pModelCom->SetUp_Animation(3, true, true);
        // detect
        if (*m_pParentState == 5)
            m_pModelCom->SetUp_Animation(4, true, true);

        m_iPreAnimationState = *m_pParentState;

        //if (*m_pParentState == 1)
        //    cout << "몬스터 애니메이션 상태 : " << "가만있음" << endl;
        //if (*m_pParentState == 2)
        //    cout << "몬스터 애니메이션 상태 : " << "쏘기" << endl;
        //if (*m_pParentState == 3)
        //    cout << "몬스터 애니메이션 상태 : " << "흔들흔들" << endl;
        //if (*m_pParentState == 4)
        //    cout << "몬스터 애니메이션 상태 : " << "걷는 중" << endl;
        //if (*m_pParentState == 5)
        //    cout << "몬스터 애니메이션 상태 : " << "감지상태모드" << endl;
    }

    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentWorldMatrix));
}

void CBody_Robo_Small::Update(_float _fTimeDelta)
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

void CBody_Robo_Small::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CBody_Robo_Small::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture", 0)))
            return E_FAIL;
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, (_uint)i, aiTextureType_NORMALS, "g_NormalTexture", 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        m_pShaderCom->Begin(0);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

_bool CBody_Robo_Small::IsAnimationFinished() const
{
    return m_pModelCom->IsAnimOnceDone();
}

void CBody_Robo_Small::Activate_HittedEffect()
{
    m_fHitEffectValue = 0.55f;
}

void CBody_Robo_Small::Activate_DissolveEffect()
{
    m_fDissolveTime = 0.0f;
    m_bDissolving = true;
}

HRESULT CBody_Robo_Small::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SmallDissolve"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RoboSmall"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Robo_Small::Bind_ShaderResources()
{
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

CBody_Robo_Small* CBody_Robo_Small::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBody_Robo_Small* pInstance = new CBody_Robo_Small(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CBody_Robo_Small");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CBody_Robo_Small::Clone(void* _pArg)
{
    CBody_Robo_Small* pInstance = new CBody_Robo_Small(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CBody_Robo_Small");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_Robo_Small::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pTextureCom);
}
