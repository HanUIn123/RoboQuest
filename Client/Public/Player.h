#pragma once
#include "Client_Defines.h"
#include "ContainerObject.h"
#include "Player_Camera.h"
#include "Particle_MuzzleFire.h"

#include "CrossHair.h"
#include "BasicTerrain.h"

#include "PlayerHpbar.h"
#include "Body_Player.h"

#include "HandGun.h"   
#include "Gun.h"
#include "WeaponItem.h"
#include "Rifle.h"
#include "Sniper.h"

#include "DashEffect.h"
#include "HurtEffect.h"
#include "BurnEffect.h"
#include "HealEffect.h"

#include "UIWeapon.h"
#include "UIInteraction.h"
#include "UILetters.h"
#include "ScopeScreen.h"

//#include "Boss_Barrier.h"
//#include "Boss_Tower.h"


BEGIN(Engine)
class CNavigation;
class CCollider;
END

BEGIN(Client)
class CBullet;
class CPlayer final : public CContainerObject
{
public:
    _uint Get_PlayerState() const { return m_iState; }
    void Set_PlayerState(_uint state) { m_iState = state; }

    _bool IsJumping() const { return m_bJumping; }
    void Set_Jumping(_bool jumping) { m_bJumping = jumping; }

    _float Get_JumpSpeed() const { return m_fJumpSpeed; }
    void Set_JumpSpeed(_float speed) { m_fJumpSpeed = speed; }

    _float3 Get_JumpHeight() const { return m_vJumping_Height_Position; }
    void Set_JumpHeight(_float3 height) { m_vJumping_Height_Position = height; }

    _bool IsShooting() const { return m_bIsShootingState; }
    void Set_Shooting(_bool shooting) { m_bIsShootingState = shooting; }

    _bool IsReloading() const { return m_bReLoading; }
    void Set_Reloading(_bool reloading) { m_bReLoading = reloading; }

    //_int Get_Health() const { return m_iHp; }
    //void Set_Health(_int hp) { m_iHp = hp; }

    _uint Get_AmmoCount() const { return m_iAmmoCount; }
    void Set_AmmoCount(_uint ammo) { m_iAmmoCount = ammo; }

    _float Get_ShootingAnimationTime() const { return m_fShootingAnimationTime; }
    void Set_ShootingAnimationTime(_float time) { m_fShootingAnimationTime = time; }

    void Set_PlayerPosition(_vector pos) { m_pTransformCom->Set_State(CTransform::STATE_POSITION, pos); }
    void Set_PlayerLook(_vector look) { m_pTransformCom->Set_State(CTransform::STATE_LOOK, look); }
    void Set_PlayerUp(_vector up) { m_pTransformCom->Set_State(CTransform::STATE_UP, up); }
    void Set_PlayerRight(_vector right) { m_pTransformCom->Set_State(CTransform::STATE_RIGHT, right); }

    _uint Get_HandGunCount() { return m_pHandGun->Get_HandGunBulletCount(); }


    // 현재 장전된 총알 수 (HandGun에서 가져옴)
    //_uint Get_CurrentAmmoInMagazine() const {return m_pHandGun ? m_pHandGun->Get_CurrentAmmoInMagazine() : 0;}
    _uint Get_CurrentAmmoInMagazine() const {return m_pCurrentGun ? m_pCurrentGun->Get_CurrentAmmoInMagazine() : 0;}

    // 전체 남은 총알 수 (HandGun에서 가져옴)
    //_uint Get_TotalAmmo() const {return m_pHandGun ? m_pHandGun->Get_TotalAmmo() : 0;}
    _uint Get_TotalAmmo() const {return m_pCurrentGun ? m_pCurrentGun->Get_TotalAmmo() : 0;}

public:
    enum PLAYER_STATE {
        //STATE_IDLE = 0x00000001,
        //STATE_WALK = 0x00000002,
        //STATE_DASH = 0x00000003,
        //STATE_SHOOT = 0x00000004,
        //STATE_RELOAD = 0x00000005

        //STATE_TWO_RELOAD = 0x00000006,


        STATE_SNIPER_ZOOMIN = 0x00000001,   // 0 - 스나이퍼 줌인
        STATE_SNIPER_ZOOMOUT = 0x00000002,  // 1 - 스나이퍼 줌아웃
        STATE_SNIPER_SHOOT = 0x00000003,    // 2 - 스나이퍼 슛

        STATE_RIFLE_SHOOT = 0x00000004,  // 3 - 라이플 쏘기
        STATE_RIFLE_IDLE = 0x00000005,   // 4 - 라이플 가만히
        STATE_RIFLE_WALK = 0x00000006,    // 5 - 라이플 달리기
        STATE_RIFLE_DASH = 0x00000007,    // 6 - 라이플 대쉬하기
        STATE_RIFLE_SWITCH = 0x00000008,    // 7 - 라이플 교체

        STATE_HANDGUN_IDLE = 0x00000009,  // 8 - 핸드건 가만히
        STATE_HANDGUN_WALK = 0x0000000A,  // 9 - 핸드건 걷기
        STATE_HANDGUN_DASH = 0x0000000B,  // 10 - 핸드건 대쉬
        STATE_HANDGUN_SWITCH = 0x0000000C,  // 11 - 핸드건 교체
        STATE_HANDGUN_SHOOT = 0x0000000D, // 12 - 핸드건 쏘기
        STATE_HANDGUN_RELOAD = 0x0000000E,  // 13 - 핸드건 장전
        STATE_RIFLE_RELOAD = 0x0000000F  // 14 - 라이플 장전
    };

public:
    _vector Get_PlayerUp() {
        return m_pTransformCom->Get_State(CTransform::STATE_UP);
    }

