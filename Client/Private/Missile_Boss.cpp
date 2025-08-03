#include "pch.h"
#include "Missile_Boss.h"
#include "GameInstance.h"


CMissile_Boss::CMissile_Boss(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CMissile{ _pDevice, _pContext }
{
}

CMissile_Boss::CMissile_Boss(const CMissile_Boss& _Prototype)
    :CMissile(_Prototype)
{
}

HRESULT CMissile_Boss::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMissile_Boss::Initialize(void* _pArg)
{
    MISSILE_BOSS_DESC* pDesc = static_cast<MISSILE_BOSS_DESC*>(_pArg);

    pDesc->fSpeedPerSec = 13.f;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    //m_pTransformCom->Scaling(_float3(0.0003f, 0.0003f, 0.0003f));

    m_vInitialMissilePos = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

    m_vLastPos = m_vInitialMissilePos;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vInitialMissilePos);

    CMissileSmokeParticle::SMOKE_PARTICLE_DESC SmokeDesc = {};
    m_pSmokeParticle = dynamic_cast<CMissileSmokeParticle*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Particle_Smoke"), &SmokeDesc));

    if (!m_pSmokeParticle)
        return E_FAIL;

    m_vecSmokeParticle.push_back(m_pSmokeParticle);



    //CDying_Explode::DYING_EXPLO_DESC mortaldesc = {};
    //m_pMortalExplosion = dynamic_cast<CDying_Explode*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Dying_Explosion"), &mortaldesc));

    //m_vecMortalExplosion.push_back(m_pMortalExplosion);




    return S_OK;
}

void CMissile_Boss::Priority_Update(_float _fTimeDelta)
{
}

void CMissile_Boss::Update(_float _fTimeDelta)
{

    //for (auto& pExplosion : m_vecMortalExplosion)
    //{
    //    if (pExplosion->IsActive())
    //    {
    //        pExplosion->Update(_fTimeDelta);
    //    }
    //}


    if (!m_bActive)
    {
        return;
    }

    __super::Update(_fTimeDelta);

    _vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _vector vMissileDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK); // 기본 방향

    if (m_bBezierMode)
    {
        _vector vPreviousPos = m_vLastPos;
        vMissileDir = XMVector3Normalize(vCurrentPos - vPreviousPos);
    }

    for (auto& pSmoke : m_vecSmokeParticle)
    {
        if (pSmoke->IsActive())
        {
            _vector vSmokePos = vCurrentPos - (vMissileDir * 2.0f);
            pSmoke->Set_Position(vSmokePos, vMissileDir);
            pSmoke->Update(_fTimeDelta);
        }
    }




    if (m_bBezierMode)
    {
        if (XMVectorGetY(vCurrentPos) <= 0.0f)
        {
            DeActivate_Missile();
            return;
        }
        Missile_Fly_Bezier(_fTimeDelta);
    }
    else
    {
        static const _float fMaxDistance = 25.0f;
        _float fDistance = XMVectorGetX(XMVector3Length(vCurrentPos - m_vInitialMissilePos));

        if (fDistance > fMaxDistance)
        {
            DeActivate_Missile();
            return;
        }
        Missile_Fly(_fTimeDelta);
    }


    // 현재 위치를 저장하여 다음 프레임에서 벨지어 방향 계산에 사용
    m_vLastPos = vCurrentPos;

    _matrix WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    m_pColliderCom->Update(WorldMatrix);

    Check_Collision_With_Player();
}

void CMissile_Boss::Late_Update(_float _fTimeDelta)
{
    m_pTargetCollider = static_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), 0));

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);


    for (auto& pSmoke : m_vecSmokeParticle)
    {
        if (pSmoke->IsActive())
        {
            pSmoke->Late_Update(_fTimeDelta);
        }
    }

    //for (auto& pExplosion : m_vecMortalExplosion)
    //{
    //    if (pExplosion->IsActive())
    //    {
    //        pExplosion->Late_Update(_fTimeDelta);
    //    }
    //}
}

HRESULT CMissile_Boss::Render()
{
#ifdef _DEBUG
    //m_pColliderCom->Render();
#endif


    if (FAILED(__super::Render())) 
        return E_FAIL;

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

HRESULT CMissile_Boss::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Missile_Boss"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    /* Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
    ColliderDesc.vExtents = _float3(1.5f, 1.5f, 1.5f);
    ColliderDesc.vCenter = _float3(0.f, 0.0f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMissile_Boss::Bind_ShaderResource()
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

void CMissile_Boss::Activate_Missile(const _vector& _vPosition, const _vector& _vGunMuzzleLook)
{
    //__super::Activate_Missile();

    //m_vInitialMissilePos = _vPosition;
    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPosition);

    //_vector vTargetDir = XMVector3Normalize(_vGunMuzzleLook);
    //m_pTransformCom->LookAt(_vPosition + vTargetDir);


    __super::Activate_Missile();

    m_bBezierMode = false;

    m_vInitialMissilePos = _vPosition;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPosition);


   



    _vector vTargetPos = m_pPlayer->Get_PlayerPosition();

    _vector vTargetDir = XMVector3Normalize(vTargetPos - _vPosition);

    m_pTransformCom->LookAt(_vPosition + vTargetDir);




    for (auto& pSmoke : m_vecSmokeParticle)
    {
        pSmoke->Activate_Smoke();
    }


    m_vLastPos = _vPosition;
}

