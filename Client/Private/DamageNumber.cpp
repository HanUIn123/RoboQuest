#include "pch.h"
#include "DamageNumber.h"
#include "GameInstance.h"

CDamageNumber::CDamageNumber(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CUIPartObject{ _pDevice, _pContext }
{
}

CDamageNumber::CDamageNumber(const CDamageNumber& _Prototype)
    :CUIPartObject(_Prototype)
{
}

HRESULT CDamageNumber::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CDamageNumber::Initialize(void* _pArg)
{

    DAMAGE_NUM_DESC* pDesc = static_cast<DAMAGE_NUM_DESC*>(_pArg);
    _float fOffset = (g_iWinSizeY * 0.5f);

    pDesc->fSizeX = 1.0f;
    pDesc->fSizeY = 1.0f;
    pDesc->fX = g_iWinSizeX * 0.5f;

    //m_fHeight = 
    //DAMAGE_NUM_DESC::fY 
    // 내릴수록,. 내려간다. 
    //  _float fOffset = (g_iWinSizeY * 0.5f);
    //pDesc->fY = fOffset - (fOffset * 0.006f);
    pDesc->fZ = 0.2f;
    pDesc->fNear = 0.0f;
    pDesc->fFar = 1.0f;

    m_fLifeTime = pDesc->fLifeTime;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CDamageNumber::Priority_Update(_float _fTimeDelta)
{
}

void CDamageNumber::Update(_float _fTimeDelta)
{
    if (m_fLifeTime <= 0.0f)
        return;

    m_fLifeTime -= _fTimeDelta;

    m_fVerticalOffset += 0.5f * _fTimeDelta;

    //m_fAlpha -= 0.2f * _fTimeDelta;
    //if (m_fAlpha < 0.0f) 
    //    m_fAlpha = 0.0f;
}

void CDamageNumber::Late_Update(_float _fTimeDelta)
{
    if (m_fLifeTime <= 0.0f)
        return;

    _float4x4 ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
    _float4x4 BillBoardMatrix;

    XMStoreFloat4x4(&BillBoardMatrix, XMMatrixIdentity());

    BillBoardMatrix =
    {
        ViewMatrix._11,   0.0f,   ViewMatrix._13,  0.0f,
        0.0f,             1.0f,   0.0f,            0.0f,
        ViewMatrix._31,   0.0f,   ViewMatrix._33,  0.0f,
        0.0f,             0.0f,   0.0f,            1.0f,
    };

    _float4x4 BillBoardMatrixInverse;
    XMStoreFloat4x4(&BillBoardMatrixInverse, XMMatrixInverse(nullptr, XMLoadFloat4x4(&BillBoardMatrix)));

    _vector vScale{}, vRotation{}, vTranslation{};

    XMMatrixDecompose(&vScale, &vRotation, &vTranslation, XMLoadFloat4x4(m_pParentWorldMatrix));

    vTranslation = XMVectorSetY(vTranslation, XMVectorGetY(vTranslation) + m_fVerticalOffset);

    _float4x4 matWithOutLookMatrix;
    XMStoreFloat4x4(&matWithOutLookMatrix, XMMatrixAffineTransformation(vScale, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), vTranslation));

    _float4x4 HpBarMatrix;
    XMStoreFloat4x4(&HpBarMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(&BillBoardMatrixInverse) * XMLoadFloat4x4(&matWithOutLookMatrix));

    Set_WorldMatrix(&HpBarMatrix);

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this);
}

HRESULT CDamageNumber::Render()
{
    if (m_fLifeTime <= 0.0f)
        return S_OK;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    //if (FAILED(m_pShaderCom->Bind_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
    //    return E_FAIL;

    m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iDigit);
    m_pShaderCom->Begin(2);
    m_pVIBufferCom->Bind_InputAssembler();
    m_pVIBufferCom->Render();

    return S_OK;
}


void CDamageNumber::Set_DamageNumberDigit(_uint _iDigit, _uint _iIndex)
{
    m_iDigit = _iDigit;
    m_iIndex = _iIndex;

    if (_iIndex > 2)
    {
        _iIndex = 2; 
    }

    _float fNumberInterval = 0.3f;
    _float fSortingInterval = -0.3f;

    m_fVerticalOffset = 0.0f;

    _vector vBasePos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    _float xOffset = (_iIndex * fNumberInterval) + fSortingInterval;

    _vector vNewPosition = XMVectorSet(XMVectorGetX(vBasePos) + xOffset, XMVectorGetY(vBasePos), XMVectorGetZ(vBasePos), 1.0f);
    if (!m_bShow)
    {
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vNewPosition);
        m_bShow = true;
    }
}

void CDamageNumber::Set_LifeTime(_float _fTime)
{
    m_fLifeTime = _fTime;
}

HRESULT CDamageNumber::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DamageNum"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CDamageNumber* CDamageNumber::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CDamageNumber* pInstance = new CDamageNumber(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CDamageNumber");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CDamageNumber::Clone(void* _pArg)
{
    CDamageNumber* pInstance = new CDamageNumber(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CDamageNumber");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDamageNumber::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
