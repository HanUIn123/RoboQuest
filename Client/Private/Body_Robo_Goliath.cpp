#include "pch.h"
#include "Body_Robo_Goliath.h"
#include "GameInstance.h"

#include "Monster.h"

CBody_Robo_Goliath::CBody_Robo_Goliath(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CPartObject{_pDevice, _pContext}
{
}

CBody_Robo_Goliath::CBody_Robo_Goliath(const CBody_Robo_Goliath& _Prototype)
    :CPartObject(_Prototype)
{
}

HRESULT CBody_Robo_Goliath::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Robo_Goliath::Initialize(void* _pArg)
{
    CBody_Robo_Goliath::BODY_GOLIATH_DESC* pDesc = static_cast<CBody_Robo_Goliath::BODY_GOLIATH_DESC*>(_pArg);

    m_pParentState = pDesc->pParentState;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //m_bIsEffectiveObject = false;

    m_bIsEffectiveObject = true;
    return S_OK;
}

void CBody_Robo_Goliath::Priority_Update(_float _fTimeDelta)
{
    if (true == m_pModelCom->Play_Animation(_fTimeDelta))
        int a = 10;

    if (*m_pParentState != m_iPreAnimationState)
    {
        // Dash = Charge 
        if (*m_pParentState == 1)
        {
            m_pModelCom->SetUp_Animation(0, true, true);
        }
        // Normal Shoot 
        if (*m_pParentState == 2)
        {
            m_pModelCom->SetUp_Animation(1, true, true);
        }
        // Idle
        if (*m_pParentState == 3)
        {
            m_pModelCom->SetUp_Animation(2, true, true);
        }
        // Mortal In
        if (*m_pParentState == 4)
        {
            m_pModelCom->SetUp_Animation(3, false, true);
            //m_pModelCom->Set_AnimationSpeed(1.5f, 4);
        }
        // Mortal Loop
        if (*m_pParentState == 5)
        {
            m_pModelCom->SetUp_Animation(4, true, true);
        }
        // Mortal Out
        if (*m_pParentState == 6)
        {
            m_pModelCom->SetUp_Animation(5, false, true);
        }
        // Mortal Shoot
        if (*m_pParentState == 7)
        {
            m_pModelCom->SetUp_Animation(6, true, true);
            m_pModelCom->Set_AnimationSpeed(0.25f, 7);
        }
        // Run Front
        if (*m_pParentState == 8)
        {
            m_pModelCom->SetUp_Animation(7, true, true);
        }
        // Run Left
        if (*m_pParentState == 9)
        {
            m_pModelCom->SetUp_Animation(8, true, true);
        }
        // Run Right
        if (*m_pParentState == 10)
        {
            m_pModelCom->SetUp_Animation(9, true, true);
        }
        // Stun
        if (*m_pParentState == 11)
        {
            m_pModelCom->SetUp_Animation(10, false, true);
            m_pModelCom->Set_AnimationSpeed(2.0f, 11);
        }

        m_iPreAnimationState = *m_pParentState;

        //if (*m_pParentState == 1)
        //    cout << "몬스터 애니메이션 상태 : " << "존나뜀" << endl;
        //if (*m_pParentState == 2)
        //    cout << "몬스터 애니메이션 상태 : " << "쏘기" << endl;
        //if (*m_pParentState == 3)
        //    cout << "몬스터 애니메이션 상태 : " << "가만있음" << endl;
        //if (*m_pParentState == 4)
        //    cout << "몬스터 애니메이션 상태 : " << "포탄준비" << endl;
        ////if (*m_pParentState == 5)
        ////    cout << "몬스터 애니메이션 상태 : " << "포탄중" << endl;
        //if (*m_pParentState == 6)
        //    cout << "몬스터 애니메이션 상태 : " << "포탄끝" << endl;
        //if (*m_pParentState == 7)
        //    cout << "몬스터 애니메이션 상태 : " << "포탄중" << endl;
        //if (*m_pParentState == 8)
        //    cout << "몬스터 애니메이션 상태 : " << "앞걸어감" << endl;
        //if (*m_pParentState == 9)
        //    cout << "몬스터 애니메이션 상태 : " << "왼걸어감" << endl;
        //if (*m_pParentState == 10)
        //    cout << "몬스터 애니메이션 상태 : " << "오걸어감" << endl;
        //if (*m_pParentState == 11)
        //    cout << "몬스터 애니메이션 상태 : " << "스턴" << endl;

    }


    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentWorldMatrix));
}

void CBody_Robo_Goliath::Update(_float _fTimeDelta)
{
    //_matrix BodyRoboBoomMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    //_vector vBodyPosition = BodyRoboBoomMatrix.r[3];

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

void CBody_Robo_Goliath::Late_Update(_float _fTimeDelta)
{
    //if (true == m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.5f))
        m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CBody_Robo_Goliath::Render()
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

_bool CBody_Robo_Goliath::IsAnimationFinished() const
{
    return m_pModelCom->IsAnimOnceDone();
}

void CBody_Robo_Goliath::Activate_HittedEffect()
{
    m_fHitEffectValue = 0.55f;
}

void CBody_Robo_Goliath::Activate_DissolveEffect()
{
    m_fDissolveTime = 0.0f;
    m_bDissolving = true;
}

HRESULT CBody_Robo_Goliath::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GoliathDissolve"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RoboGoliath"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Robo_Goliath::Bind_ShaderResources()
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

CBody_Robo_Goliath* CBody_Robo_Goliath::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBody_Robo_Goliath* pInstance = new CBody_Robo_Goliath(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CBody_Robo_Goliath");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CBody_Robo_Goliath::Clone(void* _pArg)
{
    CBody_Robo_Goliath* pInstance = new CBody_Robo_Goliath(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CBody_Robo_Goliath");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_Robo_Goliath::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pTextureCom);
}
