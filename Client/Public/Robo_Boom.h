#pragma once
#include "Client_Defines.h"
#include "Monster.h"
#include "Body_Robo_Boom.h"
#include "Missile_Boom.h"
#include "HpBar_Robo_Boom.h"

#include "UINumber_Manager.h"
#include "DamageNumber.h"

#include "Dying_Explode.h"

BEGIN(Engine)
class CNavigation;
class CCollider;
END

BEGIN(Client)
class CRobo_Boom final : public CMonster
{
    enum COLLIDERTYPE { COLL_AABB, COLL_OBB, COLL_SPHERE, COLL_END };

    enum ROBOBOOM_STATE {
        STATE_IDLE = 0x00000001,
        STATE_RUN = 0x00000002,
        STATE_SHOOT = 0x00000003,
        STATE_STUN = 0x00000004
    };

public:
    struct ROBO_BOOM_DESC final : public CGameObject::GAMEOBJECT_DESC
    {
        _float3 vMonsterpos;
        _float3 vMonsterRotation;
        _float3 vMonsterScale;
    };

private:
    CRobo_Boom(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CRobo_Boom(const CMonster& _Prototype);
    virtual ~CRobo_Boom() = default;

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
    void                        Setting_ShootState(_float _fTimeDelta);
    void                        Setting_StunState(_float _fTimeDelta);
    void                        Shoot_Missiles(_float _fTimeDelta);
    void                        Late_Update_Missiles(_float _fTimeDelta);
    void                        Robo_Boom_MoveState();

    void                        SelfMove_Randomly();
private:
    CNavigation*                m_pNavigationCom = { nullptr };
    CCollider*                  m_pColliderCom[COLL_END] = {nullptr};
public:
    virtual HRESULT             Ready_Component();
    virtual HRESULT             Ready_PartObjects();

public:
    _bool                       Check_Collision_With_Ray(const _vector& _vRayStart, const _vector& _vRayDirection, _float& _fDistance);
    void                        Be_Hited(_uint _iCurrentGunDamage);
    void                        Activate_Missile();
    void                        Setting_BillBoard();


private:
    _uint						m_iState = {};

private:
    CBody_Robo_Boom*            m_pBody_Robo_Boom = {nullptr};
    CHpBar_Robo_Boom*           m_pHpBar_Robo_Boom = { nullptr };
    CDamageNumber*              m_pDamageNumber[3] = {nullptr, nullptr, nullptr };

    CDying_Explode*             m_pDyingExplosion = { nullptr };
private:
    //_uint                       m_iHp = { };
    _int                       m_iHp = { };
    _bool                       m_bDetect_Target = { false };

private:
    _float                      m_fStunTime = {};
    _float                      m_fIdleTime = {};
    _float                      m_fShootCoolTime = {};
    _float                      m_fSelfMoveCoolTime = {};
    _vector                     m_vRandomPosition = {};
    _int                        m_iPrevState = 0;         
    _bool                       m_bIsStunned = false;   

    _float                      m_fMissile_Height_Offset = {};
    _float                      m_fMissile_Width_Offset = {};


private:
    _float m_fCurrentHp = 100.0f;  // 현재 체력 (기본값 100)
    _float m_fMaxHp = 100.0f;      // 최대 체력 (기본값 100)


private:
    bool m_bShowDamageText = false; 
    _wstring m_DamageText;               // 표시할 데미지 텍스트
    _float m_fDamageTextTime = 0.0f;     // 텍스트 표시 남은 시간
    _float3 m_DamageTextPosition;        // 텍스트 월드 좌표

private:
    _bool           m_bDying = false;
    _float          m_fDyingTime = 0;

private:
    list<CMissile_Boom*>        m_listMissile;

private:
    map<_uint, _float> m_mapDamageUI; 
public:
    static CMonster*            Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*        Clone(void* _pArg) override;
    virtual void                Free() override;
};
END
