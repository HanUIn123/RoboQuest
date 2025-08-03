#include "pch.h"
#include "BossHpBarBackGround.h"
#include "GameInstance.h"

CBossHpBarBackGround::CBossHpBarBackGround(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CUIObject{_pDevice, _pContext}
{
}

CBossHpBarBackGround::CBossHpBarBackGround(const CBossHpBarBackGround& _Prototype)
    :CUIObject(_Prototype)
{
}

HRESULT CBossHpBarBackGround::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBossHpBarBackGround::Initialize(void* _pArg)
{
    BOSS_HPBARCGROUND_DESC* pDesc = static_cast<BOSS_HPBARCGROUND_DESC*>(_pArg);

    pDesc->fSizeX = 900.0f;
    pDesc->fSizeY = 100.0f;
    pDesc->fX = 650.0f;
    pDesc->fY = 50.0f;
    pDesc->fZ = 0.01f;
    pDesc->fNear = 0.0f;
    pDesc->fFar = 1.0f;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    return S_OK;
}

void CBossHpBarBackGround::Priority_Update(_float _fTimeDelta)
{
}

void CBossHpBarBackGround::Update(_float _fTimeDelta)
{
}

void CBossHpBarBackGround::Late_Update(_float _fTimeDelta)
{
    if(m_bIsVisible)
        m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this);
}

HRESULT CBossHpBarBackGround::Render()
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

HRESULT CBossHpBarBackGround::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossBarHpBackGround"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_BossBarHpShader"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_BossHpBackGround"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

void CBossHpBarBackGround::Activate_HpBarBackGround()
{
    m_bIsVisible = true;
}

CBossHpBarBackGround* CBossHpBarBackGround::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBossHpBarBackGround* pInstance = new CBossHpBarBackGround(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CBossHpBarBackGround");
        Safe_Release(pInstance);
    }

    return pInstance;;
}

CGameObject* CBossHpBarBackGround::Clone(void* _pArg)
{
    CBossHpBarBackGround* pInstance = new CBossHpBarBackGround(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CBossHpBarBackGround");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBossHpBarBackGround::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
