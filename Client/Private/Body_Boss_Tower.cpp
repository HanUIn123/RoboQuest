#include "pch.h"
#include "Body_Boss_Tower.h"
#include "GameInstance.h"

#include "Monster.h"

CBody_Boss_Tower::CBody_Boss_Tower(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CPartObject{ _pDevice, _pContext }
{
}

CBody_Boss_Tower::CBody_Boss_Tower(const CBody_Boss_Tower& _Prototype)
    :CPartObject(_Prototype)
{
}

HRESULT CBody_Boss_Tower::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Boss_Tower::Initialize(void* _pArg)
{
    CBody_Boss_Tower::BODY_BOSS_DESC* pDesc = static_cast<CBody_Boss_Tower::BODY_BOSS_DESC*>(_pArg);

    m_pParentState = pDesc->pParentState;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_bIsEffectiveObject = true;

    return S_OK;
}

void CBody_Boss_Tower::Priority_Update(_float _fTimeDelta)
{
    if (true == m_pModelCom->Play_Animation(_fTimeDelta))
        int a = 10;

    if (*m_pParentState != m_iPreAnimationState)
    {
        // canon idle
        if (*m_pParentState == 1)
        {
            m_pModelCom->SetUp_Animation(0, true, true);
        }
        // canon in
        if (*m_pParentState == 2)
        {
            m_pModelCom->SetUp_Animation(1, false, true);
        }
        // canon out
        if (*m_pParentState == 3)
        {
            m_pModelCom->SetUp_Animation(2, false, true);
        }
        // canon preshot
        if (*m_pParentState == 4)
        {
            m_pModelCom->SetUp_Animation(3, false, true);
        }
        // canon shot
        if (*m_pParentState == 5)
        {
            m_pModelCom->SetUp_Animation(4, true, true);
        }
        // canon shot loop
        if (*m_pParentState == 6)
        {
            m_pModelCom->SetUp_Animation(5, true, true);
        }
        // firethrower idle
        if (*m_pParentState == 7)
        {
            m_pModelCom->SetUp_Animation(6, true, true);
        }
        // firethrower in
        if (*m_pParentState == 8)
        {
            m_pModelCom->SetUp_Animation(7, false, true);
        }
        // firethrower out
        if (*m_pParentState == 9)
        {
            m_pModelCom->SetUp_Animation(8, false, true);
        }
        // firethrower preshot
        if (*m_pParentState == 10)
        {
            m_pModelCom->SetUp_Animation(9, false, true);
        }
        // firethrower shot
        if (*m_pParentState == 11)
        {
            m_pModelCom->SetUp_Animation(10, false, true);
            m_pModelCom->Set_AnimationSpeed(0.5f, 11);
        }
        // handbim in
        if (*m_pParentState == 12)
        {
            m_pModelCom->SetUp_Animation(11, false, true);
        }
        // handbim out
        if (*m_pParentState == 13)
        {
            m_pModelCom->SetUp_Animation(12, false, true);
        }
        // handbim shootclose
        if (*m_pParentState == 14)
        {
            m_pModelCom->SetUp_Animation(13, false, true);
        }
        // handbim shootopen
        if (*m_pParentState == 15)
        {
            m_pModelCom->SetUp_Animation(14, true, true);
        }
        // tower idle
        if (*m_pParentState == 16)
        {
            m_pModelCom->SetUp_Animation(15, true, true);
        }
        // tower intro
        if (*m_pParentState == 17)
        {
            m_pModelCom->SetUp_Animation(16, false, true);
        }
        // mortal preshot
        if (*m_pParentState == 18)
        {
            m_pModelCom->SetUp_Animation(17, false, true);
        }
        // mortal shot
        if (*m_pParentState == 19)
        {
            m_pModelCom->SetUp_Animation(18, true, true);
            m_pModelCom->Set_AnimationSpeed(0.5f, 19);
        }
        m_iPreAnimationState = *m_pParentState;


        /*if (*m_pParentState == 17)
            cout << "몬스터 애니메이션 상태 : " << "인트로" << endl;
        if (*m_pParentState == 16)
            cout << "몬스터 애니메이션 상태 : " << "가만있음" << endl;
        if (*m_pParentState == 2)
            cout << "몬스터 애니메이션 상태 : " << "캐논꺼내는중" << endl;
        if (*m_pParentState == 4)
            cout << "몬스터 애니메이션 상태 : " << "캐논사격준비자세" << endl;
        if (*m_pParentState == 5)
            cout << "몬스터 애니메이션 상태 : " << "캐논사격쏘는중" << endl;
        if (*m_pParentState == 3)
            cout << "몬스터 애니메이션 상태 : " << "캐논집어넣음" << endl;
        if (*m_pParentState == 8)
            cout << "몬스터 애니메이션 상태 : " << "불 모션 나오는중" << endl;
        if (*m_pParentState == 10)
            cout << "몬스터 애니메이션 상태 : " << "불 사격준비자세 " << endl;
        if (*m_pParentState == 11)
            cout << "몬스터 애니메이션 상태 : " << "불 지지는 중" << endl;
        if (*m_pParentState == 9)
            cout << "몬스터 애니메이션 상태 : " << "불 모션 집어넣음" << endl;
        if (*m_pParentState == 7)
            cout << "몬스터 애니메이션 상태 : " << "불 끝나고 가만있기" << endl;
        if (*m_pParentState == 12)
            cout << "몬스터 애니메이션 상태 : " << "손 빔 꺼내는 중" << endl;
        if (*m_pParentState == 15)
            cout << "몬스터 애니메이션 상태 : " << "손 뚜껑 열면서 쏨" << endl;
        if (*m_pParentState == 13)
            cout << "몬스터 애니메이션 상태 : " << "손 빔 집어넣음" << endl;
        if (*m_pParentState == 18)
            cout << "몬스터 애니메이션 상태 : " << "대포 펑 쏘고끝나기직전" << endl;
        if (*m_pParentState == 19)
            cout << "몬스터 애니메이션 상태 : " << "대포 쏘고 돌아옴" << endl;*/

    }

    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentWorldMatrix));
}

