#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Engine)
class CNavigation;
class CCollider;
END

BEGIN(Client)
class CRobo_Billy final : public CMonster
{
public:
    struct ROBO_BILLY_DESC final : public CGameObject::GAMEOBJECT_DESC
    {
        _float3 vMonsterpos;
        _float3 vMonsterRotation;
        _float3 vMonsterScale;
    };

private:
    CRobo_Billy(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CRobo_Billy(const CRobo_Billy& _Prototype);
    virtual ~CRobo_Billy() = default;

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

private:
    CNavigation*                m_pNavigationCom = { nullptr };
    CCollider*                  m_pColliderCom = { nullptr };

public:
    virtual HRESULT             Ready_Component();
    virtual HRESULT             Ready_PartObjects();

private:
    _uint						m_iState = {};

public:
    static CMonster*            Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*        Clone(void* _pArg) override;
    virtual void                Free() override;
};
END
