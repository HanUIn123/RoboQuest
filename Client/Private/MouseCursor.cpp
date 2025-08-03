#include "pch.h"
#include "MouseCursor.h"
#include "GameInstance.h"

CMouseCursor::CMouseCursor(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CUIObject{_pDevice, _pContext}
{
}

CMouseCursor::CMouseCursor(const CMouseCursor& _Prototype)
    :CUIObject(_Prototype)
{
}

HRESULT CMouseCursor::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMouseCursor::Initialize(void* _pArg)
{
    CURSOR_DESC* pDesc = static_cast<CURSOR_DESC*>(_pArg);

    pDesc->fSizeX = 25.0f;
    pDesc->fSizeY = 25.0f;
    
    //pDesc->fX = 640.0f;
    //pDesc->fY = 360;

    pDesc->fZ = 0.1f;
    pDesc->fNear = 0.0f;
    pDesc->fFar = 1.0f;



    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    //if (FAILED(Ready_Component()))
    //    return E_FAIL;

    return S_OK;
}

void CMouseCursor::Priority_Update(_float _fTimeDelta)
{
}

void CMouseCursor::Update(_float _fTimeDelta)
{
    POINT myCursor = m_pGameInstance->Get_MousePosition(g_hWnd);

    _uint2  vViewPortSize = m_pGameInstance->Get_ViewportSize();

    _float fScreenWidth = (_float)vViewPortSize.x;    
    _float fScreenHeight = (_float)vViewPortSize.y;

    _float fCursorCenterX = fScreenWidth * 0.5f;
    _float fCursorCenterY = fScreenHeight * 0.5f;

    _float normalizedX = static_cast<float>(myCursor.x) - fCursorCenterX;
    _float normalizedY = fCursorCenterY - static_cast<_float>(myCursor.y); 

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(normalizedX, normalizedY, 0.f, 1.0f));
}

void CMouseCursor::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CMouseCursor::Render()
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

    //ShowCursor(false);

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CMouseCursor::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MouseCursor"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCursorTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect_MouseCursor"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    //if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MouseCursor"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
    //    return E_FAIL;
    //if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCursorTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
    //    return E_FAIL;
    //if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect_MouseCursor"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
    //    return E_FAIL;

    //if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_MouseCursor"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
    //    return E_FAIL;
    //if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Shader_VtxCursorTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
    //    return E_FAIL;
    //if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_VIBuffer_Rect_MouseCursor"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
    //    return E_FAIL;

    return S_OK;
}

CMouseCursor* CMouseCursor::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CMouseCursor* pInstance = new CMouseCursor(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CMouseCursor");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMouseCursor::Clone(void* _pArg)
{
    // this : ¿øÇü. 
    CMouseCursor* pInstance = new CMouseCursor(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CMouseCursor");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMouseCursor::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
