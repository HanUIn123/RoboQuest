#include "pch.h"
#include "BreakEffect.h"
#include "GameInstance.h"

CBreakEffect::CBreakEffect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CUIObject{ _pDevice, _pContext }
{
}

CBreakEffect::CBreakEffect(const CBreakEffect& _Prototype)
    :CUIObject(_Prototype)
{
}

HRESULT CBreakEffect::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBreakEffect::Initialize(void* _pArg)
{
    BREAK_EFFECT_DESC* pDesc = static_cast<BREAK_EFFECT_DESC*>(_pArg);

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

void CBreakEffect::Priority_Update(_float _fTimeDelta)
{
}

void CBreakEffect::Update(_float _fTimeDelta)
{
    if (!m_bActivate)
        return;

    if (m_fAlpha > 0.0f)
    {
        m_fAlpha -= _fTimeDelta * 2.5f;
        if (m_fAlpha < 0.0f)
            m_fAlpha = 0.0f;
    }

    m_fAnimationTime += _fTimeDelta * 16.0f;

    if ((m_fFlowTime += _fTimeDelta) >= 2.5f)
    {
        m_fFlowTime = 0.0f;
        m_bActivate = false;
    }
}

void CBreakEffect::Late_Update(_float _fTimeDelta)
{
    if (!m_bActivate)
        return;

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this);
}

HRESULT CBreakEffect::Render()
{
    if (!m_bActivate)
        return S_OK;

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

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAnimationTime", &m_fAnimationTime, sizeof(_float))))
        return E_FAIL;

    m_pShaderCom->Begin(8);

    m_pVIBufferCom->Bind_InputAssembler();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CBreakEffect::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Break"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Hurt"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_Hurt"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CBreakEffect* CBreakEffect::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBreakEffect* pInstance = new CBreakEffect(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CBreakEffect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBreakEffect::Clone(void* _pArg)
{
    CBreakEffect* pInstance = new CBreakEffect(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CBreakEffect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBreakEffect::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
