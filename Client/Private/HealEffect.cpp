#include "pch.h"
#include "HealEffect.h"
#include "GameInstance.h"
CHealEffect::CHealEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CUIObject{ _pDevice, _pContext }
{
}

CHealEffect::CHealEffect(const CHealEffect& _Prototype)
    :CUIObject(_Prototype)
{
}

HRESULT CHealEffect::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CHealEffect::Initialize(void* _pArg)
{
    HEAL_EFFECT_DESC* pDesc = static_cast<HEAL_EFFECT_DESC*>(_pArg);

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

    return S_OK;
}

void CHealEffect::Priority_Update(_float _fTimeDelta)
{
}

void CHealEffect::Update(_float _fTimeDelta)
{
    if (m_fAlpha > 0.0f)
    {
        m_fAlpha -= _fTimeDelta * 2.5f;
        if (m_fAlpha < 0.0f)
            m_fAlpha = 0.0f;
    }
}

void CHealEffect::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this);
}

HRESULT CHealEffect::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")) ||
        FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)) ||
        FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
    {
        return E_FAIL;
    }


    static int frameCount = 0;
    static float fTime = 0.0f;

    frameCount++;
    fTime = frameCount * 0.016f;  // 60FPS ±âÁØ (16ms per frame)

    if (FAILED(m_pShaderCom->Bind_RawValue("g_Time", &fTime, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_FadeAlpha", &m_fAlpha, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    m_pShaderCom->Begin(9);

    m_pVIBufferCom->Bind_InputAssembler();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CHealEffect::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Heal"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Hurt"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_Hurt"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CHealEffect* CHealEffect::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CHealEffect* pInstance = new CHealEffect(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CHealEffect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHealEffect::Clone(void* _pArg)
{
    CHealEffect* pInstance = new CHealEffect(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CHealEffect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHealEffect::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
