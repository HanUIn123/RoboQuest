#include "pch.h"
#include "Particle_MuzzleFire.h"
#include "GameInstance.h"

CParticle_MuzzleFire::CParticle_MuzzleFire(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CPartObject{ _pDevice, _pContext }
{
}

CParticle_MuzzleFire::CParticle_MuzzleFire(const CParticle_MuzzleFire& _Prototype)
    :CPartObject(_Prototype)
{
}

HRESULT CParticle_MuzzleFire::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CParticle_MuzzleFire::Initialize(void* _pArg)
{
    PARTICLE_MUZZLE_FIRE_DESC* pDesc = static_cast<PARTICLE_MUZZLE_FIRE_DESC*>(_pArg);

    m_pParentWorldMatrix = pDesc->pParentWorldMatrix;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CParticle_MuzzleFire::Priority_Update(_float _fTimeDelta)
{
    if (!m_bActivate)
        return;

    m_fAnimationTime += _fTimeDelta;

    m_fEffectDuration -= _fTimeDelta;

    if (m_fEffectDuration <= 0.0f)
    {
        m_bActivate = false;
        return;
    }
}

void CParticle_MuzzleFire::Update(_float _fTimeDelta)
{

  
}

void CParticle_MuzzleFire::Late_Update(_float _fTimeDelta)
{
    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentWorldMatrix));
    m_pVIBufferCom->Explosion(_fTimeDelta);

    if(m_bActivate)
        m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONLIGHT, this);
}

HRESULT CParticle_MuzzleFire::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCameraPosition", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAnimationTime", &m_fAnimationTime, sizeof(_float))))
        return E_FAIL;


    m_pShaderCom->Begin(4);

    m_pVIBufferCom->Bind_InputAssembler();
 
    m_pVIBufferCom->Render();

    return S_OK;
}


void CParticle_MuzzleFire::Set_FireParticle_To_Muzzle(const _vector& _vGunMuzzlePos)
{
    //_vector vOffset = XMVectorSet(0.49f, 0.01f, 0.4f, 0.0f);
    //_vector vParticlePos = _vGunMuzzlePos + vOffset;
    //_vector vParticlePos = _vGunMuzzlePos;
    
    m_bActivate = true;
    m_fEffectDuration = 0.08f;
    
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vGunMuzzlePos);
}

HRESULT CParticle_MuzzleFire::Ready_Components()
{
    /* Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MuzzleFlash"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Point_Instancing"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CParticle_MuzzleFire* CParticle_MuzzleFire::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CParticle_MuzzleFire* pInstance = new CParticle_MuzzleFire(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CParticle_MuzzleFire");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CParticle_MuzzleFire::Clone(void* _pArg)
{
    CParticle_MuzzleFire* pInstance = new CParticle_MuzzleFire(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CParticle_MuzzleFire");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CParticle_MuzzleFire::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
