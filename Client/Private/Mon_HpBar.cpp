#include "pch.h"
#include "Mon_HpBar.h"
#include "GameInstance.h"

CMon_HpBar::CMon_HpBar(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CUIPartObject{_pDevice, _pContext}
{
}

CMon_HpBar::CMon_HpBar(const CMon_HpBar& _Prototype)
    :CUIPartObject(_Prototype)
{
}

HRESULT CMon_HpBar::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

  

    return S_OK;
}

HRESULT CMon_HpBar::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    MONSTER_HPBAR_DESC* pDesc = static_cast<MONSTER_HPBAR_DESC*>(_pArg);
    if (pDesc)
    {
        m_fCurrentHp = static_cast<_float>((*pDesc->pMonsterHp));
        m_fBackgroundHp = m_fCurrentHp;
        m_fMaxHp = m_fCurrentHp;
    }

    return S_OK;
}

void CMon_HpBar::Priority_Update(_float _fTimeDelta)
{
}

void CMon_HpBar::Update(_float _fTimeDelta)
{
    if (m_fBackgroundHp > m_fCurrentHp)
    {
        _float fDecreaseSpeed = 50.0f; 
        _float fDifference = m_fBackgroundHp - m_fCurrentHp;

        m_fBackgroundHp -= min(fDifference, _fTimeDelta * fDecreaseSpeed);
    }

        __super::Update(_fTimeDelta);
}

void CMon_HpBar::Late_Update(_float _fTimeDelta)
{
    _float4x4 ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);

    _float4x4 ViewMatrixInverse;
    XMStoreFloat4x4(&ViewMatrixInverse, XMMatrixInverse(nullptr, XMLoadFloat4x4(&ViewMatrix)));

    _float4x4 BillBoardMatrix;
    XMStoreFloat4x4(&BillBoardMatrix, XMMatrixIdentity());

    BillBoardMatrix =
    {
           ViewMatrix._11,   0.0f,   ViewMatrix._13,  0.0f,
            0.0f,            1.0f,             0.0f,  0.0f,
           ViewMatrix._31,   0.0f,   ViewMatrix._33,  0.0f,
                     0.0f,   0.0f,             0.0f,  1.0f,
    };

    _float4x4 BillBoardMatrixInverse;
    XMStoreFloat4x4(&BillBoardMatrixInverse, XMMatrixInverse(nullptr, XMLoadFloat4x4(&BillBoardMatrix)));

    _vector vScale{}, vRotation{}, vTranslation{};

    XMMatrixDecompose(&vScale, &vRotation, &vTranslation, XMLoadFloat4x4(m_pParentWorldMatrix));

    _float4x4 matWithOutLookMatrix;
    XMStoreFloat4x4(&matWithOutLookMatrix, XMMatrixAffineTransformation(vScale, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), vTranslation));

    _float4x4 HpBarMatrix;
    XMStoreFloat4x4(&HpBarMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(&BillBoardMatrixInverse) * XMLoadFloat4x4(&matWithOutLookMatrix));

    Set_WorldMatrix(&HpBarMatrix);

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this);
}

HRESULT CMon_HpBar::Render()
{
    return S_OK;
}

void CMon_HpBar::Decrease_Hp(_uint _iCurrentHp)
{
    m_fCurrentHp = static_cast<_float>(_iCurrentHp);

    if (m_fBackgroundHp > m_fCurrentHp)
    {
        _float fDecreaseSpeed = 30.0f; 
        m_fBackgroundHp -= (m_fBackgroundHp - m_fCurrentHp) * 0.1f;
    }
}

CMon_HpBar* CMon_HpBar::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CMon_HpBar* pInstance = new CMon_HpBar(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CMon_HpBar");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CMon_HpBar::Clone(void* _pArg)
{
    CMon_HpBar* pInstance = new CMon_HpBar(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CMon_HpBar");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMon_HpBar::Free()
{
    __super::Free();
}
