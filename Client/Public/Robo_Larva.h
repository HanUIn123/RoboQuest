#pragma once
#include "Client_Defines.h"
#include "Monster.h"
#include "Body_Robo_Larva.h"
#include "Laser_Larva.h"
#include "Player_Camera.h"
#include "HpBar_Robo_Larva.h"
#include "DamageNumber.h"


BEGIN(Engine)
class CNavigation;
class CCollider;
END

BEGIN(Client)
class CRobo_Larva final : public CMonster
{
    enum COLLIDERTYPE { COLL_AABB, COLL_OBB, COLL_SPHERE, COLL_END };

    enum ROBO_LARVA_STATE
    {
        STATE_LAR_CLAP = 0x00000001,
        STATE_LAR_NULL = 0x00000002,
        STATE_LAR_NULL2 = 0x00000003,
        STATE_LAR_IDLE = 0x00000004,
        STATE_LAR_LASER_READY = 0x00000005,
        STATE_LAR_LASER_LOOP = 0x00000006,
        STATE_LAR_BACKSHOT_READY = 0x00000007,
        STATE_LAR_BACKSHOT_LOOP = 0x00000008,
        STATE_LAR_SPRINT_READY = 0x00000009,
        STATE_LAR_SPRINT_DON = 0x0000000A,
        STATE_LAR_MOVE = 0x0000000B
    };

public:
    struct ROBO_LARVA_DESC final : public CGameObject::GAMEOBJECT_DESC
    {
        _float3 vMonsterpos;
        _float3 vMonsterRotation;
        _float3 vMonsterScale;
    };

private:
    CRobo_Larva(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CRobo_Larva(const CMonster& _Prototype);
    virtual ~CRobo_Larva() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;

    virtual void                Priority_Update(_float _fTimeDelta)override;
    virtual void                Update(_float _fTimeDelta)override;
    virtual void                Late_Update(_float _fTimeDelta)override;

    virtual HRESULT             Render()override;

private:

    void                        Robo_Larva_MoveState(_float _fTimeDelta);
    void                        Setting_IdleState(_float _fTimeDelta);
    void                        Setting_RunState(_float _fTimeDelta);
    //void                        Setting_ClapState(_float _fTimeDelta);
    //void                        Setting_Ready_LaserState(_float _fTimeDelta);
    //void                        Setting_Fire_LaserState(_float _fTimeDelta);
    //void                        Setting_Ready_ShootState(_float _fTimeDelta);
    void                        Setting_Fire_ShootState(_float _fTimeDelta);
    //void                        Setting_Ready_SprintState(_float _fTimeDelta);
    //void                        Setting_End_SprintState(_float _fTimeDelta);
    //
    void                        SelfMove_Randomly();
    void                        Fire_Laser(_float _fTimeDelta);
    void                        Late_Update_Laser(_float _fTimeDelta);

    _vector                     Compute_LaserHeight_Offset();


private:
    CNavigation*                m_pNavigationCom = { nullptr };
    CCollider*                  m_pColliderCom[COLL_END] = { nullptr };

public:
    virtual HRESULT             Ready_Component();
    virtual HRESULT             Ready_PartObjects();

    void                        Activate_Laser();

public:
    _bool                       Check_Collision_With_Ray(const _vector& _vRayStart, const _vector& _vRayDirection, _float& _fDistance);
    void                        Be_Hited(_uint _iCurrentGunDamage);

private:
    _bool                       m_bDetect_Target = { false };
    _bool                       m_bLaser_Mode = { false };


    _uint						m_iState = {};
    _vector                     m_vRandomPosition = {};


    _float                      m_fIdleTime = {};
    _float                      m_fSelfMoveCoolTime = {};

    _float                      m_fReadyLaserTime = {};
    _float                      m_fFireLaserTime = {};

private:
    _float m_fLaserCoolTime = {}; 
    _float m_fLaserFireInterval = {}; 
private:
    //_uint                       m_iHp = { };
    _int                       m_iHp = { };
    CDamageNumber* m_pDamageNumber[3] = { nullptr, nullptr, nullptr };


private:
    CBody_Robo_Larva*           m_pBody_Robo_Larva = { nullptr };
    list<CLaser_Larva*>         m_listLaser;
    CPlayer_Camera*             m_pPlayerCamera = { nullptr };

    CHpBar_Robo_Larva*          m_pHpBar_Robo_Larva = { nullptr };

public:
    static CMonster*            Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*        Clone(void* _pArg) override;
    virtual void                Free() override;
};
END

