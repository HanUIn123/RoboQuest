#include "pch.h"
#include "CrossHair.h"
#include "GameInstance.h"

CCrossHair::CCrossHair(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CUIObject{ _pDevice, _pContext }
{
}

CCrossHair::CCrossHair(const CCrossHair& _Prototype)
    :CUIObject(_Prototype)
{
}

HRESULT CCrossHair::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CCrossHair::Initialize(void* _pArg)
{
    CROSS_HAIR_DESC* pDesc = static_cast<CROSS_HAIR_DESC*>(_pArg);
    pDesc->fSizeX = 50.0f;
    pDesc->fSizeY = 50.0f;
    pDesc->fX = _float(m_pGameInstance->Get_ViewportSize().x / 2);
    pDesc->fY = _float(m_pGameInstance->Get_ViewportSize().y / 2);
    pDesc->fZ = 0.1f;
    pDesc->fNear = 0.0f;
    pDesc->fFar = 1.0f;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    return S_OK;
}

void CCrossHair::Priority_Update(_float _fTimeDelta)
{
}

void CCrossHair::Update(_float _fTimeDelta)
{
}

void CCrossHair::Late_Update(_float _fTimeDelta)
{
    if(m_bIsVisible)
        m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this);
}

HRESULT CCrossHair::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")) ||
        FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)) ||
        FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
    {
        return E_FAIL;
    }

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
        return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_InputAssembler();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CCrossHair::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CrossHair"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_CrossHair"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CCrossHair* CCrossHair::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCrossHair* pInstance = new CCrossHair(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CCrossHair");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CCrossHair::Clone(void* _pArg)
{
    // this : ¿øÇü. 
    CCrossHair* pInstance = new CCrossHair(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CCrossHair");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCrossHair::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
