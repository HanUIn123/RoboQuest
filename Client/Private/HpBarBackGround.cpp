#include "pch.h"
#include "../Public/HpBarBackGround.h"
#include "GameInstance.h"

CHpBarBackGround::CHpBarBackGround(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CUIObject{_pDevice, _pContext }
{
}

CHpBarBackGround::CHpBarBackGround(const CHpBarBackGround& _Prototype)
    :CUIObject(_Prototype)
{
}

HRESULT CHpBarBackGround::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CHpBarBackGround::Initialize(void* _pArg)
{
    HPBACK_DESC* pDesc = static_cast<HPBACK_DESC*>(_pArg);

    pDesc->fSizeX = 360.0f;
    pDesc->fSizeY = 120.0f;
    pDesc->fX = 250.0f;
    pDesc->fY = 600;
    pDesc->fZ = 0.2f;
    pDesc->fNear = 0.0f;
    pDesc->fFar = 1.0f;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    return S_OK;
}

void CHpBarBackGround::Priority_Update(_float _fTimeDelta)
{
}

void CHpBarBackGround::Update(_float _fTimeDelta)
{
}

void CHpBarBackGround::Late_Update(_float _fTimeDelta)
{
    if(m_bIsVisible)
        m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this);
}

HRESULT CHpBarBackGround::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")) ||
        FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)) ||
        FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
    {
        return E_FAIL;
    }

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_InputAssembler();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CHpBarBackGround::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HpBarBackGround"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_HpBackGround"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CHpBarBackGround* CHpBarBackGround::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CHpBarBackGround* pInstance = new CHpBarBackGround(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CHpBarBackGround");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHpBarBackGround::Clone(void* _pArg)
{
    // this : ¿øÇü. 
    CHpBarBackGround* pInstance = new CHpBarBackGround(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CHpBarBackGround");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHpBarBackGround::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
