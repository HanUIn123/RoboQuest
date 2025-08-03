#include "pch.h"
#include "HpBar_Robo_Boom.h"
#include "GameInstance.h"

CHpBar_Robo_Boom::CHpBar_Robo_Boom(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CMon_HpBar{ _pDevice, _pContext }
{
}

CHpBar_Robo_Boom::CHpBar_Robo_Boom(const CHpBar_Robo_Boom& _Prototype)
    :CMon_HpBar(_Prototype)
{
}

HRESULT CHpBar_Robo_Boom::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CHpBar_Robo_Boom::Initialize(void* _pArg)
{
    _float fOffset = (g_iWinSizeY * 0.5f);

    CMon_HpBar::MONSTER_HPBAR_DESC* pDesc = static_cast<CMon_HpBar::MONSTER_HPBAR_DESC*>(_pArg);
    pDesc->fSizeX = 2.0f;
    pDesc->fSizeY = 0.2f;
    pDesc->fX = g_iWinSizeX * 0.5f;

    // ��������,. ��������. 
    //pDesc->fY = fOffset - (fOffset * 0.004f);
    pDesc->fZ = 0.2f;
    pDesc->fNear = 0.0f;
    pDesc->fFar = 1.0f;

    //pDesc->pMonsterHp = 200;

    //m_fCurrentHp = *pDesc->pMonsterHp;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CHpBar_Robo_Boom::Priority_Update(_float _fTimeDelta)
{


}

//void CHpBar_Robo_Boom::Update(_float _fTimeDelta)
//{
//
//  /*  if (m_fBackgroundHp > m_fCurrentHp)
//    {
//        _float fDifference = m_fBackgroundHp - m_fCurrentHp;
//        m_fBackgroundHp -= min(fDifference, _fTimeDelta * 0.5f); 
//    }*/
//}

void CHpBar_Robo_Boom::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT CHpBar_Robo_Boom::Render()
{
    if (!m_bShowHealthBar)
        return S_OK;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;


    // ü�� ���� ���̴��� ����
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

//void CHpBar_Robo_Boom::Decrease_Hp(_uint _iDamageValue)
//{
//    _float3 vScale = m_pTransformCom->Compute_Scaled();
//    _float fDecreaseRatio = (vScale.x / (_float)_iDamageValue);
//
//    vScale.x = max(0.0f, vScale.x - fDecreaseRatio);
//
//    m_pTransformCom->Scaling(vScale);
//
//    _vector vHpBarPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
//    vHpBarPos = XMVectorSetX(vHpBarPos, XMVectorGetX(vHpBarPos) - (fDecreaseRatio * 0.5f));
//    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vHpBarPos);
//}

HRESULT CHpBar_Robo_Boom::Ready_Components()
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


CHpBar_Robo_Boom* CHpBar_Robo_Boom::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CHpBar_Robo_Boom* pInstance = new CHpBar_Robo_Boom(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CHpBar_Robo_Boom");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CHpBar_Robo_Boom::Clone(void* _pArg)
{
    CHpBar_Robo_Boom* pInstance = new CHpBar_Robo_Boom(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CHpBar_Robo_Boom");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHpBar_Robo_Boom::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
