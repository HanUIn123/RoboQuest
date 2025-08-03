#include "pch.h"
#include "Missile_Mortal.h"
#include "GameInstance.h"
//#include "Player.h"

CMissile_Mortal::CMissile_Mortal(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CMissile{ _pDevice, _pContext }
{
}

CMissile_Mortal::CMissile_Mortal(const CMissile_Mortal& _Prototype)
    :CMissile(_Prototype)
{
}

HRESULT CMissile_Mortal::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMissile_Mortal::Initialize(void* _pArg)
{
    MISSILE_MORTAL_DESC* pDesc = static_cast<MISSILE_MORTAL_DESC*>(_pArg);

    pDesc->fSpeedPerSec = 10.f;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    m_pTransformCom->Scaling(_float3(0.0001f, 0.0001f, 0.0001f));

    m_vInitialMissilePos = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

    //m_pCurPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vInitialMissilePos);

    CDying_Explode::DYING_EXPLO_DESC mortaldesc = {};
    m_pMortalExplosion = dynamic_cast<CDying_Explode*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Dying_Explosion"), &mortaldesc));

    m_vecMortalExplosion.push_back(m_pMortalExplosion);


    return S_OK;
}

void CMissile_Mortal::Priority_Update(_float _fTimeDelta)
{
}

void CMissile_Mortal::Update(_float _fTimeDelta, _vector _vTargetPosition)
{
    if (!m_bActive)
    {
        return;
    }

    _vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _vector vMissileDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

    if (!m_bFalling && (XMVectorGetY(vCurrentPos) >= 10.0f))
    {
        m_bFalling = true;
    }

    if (m_bFalling)
    {
        Drop_Missile(_fTimeDelta/*, _vTargetPosition*/);
    }
    else
    {
        Missile_Fly(_fTimeDelta);

        static const _float fMaxDistance = 10.0f;
        _vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        _float fDistance = XMVectorGetX(XMVector3Length(vCurrentPos - m_vInitialMissilePos));

        if (fDistance > fMaxDistance)
        {
            DeActivate_Missile();
            return;
        }
    }

    for (auto& pImpact : m_vecMortalImpact)
    {
        if (pImpact && pImpact->IsActive())
        {
            pImpact->Update(_fTimeDelta);
        }
    }

    for (auto& pExplosion : m_vecMortalExplosion)
    {
        if (pExplosion->IsActive())
        {
            pExplosion->Update(_fTimeDelta);
        }
    }


    _matrix WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    m_pColliderCom->Update(WorldMatrix);

    Check_Collision_With_Player();
}

void CMissile_Mortal::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);

    m_pTargetCollider = static_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), 0));

    for (auto& pImpact : m_vecMortalImpact)
    {
        if (pImpact && pImpact->IsActive())
        {
            pImpact->Late_Update(_fTimeDelta);
        }
    }


    for (auto& pExplosion : m_vecMortalExplosion)
    {
        if (pExplosion->IsActive())
        {
            pExplosion->Late_Update(_fTimeDelta);
        }
    }
}

HRESULT CMissile_Mortal::Render()
{
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

HRESULT CMissile_Mortal::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Missile_Mortal"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
    ColliderDesc.vExtents = _float3(2.2f, 2.2f, 2.2f);
    ColliderDesc.vCenter = _float3(0.f, 0.0f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

#pragma region MORTAL_IMPACT
    // FIRE_ROAD 를 준비해놓자
    //CMortalImpact::MORTAL_IMPACT_DESC MortalDesc= {};
    for (_uint i = 0; i < 4; ++i)
    {
        CMortalImpact::MORTAL_IMPACT_DESC MortalDesc = {};
        CMortalImpact* pMortalImpact = dynamic_cast<CMortalImpact*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_MortalImpact"), &MortalDesc));

        if (!pMortalImpact)
            return E_FAIL;

        m_vecMortalImpact.push_back(pMortalImpact);

        //m_pMortalImpact = pMortalImpact;
    }

#pragma endregion


    return S_OK;
}

HRESULT CMissile_Mortal::Bind_ShaderResource()
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

    return S_OK;
}

