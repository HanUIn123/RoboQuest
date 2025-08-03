#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Player.h"

BEGIN(Client)
class CMissile : public CGameObject
{
public:
    struct MISSILE_DESC : public CGameObject::GAMEOBJECT_DESC
    {

    };

public:
    _bool                               IsActiveBullet() const { return m_bActive; }

protected:
    CMissile(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CMissile(const CMissile& _Prototype);
    virtual ~CMissile() = default;

public:
    virtual HRESULT                     Initialize_Prototype() override;
    virtual HRESULT                     Initialize(void* _pArg) override;

    virtual void                        Priority_Update(_float _fTimeDelta)override;
    virtual void                        Update(_float _fTimeDelta)override;
    virtual void                        Late_Update(_float _fTimeDelta)override;

    virtual HRESULT                     Render()override;

    //virtual void                        Set_MissileDirection(_float _fTimeDelta, _vector _TargetLook) = 0;

    virtual void                        Missile_Fly(_float _fTimeDelta);
    virtual void                        DeActivate_Missile();
    virtual void                        Activate_Missile();

    virtual void                        Hit_Target();


protected:
    _bool                               m_bActive = { false };
    _float                              m_fDefaultRange = 100.0f;
    _vector                             m_vInitialMissilePos;
    _vector                             m_vVelocity;
    CPlayer*                            m_pPlayer = { false };

public:
    virtual CGameObject* Clone(void* _pArg) = 0;
    virtual void                        Free() override;
};

END