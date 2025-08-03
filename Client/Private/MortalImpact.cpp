#include "pch.h"
#include "MortalImpact.h"
#include "GameInstance.h"

CMortalImpact::CMortalImpact(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGameObject{ _pDevice , _pContext }
{
}

CMortalImpact::CMortalImpact(const CMortalImpact& _Prototype)
    :CGameObject(_Prototype)
{
}

HRESULT CMortalImpact::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMortalImpact::Initialize(void* _pArg)
{
    MORTAL_IMPACT_DESC* pDesc = static_cast<MORTAL_IMPACT_DESC*>(_pArg);

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //m_fResistTime = 5.0f;

    m_vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    return S_OK;
}

void CMortalImpact::Priority_Update(_float _fTimeDelta)
{
}

void CMortalImpact::Update(_float _fTimeDelta)
{
    if (!m_bActive)
        return;

    m_fTimer += _fTimeDelta;

    //// 5초~ 지나면 불길 사라짐.
    //if (m_fResistTime <= 0.0f)
    //{
    //    m_fResistTime = 15.0f;
    //    DeActivate_Impact();
    //}

    if (m_bIncreasing)
        m_fLerpProgress += _fTimeDelta * 2.0f;

    if (m_fLerpProgress >= 1.0f)
    {
        m_bIncreasing = false;
    }

    _float fStartScale = 1.0f;
    _float fEndScale = 4.0f;
    _float fCurrentScale = fStartScale + (fEndScale - fStartScale) * m_fLerpProgress;

    m_pTransformCom->Scaling(_float3(fCurrentScale, 1.0f, fCurrentScale));
}

void CMortalImpact::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CMortalImpact::Render()
{
    if (FAILED(Bind_ShaderResource()))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    m_pShaderCom->Begin(4);
    m_pVIBufferCom->Bind_InputAssembler();
    m_pVIBufferCom->Render();

    return S_OK;
}

void CMortalImpact::Set_Active(_bool _bActive)
{
    if (_bActive)
    {
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPosition);
        m_fLerpProgress = 0.0f;
        m_bIncreasing = true;
    }
    m_bActive = _bActive;
}

void CMortalImpact::DeActivate_Impact()
{
    if (m_bActive)
    {
        m_bActive = false;
        m_fLerpProgress = 1.0f;
    }
}

void CMortalImpact::Set_Position(_vector _vPosition, _vector _vDirection)
{
    m_vPosition = _vPosition;

    _vDirection = XMVector3Normalize(_vDirection);
}

HRESULT CMortalImpact::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MortalImpact"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_PosVerRect"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_Ver"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CMortalImpact::Bind_ShaderResource()
{
    // 월드 행렬 바인딩. 
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    // 뷰 행렬 바인딩. 
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    // 투영 행렬 바인딩. 
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_Timer", &m_fTimer, sizeof(_float))))
        return E_FAIL;

    //if (FAILED(m_pShaderFireFrame->Bind_RawValue("g_vCameraPosition", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
    //    return E_FAIL;
    return S_OK;
}

CMortalImpact* CMortalImpact::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CMortalImpact* pInstance = new CMortalImpact(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CMortalImpact");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CMortalImpact::Clone(void* _pArg)
{
    CMortalImpact* pInstance = new CMortalImpact(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CMortalImpact");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMortalImpact::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
