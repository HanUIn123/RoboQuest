#include "pch.h"
#include "PlayerHpbar.h"

#include "GameInstance.h"

CPlayerHpbar::CPlayerHpbar(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CUIObject{ _pDevice,_pContext }
{
}

CPlayerHpbar::CPlayerHpbar(const CPlayerHpbar& _Prototype)
    :CUIObject(_Prototype)
{
}

HRESULT CPlayerHpbar::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayerHpbar::Initialize(void* _pArg)
{
    HP_BAR* pDesc = static_cast<HP_BAR*>(_pArg);

    pDesc->fSizeX = 300.0f;
    pDesc->fSizeY = 20.0f;
    pDesc->fX = 250.0f;
    pDesc->fY = 622.0f;
    pDesc->fZ = 0.1f;
    pDesc->fNear = 0.0f;
    pDesc->fFar = 1.0f;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    return S_OK;
}

void CPlayerHpbar::Priority_Update(_float _fTimeDelta)
{
}

void CPlayerHpbar::Update(_float _fTimeDelta)
{
}

void CPlayerHpbar::Late_Update(_float _fTimeDelta)
{
    if (m_bIsVisible)
        m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this);
}

HRESULT CPlayerHpbar::Render()
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

    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_InputAssembler();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CPlayerHpbar::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PlayerHpBar"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPlayerHpBarTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_HpBackBar"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

void CPlayerHpbar::Decrease_Hp(_uint _iDamage, _uint _iMaxHp)
{
    _float3 vScale = m_pTransformCom->Compute_Scaled();

    const _float fDecreaseRatio = (300.0f / _iMaxHp) * _iDamage;

    vScale.x = max(0.0f, vScale.x - fDecreaseRatio);
    m_pTransformCom->Scaling(vScale);

    _vector vHpBarPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    vHpBarPos = XMVectorSetX(vHpBarPos, XMVectorGetX(vHpBarPos) - (fDecreaseRatio * 0.5f));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vHpBarPos);
}

void CPlayerHpbar::Increase_Hp(_uint _iHeal, _uint _iMaxHp)
{
    _float3 vScale = m_pTransformCom->Compute_Scaled();
    const _float fIncreaseRatio = (300.0f / _iMaxHp) * _iHeal;

    _float fOldScaleX = vScale.x;

    vScale.x = min(300.0f, vScale.x + fIncreaseRatio);
    m_pTransformCom->Scaling(vScale);

    if (fOldScaleX < 300.0f)
    {
        _vector vHpBarPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        _float fPosX = XMVectorGetX(vHpBarPos);

        if (fPosX <= vScale.x && vScale.x < 300.0f)
        {
            vHpBarPos = XMVectorSetX(vHpBarPos, fPosX + (fIncreaseRatio * 0.5f));
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, vHpBarPos);
        }
        else
            return;
    }
}

CPlayerHpbar* CPlayerHpbar::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPlayerHpbar* pInstance = new CPlayerHpbar(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CPlayerHpbar");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayerHpbar::Clone(void* _pArg)
{
    CPlayerHpbar* pInstance = new CPlayerHpbar(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CPlayerHpbar");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayerHpbar::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
