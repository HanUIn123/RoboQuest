#include "pch.h"
#include "Robo_Boom.h"
#include "GameInstance.h"

//#include "Body_Robo_Boom.h"

#include "Missile_Boom.h"

#include "Player.h"
#include "Player_Camera.h"


CRobo_Boom::CRobo_Boom(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CMonster{ _pDevice, _pContext }
{
}

CRobo_Boom::CRobo_Boom(const CMonster& _Prototype)
    :CMonster(_Prototype)
{
}

HRESULT CRobo_Boom::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRobo_Boom::Initialize(void* _pArg)
{
    ROBO_BOOM_DESC* pDesc = static_cast<ROBO_BOOM_DESC*>(_pArg);

    pDesc->fSpeedPerSec = 1.0f;

    m_fMissile_Height_Offset = 0.8f;
    m_fMissile_Width_Offset = -0.05f;
    m_iHp = 50;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_iState = MONSTER_STATE_IDLE;

    // Monster Position 값 Loading 해온 값으로 세팅
    _vector vMonsterPos = XMLoadFloat3(&pDesc->vMonsterpos);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vMonsterPos, 1.0f));

    // Monster Rotation 값 Loading 해온 값으로 세팅
    _matrix matRotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&pDesc->vMonsterRotation));
    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, matRotation.r[0]);
    m_pTransformCom->Set_State(CTransform::STATE_UP, matRotation.r[1]);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, matRotation.r[2]);

    // Monster Scale 값 Loading 해온 값으로 세팅
    m_pTransformCom->Scaling(pDesc->vMonsterScale);

    m_pNavigationCom->Setting_ShrinkCell(1.0f);

    m_pNavigationCom->Set_CurrentNaviIndex(vMonsterPos);

    m_pDyingExplosion = static_cast<CDying_Explode*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Dying_Explosion"), LEVEL_GAMEPLAY, TEXT("Layer_FrameEffect")));

    return S_OK;
}

void CRobo_Boom::Priority_Update(_float _fTimeDelta)
{
    m_fTargetDistance = Compute_Distance_To_Target();

    // 얘는 레알로 몬스터 상제 제어만 담당함. 
    Robo_Boom_MoveState();

    _vector vMonsterPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _float fPosX = XMVectorGetX(vMonsterPosition);
    _float fPosZ = XMVectorGetZ(vMonsterPosition);

    _float fNavigationHeight = m_pNavigationCom->Compute_Height(vMonsterPosition);
    _float fResultY = (fNavigationHeight > 0.0f) ? (Compute_Monster_Position_Terrain(vMonsterPosition)) : (fNavigationHeight);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosX, fResultY, fPosZ, 1.0f));

    if (m_pHpBar_Robo_Boom)
    {

    }

    __super::Priority_Update(_fTimeDelta);
}

void CRobo_Boom::Update(_float _fTimeDelta)
{
    //if (m_bDying)
    //{
    //    m_fDyingTime -= _fTimeDelta;
    //    if (m_fDyingTime <= 0.0f)
    //    {
    //        if (m_pDyingExplosion->Get_AnimTime() >= 1.0f)
    //        {
    //            m_pGameInstance->Delete_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), this);
    //            return;
    //        }
    //    }
    //    return;  
    //}

    _vector vMonsterPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    if (!m_pNavigationCom->bIsIn_ShrinkedCell(vMonsterPosition))
    {
        SelfMove_Randomly();
    }

    switch (m_iState)
    {
    case MONSTER_STATE_IDLE:
        Setting_IdleState(_fTimeDelta);
        break;

    case MONSTER_STATE_RUN:
        Setting_RunState(_fTimeDelta);
        break;

    case MONSTER_STATE_SHOOT:
        Setting_ShootState(_fTimeDelta);
        break;

    case MONSTER_STATE_STUN:
        Setting_StunState(_fTimeDelta);
        break;
    }

    Shoot_Missiles(_fTimeDelta);

    if (m_pBody_Robo_Boom->IsDissolving())
    {
        if (m_pBody_Robo_Boom->Get_DissolveTime() >= 0.8f)
        {

            m_pGameInstance->Delete_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), this);
            return;
        }
    }


    auto WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    m_pColliderCom[COLL_AABB]->Update(WorldMatrix);
    m_pColliderCom[COLL_SPHERE]->Update(WorldMatrix);

    __super::Update(_fTimeDelta);
}

