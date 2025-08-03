#include "pch.h"
#include "Particle_Fire.h"
#include "GameInstance.h"

CParticle_Fire::CParticle_Fire(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CPartObject{ _pDevice, _pContext }
{
}

CParticle_Fire::CParticle_Fire(const CParticle_Fire& _Prototype)
    :CPartObject(_Prototype)
{
}

HRESULT CParticle_Fire::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_Fire::Initialize(void* _pArg)
{
    PARTICLE_FIRE_DESC* pDesc = static_cast<PARTICLE_FIRE_DESC*>(_pArg);

    m_pParentWorldMatrix = pDesc->pParentWorldMatrix;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CParticle_Fire::Priority_Update(_float _fTimeDelta)
{
    if (!m_bActive)
        return;

    // 불꽃 지속 시간 감소
    m_fDuration -= _fTimeDelta;

    if (m_fDuration <= 0.0f)
    {
        Deactivate();
        return;
    }
}

void CParticle_Fire::Update(_float _fTimeDelta)
{
    if (!m_bActive)
        return;

    m_fAnimationTime += _fTimeDelta * 8.0f;

    m_vFirePosition += m_vFireDirection * _fTimeDelta * 100.0f;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vFirePosition);

    _vector vDown = XMVectorSet(0.f, -0.3f, 0.f, 0.f);

    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr())/* * XMLoadFloat4x4(m_pParentWorldMatrix)*/);
    m_pVIBufferCom->FireThrower(_fTimeDelta, m_vFireDirection + vDown);
}

void CParticle_Fire::Late_Update(_float _fTimeDelta)
{
    if (!m_bActive)
        return;
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONLIGHT, this);
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_GLOW, this);
}

HRESULT CParticle_Fire::Render()
{
    if (!m_bActive)
        return S_OK;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    //if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 1)))
    //    return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCameraPosition", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDuration", &m_fDuration, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAnimationTime", &m_fAnimationTime, sizeof(_float))))
        return E_FAIL;

    m_pShaderCom->Begin(1);
    m_pVIBufferCom->Bind_InputAssembler();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CParticle_Fire::Render_Glow()
{
    if (!m_bActive)
        return S_OK;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    //if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_NoiseTexture", 1)))
    //    return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCameraPosition", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fDuration", &m_fDuration, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAnimationTime", &m_fAnimationTime, sizeof(_float))))
        return E_FAIL;

    //_float RimPower = 2.2f; 
    //_float4 RimColor = _float4(1.0f, 0.4f, 0.1f, 1.0f);

    //if (FAILED(m_pShaderCom->Bind_RawValue("g_RimPower", &RimPower, sizeof(float))))
    //    return E_FAIL;

    //if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &RimColor, sizeof(_float4))))
    //    return E_FAIL;


    m_pShaderCom->Begin(2);
    m_pVIBufferCom->Bind_InputAssembler();
    m_pVIBufferCom->Render();

    return S_OK;
}

void CParticle_Fire::Activate(const _vector& _vPosition, const _vector& _vDirection)
{
    m_bActive = true;
    m_fDuration = 3.0f; // 불꽃 지속 시간 (3초)
    m_vFirePosition = _vPosition;
    m_vFireDirection = _vDirection;
}

void CParticle_Fire::Deactivate()
{
    m_bActive = false;
    m_pVIBufferCom->FireThorwer_Reset();
}

HRESULT CParticle_Fire::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Fire"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

 
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance_Fire"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Point_Instancing_Fire"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CParticle_Fire* CParticle_Fire::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CParticle_Fire* pInstance = new CParticle_Fire(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CParticle_Fire");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_Fire::Clone(void* _pArg)
{
    CParticle_Fire* pInstance = new CParticle_Fire(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CParticle_Fire");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_Fire::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
