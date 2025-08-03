#include "pch.h"
#include "ScopeScreen.h"
#include "GameInstance.h"

CScopeScreen::CScopeScreen(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CUIObject{ _pDevice, _pContext }
{
}

CScopeScreen::CScopeScreen(const CScopeScreen& _Prototype)
    :CUIObject(_Prototype)
{
}

HRESULT CScopeScreen::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CScopeScreen::Initialize(void* _pArg)
{
    SCOPE_SCRREN_DESC* pDesc = static_cast<SCOPE_SCRREN_DESC*>(_pArg);

    pDesc->fSizeX = g_iWinSizeX;
    pDesc->fSizeY = g_iWinSizeY;
    pDesc->fX = g_iWinSizeX * 0.5f;
    pDesc->fY = g_iWinSizeY * 0.5f;
    pDesc->fZ = 0.4f;
    pDesc->fNear = 0.0f;
    pDesc->fFar = 1.0f;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    m_pGaugeBar = static_cast<CScopeGauge*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_ScopeGauge"), LEVEL_GAMEPLAY, TEXT("Layer_PlayerEffect")));

    return S_OK;
}

void CScopeScreen::Priority_Update(_float _fTimeDelta)
{
}

void CScopeScreen::Update(_float _fTimeDelta)
{
}

void CScopeScreen::Late_Update(_float _fTimeDelta)
{
    if (!m_bIsVisible)
        return;

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this);
}

HRESULT CScopeScreen::Render()
{
    if (!m_bIsVisible)
        return S_OK;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")) ||
        FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)) ||
        FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
    {
        return E_FAIL;
    }

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    m_pShaderCom->Begin(3);

    m_pVIBufferCom->Bind_InputAssembler();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CScopeScreen::Activate_ScopeScreen()
{
    if (!m_bIsVisible)
    {
        m_bIsVisible = true;

        m_pGaugeBar->Activate_Gauge();
    }
}

void CScopeScreen::DeActivate_ScopeScreen()
{
    if (m_bIsVisible)
    {
        m_bIsVisible = false;

        m_pGaugeBar->DeActivate_Gauge();
    }
}

HRESULT CScopeScreen::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ScopeScreen"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_ScopeScreen"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_ScopeScreen"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CScopeScreen* CScopeScreen::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CScopeScreen* pInstance = new CScopeScreen(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CScopeScreen");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CScopeScreen::Clone(void* _pArg)
{
    CScopeScreen* pInstance = new CScopeScreen(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CScopeScreen");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CScopeScreen::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
