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
            m_pModelCom->SetUp_Animation(0, false, true);  // 0: �������� ����
        }
        // Rifle Shoot Animation
        else if (*m_pParentState == 2)  // STATE_RIFLE_SHOOT
        {
            m_pModelCom->SetUp_Animation(1, false, true);  // 1: �������� �ܾƿ�
        }
        // Rifle Idle Animation
        else if (*m_pParentState == 3)  // STATE_RIFLE_IDLE
        {
            m_pModelCom->SetUp_Animation(2, false, true);  // 2: �������� ��
        }
        if (*m_pParentState == 4)  // STATE_RIFLE_RELOAD
        {
            m_pModelCom->SetUp_Animation(3, false, true);  // 3 : ������ ���
            m_pModelCom->Set_AnimationSpeed(4.0f, 4);
        }
        // Rifle Shoot Animation
        else if (*m_pParentState == 5)  // STATE_RIFLE_SHOOT
        {
            m_pModelCom->SetUp_Animation(4, true, true);  // 4: ������ ������
        }
        // Rifle Idle Animation
        else if (*m_pParentState == 6)  // STATE_RIFLE_IDLE
        {
            m_pModelCom->SetUp_Animation(5, true, true);  // 5: ������ �޸���
        }
        // Rifle Walk Animation
        else if (*m_pParentState == 7)  // STATE_RIFLE_WALK
        {
            m_pModelCom->SetUp_Animation(6, true, true);  // 6: ������ �뽬�ϱ�
        }
        // Rifle Dash Animation
        else if (*m_pParentState == 8)  // STATE_RIFLE_DASH
        {
            m_pModelCom->SetUp_Animation(7, false, true);  // 7: ������ ��ü
        }
        // Rifle Switch Animation
        else if (*m_pParentState == 9)  // STATE_RIFLE_SWITCH
        {
            m_pModelCom->SetUp_Animation(8, true, true); // 8: �ڵ�� ������
        }
        // Handgun Idle Animation
        else if (*m_pParentState == 10)  // STATE_HANDGUN_IDLE
        {
            m_pModelCom->SetUp_Animation(9, true, true);  // 9: �ڵ�� �ȱ�
        }
        // Handgun Walk Animation
        else if (*m_pParentState == 11)  // STATE_HANDGUN_WALK
        {
            m_pModelCom->SetUp_Animation(10, true, true);  // 10: �ڵ�� �뽬
        }
        // Handgun Dash Animation
        else if (*m_pParentState == 12)  // STATE_HANDGUN_DASH
        {
            m_pModelCom->SetUp_Animation(11, false, true);  // 11: �ڵ�� ��ü
        }
        // Handgun Switch Animation
        else if (*m_pParentState == 13)  // STATE_HANDGUN_SWITCH
        {
            m_pModelCom->SetUp_Animation(12, true, true);  // 12: �ڵ�� ���
            m_pModelCom->Set_AnimationSpeed(1.5f, 13);
        }
        // Handgun Shoot Animation
        else if (*m_pParentState == 14)  // STATE_HANDGUN_SHOOT
        {
            m_pModelCom->SetUp_Animation(13, false, true); // 13: �ڵ�� ����
            m_pModelCom->Set_AnimationSpeed(2.0f, 14);
        }
        // Handgun Reload Animation
        else if (*m_pParentState == 15)  // STATE_HANDGUN_RELOAD
        {
            m_pModelCom->SetUp_Animation(14, false, true); // 14: ������ ����
            m_pModelCom->Set_AnimationSpeed(2.0f, 15);
        }

        m_iPreAnimationState = *m_pParentState;

       /* if (*m_pParentState == 1)
            cout << "�÷��̾� �ִϸ��̼� ���� : " << "�������� ����" << endl;
        if (*m_pParentState == 2)
            cout << "�÷��̾� �ִϸ��̼� ���� : " << "�������� �ܾƿ�" << endl;
        if (*m_pParentState == 3)
            cout << "�÷��̾� �ִϸ��̼� ���� : " << "�������� ���" << endl;

        if (*m_pParentState == 4)
            cout << "�÷��̾� �ִϸ��̼� ���� : " << "������ ���" << endl;
        if (*m_pParentState == 5)
            cout << "�÷��̾� �ִϸ��̼� ���� : " << "������ ������" << endl;
        if (*m_pParentState == 6)
            cout << "�÷��̾� �ִϸ��̼� ���� : " << "������ �޸���" << endl;
        if (*m_pParentState == 7)
            cout << "�÷��̾� �ִϸ��̼� ���� : " << "������ �뽬�ϱ�" << endl;
        if (*m_pParentState == 8)
            cout << "�÷��̾� �ִϸ��̼� ���� : " << "������ ��ü" << endl;

        if (*m_pParentState == 9)
            cout << "�÷��̾� �ִϸ��̼� ���� : " << "�ڵ�� ������" << endl;
        if (*m_pParentState == 10)
            cout << "�÷��̾� �ִϸ��̼� ���� : " << "�ڵ�� �޸���" << endl;
        if (*m_pParentState == 11)
            cout << "�÷��̾� �ִϸ��̼� ���� : " << "�ڵ�� �뽬�ϱ�" << endl;
        if (*m_pParentState == 12)
            cout << "�÷��̾� �ִϸ��̼� ���� : " << "�ڵ�� ��ü" << endl;
        if (*m_pParentState == 13)
            cout << "�÷��̾� �ִϸ��̼� ���� : " << "�ڵ�� ���" << endl;
        if (*m_pParentState == 14)
            cout << "�÷��̾� �ִϸ��̼� ���� : " << "�ڵ�� �����ϱ�" << endl;
        if (*m_pParentState == 15)
            cout << "�÷��̾� �ִϸ��̼� ���� : " << "������ �����ϱ�" << endl;*/
    }


    // Body Player �� ������� �޹��� ��Ŀ��ٰ�, �� Body Player �� Ʈ���������� ������ ���� ��� * �θ� ���� ����� ����,( �ڽ� ����x �θ� ���� ) 
    // �Ѱ��� ���������� .
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
    // ���� �� BodyPlayer�� �����, �θ��� ��ı��� ���� ���� ������ ����� ���̴��� �����ٲ��̴�.. 
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