void CRobo_Boom::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);

    CCollider* pTargetCollider = static_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), 0));
    m_bDetect_Target = m_pColliderCom[COLL_SPHERE]->Intersect(pTargetCollider);

    Late_Update_Missiles(_fTimeDelta);

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CRobo_Boom::Render()
{
#ifdef _DEBUG
    //m_pNavigationCom->Render();
    //m_pColliderCom[COLL_AABB]->Render();
    //m_pColliderCom[COLL_SPHERE]->Render();
#endif
    return S_OK;
}

void CRobo_Boom::Setting_IdleState(_float _fTimeDelta)
{
    m_fIdleTime -= _fTimeDelta;

    if (m_fIdleTime <= 0.0f)
    {
        m_fIdleTime = 3.0f;
        m_fSelfMoveCoolTime = 2.0f;
        SelfMove_Randomly();
        m_iState = MONSTER_STATE_RUN;
    }
}

void CRobo_Boom::Setting_StunState(_float _fTimeDelta)
{
    m_fStunTime -= _fTimeDelta;
    if (m_fStunTime <= 0.0f)
    {
        //m_iState &= ~MONSTER_STATE_STUN;
        m_iState = m_iPrevState;
    }
}

void CRobo_Boom::Setting_RunState(_float _fTimeDelta)
{
    if (m_bDetect_Target)
    {
        if (SUCCEEDED(Setting_Direction_To_Target(_fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
        {
            m_pTransformCom->Go_Straight(_fTimeDelta, m_pNavigationCom);
        }
        return;
    }

    m_fSelfMoveCoolTime -= _fTimeDelta;
    _vector vCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    // 랜덤 이동
    _vector vDirection = XMVector3Normalize(m_vRandomPosition - vCurrentPosition);
    _vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
    _vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vDirection));
    _vector vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));

    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
    m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
    m_pTransformCom->Go_Straight(_fTimeDelta, m_pNavigationCom);

    if (XMVectorGetX(XMVector3Length(m_vRandomPosition - vCurrentPosition)) < 0.5f)
    {
        m_iState = MONSTER_STATE_IDLE;
        m_fIdleTime = 3.0f;
    }
}

