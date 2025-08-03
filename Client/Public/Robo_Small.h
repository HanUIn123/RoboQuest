#pragma once
#include "Client_Defines.h"
#include "Monster.h"
#include "Body_Robo_Small.h"
#include "Small_Explosion.h"

#include "HpBar_Robo_Small.h"
#include "DamageNumber.h"

BEGIN(Engine)
class CNavigation;
class CCollider;
END

BEGIN(Client)
class CRobo_Small  final :public CMonster
{
    enum COLLIDERTYPE { COLL_AABB, COLL_OBB, COLL_SPHERE, COLL_END };

    enum ROBO_SMALL_STATE
    {
        STATE_SMALL_IDLE = 0x00000001,
        STATE_SMALL_SHOOT = 0x00000002,
        STATE_SMALL_SHAKE = 0x00000003,
        STATE_SMALL_RUN = 0x00000004,
        STATE_SMALL_DETECT = 0x00000005
    };

public:
    struct ROBO_SMALL_DESC final : public CGameObject::GAMEOBJECT_DESC
    {
        _float3 vMonsterpos;
        _float3 vMonsterRotation;
        _float3 vMonsterScale;
    };

private:
    CRobo_Small(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CRobo_Small(const CMonster& _Prototype);
    virtual ~CRobo_Small() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;

    virtual void                Priority_Update(_float _fTimeDelta)override;
    virtual void                Update(_float _fTimeDelta)override;
    virtual void                Late_Update(_float _fTimeDelta)override;

    virtual HRESULT             Render()override;

public:
    void SetActive(bool bActive) { m_bActive = bActive; }
    bool IsActive() const { return m_bActive; }

private:
    void                        Robo_Small_MoveState(_float _fTimeDelta);
    void                        Setting_IdleState(_float _fTimeDelta);
    void                        Setting_RunState(_float _fTimeDelta);
    void                        Setting_ShootState(_float _fTimeDelta);
    void                        Setting_ShakeState(_float _fTimeDelta);
    void                        Setting_DetectState(_float _fTimeDelta);

    void                        SelfMove_Randomly();
    //void                        Shoot_Missile(_float _fTimeDelta);

public:
    _bool                       Check_Collision_With_Ray(const _vector& _vRayStart, const _vector& _vRayDirection, _float& _fDistance);
    void                        Be_Hited(_uint _iCurrentGunDamage);

private:
    CNavigation*                m_pNavigationCom = { nullptr };
    CCollider*                  m_pColliderCom[COLL_END] = { nullptr };

    _uint						m_iState = {};
    _vector                     m_vRandomPosition = {};

private:
    CBody_Robo_Small*           m_pBody_Robo_Small = { nullptr };
    CSmall_Explosion*           m_pSmallExplosion = { nullptr };
    CHpBar_Robo_Small*          m_pHpBar_Small = { nullptr };
    CDamageNumber*              m_pDamageNumber[3] = { nullptr, nullptr, nullptr };
    //_uint                       m_iHp = {};
    _int                       m_iHp = { };
private:
    _bool                       m_bDetect_Target = { false };
    _bool                       m_bSuicideMode = { false };

    _bool                       m_bSuicided = { false };


    _float                      m_fIdleTime = {};
    _float                      m_fSelfMoveCoolTime = {};
    _float                      m_fSuicideTime = {};

    _bool                       m_bActive = { true };

    _float                      m_fShakeTimer = {};
    _float                      m_fDetectTimer = {};

public:
    virtual HRESULT             Ready_Component();
    virtual HRESULT             Ready_PartObjects();

public:
    static CMonster*            Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*        Clone(void* _pArg) override;
    virtual void                Free() override;
};

END