#pragma once
#include "Client_Defines.h"
#include "Missile.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)
class CMissile_Boom final : public CMissile
{
public:
    struct MISSILE_BOOM_DESC : public CGameObject::GAMEOBJECT_DESC
    {

    };

private:
    CMissile_Boom(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CMissile_Boom(const CMissile_Boom& _Prototype);
    virtual ~CMissile_Boom() = default;

public:
    virtual HRESULT                     Initialize_Prototype() override;
    virtual HRESULT                     Initialize(void* _pArg) override;

    virtual void                        Priority_Update(_float _fTimeDelta)override;
    virtual void                        Update(_float _fTimeDelta)override;
    virtual void                        Late_Update(_float _fTimeDelta)override;

    virtual HRESULT                     Render()override;

public:
    HRESULT                             Ready_Component();
    HRESULT                             Bind_ShaderResource();

    void                                Activate_Missile(const _vector& _vPosition, const _vector& _vGunMuzzleLook);
public:
    void                                DeActivate_Missile();
    virtual void                        Missile_Fly(_float _fTimeDelta) override;
    void                                Check_Collision_With_Player();
private:
    CShader*                            m_pShaderCom = { nullptr };
    CModel*                             m_pModelCom = { nullptr };
    CCollider*                          m_pColliderCom = { nullptr };
    CCollider*                          m_pTargetCollider = { nullptr };
private:
    _vector                             m_vInitialMissilePos = {};

public:
    static CMissile_Boom*               Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*                Clone(void* _pArg) override;
    virtual void                        Free() override;
};
END

