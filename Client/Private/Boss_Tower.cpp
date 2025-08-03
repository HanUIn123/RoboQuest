#include "pch.h"
#include "Boss_Tower.h"
#include "GameInstance.h"
#include "Player.h"

CBoss_Tower::CBoss_Tower(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CMonster{ _pDevice, _pContext }
{
}

CBoss_Tower::CBoss_Tower(const CMonster& _Prototype)
    :CMonster(_Prototype)
{
}

HRESULT CBoss_Tower::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBoss_Tower::Initialize(void* _pArg)
{
    ROBO_BOSS_DESC* pDesc = static_cast<ROBO_BOSS_DESC*>(_pArg);

    pDesc->fSpeedPerSec = 1.5f;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_iState = STATE_BOSS_TOWER_IDLE;

    // Monster Position 값 Loading 해온 값으로 세팅ㄱㄱㄱ
    _vector vMonsterPos = XMLoadFloat3(&pDesc->vMonsterpos);
    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vMonsterPos, 1.0f));
    vMonsterPos = XMVectorSetY(vMonsterPos, XMVectorGetY(vMonsterPos) - 2.0f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vMonsterPos, 1.0f));

    // Monster Rotation 값 Loading 해온 값으로 세팅ㄱㄱㄱ
    _matrix matRotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&pDesc->vMonsterRotation));
    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, matRotation.r[0]);
    m_pTransformCom->Set_State(CTransform::STATE_UP, matRotation.r[1]);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, matRotation.r[2]);

    // Monster Scale 값 Loading 해온 값으로 세팅ㄱㄱㄱ
    m_pTransformCom->Scaling(pDesc->vMonsterScale);

    //m_pNavigationCom->Setting_ShrinkCell(1.0f);
    m_pBossHpBar = static_cast<CBossHpBar*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_BossHpBar"), LEVEL_GAMEPLAY, TEXT("Layer_BossHpBar")));

    m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, TEXT("Layer_Player")));

    m_pPlayer_Camera = m_pPlayer->Get_PlayerCamera();

    m_pBossIntro = static_cast<CBossIntroScreen*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_BossIntro"), LEVEL_GAMEPLAY, TEXT("Layer_PlayerEffect")));

    m_pPlayerBar = static_cast<CPlayerHpbar*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_PlayerHpBar"), LEVEL_GAMEPLAY, TEXT("Layer_PlayerHpBar")));
    m_pPlayerHpBarBackGround = static_cast<CHpBarBackGround*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_HpBarBackGround"), LEVEL_GAMEPLAY, TEXT("Layer_PlayerHpBar")));
    m_pBossHpBarBackGround = static_cast<CBossHpBarBackGround*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_BossHpBarBackGround"), LEVEL_GAMEPLAY, TEXT("Layer_BossHpBar")));
    m_pCrossHair = static_cast<CCrossHair*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_CrossHair"), LEVEL_GAMEPLAY, TEXT("Layer_UI")));

    m_fBossHp = 2000.0f;

    m_fIdleTime = 2.0f;
    m_fCanonShootTime = 2.0f;
    m_fCanonCoolTime = 2.0f;
    m_fCanonFinshTime = 1.0f;


    m_fFireCoolTime = 1.0f;
    m_fFireShootTime = 3.0f;
    m_fFireFinishTime = 1.0f;


    m_fHandBimCoolTime = 1.0f;
    m_fHandBimShootTime = 3.0f;
    m_fHandBimFinishTime = 1.0f;


    m_fMortalCoolTime = 0.5f;
    m_fMortalShootTime = 3.0f;
    m_fMortalFinishTime = 1.0f;

    return S_OK;
}