    _vector Get_PlayerLook() {
        return m_pTransformCom->Get_State(CTransform::STATE_LOOK);
    }

    _vector Get_PlayerPosition() const { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }

    _uint Get_CurrentHp() { return m_iCurrentHp; }
    _uint Get_MaxHp() const { return m_iMaxHp; }         // 최대 체력

public:
    void PickUpItem(CItem* _pItem);
    void AddWeapon(CWeaponItem* _pWeapon);

    void CheckForNearbyItems();

public:
    CPlayer_Camera* Get_PlayerCamera() { return static_cast<CPlayer_Camera*>(Find_PartObject(TEXT("Part_Camera"))); }
    CParticle_MuzzleFire* Get_FireParticle() { return static_cast<CParticle_MuzzleFire*>(Find_PartObject(TEXT("Part_Muzzle_Fire_Effect"))); }

private:
    CPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CPlayer(const CPlayer& _Prototype);
    virtual ~CPlayer() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;

    virtual void                Priority_Update(_float _fTimeDelta) override;
    virtual void                Update(_float _fTimeDelta) override;
    virtual void                Late_Update(_float _fTimeDelta) override;

    virtual HRESULT             Render() override;



public:
    HRESULT                     Ready_Components();
    HRESULT                     Ready_PartObjects();

    void                        Activate_PlayerCamera();
    void                        Setting_PlayerMove(_float _fTimeDelta);

    void                        Shoot_Ray();
    _float                      Player_Jump(_float _fResultY, _float _fTerrainHeight, _float _fNaviHeight, _float _fTimeDelta);

    _bool                       Collide_With_Collider();

    void                        Switch_Weapon(_uint _iWeaponIndex);
    void                        Change_WeaponImage();


    void                        Be_Hited(_uint _iDamage);
    void                        Be_Burned(_uint _iDamage);


    void                        Initialize_WeaponUI();


    void                        Initialize_InteractionUI();
    void                        Check_Interaction(_float _fTimeDelta, _uint _iTextureIndex, _bool _bVisible, _bool _bHighLighted = false);

    void                        Initialize_LetterUI();

    void                        Heal();

private:
    _uint						m_iState = {};
private:
    CNavigation*                m_pNavigationCom = { nullptr };
    CCollider*                  m_pColliderCom = { nullptr };

private:
    _bool                       m_bUsingPlayerCamera = { false };
    _bool                       m_bIsShootingState = { false };
    _bool                       m_bReLoading = { false };

    _bool                       m_bJumping = { false };
    _float                      m_fJumpSpeed = 0.0f;
    const _float                m_fGravity = -9.8f;
    const _float                m_fInitialJumpSpeed = 5.0f;


    _float                      m_fShootingAnimationTime = {};

    _uint                       m_iCurrentHp = {};
    _uint                       m_iMaxHp = {};
    _uint                       m_iAmmoCount = {};

private:
    _bool                       m_bHited = { false };
    _float                      m_fRayDistance = {};
private:
    _float3                     m_vJumping_Height_Position = {};
private:
    CCrossHair*                 m_pCrossHair = { nullptr };
    CBasicTerrain*              m_pBasicTerrain = { nullptr };
    CHandGun*                   m_pHandGun = { nullptr };
    CRifle*                     m_pRifle = { nullptr };
    CSniper*                    m_pSniper = { nullptr };
    CPlayer_Camera*             m_pPlayerCamera = { nullptr };
    CBody_Player*               m_pBody_Player = { nullptr };


    //CBoss_Barrier*              m_pBoss_Barrier = { nullptr };
    //CBoss_Tower*                m_pBoss_Tower = { nullptr };


    CDashEffect*                m_pDashEffect = { nullptr };
    _float                      m_fDashEffectTime = 0.0f;      // 이펙트 지속 시간

    CHurtEffect*                m_pHurtEffect = { nullptr };
    _float                      m_fHurtEffectTime = 0.0f;      // 이펙트 지속 시간


    CHealEffect*                m_pHealEffect = { nullptr };
    _float                      m_fHealEffectTime = 0.0f;
    
    CBurnEffect*                m_pBurnEffect = { nullptr };

    CPlayerHpbar*               m_pHpBar = { nullptr };


    vector<CGun*>               m_vecWeapons; 
    CGun*                       m_pCurrentGun = { nullptr };
    _uint                       m_iCurrentWeaponIndex = 0;



    vector<CUIWeapon*>          m_vecUIWeapons;
    CUIWeapon*                  m_pCurrentWeaponUI = { nullptr };



    vector<CUIInteraction*>     m_vecUIInteractions;
    CUIInteraction*             m_pUIInteraction = { nullptr };


    vector<CUILetters*>         m_vecUILetters;


    CScopeScreen*               m_pScopeScreen = { nullptr };


    _bool                       m_bSwap = { false };

private:
    _bool                       m_bCheck_With_LCCOllider = { false };

public:
    void Set_BasicTerrain(CBasicTerrain* pTerrain) { m_pBasicTerrain = pTerrain; }

public:
    static CPlayer* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void                Free() override;
};

END