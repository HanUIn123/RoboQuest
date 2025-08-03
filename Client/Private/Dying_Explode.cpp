#include "pch.h"
#include "Dying_Explode.h"
#include "GameInstance.h"

CDying_Explode::CDying_Explode(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGameObject{ _pDevice, _pContext }
{
}

CDying_Explode::CDying_Explode(const CDying_Explode& _Prototype)
    :CGameObject(_Prototype)
{
}

HRESULT CDying_Explode::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CDying_Explode::Initialize(void* _pArg)
{
    DYING_EXPLO_DESC* pDesc = static_cast<DYING_EXPLO_DESC*>(_pArg);

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CDying_Explode::Priority_Update(_float _fTimeDelta)
{
}

void CDying_Explode::Update(_float _fTimeDelta)
{
    //if (!m_bActivate)
    //    return;

    m_fAnimationTime += _fTimeDelta * 2.0f;
}

void CDying_Explode::Late_Update(_float _fTimeDelta)
{
    if (!m_bActivate)
        return;

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CDying_Explode::Render()
{
    if (!m_bActivate)
        return S_OK;

    if (FAILED(Bind_ShaderResource()))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    m_pShaderCom->Begin(5);

    m_pVIBufferCom->Bind_InputAssembler();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CDying_Explode::Activate_Explode()
{
    m_bActivate = true;
    m_fAnimationTime = 0.0f;
}

void CDying_Explode::DeActivate_Explode()
{
    if (m_bActivate)
        m_bActivate = false;
}

void CDying_Explode::Set_Position(const _vector& _vMissilePos, const _vector& _vMissileDir)
{
    _vector vMortalExplosionPos = _vMissilePos;

    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMortalExplosionPos);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vMortalExplosionPos, 1.0f));

    //m_vSmokeDirection = -_vMissileDir;  // 미사일 진행 반대 방향 저장
}

HRESULT CDying_Explode::Ready_Components()
{
    /* Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DyingExplosion"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    // Main APP 클래스에서 바로 그냥 쉐이더랑, POSTEX 버퍼 (사각형) 미리 등록해놨음. 
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Point_MortalExplosion"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CDying_Explode::Bind_ShaderResource()
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


    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCameraPosition", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_fAnimationTime", &m_fAnimationTime, sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

CDying_Explode* CDying_Explode::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CDying_Explode* pInstance = new CDying_Explode(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CDying_Explode");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CDying_Explode::Clone(void* _pArg)
{
    CDying_Explode* pInstance = new CDying_Explode(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CDying_Explode");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDying_Explode::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
}