void CBoss_Tower::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CBoss_Tower::Update(_float _fTimeDelta)
{
    _vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    if (m_iState != STATE_BOSS_FIRETHORW_SHOT && m_iState != STATE_BOSS_FIRETHORW_OUT && m_iState != STATE_BOSS_HANDBIM_SHOOTOPEN)
    {
        if (SUCCEEDED(Setting_Direction_To_Target_Lerp(_fTimeDelta, vMonsterPos)))
        {
            switch (m_iState)
            {
            case STATE_BOSS_IDLE:

                break;
            case STATE_BOSS_IN:
                Setting_NormalShotReady(_fTimeDelta);
                break;
            case STATE_BOSS_OUT:
                Setting_NormalShotFinish(_fTimeDelta);
                break;
            case STATE_BOSS_CANON_PRESHOT:
            {
                if (m_pBody_Boss->IsAnimationFinished())
                {
                    m_iState = STATE_BOSS_CANON_SHOT;
                }
            }
            break;
            case STATE_BOSS_CANON_SHOT:
                Setting_NormalShotState(_fTimeDelta);
                break;
            case STATE_BOSS_CANON_SHOT_LOOP:
                break;
            case STATE_BOSS_FIRETHORW_IN:
                Setting_FireThrowReady(_fTimeDelta);
                break;
            //case STATE_BOSS_FIRETHORW_OUT:
            //    Setting_FireThorwFinish(_fTimeDelta);
            //    break;
            case STATE_BOSS_FIRETHORW_PRESHOT:
            {
                if (m_pBody_Boss->IsAnimationFinished())
                {
                    m_iState = STATE_BOSS_FIRETHORW_SHOT;
                }
            }
            break;
            //case STATE_BOSS_FIRETHORW_SHOT:
            //    Setting_FireThrowState(_fTimeDelta);
            //    break;
            case STATE_BOSS_HANDBIM_IN:
                Setting_HandBimReady(_fTimeDelta);
                break;
            case STATE_BOSS_HANDBIM_OUT:
                Setting_HandBimFinsih(_fTimeDelta);
                break;
            case STATE_BOSS_HANDBIM_SHOOTCLOSE:
                break;
          /*  case STATE_BOSS_HANDBIM_SHOOTOPEN:
                Setting_HandBimState(_fTimeDelta);
                break;*/
            case STATE_BOSS_TOWER_IDLE:
                Setting_IdleState(_fTimeDelta);
                break;
            case STATE_BOSS_TOWER_INTRO:
            {
                //m_pGameInstance->StopSound(SOUND_BGM);
               
                Setting_IntroState(_fTimeDelta);
            }
                break;
            case STATE_BOSS_MORTAL_SHOT:
                Setting_MortalReady(_fTimeDelta);
                break;
            case STATE_BOSS_MORTAL_SHOT_OUT:
                Setting_MortalShotState(_fTimeDelta);
                break;
            }
        }
    }

    if (m_iState == STATE_BOSS_FIRETHORW_SHOT)
        Setting_FireThrowState(_fTimeDelta);
    if (m_iState == STATE_BOSS_FIRETHORW_OUT)
        Setting_FireThorwFinish(_fTimeDelta);
    if (m_iState == STATE_BOSS_HANDBIM_SHOOTOPEN)
        Setting_HandBimState(_fTimeDelta);

    for (auto& pMissile : m_listMissile)
    {
        if (pMissile->IsActiveBullet())
        {
            pMissile->Update(_fTimeDelta);
        }
    }

    if (m_pBody_Boss->IsDissolving())
    {
        if (m_pBody_Boss->Get_DissolveTime() >= 1.0f)
        {
            m_pGameInstance->Delete_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), this);
            return;
        }
    }

    for (auto& pFireRoad : m_vecFireRoad)
    {
        if (pFireRoad->IsActive())
            pFireRoad->Update(_fTimeDelta);
    }

    for (auto& pLaser : m_listLaser)
    {
        if (pLaser->IsActiveBullet())
        {
            pLaser->Update(_fTimeDelta);
        }
    }

    for (auto& pBarrier : m_listBarrier)
    {
        if (pBarrier->IsActiveBarrier())
        {
            pBarrier->Update(_fTimeDelta);
        }
    }

    auto WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    m_pColliderCom[COLL_AABB]->Update(WorldMatrix);
    m_pColliderCom[COLL_SPHERE]->Update(WorldMatrix);

    __super::Update(_fTimeDelta);
}

