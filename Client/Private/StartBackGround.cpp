#include "pch.h"
#include "StartBackGround.h"
#include "GameInstance.h"


CStartBackGround::CStartBackGround(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CUIObject{_pDevice, _pContext}
{
}

CStartBackGround::CStartBackGround(const CStartBackGround& _Prototype)
    :CUIObject(_Prototype)
{
}

HRESULT CStartBackGround::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CStartBackGround::Initialize(void* _pArg)
{
    STBACKGROUND_DESC* pDesc = static_cast<STBACKGROUND_DESC*>(_pArg);

    pDesc->fSizeX = g_iWinSizeX;
    pDesc->fSizeY = g_iWinSizeY;
    pDesc->fX = g_iWinSizeX * 0.5f;
    pDesc->fY = g_iWinSizeY * 0.5f;
    //pDesc->fNear = 0.1f;
    //pDesc->fFar = 1000.0f;
    pDesc->fNear = 0.0f;
    pDesc->fFar = 0.98f;

    //// GameObject 의 구조체를 상속받는, 즉 background 사본 객체 만들 때, 한꺼번에 초기화를 하게끔.
    pDesc->fSpeedPerSec = 5.0f;
    pDesc->fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    //// 이 BackGround 객체를 만들 때, 그냥 Component도 같이 붙여버리겠다~.
    if (FAILED(Ready_Component()))
        return E_FAIL;

    return S_OK;
}

void CStartBackGround::Priority_Update(_float _fTimeDelta)
{
}

void CStartBackGround::Update(_float _fTimeDelta)
{
    m_fTimer += _fTimeDelta;
}

void CStartBackGround::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_PRIORITY, this);
}

HRESULT CStartBackGround::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_Timer", &m_fTimer, sizeof(_float))))
        return E_FAIL;

    // Shader 컴객체의 Begin 함수로, 0 번 째 pass 호출.
    m_pShaderCom->Begin(10);

    m_pVIBufferCom->Bind_InputAssembler();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CStartBackGround::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_LOADING, TEXT("Prototype_Component_Texture_BackGround"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_LOADING, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_LOADING, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CStartBackGround* CStartBackGround::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CStartBackGround* pInstance = new CStartBackGround(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CStartBackGround");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CStartBackGround::Clone(void* _pArg)
{
    CStartBackGround* pInstance = new CStartBackGround(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CStartBackGround");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CStartBackGround::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