void CRobo_Boom::Setting_ShootState(_float _fTimeDelta)
{
    if (SUCCEEDED(Setting_Direction_To_Target(_fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
    {
        if (m_fShootCoolTime <= 0.0f)
        {
           
            Activate_Missile();
            m_fShootCoolTime = 0.5f;
        }
        else
        {
            m_fShootCoolTime -= _fTimeDelta;
        }
    }
}

void CRobo_Boom::Shoot_Missiles(_float _fTimeDelta)
{
    for (auto& pMissile : m_listMissile)
    {
        if (pMissile->IsActiveBullet())
        {
            pMissile->Update(_fTimeDelta);
        }
    }
}

void CRobo_Boom::Late_Update_Missiles(_float _fTimeDelta)
{
    for (auto& pMissile : m_listMissile)
    {
        if (pMissile->IsActiveBullet())
        {
            pMissile->Late_Update(_fTimeDelta);
        }
    }
}

void CRobo_Boom::Robo_Boom_MoveState()
{
    if (m_iState & MONSTER_STATE_STUN)
        return;

    if (m_bDetect_Target)
    {
        if (m_fTargetDistance > 5.0f)
            m_iState = MONSTER_STATE_RUN;
        else
            m_iState = MONSTER_STATE_SHOOT;
    }
    else if (!(m_iState & MONSTER_STATE_RUN))
    {
        m_iState = MONSTER_STATE_IDLE;
    }
}

void CRobo_Boom::SelfMove_Randomly()
{
    _vector vCurrentMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    _float fRandomX = ((rand() % 2001) - 1000) / 100.0f;
    _float fRandomZ = ((rand() % 2001) - 1000) / 100.0f;

    m_vRandomPosition = XMVectorSet(XMVectorGetX(vCurrentMonsterPos) + fRandomX, XMVectorGetY(vCurrentMonsterPos), XMVectorGetZ(vCurrentMonsterPos) + fRandomZ, 1.0f);

    _float fTerrainHeight = Compute_Monster_Position_Terrain(m_vRandomPosition);
    m_vRandomPosition = XMVectorSetY(m_vRandomPosition, fTerrainHeight);
}

HRESULT CRobo_Boom::Ready_Component()
{
    /* Com_Navigation */
    CNavigation::NAVIGATION_DESC		NaviDesc{};

    NaviDesc.iCurrentCellIndex = 0;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;

    /* Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.40f, 0.55f, 0.40f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom[COLL_AABB]), &ColliderDesc)))
        return E_FAIL;


    CBounding_Sphere::BOUNDING_SPHERE_DESC  ColliderSphereDesc{};
    ColliderSphereDesc.fRadius = 10.0f;
    ColliderSphereDesc.vCenter = _float3(0.f, Compute_Monster_Position_Terrain(m_pTransformCom->Get_State(CTransform::STATE_POSITION)), 0.f);
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider_SPHERE"), reinterpret_cast<CComponent**>(&m_pColliderCom[COLL_SPHERE]), &ColliderSphereDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CRobo_Boom::Ready_PartObjects()
{
#pragma region BODY_BOOM
    // BODY_ROBO_BOOM을 만들자. 
    CBody_Robo_Boom::BODY_BOOM_DESC       BodyDesc{};
    BodyDesc.pParentState = &m_iState;
    BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    BodyDesc.fSpeedPerSec = 0.0f;
    BodyDesc.fRotationPerSec = 0.0f;

    if (FAILED(__super::Add_PartObject(TEXT("Part_Body"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Body_RoboBoom"), &BodyDesc)))
        return E_FAIL;

    m_pBody_Robo_Boom = static_cast<CBody_Robo_Boom*>(Find_PartObject(TEXT("Part_Body")));
    if (nullptr == m_pBody_Robo_Boom)
        return E_FAIL;
#pragma endregion

#pragma region MISSILE_BOOM
    // MISSILE_BOOM 을 만들자. 
    for (_uint i = 0; i < 100; ++i)
    {
        CMissile_Boom::MISSILE_BOOM_DESC MissileDesc{};
        CMissile_Boom* pMissile = dynamic_cast<CMissile_Boom*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Missile_Boom"), &MissileDesc));

        if (!pMissile)
            return E_FAIL;

        m_listMissile.push_back(pMissile);
    }
#pragma endregion

#pragma region HPBAR
    // 몬스터의 체력바를 만들자. 
    CHpBar_Robo_Boom::BOOM_HPBAR_DESC       HpBarDesc{};
    HpBarDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    HpBarDesc.pMonsterHp = &m_iHp;
    _float fOffset = (g_iWinSizeY * 0.5f);
    HpBarDesc.fY = fOffset - (fOffset * 0.004f);


    if (FAILED(__super::Add_UIPartObject(TEXT("Part_HpBar_RoboBoom"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_HpBar_RoboBoom"), &HpBarDesc)))
        return E_FAIL;

    m_pHpBar_Robo_Boom = static_cast<CHpBar_Robo_Boom*>(Find_UIPartObject(TEXT("Part_HpBar_RoboBoom")));

    if (nullptr == m_pHpBar_Robo_Boom)
        return E_FAIL;
#pragma endregion

#pragma region DAMAGE_NUMBER
    for (_uint i = 0; i < 3; ++i)
    {
        CDamageNumber::DAMAGE_NUM_DESC      DamageNumDesc{};
        DamageNumDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
        DamageNumDesc.fLifeTime = 1.0f;
        DamageNumDesc.iIndex = i;

        _float fOffset = (g_iWinSizeY * 0.5f);
        DamageNumDesc.fY = fOffset - (fOffset * 0.006f);

        wstring strPartName = L"Part_DamageNumber_" + to_wstring(i);

        if (FAILED(__super::Add_UIPartObject(strPartName.c_str(), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_DamageNumber"), &DamageNumDesc)))
            return E_FAIL;

        m_pDamageNumber[i] = static_cast<CDamageNumber*>(Find_UIPartObject(strPartName.c_str()));
        if (nullptr == m_pDamageNumber[i])
            return E_FAIL;
    }
#pragma endregion

    return S_OK;
}

_bool CRobo_Boom::Check_Collision_With_Ray(const _vector& _vRayStart, const _vector& _vRayDirection, _float& _fDistance)
{
    if (nullptr == m_pColliderCom[COLL_AABB])
        return false;

    return m_pColliderCom[COLL_AABB]->Intersect_Ray(_vRayStart, _vRayDirection, _fDistance);
}

void CRobo_Boom::Be_Hited(_uint _iCurrentGunDamage)
{
    if (m_iHp > 0)
    {
        //ST_Artefact_Grapple_Impact
        m_pGameInstance->Play(L"ST_Enemy_Bolt_Shock_Impact.wav", PLAYER_SHOOT, 0.6f);
        if ((m_iState & MONSTER_STATE_IDLE) || (m_iState & MONSTER_STATE_RUN) || (m_iState & MONSTER_STATE_SHOOT))
        {
            m_iPrevState = m_iState;
            m_iState = MONSTER_STATE_STUN;
            m_fStunTime = 0.5f;
        }

        // 일단 체력 1씩 깎이게 해놨음. 나중에 총기류 데미지 추가되면 변경하면 될듯. 
        //m_pHpBar_Robo_Boom->Decrease_Hp(m_iHp--);

        m_pHpBar_Robo_Boom->Show_HealthBar(true);

        //m_iHp = max(0, m_iHp - 10);  // 예제: 데미지 10
        m_iHp = max(0, m_iHp - _iCurrentGunDamage);  // 예제: 데미지 10
        m_pHpBar_Robo_Boom->Decrease_Hp(m_iHp);


        // 체력바 쭈욱쭉 떨어지는 연출
        m_pHpBar_Robo_Boom->Update(0.016f);

        //_vector damagePosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        //XMStoreFloat3(&m_DamageTextPosition, damagePosition);
        //
        //m_DamageTextPosition.y += 1.5f; 
        //m_DamageText = to_wstring(10);
        //m_fDamageTextTime = 1.0f;
        //m_bShowDamageText = true;

        //_uint iDamage = 10;
        _uint iDamage = _iCurrentGunDamage;
        _uint iDigits[3] = { (iDamage / 100) % 10, (iDamage / 10) % 10, iDamage % 10 };

        _uint iStartIndex = (iDigits[0] == 0) ? 1 : 0;

        for (_uint i = iStartIndex; i < 3; ++i)
        {
            if (m_pDamageNumber[i])
            {
                m_pDamageNumber[i]->Set_DamageNumberDigit(iDigits[i], i - iStartIndex);
                m_pDamageNumber[i]->Set_LifeTime(0.3f);
            }
        }



        if (m_pBody_Robo_Boom)
            m_pBody_Robo_Boom->Activate_HittedEffect();


        if (m_iHp <= 0 && !m_pBody_Robo_Boom->IsDissolving())
        {
            m_pBody_Robo_Boom->Activate_DissolveEffect();
            m_pHpBar_Robo_Boom->Show_HealthBar(false);
            return;
        }

    }

  
    //if (m_iHp <= 0 && !m_bDying)
    //{
    //    //m_pGameInstance->Delete_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), this);


    //    m_bDying = true;
    //    m_fDyingTime = 1.0f;

    //    _vector vDeadPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    //    m_pDyingExplosion->Activate_Explode(true, vDeadPos);
    //}
}

void CRobo_Boom::Activate_Missile()
{
    for (auto& pMissile : m_listMissile)
    {
        if (!pMissile->IsActiveBullet())
        {
            _vector vMuzzlePos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

            _float fTerrainHeight = Compute_Monster_Position_Terrain(vMuzzlePos);
            vMuzzlePos = XMVectorSetY(vMuzzlePos, fTerrainHeight + 0.8f);
            //ST_Enemy_Bolt_Shock_Shoot
            
            //_vector vDirection = Setting_MissileDirection_To_Target(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 0.8f, -0.05f);
            _vector vDirection = Setting_MissileDirection_To_Target(m_pTransformCom->Get_State(CTransform::STATE_POSITION), m_fMissile_Height_Offset, m_fMissile_Width_Offset);

            m_pGameInstance->Play(L"ST_DrTurret_Saw_Loop_Stop.wav", MONSTER_SHOOT, 0.1f);
            pMissile->Activate_Missile(vMuzzlePos, vDirection);
            return;
        }
    }
}

void CRobo_Boom::Setting_BillBoard()
{
}

CMonster* CRobo_Boom::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CRobo_Boom* pInstance = new CRobo_Boom(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CRobo_Boom");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CRobo_Boom::Clone(void* _pArg)
{
    CRobo_Boom* pInstance = new CRobo_Boom(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CRobo_Boom");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRobo_Boom::Free()
{
    __super::Free();

    for (auto& pMissile : m_listMissile)
        Safe_Release(pMissile);

    m_listMissile.clear();

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pColliderCom[COLL_AABB]);
    Safe_Release(m_pColliderCom[COLL_SPHERE]);
}
