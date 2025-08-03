#pragma once
#include "Client_Defines.h"
#include "Monster.h"
#include "Body_Robo_Goliath.h"
#include "Mortal.h"

#include "Missile_Goliath.h"
#include "Missile_Mortal.h"

#include "Mortal_Explosion.h"
#include "HpBar_Robo_Goliath.h"

#include "DamageNumber.h"

//#include "MortalImpact.h"

#include "BreakEffect.h"

BEGIN(Engine)
class CNavigation;
class CCollider;
END

BEGIN(Client)
class CRobo_Goliath final : public CMonster
{
    enum COLLIDERTYPE { COLL_AABB, COLL_OBB, COLL_SPHERE, COLL_END };

    enum ROBO_GOLIATH_STATE 
    {
        STATE_GOL_DASH =            0x00000001,
        STATE_GOL_MISSILE_SHOT =    0x00000002,
        STATE_GOL_IDLE =            0x00000003,
        STATE_GOL_MORTAL_IN =       0x00000004,
        STATE_GOL_MORTAL_LOOP =     0x00000005,
        STATE_GOL_MORTAL_OUT =      0x00000006,
        STATE_GOL_MORTAL_SHOOT =    0x00000007,
        STATE_GOL_RUN_FRONT =       0x00000008,
        STATE_GOL_RUN_LEFT =        0x00000009,
        STATE_GOL_RUN_RIGHT =       0x0000000A,
        STATE_GOL_STUN =            0x0000000B
    };

public:
    struct ROBO_GOLIATH_DESC final : public CGameObject::GAMEOBJECT_DESC
    {
        _float3 vMonsterpos;
        _float3 vMonsterRotation;
        _float3 vMonsterScale;
    };

private:
    CRobo_Goliath(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CRobo_Goliath(const CMonster& _Prototype);
    virtual ~CRobo_Goliath() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;

    virtual void                Priority_Update(_float _fTimeDelta)override;
    virtual void                Update(_float _fTimeDelta)override;
    virtual void                Late_Update(_float _fTimeDelta)override;

    virtual HRESULT             Render()override;

private:
    //void                        Update_MovementState();
    void                        Setting_IdleState(_float _fTimeDelta);
    void                        Setting_RunState(_float _fTimeDelta);
    void                        Setting_DashState(_float _fTimeDelta);
    void                        Setting_ShootState(_float _fTimeDelta);
    void                        Setting_Ready_MortalShoot(_float _fTimeDelta);
    void                        Setting_Fire_MortalShoot(_float _fTimeDelta);
    void                        Setting_StunState(_float _fTimeDelta);
    void                        Shoot_Missiles(_float _fTimeDelta);
    void                        Shoot_MortalMissiles(_float _fTimeDelta, _vector _fTargetPosition = {});
    void                        Late_Update_Missiles(_float _fTimeDelta);
    void                        Late_Update_MortalMissiles(_float _fTimeDelta);
    void                        Robo_Goliath_MoveState(_float _fTimeDelta);

    void                        SelfMove_Randomly();
private:
    CNavigation*                m_pNavigationCom = { nullptr };
    CCollider*                  m_pColliderCom[COLL_END] = { nullptr };
    CCollider*                  m_pTargetCollider = { nullptr };

public:
    virtual HRESULT             Ready_Component();
    virtual HRESULT             Ready_PartObjects();

public:
    _bool                       Check_Collision_With_Ray(const _vector& _vRayStart, const _vector& _vRayDirection, _float& _fDistance);
    void                        Be_Hited(_uint _iCurrentGunDamage);
    void                        Activate_Missile();
    void                        Activate_MortalMissile();

private:
    _uint						m_iState = {};

private:
    CBody_Robo_Goliath*         m_pBody_Robo_Goliath = { nullptr };
    CMortal*                    m_pMortal = { nullptr };
    CMortal_Explosion*          m_pMortalExplosion = { nullptr };
    CHpBar_Robo_Goliath*        m_pHpBar_Robo_Goliath = { nullptr };
    CDamageNumber*              m_pDamageNumber[3] = { nullptr, nullptr, nullptr };
    //CMortalImpact*              m_pMortalImpact = { nullptr };

    CBreakEffect*               m_pBreakEffect = { nullptr };
private:
    //_uint                       m_iHp = { };
    _int                       m_iHp = { };
    _bool                       m_bDetect_Target = { false };

private:
    _float                      m_fStunTime = {};
    _float                      m_fIdleTime = {};
    _float                      m_fShootCoolTime = {};
    _float                      m_fReadyMortalTime = {};
    _float                      m_fSelfMoveCoolTime = {};
    _float                      m_fDashTime = {};
    _float                      m_fMortalShootTime = {};
    _vector                     m_vRandomPosition = {};
    _int                        m_iPrevState = 0;
    _bool                       m_bIsStunned = { false };

    _float                      m_fMissile_Height_Offset = {};
    _float                      m_fMissile_Width_Offset = {};

    _bool                       m_bDash_Mode = { false };
    _bool                       m_bMortal_Mode = { false };


    _vector                     Compute_RightArm_Muzzle();
    _vector                     Compute_Shoulder_Muzzle();

private:
    _vector                     m_vRightArmMuzzle = {};


private:
    list<CMissile_Goliath*>         m_listMissile;
    list<CMissile_Mortal*>          m_listMortal_Missile;

    //vector<CMortalImpact*>          m_vecMortalImpact;

public:
    static CMonster*            Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*        Clone(void* _pArg) override;
    virtual void                Free() override;
};
END