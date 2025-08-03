#include "pch.h"
#include "Boss_Barrier.h"
#include "GameInstance.h"

CBoss_Barrier::CBoss_Barrier(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGameObject{_pDevice, _pContext}
{
}

CBoss_Barrier::CBoss_Barrier(const CBoss_Barrier& _Prototype)
    :CGameObject(_Prototype)
{
}

HRESULT CBoss_Barrier::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBoss_Barrier::Initialize(void* _pArg)
{
    BARRIER_DESC* pDesc = static_cast<BARRIER_DESC*>(_pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    m_fBarrierDurtaion = 20.0f;

    m_vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    return S_OK;
}

void CBoss_Barrier::Priority_Update(_float _fTimeDelta)
{
}

void CBoss_Barrier::Update(_float _fTimeDelta)
{
    if (!m_bActive)
    {
        return;
    }




    if (m_bAppearingBarrier)
    {
        m_fDissolveTime -= _fTimeDelta;

        m_fDissolveTime = max(0.0f, m_fDissolveTime);

        if (m_fDissolveTime <= 0.0f)
            m_bAppearingBarrier = false;
    }

    if (m_bDissolving)
    {
        m_fDissolveTime += _fTimeDelta * 0.5f;
        m_fDissolveTime = min(1.0f, m_fDissolveTime);

        if (m_fDissolveTime >= 1.0f)
            m_bDissolving = false;
    }

    if (m_fHitEffectValue > 0.0f)
    {
        m_fHitEffectValue -= _fTimeDelta * 1.5f;
        m_fHitEffectValue = max(0.0f, m_fHitEffectValue);
    }



    m_fBarrierDurtaion -= _fTimeDelta;

    if (m_fBarrierDurtaion <= 0.0f)
    {
        m_fBarrierDurtaion = 20.0f;
        DeActivate_Barrier();
    }


    _matrix WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    m_pColliderCom->Update(WorldMatrix);
}

void CBoss_Barrier::Late_Update(_float _fTimeDelta)
{
    if (!m_bActive)
    {
        return;
    }

    m_pTargetCollider = static_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), 0));

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CBoss_Barrier::Render()
{
#ifdef _DEBUG
    //m_pColliderCom->Render();
#endif 

    if (!m_bActive)
    {
        return S_OK;
    }

    if (FAILED(Bind_ShaderResource()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture", 0)))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CBoss_Barrier::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BarrierDissolve"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_BarrierTex"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Barrier"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
    ColliderDesc.vExtents = _float3(2.0f, 4.0f, 2.0f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y - 4.0f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBoss_Barrier::Bind_ShaderResource()
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

    if (FAILED(m_pShaderCom->Bind_RawValue("g_DissolveValue", &m_fDissolveTime, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_HitEffectValue", &m_fHitEffectValue, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture", 0)))
        return E_FAIL;


    return S_OK;
}

void CBoss_Barrier::DeActivate_Barrier()
{
    m_bActive = false;

    _vector vBarrierPos = XMVectorSet(9999.0f, 9999.0f, 9999.0f, 1.0f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vBarrierPos);
}

void CBoss_Barrier::Activate_Barrier(const _vector& _vPosition, const _vector& _vBarrierLook)
{
    if (m_bActive)
        return;

    m_bActive = true;

    m_bAppearingBarrier = true;
    m_fDissolveTime = 1.0f;

    m_vInitialBarrierPos = _vPosition;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPosition);
    m_pTransformCom->LookAt(_vPosition + _vBarrierLook);
}

void CBoss_Barrier::Set_Active(_bool _bActive)
{
    if (_bActive)
    {
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPosition);
        //m_fLerpProgress = 0.0f;
        //m_bIncreasing = true;
    }
    m_bActive = _bActive;
}

void CBoss_Barrier::Set_Position(_vector _vPosition)
{
    m_vPosition = _vPosition;
}

_bool CBoss_Barrier::Check_Collision_With_Ray(const _vector& _vRayStart, const _vector& _vRayDirection, _float& _fDistance)
{
    if (nullptr == m_pColliderCom)
        return false;

    return m_pColliderCom->Intersect_Ray(_vRayStart, _vRayDirection, _fDistance);
}

void CBoss_Barrier::Activate_HittedEffect()
{
    m_fHitEffectValue = 1.0f;
}

CBoss_Barrier* CBoss_Barrier::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBoss_Barrier* pInstance = new CBoss_Barrier(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created: CBoss_Barrier");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBoss_Barrier::Clone(void* _pArg)
{
    CBoss_Barrier* pInstance = new CBoss_Barrier(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Created: CBoss_Barrier");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_Barrier::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pTextureCom);
}
