#include "pch.h"
#include "../Public/BackGround.h"
#include "GameInstance.h"


CBackGround::CBackGround(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    //: CGameObject{ _pDevice, _pContext }
    : CUIObject{ _pDevice, _pContext }
{
}


CBackGround::CBackGround(const CBackGround& _Prototype)
    //:CGameObject(_Prototype)
    :CUIObject(_Prototype)
{
}

HRESULT CBackGround::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBackGround::Initialize(void* _pArg)
{
    BACKGROUND_DESC* pDesc = static_cast<BACKGROUND_DESC*>(_pArg);

    pDesc->fSizeX = g_iWinSizeX;
    pDesc->fSizeY = g_iWinSizeY;
    pDesc->fX = g_iWinSizeX * 0.5f;
    pDesc->fY = g_iWinSizeY * 0.5f;
    pDesc->fZ = 0.2f;
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

    return S_OK;
}

void CBackGround::Priority_Update(_float _fTimeDelta)
{
    int a = 100;
}

void CBackGround::Update(_float _fTimeDelta)
{
    int a = 100;
}

void CBackGround::Late_Update(_float _fTimeDelta)
{
    // 배경이라는 뜻의 RG_PRIORITY로 Render Group에 추가.
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_PRIORITY, this);

    //if (!ImGui::CollapsingHeader("Camera Speed"))
    //    return;
}

HRESULT CBackGround::Render()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if(FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    // Shader 컴객체의 Begin 함수로, 0 번 째 pass 호출.
    m_pShaderCom->Begin(0);

    m_pVIBufferCom->Bind_InputAssembler();

    m_pVIBufferCom->Render();



    return S_OK;
}

HRESULT CBackGround::Ready_Component()
{
    // 원래는 m_comTexture 라는 매개변수를 만들어서 여기다가 담는 식으로 해도 되는데, 
    // 그렇게 하지 않고 아예 Component의 결과를 받아버리는 , 파라미터를 하나 추가해버려서 처리한다. 
    // 
    // Texture Component
    if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_LOGO, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CBackGround* CBackGround::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBackGround* pInstance = new CBackGround(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CBackGround");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBackGround::Clone(void* _pArg)
{
    // this : 원형. 
    CBackGround* pInstance = new CBackGround(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CBackGround");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBackGround::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
