#include "pch.h"
#include "ScopeGauge.h"
#include "GameInstance.h"

CScopeGauge::CScopeGauge(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CUIObject{ _pDevice, _pContext }
{
}

CScopeGauge::CScopeGauge(const CScopeGauge& _Prototype)
    :CUIObject(_Prototype)
{
}

HRESULT CScopeGauge::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CScopeGauge::Initialize(void* _pArg)
{
    SCOPE_GAUGE_DESC* pDesc = static_cast<SCOPE_GAUGE_DESC*>(_pArg);

    pDesc->fSizeX = 200.0f;
    pDesc->fSizeY = 20.0f;
    pDesc->fX = g_iWinSizeX * 0.5f;
    pDesc->fY = (g_iWinSizeY * 0.5f) + 50.0f;
    pDesc->fZ = 0.41f;
    pDesc->fNear = 0.0f;
    pDesc->fFar = 1.0f;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    m_fCurrentBarSize = 0.0f;

    m_fMaxBarSize = pDesc->fSizeX;

    return S_OK;
}

void CScopeGauge::Priority_Update(_float _fTimeDelta)
{
}

void CScopeGauge::Update(_float _fTimeDelta)
{
    if (!m_bIsVisible)
        return;

    m_fCurrentBarSize += _fTimeDelta * 160.0f;
    if (m_fCurrentBarSize >= m_fMaxBarSize)
    {
        m_fCurrentBarSize = m_fMaxBarSize;
        m_bIsFullCharge = true;
    }
    else
    {
        m_bIsFullCharge = false;
    }

}

void CScopeGauge::Late_Update(_float _fTimeDelta)
{
    if (!m_bIsVisible)
        return;

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this);
}

HRESULT CScopeGauge::Render()
{
    if (!m_bIsVisible)
        return S_OK;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")) ||
        FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)) ||
        FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
    {
        return E_FAIL;
    }

    if (FAILED(m_pBarTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if(FAILED(m_pShaderCom->Bind_RawValue("g_CurrentBarSize", &m_fCurrentBarSize, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_BackGroundBarSize", &m_fBackGroundSize, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_MaxBarSize", &m_fMaxBarSize, sizeof(_float))))
        return E_FAIL;

    m_pShaderCom->Begin(4);

    m_pVIBufferCom->Bind_InputAssembler();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CScopeGauge::Activate_Gauge()
{
    if (!m_bIsVisible)
        m_bIsVisible = true;
}

void CScopeGauge::DeActivate_Gauge()
{
    if (m_bIsVisible)
    {
        m_bIsVisible = false;
        m_fCurrentBarSize = 0.0f;
        m_bIsFullCharge = false;
    }
}

_float CScopeGauge::Compute_GaugePercent()
{
    return m_fCurrentBarSize / m_fBackGroundSize;
}

HRESULT CScopeGauge::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ScopeBar"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pBarTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_ScopeScreen"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_ScopeScreen"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CScopeGauge* CScopeGauge::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CScopeGauge* pInstance = new CScopeGauge(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CScopeGauge");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CScopeGauge::Clone(void* _pArg)
{
    CScopeGauge* pInstance = new CScopeGauge(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CScopeGauge");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CScopeGauge::Free()
{
    __super::Free();

    Safe_Release(m_pBarTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
