#include "pch.h"
#include "UINumber.h"
#include "GameInstance.h"

CUINumber::CUINumber(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CUIObject(_pDevice, _pContext)
{
}

CUINumber::CUINumber(const CUINumber& _Prototype)
    : CUIObject(_Prototype)
    , m_iDigit(_Prototype.m_iDigit)
{
}

HRESULT CUINumber::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUINumber::Initialize(void* _pArg)
{
    UINUMBER_DESC* pDesc = static_cast<UINUMBER_DESC*>(_pArg);

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    return S_OK;
}

void CUINumber::Priority_Update(_float _fTimeDelta)
{
}

void CUINumber::Update(_float _fTimeDelta)
{
}

void CUINumber::Late_Update(_float _fTimeDelta)
{
    if(m_bVisible)
        m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this);
}

HRESULT CUINumber::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")) ||
        FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)) ||
        FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
    {
        return E_FAIL;
    }

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iDigit)))
        return E_FAIL;

    m_pShaderCom->Begin(2);
    m_pVIBufferCom->Bind_InputAssembler();
    m_pVIBufferCom->Render();

    return S_OK;
}

void CUINumber::Set_Digit(_int _iDigit)
{
    m_iDigit = _iDigit;
}

HRESULT CUINumber::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Number"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CUINumber* CUINumber::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CUINumber* pInstance = new CUINumber(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CUINumber");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUINumber::Clone(void* _pArg)
{
    CUINumber* pInstance = new CUINumber(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CUINumber");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUINumber::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}