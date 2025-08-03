#include "pch.h"
#include "UILetters.h"

#include "GameInstance.h"

CUILetters::CUILetters(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CUIObject{ _pDevice,_pContext }
{
}

CUILetters::CUILetters(const CUILetters& _Prototype)
    :CUIObject(_Prototype)
{
}

HRESULT CUILetters::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CUILetters::Initialize(void* _pArg)
{
    LETTER_DESC* pDesc = static_cast<LETTER_DESC*>(_pArg);

    //pDesc->fSizeX = 300.0f;
    //pDesc->fSizeY = 120.0f;
    //pDesc->fX = 550.0f;
    //pDesc->fY = 622.0f;
    //pDesc->fZ = 0.1f;
    //pDesc->fNear = 0.0f;
    //pDesc->fFar = 1.0f;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    return S_OK;
}

void CUILetters::Priority_Update(_float _fTimeDelta)
{
}

void CUILetters::Update(_float _fTimeDelta)
{
    if (!m_bVisible) 
        return;

    m_fTime += _fTimeDelta;

    m_fLifeTime -= _fTimeDelta;
    if (m_fLifeTime <= 0.0f)
    {
        Set_Visible(false);
    }
}

void CUILetters::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this);
}

HRESULT CUILetters::Render()
{
    if (!m_bVisible) 
        return S_OK;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")) ||
        FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)) ||
        FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
    {
        return E_FAIL;
    }

    int textureIndex = 0;
    m_pShaderCom->Bind_RawValue("g_TextureIndex", &textureIndex, sizeof(int));

    m_pShaderCom->Bind_RawValue("g_Timer", &m_fTime, sizeof(_float));

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", m_iLetterTextureIndex)))
        return E_FAIL;



    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_InputAssembler();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CUILetters::Set_Visible(_bool _bVisible)
{
     m_bVisible = _bVisible; 

     if (_bVisible)
     {
         m_fLifeTime = 1.0f; 
     }
}

void CUILetters::Set_Position(_vector _vPos)
{
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPos);
}

HRESULT CUILetters::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UILetters"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxUILetters"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_UILetters"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CUILetters* CUILetters::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CUILetters* pInstance = new CUILetters(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CUILetters");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CUILetters::Clone(void* _pArg)
{
    CUILetters* pInstance = new CUILetters(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CUILetters");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CUILetters::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
