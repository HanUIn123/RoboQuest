#include "pch.h"
#include "../Public/Body_Player.h"
#include "GameInstance.h"
#include "Player.h"


CBody_Player::CBody_Player(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CPartObject{ _pDevice, _pContext }
{
}

CBody_Player::CBody_Player(const CBody_Player& _Prototype)
    :CPartObject(_Prototype)
{
}

HRESULT CBody_Player::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Player::Initialize(void* _pArg)
{
    CBody_Player::BODY_PLAYER_DESC* pDesc = static_cast<CBody_Player::BODY_PLAYER_DESC*>(_pArg);


    m_pParentState = pDesc->pParentState;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //m_pModelCom->SetUp_Animation(3, true, true);

    return S_OK;
}

void CBody_Player::Priority_Update(_float _fTimeDelta)
{
    if (true == m_pModelCom->Play_Animation(_fTimeDelta))
        int a = 10;

    if (*m_pParentState != m_iPreAnimationState)
    {
        // Rifle Reload Animation
        if (*m_pParentState == 1)  // STATE_RIFLE_RELOAD
        {
            m_pModelCom->SetUp_Animation(0, false, true);  // 0: 스나이퍼 줌인
        }
        // Rifle Shoot Animation
        else if (*m_pParentState == 2)  // STATE_RIFLE_SHOOT
        {
            m_pModelCom->SetUp_Animation(1, false, true);  // 1: 스나이퍼 줌아웃
        }
        // Rifle Idle Animation
        else if (*m_pParentState == 3)  // STATE_RIFLE_IDLE
        {
            m_pModelCom->SetUp_Animation(2, false, true);  // 2: 스나이퍼 슛
        }
        if (*m_pParentState == 4)  // STATE_RIFLE_RELOAD
        {
            m_pModelCom->SetUp_Animation(3, false, true);  // 3 : 라이플 쏘기
            m_pModelCom->Set_AnimationSpeed(4.0f, 4);
        }
        // Rifle Shoot Animation
        else if (*m_pParentState == 5)  // STATE_RIFLE_SHOOT
        {
            m_pModelCom->SetUp_Animation(4, true, true);  // 4: 라이플 가만히
        }
        // Rifle Idle Animation
        else if (*m_pParentState == 6)  // STATE_RIFLE_IDLE
        {
            m_pModelCom->SetUp_Animation(5, true, true);  // 5: 라이플 달리기
        }
        // Rifle Walk Animation
        else if (*m_pParentState == 7)  // STATE_RIFLE_WALK
        {
            m_pModelCom->SetUp_Animation(6, true, true);  // 6: 라이플 대쉬하기
        }
        // Rifle Dash Animation
        else if (*m_pParentState == 8)  // STATE_RIFLE_DASH
        {
            m_pModelCom->SetUp_Animation(7, false, true);  // 7: 라이플 교체
        }
        // Rifle Switch Animation
        else if (*m_pParentState == 9)  // STATE_RIFLE_SWITCH
        {
            m_pModelCom->SetUp_Animation(8, true, true); // 8: 핸드건 가만히
        }
        // Handgun Idle Animation
        else if (*m_pParentState == 10)  // STATE_HANDGUN_IDLE
        {
            m_pModelCom->SetUp_Animation(9, true, true);  // 9: 핸드건 걷기
        }
        // Handgun Walk Animation
        else if (*m_pParentState == 11)  // STATE_HANDGUN_WALK
        {
            m_pModelCom->SetUp_Animation(10, true, true);  // 10: 핸드건 대쉬
        }
        // Handgun Dash Animation
        else if (*m_pParentState == 12)  // STATE_HANDGUN_DASH
        {
            m_pModelCom->SetUp_Animation(11, false, true);  // 11: 핸드건 교체
        }
        // Handgun Switch Animation
        else if (*m_pParentState == 13)  // STATE_HANDGUN_SWITCH
        {
            m_pModelCom->SetUp_Animation(12, true, true);  // 12: 핸드건 쏘기
            m_pModelCom->Set_AnimationSpeed(1.5f, 13);
        }
        // Handgun Shoot Animation
        else if (*m_pParentState == 14)  // STATE_HANDGUN_SHOOT
        {
            m_pModelCom->SetUp_Animation(13, false, true); // 13: 핸드건 장전
            m_pModelCom->Set_AnimationSpeed(2.0f, 14);
        }
        // Handgun Reload Animation
        else if (*m_pParentState == 15)  // STATE_HANDGUN_RELOAD
        {
            m_pModelCom->SetUp_Animation(14, false, true); // 14: 라이플 장전
            m_pModelCom->Set_AnimationSpeed(2.0f, 15);
        }

        m_iPreAnimationState = *m_pParentState;

       /* if (*m_pParentState == 1)
            cout << "플레이어 애니메이션 상태 : " << "스나이퍼 줌인" << endl;
        if (*m_pParentState == 2)
            cout << "플레이어 애니메이션 상태 : " << "스나이퍼 줌아웃" << endl;
        if (*m_pParentState == 3)
            cout << "플레이어 애니메이션 상태 : " << "스나이퍼 쏘기" << endl;

        if (*m_pParentState == 4)
            cout << "플레이어 애니메이션 상태 : " << "라이플 쏘기" << endl;
        if (*m_pParentState == 5)
            cout << "플레이어 애니메이션 상태 : " << "라이플 가만히" << endl;
        if (*m_pParentState == 6)
            cout << "플레이어 애니메이션 상태 : " << "라이플 달리기" << endl;
        if (*m_pParentState == 7)
            cout << "플레이어 애니메이션 상태 : " << "라이플 대쉬하기" << endl;
        if (*m_pParentState == 8)
            cout << "플레이어 애니메이션 상태 : " << "라이플 교체" << endl;

        if (*m_pParentState == 9)
            cout << "플레이어 애니메이션 상태 : " << "핸드건 가만히" << endl;
        if (*m_pParentState == 10)
            cout << "플레이어 애니메이션 상태 : " << "핸드건 달리기" << endl;
        if (*m_pParentState == 11)
            cout << "플레이어 애니메이션 상태 : " << "핸드건 대쉬하기" << endl;
        if (*m_pParentState == 12)
            cout << "플레이어 애니메이션 상태 : " << "핸드건 교체" << endl;
        if (*m_pParentState == 13)
            cout << "플레이어 애니메이션 상태 : " << "핸드건 쏘기" << endl;
        if (*m_pParentState == 14)
            cout << "플레이어 애니메이션 상태 : " << "핸드건 장전하기" << endl;
        if (*m_pParentState == 15)
            cout << "플레이어 애니메이션 상태 : " << "라이플 장전하기" << endl;*/
    }


    // Body Player 의 최종행렬 콤바인 행렬에다가, 이 Body Player 의 트랜스폼에서 가져온 월드 행렬 * 부모 월드 행렬을 곱한,( 자식 월행x 부모 월행 ) 
    // 한것을 저장해주자 .
    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentWorldMatrix));
}

void CBody_Player::Update(_float _fTimeDelta)
{
}

void CBody_Player::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
    //m_pGameInstance->Add_RenderGroup(CRenderer::RG_SHADOW, this);
}

HRESULT CBody_Player::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture", 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture", 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        m_pShaderCom->Begin(0);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CBody_Player::Render_Shadow()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_Shadow_Matrices(m_pShaderCom, "g_ViewMatrix", "g_ProjMatrix")))
        return E_FAIL;

    _uint			iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        m_pShaderCom->Begin(1);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

_bool CBody_Player::IsAnimationFinished() const
{
    return m_pModelCom->IsAnimOnceDone();
}

HRESULT CBody_Player::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPlayerAnimMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Player::Bind_ShaderResources()
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

    return S_OK;
}


CBody_Player* CBody_Player::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBody_Player* pInstance = new CBody_Player(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CBody_Player");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBody_Player::Clone(void* _pArg)
{
    CBody_Player* pInstance = new CBody_Player(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CBody_Player");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_Player::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