void CMissile_Mortal::Activate_MortalMissile(const _vector& _vPosition, const _vector& _vGunMuzzleLook, const _vector& _vTargetPosition)
{
    __super::Activate_Missile();

    m_vInitialMissilePos = _vPosition;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPosition);
    m_pTransformCom->LookAt(_vPosition + _vGunMuzzleLook);

    //_float fPosY =  XMVectorGetY(m_vInitialMissilePos);

    m_vTargetPos = _vTargetPosition;


    for (auto& pExplosion : m_vecMortalExplosion)
    {
        if (!pExplosion->IsActive())
        {
            pExplosion->Activate_Explode();
        }
    }


}

void CMissile_Mortal::DeActivate_Missile()
{
    m_bActive = false;
    m_bFalling = false;
    m_bImpactShow = false;
    _vector vMissilePos = XMVectorSet(9999.0f, 9999.0f, 9999.0f, 1.0f);

    for (auto& pImpact : m_vecMortalImpact)
    {
        if (pImpact->IsActive())
        {
            pImpact->DeActivate_Impact();
        }
    }


  /*  for (auto& pExplosion : m_vecMortalExplosion)
    {
        if (pExplosion->IsActive())
        {
            pExplosion->DeActivate_Explode();
        }
    }*/
}

void CMissile_Mortal::DeActivate_DropMissile()
{
    m_bActive = false;
    m_bFalling = false;
    m_bImpactShow = false;
    _vector vMissilePos = XMVectorSet(9999.0f, 9999.0f, 9999.0f, 1.0f);

    for (auto& pImpact : m_vecMortalImpact)
    {
        if (pImpact->IsActive())
        {
            pImpact->DeActivate_Impact();
        }
    }


    for (auto& pExplosion : m_vecMortalExplosion)
    {
        if (pExplosion->IsActive())
        {
            pExplosion->DeActivate_Explode();
        }
    }
}

void CMissile_Mortal::Missile_Fly(_float _fTimeDelta)
{
    if (!m_bActive)
        return;

    m_pTransformCom->Go_Straight(_fTimeDelta);
}

void CMissile_Mortal::Drop_Missile(_float _fTimeDelta/*, _vector _vTargetPosition*/)
{
    _vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    _float fPosX = XMVectorGetX(m_vTargetPos);
    _float fPosZ = XMVectorGetZ(m_vTargetPos);
    _float fPosY = XMVectorGetY(vCurrentPos);


    if (!m_bImpactShow && m_bFalling)
    {
        m_bImpactShow = true;

        _vector vImpactPos = XMVectorSet(fPosX, 0.1f, fPosZ, 1.0f);

        for (auto& pImpact : m_vecMortalImpact)
        {
            if (!pImpact->IsActive()) 
            {
                pImpact->Set_Position(vImpactPos, XMVectorZero());
                pImpact->Set_Active(true);
                break; 
            }
        }
    }

    if (fPosY <= 0.0f)
    {
        //DeActivate_Missile();

        DeActivate_DropMissile();
    }
    else
    {
        fPosY -= 20.0f * _fTimeDelta;

        m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
        m_pTransformCom->Scaling(_float3(0.0001f, 0.0001f, 0.0001f));
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosX, fPosY, fPosZ, 1.0f));
    }

    for (auto& pExplosion : m_vecMortalExplosion)
    {
        if (pExplosion->IsActive() && XMVectorGetY(vCurrentPos) <= 1.0f)
        {
            _vector vExplosionPos = vCurrentPos;
            m_pGameInstance->PlayOnce(L"ST_Goliath_Shockwave.wav", MONSTER_MORTAL_SHOOT, 0.1f);
            pExplosion->Set_Position(vExplosionPos, XMVectorZero());
        }
    }
}

void CMissile_Mortal::Check_Collision_With_Player()
{
    if (!m_pColliderCom || !m_pTargetCollider)
        return;

    if (m_pColliderCom->Intersect(m_pTargetCollider))
    {
        Hit_Target();
    }
}

CMissile_Mortal* CMissile_Mortal::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CMissile_Mortal* pInstance = new CMissile_Mortal(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created: CMissile_Mortal");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMissile_Mortal::Clone(void* _pArg)
{
    CMissile_Mortal* pInstance = new CMissile_Mortal(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Created: CMissile_Mortal");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMissile_Mortal::Free()
{
    __super::Free();

    for (auto& pMortalImpact : m_vecMortalImpact)
        Safe_Release(pMortalImpact);
    m_vecMortalImpact.clear();


    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);


    for (auto& pMortalExplosion : m_vecMortalExplosion)
        Safe_Release(pMortalExplosion);
    m_vecMortalExplosion.clear();
}
