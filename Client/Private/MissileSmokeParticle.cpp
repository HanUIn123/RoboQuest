#include "pch.h"
#include "MissileSmokeParticle.h"
#include "GameInstance.h"

CMissileSmokeParticle::CMissileSmokeParticle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGameObject{ _pDevice ,_pContext}
{
}

CMissileSmokeParticle::CMissileSmokeParticle(const CMissileSmokeParticle& _Prototype)
    :CGameObject(_Prototype)
{
}

HRESULT CMissileSmokeParticle::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMissileSmokeParticle::Initialize(void* _pArg)
{
    SMOKE_PARTICLE_DESC			Desc{};

    Desc.fSpeedPerSec = 0.f;
    Desc.fRotationPerSec = XMConvertToRadians(0.f);

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    return S_OK;
}

void CMissileSmokeParticle::Priority_Update(_float _fTimeDelta)
{
}

void CMissileSmokeParticle::Update(_float _fTimeDelta)
{
    if (!m_bActive)
        return;

    //m_pVIBufferCom->Drop(fTimeDelta);
    m_fAnimationTime += _fTimeDelta * 8.0f;

    //m_pVIBufferCom->MissileSmoke(_fTimeDelta);
    //m_pVIBufferCom->Explosion(_fTimeDelta);
    //m_pVIBufferCom->Rise(_fTimeDelta);

      // 미사일 진행 반대 방향 설정
    //_vector vSmokeDirection = -m_pTransformCom->Get_State(CTransform::STATE_LOOK);

    _vector vSmokeDirection = m_vSmokeDirection;  // 저장된 연기 방향 사용

    // 연기 이동 함수 호출 (미사일 진행 반대 방향)
    m_pVIBufferCom->Following_Smoke(_fTimeDelta, vSmokeDirection);
}

void CMissileSmokeParticle::Late_Update(_float _fTimeDelta)
{
    if (!m_bActive)
        return;

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONLIGHT, this);
}

HRESULT CMissileSmokeParticle::Render()
{
    if (!m_bActive)
        return S_OK;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
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

    m_pShaderCom->Begin(1);

    m_pVIBufferCom->Bind_InputAssembler();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CMissileSmokeParticle::Set_Position(const _vector& _vMissilePos, const _vector& _vMissileDir)
{ 
    _vector vSmokePos = _vMissilePos - (_vMissileDir * 2.0f);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vSmokePos);

    m_vSmokeDirection = -_vMissileDir;  // 미사일 진행 반대 방향 저장
}

void CMissileSmokeParticle::Activate_Smoke()
{
    m_bActive = true;    // 스모크 활성화
    m_fAnimationTime = 0.0f; // 애니메이션 시간 초기화

    // 크기 랜덤 설정 (조금씩 다른 크기의 스모크 표현)
    _float fRandomScale = 0.8f + ((rand() % 5) * 0.1f); // 0.8 ~ 1.2 범위
    m_pTransformCom->Scaling(_float3(fRandomScale, fRandomScale, fRandomScale));

    //// 랜덤 회전 (연기가 자연스럽게 퍼지도록)
    //_float fRandomRotation = XMConvertToRadians(rand() % 360);
    //m_pTransformCom->RotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRandomRotation);
}

void CMissileSmokeParticle::DeActivate_Smoke()
{
    if (m_bActive)
        m_bActive = false;
}

HRESULT CMissileSmokeParticle::Ready_Components()
{
    /* Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Smoke"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_SmokeEffect"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Smoke"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CMissileSmokeParticle* CMissileSmokeParticle::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CMissileSmokeParticle* pInstance = new CMissileSmokeParticle(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CMissileSmokeParticle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMissileSmokeParticle::Clone(void* pArg)
{
    CMissileSmokeParticle* pInstance = new CMissileSmokeParticle(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed To Cloned : CMissileSmokeParticle");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMissileSmokeParticle::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
}