void CBoss_Tower::Late_Update(_float _fTimeDelta)
{
    if (m_bIsVisible)
    {
        __super::Late_Update(_fTimeDelta);

        CCollider* pTargetCollider = static_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), 0));
        m_bDetect_Target = m_pColliderCom[COLL_SPHERE]->Intersect(pTargetCollider);

        if (m_bDetect_Target && !m_bIntroShow)
        {
            m_bIntroShow = true;

            m_pGameInstance->StopSound(SOUND_BGM);
            m_pGameInstance->Play(L"BossIntro.wav", MONSTER_DEAD, 0.6f);
            m_iState = STATE_BOSS_TOWER_INTRO;
        }

        m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
    }

    for (auto& pMissile : m_listMissile)
    {
        if (pMissile->IsActiveBullet())
        {
            pMissile->Late_Update(_fTimeDelta);
        }
    }

    for (auto& pFireRoad : m_vecFireRoad)
    {
        if (pFireRoad->IsActive())
            pFireRoad->Late_Update(_fTimeDelta);
    }

    for (auto& pLaser : m_listLaser)
    {
        if (pLaser->IsActiveBullet())
        {
            pLaser->Late_Update(_fTimeDelta);
        }
    }

    for (auto& pBarrier : m_listBarrier)
    {
        if (pBarrier->IsActiveBarrier())
        {
            pBarrier->Late_Update(_fTimeDelta);
        }
    }
}

HRESULT CBoss_Tower::Render()
{
#ifdef _DEBUG
    //m_pNavigationCom->Render();
    //m_pColliderCom[COLL_AABB]->Render();
    //m_pColliderCom[COLL_SPHERE]->Render();
#endif


    CFade_Manager::GetInstance()->Render();

    return S_OK;
}

void CBoss_Tower::Activate_Boss()
{
    m_bIsVisible = true;
}

void CBoss_Tower::Robo_Boss_MoveState(_float _fTimeDelta)
{
}

void CBoss_Tower::Setting_IdleState(_float _fTimeDelta)
{
    if (m_bDetect_Target)
    {
        m_fIdleTime -= _fTimeDelta;

        if (m_fIdleTime <= 0.0f)
        {
            m_fIdleTime = 0.2f;

            _uint iPrePattern;
            do {
                iPrePattern = rand() % 4;
            } while (iPrePattern == m_iCurrentPattern);

            m_iCurrentPattern = iPrePattern;
            //m_iCurrentPattern = 3;

            switch (m_iCurrentPattern)
            {
            case 0:
                m_iState = STATE_BOSS_IN;
                break;
            case 1:
                m_iState = STATE_BOSS_FIRETHORW_IN;
                break;
            case 2:
                m_iState = STATE_BOSS_HANDBIM_IN;
                break;
            case 3:
                m_iState = STATE_BOSS_MORTAL_SHOT;
                break;
            }

            //m_iCurrentPattern = (m_iCurrentPattern + 1) % 4;
        }
    }
}

void CBoss_Tower::Setting_NormalShotState(_float _fTimeDelta)
{
    m_fCanonShootTime -= _fTimeDelta;

    _float fInterval = 2.0f / 3.0f;

    if (m_iShootCount < 3 && m_fCanonShootTime <= (2.0f - (m_iShootCount + 1) * fInterval))
    {
        Activate_Missile();
        m_iShootCount++;
    }

    if (m_fCanonShootTime <= 0.0f)
    {
        m_fCanonShootTime = 2.0f;
        m_iShootCount = 0;
        m_iState = STATE_BOSS_OUT;
    }
}

void CBoss_Tower::Setting_NormalShotReady(_float _fTimeDelta)
{
    m_fCanonCoolTime -= _fTimeDelta;

    if (m_fCanonCoolTime <= 0.0f)
    {
        m_fCanonCoolTime = 2.0f;

        m_iState = STATE_BOSS_CANON_PRESHOT;
    }
}

void CBoss_Tower::Setting_NormalShotFinish(_float _fTimeDelta)
{
    m_fCanonFinshTime -= _fTimeDelta;

    if (m_fCanonFinshTime <= 0.0f)
    {
        m_fCanonFinshTime = 1.0f;

        m_iState = STATE_BOSS_TOWER_IDLE;
    }

}

