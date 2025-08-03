#pragma once
#include "Client_Defines.h"
#include "Missile.h"
#include "MortalImpact.h"
#include "Dying_Explode.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)
class CMissile_Mortal final: public CMissile
{
public:
    struct MISSILE_MORTAL_DESC : public CGameObject::GAMEOBJECT_DESC
    {
    };

public:
    _vector Get_MissileLook() { return m_pTransformCom->Get_State(CTransform::STATE_LOOK); }
    _vector Get_MissileUp() { return m_pTransformCom->Get_State(CTransform::STATE_UP); }
    _vector Get_MissileRight() { return m_pTransformCom->Get_State(CTransform::STATE_RIGHT); }


    void            Set_TargetPosition(_vector _vTargetPos) { m_vTargetPos = _vTargetPos; }

private:
    CMissile_Mortal(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CMissile_Mortal(const CMissile_Mortal& _Prototype);
    virtual ~CMissile_Mortal() = default;

public:
    virtual HRESULT                     Initialize_Prototype() override;
    virtual HRESULT                     Initialize(void* _pArg) override;

    virtual void                        Priority_Update(_float _fTimeDelta)override;
    void                                Update(_float _fTimeDelta, _vector _vTargetPosition = {});
    virtual void                        Late_Update(_float _fTimeDelta)override;

    virtual HRESULT                     Render()override;

public:
    HRESULT                             Ready_Component();
    HRESULT                             Bind_ShaderResource();


    void                                Activate_MortalMissile(const _vector& _vPosition, const _vector& _vGunMuzzleLook, const _vector& _vTargetPosition);
public:
    void                                DeActivate_Missile();
    void                                DeActivate_DropMissile();
    virtual void                        Missile_Fly(_float _fTimeDelta) override;
    void                                Drop_Missile(_float _fTimeDelta/*, _vector _vTargetPosition = {}*/);

    void                                Check_Collision_With_Player();

private:
    CShader*                            m_pShaderCom = { nullptr };
    CModel*                             m_pModelCom = { nullptr };
    CCollider*                          m_pColliderCom = { nullptr };
    CCollider*                          m_pTargetCollider = { nullptr };

    _vector                             m_pCurPosition = {};
    _vector                             m_vTargetPos = {};

    CDying_Explode*                     m_pMortalExplosion = { nullptr };
    vector<CDying_Explode*>             m_vecMortalExplosion;

private:

    CMortalImpact*                      m_pMortalImpact = { nullptr };
    vector<CMortalImpact*>              m_vecMortalImpact;
    _bool                               m_bImpactShow = { false };

private:
    _vector                             m_vInitialMissilePos = {};
    _vector                             m_vInitialMortalMissilePos = {};
    _bool                               m_bFalling = { false };

public:
    static CMissile_Mortal* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void                        Free() override;
};
END