#include "pch.h"
#include "BossHpBar.h"

#include "GameInstance.h"

CBossHpBar::CBossHpBar(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CUIObject{_pDevice, _pContext}
{
}

CBossHpBar::CBossHpBar(const CBossHpBar& _Prototype)
    :CUIObject(_Prototype)
{
}

HRESULT CBossHpBar::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBossHpBar::Initialize(void* _pArg)
{
    BOSS_HPBAR_DESC* pDesc = static_cast<BOSS_HPBAR_DESC*>(_pArg);

    pDesc->fSizeX = 680.0f;
    m_fHpBarWidth = pDesc->fSizeX;
    pDesc->fSizeY = 35.0f;
    pDesc->fX = 700.0f;
    pDesc->fY = 70.0f;
    pDesc->fZ = 0.001f;
    pDesc->fNear = 0.0f;
    pDesc->fFar = 1.0f;

    m_fMaxHp = 100.0f; 
    m_fCurrentHp = m_fMaxHp; 

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    return S_OK;
}

void CBossHpBar::Priority_Update(_float _fTimeDelta)
{
}

void CBossHpBar::Update(_float _fTimeDelta)
{
}

void CBossHpBar::Late_Update(_float _fTimeDelta)
{
    if(m_bIsVisible)
        m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this);
}

HRESULT CBossHpBar::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")) ||
        FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)) ||
        FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
    {
        return E_FAIL;
    }

    int textureIndex = 0;
    m_pShaderCom->Bind_RawValue("g_TextureIndex", &textureIndex, sizeof(int));
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    m_pShaderCom->Begin(1);

    m_pVIBufferCom->Bind_InputAssembler();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CBossHpBar::Activate_HpBar()
{
    m_bIsVisible = true;
}

HRESULT CBossHpBar::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossHpBar"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_BossHpBar"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_BossHpBar"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

void CBossHpBar::Decrease_Hp(_uint _iDamageValue)
{
    m_fCurrentHp = max(0.0f, m_fCurrentHp - _iDamageValue);

    _float fDecreaseRatio = (m_fHpBarWidth / m_fMaxHp) * _iDamageValue;

    _float3 vScale = m_pTransformCom->Compute_Scaled();

    vScale.x = max(0.0f, vScale.x - fDecreaseRatio);
    m_pTransformCom->Scaling(vScale);

    _vector vHpBarPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    vHpBarPos = XMVectorSetX(vHpBarPos, XMVectorGetX(vHpBarPos) - (fDecreaseRatio * 0.5f));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vHpBarPos);
}

CBossHpBar* CBossHpBar::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBossHpBar* pInstance = new CBossHpBar(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CBossHpBar");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBossHpBar::Clone(void* _pArg)
{
    CBossHpBar* pInstance = new CBossHpBar(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CBossHpBar");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBossHpBar::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