void CBody_Boss_Tower::Update(_float _fTimeDelta)
{
    if (m_fHitEffectValue > 0.0f)
    {
        m_fHitEffectValue -= _fTimeDelta * 1.5f; 
        m_fHitEffectValue = max(0.0f, m_fHitEffectValue);
    }

    if (m_bDissolving)
    {
        m_fDissolveTime += _fTimeDelta * 0.5f; 
        m_fDissolveTime = min(m_fDissolveTime, 1.0f);
    }
}

void CBody_Boss_Tower::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
    //m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this);
}

HRESULT CBody_Boss_Tower::Render()
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

_bool CBody_Boss_Tower::IsAnimationFinished() const
{
    return m_pModelCom->IsAnimOnceDone();
}

void CBody_Boss_Tower::Activate_HittedEffect()
{
    m_fHitEffectValue = 1.0f;
}

void CBody_Boss_Tower::Activate_DissolveEffect()
{
    m_fDissolveTime = 0.0f;
    m_bDissolving = true;
}

HRESULT CBody_Boss_Tower::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RoboBoss"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Boss_Tower::Bind_ShaderResources()
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

    if(FAILED(m_pShaderCom->Bind_RawValue("g_DissolveValue", &m_fDissolveTime, sizeof(_float))))
        return E_FAIL;

    //_float4 vColor = { 1.0f, 0.f, 0.0f, 1.0f };
    //if (FAILED(m_pShaderCom->Bind_RawValue("g_DissolveColor", &vColor, sizeof(_float4))))
    //    return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 0)))
        return E_FAIL;

    return S_OK;
}

CBody_Boss_Tower* CBody_Boss_Tower::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBody_Boss_Tower* pInstance = new CBody_Boss_Tower(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CBody_Boss_Tower");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CBody_Boss_Tower::Clone(void* _pArg)
{
    CBody_Boss_Tower* pInstance = new CBody_Boss_Tower(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CBody_Boss_Tower");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_Boss_Tower::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pTextureCom);
}
