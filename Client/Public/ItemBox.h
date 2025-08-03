#pragma once
#include "Client_Defines.h"
#include "Monster.h"
#include "Body_Box.h"
#include "WeaponItem.h"


BEGIN(Engine)
class CNavigation;
class CCollider;
END

BEGIN(Client)
class CItemBox final: public CMonster
{
    enum ITEM_BOX_STATE
    {
        STATE_BOX_HOVER = 0x00000001,
        STATE_BOX_IDLE = 0x00000002,
        STATE_BOX_OPEN = 0x00000003,
    };

public:
    struct ITEM_BOX_DESC final : public CGameObject::GAMEOBJECT_DESC
    {
        _float3 vMonsterpos;
        _float3 vMonsterRotation;
        _float3 vMonsterScale;

        _uint   iWeaponTypeIndex;
    };

private:
    CItemBox(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CItemBox(const CMonster& _Prototype);
    virtual ~CItemBox() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;

    virtual void                Priority_Update(_float _fTimeDelta)override;
    virtual void                Update(_float _fTimeDelta)override;
    virtual void                Late_Update(_float _fTimeDelta)override;

    virtual HRESULT             Render()override;

public:
    _bool                       Check_Collision_With_Ray(const _vector& _vRayStart, const _vector& _vRayDirection, _float& _fDistance);
    void                        Be_Hited(_uint _iCurrentGunDamage);

public:
    virtual HRESULT             Ready_Component();
    virtual HRESULT             Ready_PartObjects();

    void                        Drop_Item(/*_uint _iWeaponTypeIndex*/);

    void                        Set_State(ITEM_BOX_STATE _eState);
private:
    _uint						m_iState = {};
    CCollider*                  m_pColliderCom = { nullptr };
    CBody_Box*                  m_pBodyBox = { nullptr };
    _bool                       m_bItemDropped = { false };

    _uint                       m_iCurrentWeaponIndex = 1;

public:
    static CMonster*            Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*        Clone(void* _pArg) override;
    virtual void                Free() override;
};
END
