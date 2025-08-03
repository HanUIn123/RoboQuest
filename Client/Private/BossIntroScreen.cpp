#include "pch.h"
#include "BossIntroScreen.h"
#include "GameInstance.h"

CBossIntroScreen::CBossIntroScreen(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CUIObject{ _pDevice, _pContext }
{
}

CBossIntroScreen::CBossIntroScreen(const CBossIntroScreen& _Prototype)
    :CUIObject(_Prototype)
{
}

HRESULT CBossIntroScreen::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBossIntroScreen::Initialize(void* _pArg)
{
    INTRO_DESC* pDesc = static_cast<INTRO_DESC*>(_pArg);

    pDesc->fSizeX = g_iWinSizeX;
    pDesc->fSizeY = g_iWinSizeY;
    pDesc->fX = g_iWinSizeX * 0.5f;
    pDesc->fY = g_iWinSizeY * 0.5f;
    pDesc->fZ = 0.005f;
    pDesc->fNear = 0.0f;
    pDesc->fFar = 1.0f;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;


    return S_OK;
}

void CBossIntroScreen::Priority_Update(_float _fTimeDelta)
{
}

void CBossIntroScreen::Update(_float _fTimeDelta)
{
    if (m_bIsVisible)
    {
        m_fDissolveTime -= _fTimeDelta;

        m_fDissolveTime = max(0.0f, m_fDissolveTime);

        if (!m_bDissolving)
        {
            m_fDissolveTime += _fTimeDelta * 0.2f;
            m_fDissolveTime = min(1.0f, m_fDissolveTime);

            if (m_fDissolveTime >= 1.0f)
                m_bDissolving = true;
        }
    }
}

void CBossIntroScreen::Late_Update(_float _fTimeDelta)
{
    if (!m_bIsVisible)
        return;

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this);
}

HRESULT CBossIntroScreen::Render()
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

    if (FAILED(m_pTextureCom2->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 0)))
        return E_FAIL;


    if (FAILED(m_pShaderCom->Bind_RawValue("g_DissolveValue", &m_fDissolveTime, sizeof(_float))))
        return E_FAIL;

    m_pShaderCom->Begin(5);

    m_pVIBufferCom->Bind_InputAssembler();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CBossIntroScreen::Activate_BossIntroScreen()
{
    if (!m_bIsVisible)
    {
        m_bIsVisible = true;
    }
}

void CBossIntroScreen::DeActivate_BossIntroScreen()
{
    if (m_bIsVisible)
    {
        m_bIsVisible = false;
    }
}

HRESULT CBossIntroScreen::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossIntro"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_IntroDissolve"), TEXT("Com_Texture2"), reinterpret_cast<CComponent**>(&m_pTextureCom2))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_ScopeScreen"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_ScopeScreen"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CBossIntroScreen* CBossIntroScreen::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBossIntroScreen* pInstance = new CBossIntroScreen(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CBossIntroScreen");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBossIntroScreen::Clone(void* _pArg)
{
    CBossIntroScreen* pInstance = new CBossIntroScreen(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CBossIntroScreen");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBossIntroScreen::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTextureCom2);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
