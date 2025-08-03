#pragma once
#include "Client_Defines.h"
#include "Missile.h"
#include "Player.h"
#include "MissileSmokeParticle.h"
#include "Dying_Explode.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)
class CMissile_Boss final : public CMissile
{
public:
    struct MISSILE_BOSS_DESC : public CGameObject::GAMEOBJECT_DESC
    {
    };

public:
    _vector Get_MissileLook() { return m_pTransformCom->Get_State(CTransform::STATE_LOOK); }
    _vector Get_MissileUp() { return m_pTransformCom->Get_State(CTransform::STATE_UP); }
    _vector Get_MissileRight() { return m_pTransformCom->Get_State(CTransform::STATE_RIGHT); }

private:
    CMissile_Boss(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CMissile_Boss(const CMissile_Boss& _Prototype);
    virtual ~CMissile_Boss() = default;

public:
    virtual HRESULT                     Initialize_Prototype() override;
    virtual HRESULT                     Initialize(void* _pArg) override;

    virtual void                        Priority_Update(_float _fTimeDelta)override;
    virtual void                        Update(_float _fTimeDelta) override;
    virtual void                        Late_Update(_float _fTimeDelta)override;

    virtual HRESULT                     Render()override;

public:
    HRESULT                             Ready_Component();
    HRESULT                             Bind_ShaderResource();

    void                                Activate_Missile(const _vector& _vPosition, const _vector& _vGunMuzzleLook);
    void                                Activate_BezierMissile(const _vector& _vStartPos);

    _vector                             Bezier(_vector _vStartPos, _vector _vCurvePos, _vector _vEndPos, _float _fTimeDelta);
public:
    void                                DeActivate_Missile();
    virtual void                        Missile_Fly(_float _fTimeDelta) override;
    void                                Missile_Fly_Bezier(_float _fTimeDelta);

    void                                Check_Collision_With_Player();
//    void                                Hit_Target();
    //CMissileSmokeParticle* Get_SmokeParticle(){ return static_cast<CParticle_MuzzleFire*>(Find_PartObject(TEXT("Part_SmokeEffect"))); }
private:
    CShader*                            m_pShaderCom = { nullptr };
    CModel*                             m_pModelCom = { nullptr };
    CCollider*                          m_pColliderCom = { nullptr };
    CCollider*                          m_pTargetCollider = { nullptr };
    //CPlayer*                            m_pPlayer = { nullptr };

    CMissileSmokeParticle*              m_pSmokeParticle = { nullptr };
    vector<CMissileSmokeParticle*>      m_vecSmokeParticle;

    CDying_Explode*                     m_pMortalExplosion = { nullptr };
    vector<CDying_Explode*>             m_vecMortalExplosion;


private:
    _vector                             m_vInitialMissilePos = {};
    _vector                             m_vInitialMortalMissilePos = {};

private:
    _vector                             m_vStartPos;   
    _vector                             m_vEndPos;     
    _vector                             m_vCurvePos;   
    _float                              m_fElapsedTime;
    _bool                               m_bBezierMode = { false };
    _vector                             m_vLastPos;

public:
    static CMissile_Boss*               Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*                Clone(void* _pArg) override;
    virtual void                        Free() override;
};

END