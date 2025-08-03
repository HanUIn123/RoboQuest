#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "ContainerObject.h"

//#include "BasicTerrain.h"

BEGIN(Client)
// public containObject ·Î µÇ°í...
//class CMonster : public CGameObject


class CPlayer;
class CBasicTerrain;
class CMonster : public CContainerObject
{
public:
    enum INGAME_MONSTER_TYPE { MT_BOOM, MT_BILLY , MT_GOLIATH, MT_LARVA, MT_SMALL, MT_DOOR, MT_BOSS, MT_ITEM_BOX, MT_GATE, MT_IG_END };

    enum MONSTER_STATE 
    {
        MONSTER_STATE_IDLE = 0x00000001,
        MONSTER_STATE_RUN = 0x00000002,
        MONSTER_STATE_SHOOT = 0x00000003,
        MONSTER_STATE_STUN = 0x00000004
    };

 
public:
    struct MONSTER_DESC : public CGameObject::GAMEOBJECT_DESC
    {
        _float3 vMonsterpos;
        _float3 vMonsterRotation;
        _float3 vMonsterScale;
    };

protected:
    CMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CMonster(const CMonster& _Prototype);
    virtual ~CMonster() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;

    virtual void                Priority_Update(_float _fTimeDelta)override;
    virtual void                Update(_float _fTimeDelta)override;
    virtual void                Late_Update(_float _fTimeDelta)override;

    virtual HRESULT             Render()override;

public:
    virtual HRESULT             Setting_Direction_To_Target(_float _fTimeDelta, _vector _vMonsterPos);
    virtual HRESULT             Setting_Direction_To_Target_Lerp(_float _fTimeDelta, _vector _vMonsterPos);

    virtual _vector             Setting_MissileDirection_To_Target(_vector _vMonsterPos, _float _fHeightOffset, _float _fWidthOffset = {});
    virtual _vector             Setting_MissilePosition_To_Target(_vector _vMonsterPos, _float _fHeightOffset, _float _fWidthOffset = {});


    virtual _float              Compute_Monster_Position_Terrain(_vector _vMonsterPosition);
    virtual _float              Compute_Distance_To_Target();
    virtual _vector             Compute_Target_Position();
    virtual void                LerpMove_Before_Face_To_Target(_float _fTimeDelta, _vector _vMonsterPos);

    virtual void                Memorize_Position_Before_DectectMode(_vector _vMonsterPosition);

    virtual _vector             Get_PlayerCameraLook();
    virtual _vector             Get_PlayerTargetLook(const _vector& vMonsterPosition);

    virtual  _bool              Check_Collision_With_Ray(const _vector& _vRayStart, const _vector& _vRayDirection, _float& _fDistance) = 0;
    virtual void                Be_Hited(_uint _iCurrentGunDamage)= 0;

    //virtual void                Setting_BillBoard();
protected:
    CPlayer*                    m_pPlayer;
    CBasicTerrain*              m_pBasicTerrain = { nullptr };
    _float                      m_fMissile_Direction_Height_Offset = {};
    _float3                     m_fMissile_Position_Offset = {};
    _float                      m_fTargetDistance = {};
    _vector                     m_vDefaultLook = {};

    _vector                     m_vBeforePos = {};
    _bool                       m_bWasDetectTarget = { false };

public:
    virtual CGameObject*        Clone(void* _pArg) = 0;
    virtual void                Free() override;
};

END