void CBoss_Tower::Setting_FireThrowReady(_float _fTimeDelta)
{
    m_fFireCoolTime -= _fTimeDelta;

    if (m_fFireCoolTime <= 0.0f)
    {
        m_fFireCoolTime = 1.0f;

        m_iState = STATE_BOSS_FIRETHORW_PRESHOT;
    }
}

void CBoss_Tower::Setting_FireThrowState(_float _fTimeDelta)
{
    _vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    m_fFireShootTime -= _fTimeDelta;

    if (m_fFireShootTime > 0.0f)
    {
        Activate_FireThrower();

        Activate_Laser();
    }
    else
    {
        DeActivate_FireThrower();
        m_bFiringOnce = false;
        DeActivate_Laser();
        m_fFireShootTime = 3.0f;
        m_iState = STATE_BOSS_FIRETHORW_OUT;
    }
}

void CBoss_Tower::Setting_FireThorwFinish(_float _fTimeDelta)
{
    m_bLerpToPlayer = true;

    m_fFireFinishTime -= _fTimeDelta;

    if (m_fFireFinishTime <= 0.0f)
    {
        m_fFireFinishTime = 1.0f;

        m_iState = STATE_BOSS_TOWER_IDLE;
    }
}

void CBoss_Tower::Setting_HandBimReady(_float _fTimeDelta)
{
    m_fHandBimCoolTime -= _fTimeDelta;

    if (m_fHandBimCoolTime <= 0.0f)
    {
        m_fHandBimCoolTime = 1.0f;

        m_iState = STATE_BOSS_HANDBIM_SHOOTOPEN;
    }
}

void CBoss_Tower::Setting_HandBimState(_float _fTimeDelta)
{
    m_fHandBimShootTime -= _fTimeDelta;

    if (m_fHandBimShootTime > 0.0f)
    {
        Activate_Barrier();
    }
    else
    {
        m_bBarrieringOnce = false;

        m_fHandBimShootTime = 3.0f;

        m_iState = STATE_BOSS_HANDBIM_OUT;
    }
}

void CBoss_Tower::Setting_HandBimFinsih(_float _fTimeDelta)
{
    m_fHandBimFinishTime -= _fTimeDelta;

    if (m_fHandBimFinishTime <= 0.0f)
    {
        m_fHandBimFinishTime = 1.0f;

        m_iState = STATE_BOSS_TOWER_IDLE;
    }
}

void CBoss_Tower::Setting_MortalReady(_float _fTimeDelta)
{
    m_fMortalCoolTime -= _fTimeDelta;

    if (m_fMortalCoolTime <= 0.0f)
    {
        m_fMortalCoolTime = 0.5f;

        m_iState = STATE_BOSS_MORTAL_SHOT_OUT;
    }
}

void CBoss_Tower::Setting_MortalShotState(_float _fTimeDelta)
{
    m_fMortalShootTime -= _fTimeDelta;

    _float fInterval = 3.0f / 3.0f;         // 3초 동안 3발 발사 -> 5발 하려면 3 / 5 로 ㄱㄱ 
    static _uint iMaxBezierShots = 3;       // 최대 벨져 미사일 개수

    if (m_iBezierShootCount < iMaxBezierShots && m_fMortalShootTime <= (3.0f - (m_iBezierShootCount + 1) * fInterval))
    {
        m_pBossExplosion->Set_Activate_Effect(true);

        Activate_BezierMissile();
        m_iBezierShootCount++;
    }

    if (m_fMortalShootTime <= 0.0f)
    {
        m_fMortalShootTime = 3.0f;
        m_iBezierShootCount = 0;
        m_pBossExplosion->DeActivate_Effect();
        m_iState = STATE_BOSS_TOWER_IDLE;
    }
}

