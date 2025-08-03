#include "pch.h"
#include "UIInteraction.h"
#include "GameInstance.h"

CUIInteraction::CUIInteraction(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CUIObject{ _pDevice,_pContext }
{
}

CUIInteraction::CUIInteraction(const CUIInteraction& _Prototype)
    :CUIObject(_Prototype)
{
}

HRESULT CUIInteraction::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUIInteraction::Initialize(void* _pArg)
{
    UIINTERACTION_DESC* pDesc = static_cast<UIINTERACTION_DESC*>(_pArg);

    m_iTextureIndex = pDesc->iTextureIndex;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    m_fOpacity = 1.0f;

    return S_OK;
}

void CUIInteraction::Priority_Update(_float _fTimeDelta)
{
}

void CUIInteraction::Update(_float _fTimeDelta)
{
    if (m_bScaling) 
    {
        if (m_bIncreasing)
            m_fLerpProgress += _fTimeDelta * 6.0f; 
        else
            m_fLerpProgress -= _fTimeDelta * 6.0f; 

        if (m_fLerpProgress >= 1.0f)
        {
            m_fLerpProgress = 1.0f;
            m_bIncreasing = false;
        }
        else if (m_fLerpProgress <= 0.0f)
        {
            m_fLerpProgress = 0.0f;
            m_bIncreasing = true;
        }

        _float fStartScale = 80.0f;
        _float fEndScale = 120.0f;
        _float fCurrentScale = fStartScale + (fEndScale - fStartScale) * m_fLerpProgress;

        m_pTransformCom->Scaling(_float3(fCurrentScale, fCurrentScale, 1.0f));
    }
}

void CUIInteraction::Late_Update(_float _fTimeDelta)
{
    if (!m_bVisible)
        return;

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this);
}

HRESULT CUIInteraction::Render()
{
    if (!m_bVisible)
        return S_OK;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")) ||
        FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)) ||
        FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
    {
        return E_FAIL;
    }

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_Opacity", &m_fOpacity, sizeof(_float))))
        return E_FAIL;

    m_pShaderCom->Begin(5);
    m_pVIBufferCom->Bind_InputAssembler();
    m_pVIBufferCom->Render();

    return S_OK;
}

void CUIInteraction::Set_HighLight(_bool _bHighlight)
{
    if (_bHighlight)
    {
        m_bScaling = true;  
    }
    else
    {
        m_bScaling = false;  
        m_pTransformCom->Scaling(_float3(100.0f, 100.0f, 1.0f));
    }
}

HRESULT CUIInteraction::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Interaction"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CUIInteraction* CUIInteraction::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CUIInteraction* pInstance = new CUIInteraction(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CUIInteraction");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUIInteraction::Clone(void* _pArg)
{
    CUIInteraction* pInstance = new CUIInteraction(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CUIInteraction");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUIInteraction::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
