#pragma once
#include "Client_Defines.h"
#include "Missile.h"


BEGIN(Engine)
class CShader; 
class CModel;
class CCollider;
END

BEGIN(Client)
class CMissile_Goliath final : public CMissile
{
public:
    struct MISSILE_GOLIATH_DESC : public CGameObject::GAMEOBJECT_DESC
    {
    };

public:
    _vector Get_MissileLook() { return m_pTransformCom->Get_State(CTransform::STATE_LOOK); }
    _vector Get_MissileUp() { return m_pTransformCom->Get_State(CTransform::STATE_UP); }
    _vector Get_MissileRight() { return m_pTransformCom->Get_State(CTransform::STATE_RIGHT); }

private:
    CMissile_Goliath(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CMissile_Goliath(const CMissile_Goliath& _Prototype);
    virtual ~CMissile_Goliath() = default;

public:
    virtual HRESULT                     Initialize_Prototype() override;
    virtual HRESULT                     Initialize(void* _pArg) override;

    virtual void                        Priority_Update(_float _fTimeDelta)override;
    virtual void                        Update(_float _fTimeDelta) override;
    virtual void                        Late_Update(_float _fTimeDelta)override;

    virtual HRESULT                     Render()override;

    //void                                Fire_MortalMissile(_float _fTimeDelta);

    void                                Check_Collision_With_Player();
public:
     HRESULT                            Ready_Component();
    HRESULT                             Bind_ShaderResource();

    void                                Activate_Missile(const _vector& _vPosition, const _vector& _vGunMuzzleLook);
    //void                                Activate_MortalMissile(const _vector& _vPosition, const _vector& _vGunMuzzleLook);
public:
    void                                DeActivate_Missile();
    virtual void                        Missile_Fly(_float _fTimeDelta) override;

private:
    CShader*                            m_pShaderCom = { nullptr };
    CModel*                             m_pModelCom = { nullptr };
    CCollider*                          m_pColliderCom = { nullptr };
    CCollider*                          m_pTargetCollider = { nullptr };


private:
    _vector                             m_vInitialMissilePos = {};
    _vector                             m_vInitialMortalMissilePos = {};

public:
    static CMissile_Goliath*            Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*                Clone(void* _pArg) override;
    virtual void                        Free() override;
};
END