void CMissile_Boss::Activate_BezierMissile(const _vector& _vStartPos)
{
    __super::Activate_Missile();

    m_vInitialMissilePos = _vStartPos;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vStartPos);

    m_fElapsedTime = 0.0f;
    m_bActive = true;
    m_bBezierMode = true;

    if (!m_pPlayer)
    {
        m_bActive = false;
        return;
    }

    _vector vTargetPos = m_pPlayer->Get_PlayerPosition();
    _vector vTargetDir = XMVector3Normalize(vTargetPos - _vStartPos);

    m_pTransformCom->LookAt(_vStartPos + vTargetDir); 

    _vector vMiddlePoint = (_vStartPos + vTargetPos) * 0.5f;
    vMiddlePoint = XMVectorSetY(vMiddlePoint, XMVectorGetY(vMiddlePoint) + 50.0f);

    m_vEndPos = vTargetPos;
    m_vCurvePos = vMiddlePoint;



    for (auto& pSmoke : m_vecSmokeParticle)
    {
        pSmoke->Activate_Smoke();
    }

 /*   for (auto& pExplosion : m_vecMortalExplosion)
    {
        if (!pExplosion->IsActive() && (XMVectorGetY(m_vEndPos) <=  2.0f))
        {
            pExplosion->Activate_Explode();
        }
    }*/


    m_vLastPos = _vStartPos;
}

_vector CMissile_Boss::Bezier(_vector _vStartPos, _vector _vCurvePos, _vector _vEndPos, _float _fTimeDelta)
{
    return (1 - _fTimeDelta) * (1 - _fTimeDelta) * _vStartPos + 2 * (1 - _fTimeDelta) * _fTimeDelta * _vCurvePos + _fTimeDelta * _fTimeDelta * _vEndPos;
}

void CMissile_Boss::DeActivate_Missile()
{
    m_bActive = false;

    _vector vMissilePos = XMVectorSet(9999.0f, 9999.0f, 9999.0f, 1.0f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMissilePos);

    for (auto& pSmoke : m_vecSmokeParticle)
    {
        pSmoke->DeActivate_Smoke();
    }

  /*  for (auto& pExplosion : m_vecMortalExplosion)
    {
        if (pExplosion->IsActive())
        {
            pExplosion->DeActivate_Explode();
        }
    }*/
}

void CMissile_Boss::Missile_Fly(_float _fTimeDelta)
{
    if (!m_bActive)
        return;

    m_pTransformCom->Go_Straight(_fTimeDelta);
}

void CMissile_Boss::Missile_Fly_Bezier(_float _fTimeDelta)
{
    _vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    //for (auto& pExplosion : m_vecMortalExplosion)
    //{
    //    /* if (pExplosion->IsActive() && XMVectorGetY(vCurrentPos) <= 2.0f)
    //     {
    //         _vector vExplosionPos = vCurrentPos;
    //         m_pGameInstance->PlayOnce(L"ST_Goliath_Shockwave.wav", MONSTER_MORTAL_SHOOT, 0.1f);
    //         pExplosion->Set_Position(vExplosionPos, XMVectorZero());
    //     }*/

    //    if (!pExplosion->IsActive() && XMVectorGetY(vCurrentPos) <= 2.0f)
    //    {
    //        pExplosion->Activate_Explode();
    //        _vector vExplosionPos = vCurrentPos;
    //        m_pGameInstance->PlayOnce(L"ST_Goliath_Shockwave.wav", MONSTER_MORTAL_SHOOT, 0.1f);
    //        pExplosion->Set_Position(vExplosionPos, XMVectorZero());
    //    }
    //}

    if (!m_bActive)
        return;

    m_fElapsedTime += _fTimeDelta * 0.5f;

    if (m_fElapsedTime >= 20.0f)
    {
        m_bActive = false; 
        return;
    }

    _vector vMissilePos = Bezier(m_vInitialMissilePos, m_vCurvePos, m_vEndPos, m_fElapsedTime);

    _vector vDir = XMVector3Normalize(vMissilePos - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
    m_pTransformCom->LookAt(vMissilePos + vDir);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vMissilePos);

   
}

void CMissile_Boss::Check_Collision_With_Player()
{
    if (!m_pColliderCom || !m_pTargetCollider)
        return;

    if (m_pColliderCom->Intersect(m_pTargetCollider))
    {
        Hit_Target();
    }
}

CMissile_Boss* CMissile_Boss::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CMissile_Boss* pInstance = new CMissile_Boss(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created: CMissile_Boss");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CMissile_Boss::Clone(void* _pArg)
{
    CMissile_Boss* pInstance = new CMissile_Boss(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Created: CMissile_Boss");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMissile_Boss::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);

    for (auto& pSmokeParticle : m_vecSmokeParticle)
        Safe_Release(pSmokeParticle);
    m_vecSmokeParticle.clear();

    //for (auto& pMortalExplosion : m_vecMortalExplosion)
    //    Safe_Release(pMortalExplosion);
    //m_vecMortalExplosion.clear();
}