void CBoss_Tower::Setting_IntroState(_float _fTimeDelta)
{ 
    CUINumber_Manager* pNumberManager = CUINumber_Manager::GetInstance();

    static _float fCurrentFov = XMConvertToRadians(60.0f); 
    constexpr _float fLastFov = XMConvertToRadians(40.0f); 
    _float fSpeed = 3.5f; 

    fCurrentFov += (fLastFov - fCurrentFov) * fSpeed * _fTimeDelta;

    if (fabs(fCurrentFov - fLastFov) < 0.01f)
    {
        fCurrentFov = fLastFov;
    }

    m_pPlayerBar->Set_Visible(false);
    m_pPlayerHpBarBackGround->Set_Visible(false);
    m_pBossHpBar->Set_Visible(false);
    m_pBossHpBarBackGround->Set_Visible(false);
    m_pCrossHair->Set_Visible(false);
    pNumberManager->Set_UINumberVisible(false);

  
    m_pBossIntro->Activate_BossIntroScreen();
    m_pPlayer_Camera->Set_Fov(fCurrentFov);
    m_pGameInstance->Set_TimeDelta(TEXT("Timer_60"), 0.4f);

    if (m_pBody_Boss->IsAnimationFinished())
    {
        CFade_Manager::GetInstance()->Start_FadeIn(2.0f);

        m_pBossIntro->DeActivate_BossIntroScreen();

        m_pGameInstance->Set_TimeDelta(TEXT("Timer_60"), 1.0f);

        CFade_Manager::GetInstance()->Start_FadeOut(2.0f);

        m_pPlayerBar->Set_Visible(true);
        m_pPlayerHpBarBackGround->Set_Visible(true);
        m_pBossHpBar->Set_Visible(true);
        m_pBossHpBarBackGround->Set_Visible(true);
        m_pCrossHair->Set_Visible(true);
        pNumberManager->Set_UINumberVisible(true);

        m_iState = STATE_BOSS_TOWER_IDLE;

        fCurrentFov = XMConvertToRadians(60.0f);
        m_pPlayer_Camera->Set_Fov(fCurrentFov);


        m_pGameInstance->PlayBGM(L"bgm.wav", 0.5f);
    }
}

void CBoss_Tower::Activate_Missile()
{
    for (auto& pMissile : m_listMissile)
    {
        if (!pMissile->IsActiveBullet())
        {
            _vector vMuzzlePos = XMLoadFloat4x4(&m_pBoss_Mortal->Get_CombinedMatrix()).r[3];
            _vector vMuzzleUp = XMLoadFloat4x4(&m_pBoss_Mortal->Get_CombinedMatrix()).r[1];

            m_pGameInstance->Play(L"ST_Goliath_Mortar_Cast.wav", MONSTER_SHOOT, 0.25f);
            pMissile->Activate_Missile(vMuzzlePos, vMuzzleUp);

            return;
        }
    }
}

void CBoss_Tower::Activate_FireThrower()
{
    if (nullptr == m_pFire_Effect || nullptr == m_pBoss_Fire)
        return;

    _vector vMuzzlePos = XMLoadFloat4x4(&m_pBoss_Fire->Get_CombinedMatrix()).r[3];
    _vector vFireDirection = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

    _vector vAdjustedMuzzlePos = vMuzzlePos + (vFireDirection * 0.8f);

    _vector vFireRoadPos = vAdjustedMuzzlePos;

    m_pFire_Effect->Activate(vAdjustedMuzzlePos, vFireDirection);


    for (auto& pFireRoad : m_vecFireRoad)
    {
        if (!pFireRoad->IsActive() && !m_bFiringOnce)
        {
            pFireRoad->Set_Position(XMVectorSetY(vFireRoadPos + (vFireDirection * 10.0f), 0.1f), vFireDirection);

            m_pGameInstance->PlayOnce(L"ST_DrTurret_FlameThrower_Loop.wav", MONSTER_SHOOT, 0.25f);
            pFireRoad->Set_Active(true);

            m_bFiringOnce = true;

            return;
        }
        else
            continue;
    }
}

void CBoss_Tower::DeActivate_FireThrower()
{
    if (nullptr == m_pFire_Effect)
        return;

    m_pFire_Effect->Deactivate();
}

