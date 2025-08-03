#pragma once
#include "Client_Defines.h"
#include "Missile.h"
#include "Player.h"
#include "BasicTerrain.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)
class CLaser_Boss final : public CMissile
{
public:
    struct LASER_BOSS_DESC : public CGameObject::GAMEOBJECT_DESC
    {

    };

private:
    CLaser_Boss(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CLaser_Boss(const CLaser_Boss& _Prototype);
    virtual ~CLaser_Boss() = default;

public:
    virtual HRESULT                     Initialize_Prototype() override;
    virtual HRESULT                     Initialize(void* _pArg) override;

    virtual void                        Priority_Update(_float _fTimeDelta)override;
    virtual void                        Update(_float _fTimeDelta) override;
    virtual void                        Late_Update(_float _fTimeDelta)override;

    virtual HRESULT                     Render()override;
    virtual HRESULT                     Render_Glow()override;

public:
    HRESULT                             Ready_Component();
    HRESULT                             Bind_ShaderResource();

    void                                Activate_Laser(const _vector& _vPosition, const _vector& _vMuzzleLook);
public:
    void                                DeActivate_Missile();

    void                                Check_Collision_With_Player();
    void                                Hit_Target();

    void                                Show_Laser(_float _fTimeDelta);

private:
    CShader*                            m_pShaderCom = { nullptr };
    CModel*                             m_pModelCom = { nullptr };
    CCollider*                          m_pColliderCom = { nullptr };
    CCollider*                          m_pTargetCollider = { nullptr };
    CPlayer*                            m_pPlayer = { nullptr };
    CBasicTerrain*                      m_pTerrain = { nullptr };

private:
    _vector                             m_vInitialMissilePos = {};
    _float                              m_fTimer = {};

private:
    //_float3                             m_vTargetScale = _float3(0.5f, 0.5f, 5.0f); // 최종 크기 (기존 값)
    //_float3                             m_vCurrentScale = _float3(0.0f, 0.0f, 0.0f); // 시작 크기 (0에서 시작)
    //_float                              m_fScaleSpeed = 5.0f; // 스케일 증가 속도
    //bool                                m_bScalingUp = false; // 스케일 증가 여부

public:
    static CLaser_Boss*                 Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*                Clone(void* _pArg) override;
    virtual void                        Free() override;
};
END