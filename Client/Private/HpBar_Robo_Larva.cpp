#include "pch.h"
#include "HpBar_Robo_Larva.h"
#include "GameInstance.h"

CHpBar_Robo_Larva::CHpBar_Robo_Larva(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CMon_HpBar{ _pDevice, _pContext }
{
}

CHpBar_Robo_Larva::CHpBar_Robo_Larva(const CHpBar_Robo_Larva& _Prototype)
    :CMon_HpBar(_Prototype)
{
}

HRESULT CHpBar_Robo_Larva::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CHpBar_Robo_Larva::Initialize(void* _pArg)
{
    _float fOffset = (g_iWinSizeY * 0.5f);

    CMon_HpBar::MONSTER_HPBAR_DESC* pDesc = static_cast<CMon_HpBar::MONSTER_HPBAR_DESC*>(_pArg);
    pDesc->fSizeX = 2.0f;
    pDesc->fSizeY = 0.2f;
    pDesc->fX = g_iWinSizeX * 0.5f;

    // 내릴수록,. 내려간다. 
    //pDesc->fY = fOffset - (fOffset * 0.0065f);
    pDesc->fZ = 0.2f;
    pDesc->fNear = 0.0f;
    pDesc->fFar = 1.0f;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CHpBar_Robo_Larva::Priority_Update(_float _fTimeDelta)
{
}

void CHpBar_Robo_Larva::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT CHpBar_Robo_Larva::Render()
{
    if (!m_bShowHealthBar)
        return S_OK;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;


    // 체력 값을 쉐이더에 전달
    if (FAILED(m_pShaderCom->Bind_RawValue("g_CurrentHp", &m_fCurrentHp, sizeof(_float))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_RawValue("g_BackgroundHp", &m_fBackgroundHp, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_MaxHp", &m_fMaxHp, sizeof(_float))))
        return E_FAIL;

    int textureIndex = 0;
    m_pShaderCom->Bind_RawValue("g_TextureIndex", &textureIndex, sizeof(int));

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    //if (FAILED(m_pBackgroundTextureCom->Bind_ShaderResource(m_pShaderCom, "g_BackgroundTexture", 1)))
    //    return E_FAIL;

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_InputAssembler();

    m_pVIBufferCom->Render();

    return S_OK;  
}

HRESULT CHpBar_Robo_Larva::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RoboBoomHpBar"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    //if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RoboBoomHpBar_Background"), TEXT("Com_Texture_Background"), reinterpret_cast<CComponent**>(&m_pBackgroundTextureCom))))
    //    return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxBoomHpBarTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_RoboBoomHpBar"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CHpBar_Robo_Larva* CHpBar_Robo_Larva::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CHpBar_Robo_Larva* pInstance = new CHpBar_Robo_Larva(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CHpBar_Robo_Larva");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CHpBar_Robo_Larva::Clone(void* _pArg)
{
    CHpBar_Robo_Larva* pInstance = new CHpBar_Robo_Larva(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CHpBar_Robo_Larva");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHpBar_Robo_Larva::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
