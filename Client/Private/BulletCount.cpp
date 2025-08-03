#include "pch.h"
#include "BulletCount.h"
#include "GameInstance.h"

//CBulletCount::CBulletCount(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
//    :CUIObject{ _pDevice, _pContext }
//{
//}
//
//CBulletCount::CBulletCount(const CBulletCount& _Prototype)
//    :CUIObject(_Prototype)
//{
//}
//
//HRESULT CBulletCount::Initialize_Prototype()
//{
//    if (FAILED(__super::Initialize_Prototype()))
//        return E_FAIL;
//
//    return S_OK;
//}
//
//HRESULT CBulletCount::Initialize(void* _pArg)
//{
//    BULLET_COUNT_DESC* pDesc = static_cast<BULLET_COUNT_DESC*>(_pArg);
//    pDesc->fSizeX = 100.0f;
//    pDesc->fSizeY = 100.0f;
//    pDesc->fX = _float(m_pGameInstance->Get_ViewportSize().x / 2 + 420);
//    pDesc->fY = _float(m_pGameInstance->Get_ViewportSize().y / 2 + 250);
//    //
//    //m_iNumHeight = (_float)(pDesc->fY);
//    pDesc->fZ = 0.1f;
//    pDesc->fNear = 0.0f;
//    pDesc->fFar = 1.0f;
//    //
//    if (FAILED(__super::Initialize(_pArg)))
//        return E_FAIL;
//    
//    if (FAILED(Ready_Component()))
//        return E_FAIL;
//    
//    m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, TEXT("Layer_Player")));
//
//    _float fGap = 30.0f;
//
//    for (int i = 0; i < 3; ++i)
//    {
//        m_vDigitPosition[i] = XMVectorSet(pDesc->fX + i * fGap, pDesc->fY, 0.1f, 1.0f);
//    }
//
//    return S_OK;
//}
//
//
//void CBulletCount::Priority_Update(_float _fTimeDelta)
//{
//
//}
//
//void CBulletCount::Update(_float _fTimeDelta)
//{
//    m_iHunDigit = m_pPlayer->Get_AmmoCount() / 100;
//
//    m_iTenDigit = m_pPlayer->Get_AmmoCount() / 10;
//
//    if (m_iTenDigit >= 10)
//        m_iTenDigit = m_iTenDigit % 10;
//
//    m_iOneDigit = m_pPlayer->Get_AmmoCount() % 10;
//
// 
//}
//
//void CBulletCount::Late_Update(_float _fTimeDelta)
//{
//    m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this);
//}
//
//HRESULT CBulletCount::Render()
//{
//    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")) ||
//        FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)) ||
//        FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
//    {
//        return E_FAIL;
//    }
//
//    _int digits[3] = { m_iHunDigit, m_iTenDigit, m_iOneDigit };
//    for (_int i = 0; i < 3; ++i)
//    {
//        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", digits[i])))
//            return E_FAIL;
//        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vDigitPosition[i]);
//
//        if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
//            return E_FAIL;
//
//        m_pShaderCom->Begin(2);
//        m_pVIBufferCom->Bind_InputAssembler();
//        m_pVIBufferCom->Render();
//    }
//
//    return S_OK;
//}
//
//HRESULT CBulletCount::Ready_Component()
//{
//    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BulletCount"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
//        return E_FAIL;
//    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
//        return E_FAIL;
//    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_BulletCount"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
//        return E_FAIL;
//
//    return S_OK;
//}
//
//CBulletCount* CBulletCount::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
//{
//    CBulletCount* pInstance = new CBulletCount(_pDevice, _pContext);
//
//    if (FAILED(pInstance->Initialize_Prototype()))
//    {
//        MSG_BOX("Failed To Created : CBulletCount");
//        Safe_Release(pInstance);
//    }
//
//    return pInstance;
//}
//
//CGameObject* CBulletCount::Clone(void* _pArg)
//{
//    // this : ¿øÇü. 
//    CBulletCount* pInstance = new CBulletCount(*this);
//
//    if (FAILED(pInstance->Initialize(_pArg)))
//    {
//        MSG_BOX("Failed To Cloned : CBulletCount");
//        Safe_Release(pInstance);
//    }
//
//    return pInstance;
//}
//
//void CBulletCount::Free()
//{
//    __super::Free();
//
//    Safe_Release(m_pTextureCom);
//    Safe_Release(m_pShaderCom);
//    Safe_Release(m_pVIBufferCom);
//}
