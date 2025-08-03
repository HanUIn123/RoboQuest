#pragma once
#include "Client_Defines.h"
#include "Body_Boss_Tower.h"
#include "Monster.h"
#include "BossHpBar.h"
#include "Boss_Mortal.h"
#include "Missile_Boss.h"
#include "Boss_Barrier.h"

#include "Boss_Fire.h"
#include "Particle_Fire.h"


#include "FireRoad.h"
#include "Fire_Frame.h"

#include "Laser_Boss.h"

#include "Boss_Explosion.h"

#include "BossFinger.h"

#include "Player_Camera.h"

#include "Fade_Manager.h"

#include "BossIntroScreen.h"

#include "PlayerHpbar.h"
#include "HpBarBackGround.h"
#include "BossHpBarBackGround.h"
#include "CrossHair.h"
#include "UINumber_Manager.h"

BEGIN(Engine)
class CNavigation;
class CCollider;
END

BEGIN(Client)
class CPlayer;
class CBoss_Tower final : public CMonster
{
    enum COLLIDERTYPE { COLL_AABB, COLL_OBB, COLL_SPHERE, COLL_END };

    enum ROBO_BOSS_STATE
    {
        STATE_BOSS_IDLE = 0x00000001,
        STATE_BOSS_IN = 0x00000002,
        STATE_BOSS_OUT = 0x00000003,
        STATE_BOSS_CANON_PRESHOT = 0x00000004,
        STATE_BOSS_CANON_SHOT = 0x00000005,
        STATE_BOSS_CANON_SHOT_LOOP = 0X00000006,
        STATE_BOSS_FIRETHORW_IDLE = 0X00000007,
        STATE_BOSS_FIRETHORW_IN = 0X00000008,
        STATE_BOSS_FIRETHORW_OUT = 0X00000009,
        STATE_BOSS_FIRETHORW_PRESHOT = 0X0000000A,
        STATE_BOSS_FIRETHORW_SHOT = 0X0000000B,
        STATE_BOSS_HANDBIM_IN = 0X0000000C,
        STATE_BOSS_HANDBIM_OUT = 0X0000000D,
        STATE_BOSS_HANDBIM_SHOOTCLOSE = 0X0000000E,
        STATE_BOSS_HANDBIM_SHOOTOPEN = 0X0000000F,
        STATE_BOSS_TOWER_IDLE = 0X00000010,
        STATE_BOSS_TOWER_INTRO = 0X00000011,
        STATE_BOSS_MORTAL_SHOT = 0X00000012,
        STATE_BOSS_MORTAL_SHOT_OUT = 0X00000013,
        STATE_BOSS_END = 0x00000014
    };

public:
    struct ROBO_BOSS_DESC final : public CGameObject::GAMEOBJECT_DESC
    {
        _float3 vMonsterpos;
        _float3 vMonsterRotation;
        _float3 vMonsterScale;
    };

private:
    CBoss_Tower(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CBoss_Tower(const CMonster& _Prototype);
    virtual ~CBoss_Tower() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;

    virtual void                Priority_Update(_float _fTimeDelta)override;
    virtual void                Update(_float _fTimeDelta)override;
    virtual void                Late_Update(_float _fTimeDelta)override;

    virtual HRESULT             Render()override;

    void                        Activate_Boss();

private:
    void                        Robo_Boss_MoveState(_float _fTimeDelta);

    void                        Setting_IdleState(_float _fTimeDelta);

    void                        Setting_NormalShotState(_float _fTimeDelta);
    void                        Setting_NormalShotReady(_float _fTimeDelta);
    void                        Setting_NormalShotFinish(_float _fTimeDelta);

    void                        Setting_FireThrowReady(_float _fTimeDelta);
    void                        Setting_FireThrowState(_float _fTimeDelta);
    void                        Setting_FireThorwFinish(_float _fTimeDelta);

    void                        Setting_HandBimReady(_float _fTimeDelta);
    void                        Setting_HandBimState(_float _fTimeDelta);
    void                        Setting_HandBimFinsih(_float _fTimeDelta);


    void                        Setting_MortalReady(_float _fTimeDelta);
    void                        Setting_MortalShotState(_float _fTimeDelta);


    void                        Setting_IntroState(_float _fTimeDelta);


private:
    void                        Activate_Missile();

    void                        Activate_FireThrower();
    void                        DeActivate_FireThrower();

    void                        Activate_BezierMissile();

    void                        Activate_Laser();
    void                        DeActivate_Laser();


    void                        Activate_Barrier();
    void                        DeActivate_Barrier();


public:
    _bool                       Check_Collision_With_Ray(const _vector& _vRayStart, const _vector& _vRayDirection, _float& _fDistance);

    _bool                       Check_Collision_Barrier(const _vector& _vRayStart, const _vector& _vRayDirection, _float& _fDistance);

    void                        Be_Hited(_uint _iCurrentGunDamage);
    void                        Barrier_Hited();

private:
    CNavigation* m_pNavigationCom = { nullptr };
    CCollider* m_pColliderCom[COLL_END] = { nullptr };

    _uint						m_iState = {};

private:
    CBody_Boss_Tower* m_pBody_Boss = { nullptr };
    CBossHpBar* m_pBossHpBar = { nullptr };

    CPlayer*                    m_pPlayer = { nullptr };
    CPlayer_Camera*             m_pPlayer_Camera = { nullptr };

    CBossIntroScreen*           m_pBossIntro = { nullptr };

    CPlayerHpbar* m_pPlayerBar = { nullptr };
    CHpBarBackGround* m_pPlayerHpBarBackGround = { nullptr };
    CBossHpBarBackGround* m_pBossHpBarBackGround = { nullptr };
    CCrossHair* m_pCrossHair = { nullptr };




private:
    _bool                       m_bActivePattern = { false };
    _bool                       m_bDetect_Target = { false };
    _bool                       m_bIsVisible = { false };



    _bool                       m_bCanonMode = { false };
    _bool                       m_bFireThrowMode = { false };
    _bool                       m_bHandBimMode = { false };
    _bool                       m_bMortalMode = { false };
    _bool                       m_bIntroShow = { false };

    _float                      m_fIdleTime = {};
    _float                      m_fCanonShootTime = {};
    _float                      m_fCanonCoolTime = {};
    _float                      m_fCanonFinshTime = {};

    _uint                       m_iShootCount = {};
    _uint                       m_iBezierShootCount = {};
    _bool                       m_bShotTriggered = { false };



    _float                      m_fFireCoolTime = {};
    _float                      m_fFireShootTime = {};
    _float                      m_fFireFinishTime = {};


    _float                      m_fHandBimCoolTime = {};
    _float                      m_fHandBimShootTime = {};
    _float                      m_fHandBimFinishTime = {};

    _float                      m_fMortalCoolTime = {};
    _float                      m_fMortalShootTime = {};
    _float                      m_fMortalFinishTime = {};


    _float                      m_fBossHp = {};


    _bool                       m_bLerpToPlayer = { false };

private:
    _uint                       m_iCurrentPattern = {};

    CBoss_Mortal*               m_pBoss_Mortal = { nullptr };
    list<CMissile_Boss*>        m_listMissile;

    CFireRoad*                  m_pFireRoad = { nullptr };
    vector<CFireRoad*>          m_vecFireRoad;
    _bool                       m_bFiringOnce = { false };


    CFire_Frame*                m_pFireFrame = { nullptr };

    CBoss_Explosion*            m_pBossExplosion = { nullptr };

    CBossFinger*                m_pBossFinger = { nullptr };
    //CBoss_Barrier*              m_pBossBarrier = { nullptr };
    list <CBoss_Barrier*>       m_listBarrier;
    _bool                       m_bBarrieringOnce = { false };



private:
    CBoss_Fire*                 m_pBoss_Fire = { nullptr };
    CParticle_Fire*             m_pFire_Effect = { nullptr };
    _float                      m_fFIreAngle = {};

private:
    //CLaser_Boss*                m_pBoss_Laser = { nullptr };
    list<CLaser_Boss*>          m_listLaser;



public:
    virtual HRESULT             Ready_Component();
    virtual HRESULT             Ready_PartObjects();

public:
    static CMonster* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void                Free() override;
};

END