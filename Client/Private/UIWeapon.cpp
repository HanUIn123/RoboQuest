#include "pch.h"
#include "UIWeapon.h"
#include "GameInstance.h"

CUIWeapon::CUIWeapon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CUIObject{_pDevice, _pContext}
{
}

CUIWeapon::CUIWeapon(const CUIWeapon& _Prototype)
    :CUIObject(_Prototype)
{
}

HRESULT CUIWeapon::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUIWeapon::Initialize(void* _pArg)
{
    UIWEAPON_DESC* pDesc = static_cast<UIWEAPON_DESC*>(_pArg);

    m_iTextureIndex = pDesc->iTextureIndex;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    m_fOpacity = 1.0f;

    return S_OK;
}

void CUIWeapon::Priority_Update(_float _fTimeDelta)
{
}

void CUIWeapon::Update(_float _fTimeDelta)
{
}

void CUIWeapon::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this);
}

HRESULT CUIWeapon::Render()
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

    if(FAILED(m_pShaderCom->Bind_RawValue("g_Opacity", &m_fOpacity, sizeof(_float))))
        return E_FAIL;

    m_pShaderCom->Begin(4);
    m_pVIBufferCom->Bind_InputAssembler();
    m_pVIBufferCom->Render();

    return S_OK;
}

void CUIWeapon::Set_HighLight(_bool _bHighlight)
{
    m_bHighlighted = _bHighlight;

    if (_bHighlight)
    {
        //m_pTransformCom->Scaling(_float3(120.0f, 60.0f, 1.0f));
        m_pTransformCom->Scaling(_float3(230.0f, 90.0f, 1.0f));
        Set_Opacity(1.0f);
        Set_Visible(true);  
    }
    else
    {
        //m_pTransformCom->Scaling(_float3(80.0f, 30.0f, 1.0f));
        m_pTransformCom->Scaling(_float3(180.0f, 70.0f, 1.0f));
        Set_Opacity(0.5f); 
        Set_Visible(true);  
    }
}

HRESULT CUIWeapon::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WeaponIcon"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}


CUIWeapon* CUIWeapon::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CUIWeapon* pInstance = new CUIWeapon(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CUIWeapon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUIWeapon::Clone(void* _pArg)
{
    CUIWeapon* pInstance = new CUIWeapon(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CUIWeapon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUIWeapon::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}