void CBoss_Tower::Activate_BezierMissile()
{
    for (auto& pMissile : m_listMissile)
    {
        if (!pMissile->IsActiveBullet())
        {
            _vector vMuzzlePos = XMLoadFloat4x4(&m_pBoss_Mortal->Get_CombinedMatrix()).r[3];

            m_pGameInstance->Play(L"ST_Goliath_Mortar_Cast.wav", MONSTER_SHOOT, 0.25f);
            pMissile->Activate_BezierMissile(vMuzzlePos);

            return;
        }
    }
}

void CBoss_Tower::Activate_Laser()
{
    _vector vMuzzlePos = XMLoadFloat4x4(&m_pBoss_Fire->Get_CombinedMatrix()).r[3];
    _vector vLaserDirection = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
    _vector vAdjustedMuzzlePos = vMuzzlePos + (vLaserDirection * 0.8f);

    for (auto& pLaser : m_listLaser)
    {
        //ST_DrTurret_FlameThrower_Loop
        m_pGameInstance->PlayOnce(L"ST_DrTurret_FlameThrower_Loop.wav", MONSTER_SHOOT, 0.2f);
        pLaser->Activate_Laser(vAdjustedMuzzlePos, vLaserDirection);
    }
}

void CBoss_Tower::DeActivate_Laser()
{
    for (auto& pLaser : m_listLaser)
        pLaser->DeActivate_Missile();
}

void CBoss_Tower::Activate_Barrier()
{
    _vector vFingerPos = XMLoadFloat4x4(&m_pBossFinger->Get_CombinedMatrix()).r[3];
    _vector vBarrierDirection = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

    for (auto& pBarrier : m_listBarrier)
    {
        if (!pBarrier->IsActiveBarrier() && !m_bBarrieringOnce)
        {
            _vector AdjustPosition = XMVectorSetY(vFingerPos + vBarrierDirection * 3.0f, 5.0f);

            //ST_DrTurret_Saw_Loop
            m_pGameInstance->PlayOnce(L"ST_DrTurret_Blast_Shoot.wav", MONSTER_SHOOT, 0.1f);
            pBarrier->Activate_Barrier(AdjustPosition, vBarrierDirection);

            m_bBarrieringOnce = true;
            return;
        }
    }
}

void CBoss_Tower::DeActivate_Barrier()
{
    for (auto& pBarrier : m_listBarrier)
        pBarrier->DeActivate_Barrier();
}

_bool CBoss_Tower::Check_Collision_With_Ray(const _vector& _vRayStart, const _vector& _vRayDirection, _float& _fDistance)
{
    if (nullptr == m_pColliderCom[COLL_AABB])
        return false;

    return m_pColliderCom[COLL_AABB]->Intersect_Ray(_vRayStart, _vRayDirection, _fDistance);
}

_bool CBoss_Tower::Check_Collision_Barrier(const _vector& _vRayStart, const _vector& _vRayDirection, _float& _fDistance)
{
    for (auto& pBarrier : m_listBarrier)
    {
        if (pBarrier->IsActiveBarrier())
        {
            if (true == pBarrier->Check_Collision_With_Ray(_vRayStart, _vRayDirection, _fDistance))
            {
                pBarrier->Activate_HittedEffect();

                return true;
            }
            else
                continue;
        }
    }
    return false;
}

void CBoss_Tower::Be_Hited(_uint _iCurrentGunDamage)
{
    _uint iDamageValue = 10;

    m_fBossHp = max(0.0f, m_fBossHp - iDamageValue);

    if (m_pBossHpBar)
        m_pBossHpBar->Decrease_Hp(iDamageValue);

    if (m_pBody_Boss)
        m_pBody_Boss->Activate_HittedEffect();

    if (m_fBossHp <= 0 && !m_pBody_Boss->IsDissolving())
    {
        m_pBody_Boss->Activate_DissolveEffect();
    }
}

void CBoss_Tower::Barrier_Hited()
{
    for (auto& pBarrier : m_listBarrier)
    {
        if (pBarrier->IsActiveBarrier())
        {
        }
    }
}

