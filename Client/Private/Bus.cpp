#include "pch.h"
#include "Bus.h"
#include "GameInstance.h"

CBus::CBus(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CUIObject{ _pDevice, _pContext }
{
}

CBus::CBus(const CBus& _Prototype)
    :CUIObject(_Prototype)
{
}

HRESULT CBus::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBus::Initialize(void* _pArg)
{
    BUS_DESC* pDesc = static_cast<BUS_DESC*>(_pArg);

    pDesc->fSizeX = g_iWinSizeX / 3;
    pDesc->fSizeY = g_iWinSizeY/ 3;
    pDesc->fX = g_iWinSizeX * 0.5f - 250;
    pDesc->fY = g_iWinSizeY * 0.5f + 180;
    //pDesc->fNear = 0.1f;
    //pDesc->fFar = 1000.0f;
    pDesc->fNear = 0.0f;
    pDesc->fFar = 1.0f;

    //// GameObject 의 구조체를 상속받는, 즉 background 사본 객체 만들 때, 한꺼번에 초기화를 하게끔.
    pDesc->fSpeedPerSec = 5.0f;
    pDesc->fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    //// 이 BackGround 객체를 만들 때, 그냥 Component도 같이 붙여버리겠다~.
    if (FAILED(Ready_Component()))
        return E_FAIL;

    //_float fRatio = pDesc->fSizeY / pDesc->fSizeX;
    //m_pTransformCom->Scaling(_float3(fCurrentScale, fCurrentScale * fRatio, 1.0f));

    return S_OK;
}

void CBus::Priority_Update(_float _fTimeDelta)
{
}

void CBus::Update(_float _fTimeDelta)
{
    if (m_bIncreasing)
        m_fLerpProgress += _fTimeDelta * 3.0f;
    else
        m_fLerpProgress -= _fTimeDelta * 3.0f;

    if (m_fLerpProgress >= 1.0f)
    {
        m_fLerpProgress = 1.0f;
        m_bIncreasing = false;
    }
    else if (m_fLerpProgress <= 0.0f)
    {
        m_fLerpProgress = 0.0f;
        m_bIncreasing = true;
    }

    _float2 vStartScale = { 426.0f, 260.0f };
    _float2 vEndScale = { 480.0f, 341.0f };  

    _float fCurrentScaleX = vStartScale.x + (vEndScale.x - vStartScale.x) * m_fLerpProgress;
    _float fCurrentScaleY = vStartScale.y + (vEndScale.y - vStartScale.y) * m_fLerpProgress;

    m_pTransformCom->Scaling(_float3(fCurrentScaleX, fCurrentScaleY, 1.0f));



    if (m_pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
    {
        
    }
}

void CBus::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_PRIORITY, this);
}

HRESULT CBus::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    // Shader 컴객체의 Begin 함수로, 0 번 째 pass 호출.
    m_pShaderCom->Begin(9);

    m_pVIBufferCom->Bind_InputAssembler();

    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CBus::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_LOADING, TEXT("Prototype_Component_Texture_Bus"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_LOADING, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_LOADING, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CBus* CBus::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBus* pInstance = new CBus(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CBus");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBus::Clone(void* _pArg)
{
    CBus* pInstance = new CBus(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CBus");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBus::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
