#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CBullet : public CGameObject
{
public:
    struct BULLET_DESC : public CGameObject::GAMEOBJECT_DESC
    {

    };

public:
    _bool                               IsActiveBullet() const { return m_bActive; }
    _bool                               IsRayHit() const { return m_bRayHit; }

protected:
    CBullet(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CBullet(const CBullet& _Prototype);
    virtual ~CBullet() = default;

public:
    virtual HRESULT                     Initialize_Prototype() override;
    virtual HRESULT                     Initialize(void* _pArg) override;

    virtual void                        Priority_Update(_float _fTimeDelta)override;
    virtual void                        Update(_float _fTimeDelta)override;
    virtual void                        Late_Update(_float _fTimeDelta)override;

    virtual HRESULT                     Render()override;
    virtual void                        Activate_Bullet(const _vector& _vPosition, const _vector& _vGunMuzzleLook);

    virtual void                        Bullet_Fly(_float _fTimeDelta);
    virtual void                        DeActivate_Bullet();
    virtual void                        Set_RayDistance(_float _fRayDistance);
    virtual void                        Initialize_Ray();

protected:
    _bool                               m_bActive = { false };
    _bool                               m_bRayHit = { false };
    _float                              m_fDefaultRange = 300.0f;
    _float                              m_fRayDistance = {};

public:
    virtual CGameObject*                Clone(void* _pArg) = 0;
    virtual void                        Free() override;
};

END