HRESULT CBoss_Tower::Ready_Component()
{
    /* Com_Navigation */
    CNavigation::NAVIGATION_DESC		NaviDesc{};

    NaviDesc.iCurrentCellIndex = 0;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;

    /* Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
    ColliderDesc.vExtents = _float3(4.0f, 4.0f, 4.0f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y + 4.0f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom[COLL_AABB]), &ColliderDesc)))
        return E_FAIL;


    CBounding_Sphere::BOUNDING_SPHERE_DESC  ColliderSphereDesc{};
    ColliderSphereDesc.fRadius = 30.0f;
    ColliderSphereDesc.vCenter = _float3(0.f, Compute_Monster_Position_Terrain(m_pTransformCom->Get_State(CTransform::STATE_POSITION)), 0.f);
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider_SPHERE"), reinterpret_cast<CComponent**>(&m_pColliderCom[COLL_SPHERE]), &ColliderSphereDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CBoss_Tower::Ready_PartObjects()
{
#pragma region BODY_BOSS
    // BODY_BOSS을 만들자. 
    CBody_Boss_Tower::BODY_BOSS_DESC       BodyDesc{};
    BodyDesc.pParentState = &m_iState;
    BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    BodyDesc.fSpeedPerSec = 0.0f;
    BodyDesc.fRotationPerSec = 0.0f;

    if (FAILED(__super::Add_PartObject(TEXT("Part_BossBody"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Body_RoboBoss"), &BodyDesc)))
        return E_FAIL;

    m_pBody_Boss = static_cast<CBody_Boss_Tower*>(Find_PartObject(TEXT("Part_BossBody")));
    if (nullptr == m_pBody_Boss)
        return E_FAIL;
#pragma endregion

#pragma region Boss_Mortal
    // Boss_Mortal 을 만들자.
    CBoss_Mortal::BOSS_MORTAL_DESC          MortalDesc{};

    CModel* pBodyModelCom = dynamic_cast<CModel*>(__super::Find_PartObject_Component(TEXT("Part_BossBody"), TEXT("Com_Model")));
    if (nullptr == pBodyModelCom)
        return E_FAIL;

    MortalDesc.pSocketMatrix = pBodyModelCom->Get_BoneMatrix("Mortar_Body");
    MortalDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    if (FAILED(__super::Add_PartObject(TEXT("Part_BossMortal"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BossMortal"), &MortalDesc)))
        return E_FAIL;

    m_pBoss_Mortal = static_cast<CBoss_Mortal*>(Find_PartObject(TEXT("Part_BossMortal")));
    if (nullptr == m_pBoss_Mortal)
        return E_FAIL;

#pragma endregion

#pragma region Boss_Mortal_Missile
    // Mortal Missile 을 만들자.
    for (_uint i = 0; i < 50; ++i)
    {
        CMissile_Boss::MISSILE_BOSS_DESC MissileDesc{};
        CMissile_Boss* pMissile = dynamic_cast<CMissile_Boss*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Missile_Boss"), &MissileDesc));

        if (!pMissile)
            return E_FAIL;

        m_listMissile.push_back(pMissile);
    }

#pragma endregion 

#pragma region Boss_Mortal_Explosion_Effect
    CBoss_Explosion::BOSS_EXPLO_DESC BossExplosionDesc = {};
    BossExplosionDesc.pSocketMatrix = pBodyModelCom->Get_BoneMatrix("Mortar_Body");
    BossExplosionDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    if (FAILED(__super::Add_Effect_PartObject(TEXT("Part_BossExplosion"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BossExplosion"), &BossExplosionDesc)))
        return E_FAIL;

    m_pBossExplosion = static_cast<CBoss_Explosion*>(Find_Effect_PartObject(TEXT("Part_BossExplosion")));
    if (nullptr == m_pBossExplosion)
        return E_FAIL;

#pragma endregion



#pragma region Boss_FIRE ARM
    // Boss_FIRE 을 만들자.
    CBoss_Fire::BOSS_FIRE_DESC          FireArmDesc{};
    FireArmDesc.pSocketMatrix = pBodyModelCom->Get_BoneMatrix("R_Canon_IK");
    FireArmDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    if (FAILED(__super::Add_PartObject(TEXT("Part_BossFire"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BossFire"), &FireArmDesc)))
        return E_FAIL;

    m_pBoss_Fire = static_cast<CBoss_Fire*>(Find_PartObject(TEXT("Part_BossFire")));
    if (nullptr == m_pBoss_Fire)
        return E_FAIL;
#pragma endregion


#pragma region FIRE_EFFECT
    // FIRE_EFFECT 를 만들자. 
    CParticle_Fire::PARTICLE_FIRE_DESC FireEffect{};
    FireEffect.pSocketMatrix = pBodyModelCom->Get_BoneMatrix("R_Canon_IK");
    FireEffect.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    if (FAILED(__super::Add_PartObject(TEXT("Part_BossFireEffect"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Particle_BossFire"), &FireEffect)))
        return E_FAIL;

    m_pFire_Effect = static_cast<CParticle_Fire*>(Find_PartObject(TEXT("Part_BossFireEffect")));
    if (nullptr == m_pFire_Effect)
        return E_FAIL;
#pragma endregion


#pragma region FIRE_ROAD
    // FIRE_ROAD 를 준비해놓자
    CFireRoad::FIRE_ROAD_DESC FireRoadDesc = {};
    for (_uint i = 0; i < 4; ++i)
    {
        CFireRoad::FIRE_ROAD_DESC FireRoadDesc = {};
        CFireRoad* pFireRoad = dynamic_cast<CFireRoad*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_FireRoad"), &FireRoadDesc));

        if (!pFireRoad)
            return E_FAIL;

        m_vecFireRoad.push_back(pFireRoad);

        m_pFireRoad = pFireRoad;
    }

#pragma endregion

#pragma region LASER_BOSS
    CLaser_Boss::LASER_BOSS_DESC LaserDesc = {};

    CLaser_Boss* pLaser = dynamic_cast<CLaser_Boss*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Boss_Laser"), &LaserDesc));

    if (!pLaser)
        return E_FAIL;

    m_listLaser.push_back(pLaser);
#pragma endregion

#pragma region BOSS_FINGER
    // BossFinger 를 만들자.
    CBossFinger::FINGER_DESC FingerDesc{};
    FingerDesc.pSocketMatrix = pBodyModelCom->Get_BoneMatrix("L_Index_03_end");
    FingerDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    if (FAILED(__super::Add_PartObject(TEXT("Part_BossFinger"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BossFinger"), &FingerDesc)))
        return E_FAIL;

    m_pBossFinger = static_cast<CBossFinger*>(Find_PartObject(TEXT("Part_BossFinger")));
    if (nullptr == m_pBossFinger)
        return E_FAIL;
#pragma endregion

#pragma region BOSS_BARRIER
    // Barrier 를 만들자 
    for (_uint i = 0; i < 4; ++i)
    {
        CBoss_Barrier::BARRIER_DESC BarrierDesc = {};
        //wtring strBarrierName = L""

        CBoss_Barrier* pBarrier = dynamic_cast<CBoss_Barrier*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Barrier"), &BarrierDesc));

        if (!pBarrier)
            return E_FAIL;

        m_listBarrier.push_back(pBarrier);
    }
#pragma endregion
    return S_OK;
}

CMonster* CBoss_Tower::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBoss_Tower* pInstance = new CBoss_Tower(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CBoss_Tower");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CBoss_Tower::Clone(void* _pArg)
{
    CBoss_Tower* pInstance = new CBoss_Tower(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CBoss_Tower");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBoss_Tower::Free()
{
    __super::Free();

    for (auto& pMissile : m_listMissile)
        Safe_Release(pMissile);
    m_listMissile.clear();

    for (auto& pFireRoad : m_vecFireRoad)
        Safe_Release(pFireRoad);
    m_vecFireRoad.clear();


    for (auto& pLaser : m_listLaser)
        Safe_Release(pLaser);
    m_listLaser.clear();


    for (auto& pBarrier : m_listBarrier)
        Safe_Release(pBarrier);
    m_listBarrier.clear();

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pColliderCom[COLL_AABB]);
    Safe_Release(m_pColliderCom[COLL_SPHERE]);
}
