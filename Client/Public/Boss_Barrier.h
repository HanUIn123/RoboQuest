#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
//#include "Player.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;

class CTexture;
END

BEGIN(Client)
class CBoss_Barrier final : public CGameObject
{
public:
    struct BARRIER_DESC : public CGameObject::GAMEOBJECT_DESC
    {

    };

public:
    _bool                               IsActiveBarrier() const { return m_bActive; }

protected:
    CBoss_Barrier(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CBoss_Barrier(const CBoss_Barrier& _Prototype);
    virtual ~CBoss_Barrier() = default;

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

public:
    virtual void                        DeActivate_Barrier();
    virtual void                        Activate_Barrier(const _vector& _vPosition, const _vector& _vBarrierLook);

    void                                Set_Active(_bool _bActive);
    void                                Set_Position(_vector _vPosition);

    _bool                               Check_Collision_With_Ray(const _vector& _vRayStart, const _vector& _vRayDirection, _float& _fDistance);

    void                                Activate_HittedEffect();

private:
    _bool                               m_bActive = { false };
    //CPlayer*                            m_pPlayer = { false };
private:
    CShader*                            m_pShaderCom = { nullptr };
    CModel*                             m_pModelCom = { nullptr };
    CCollider*                          m_pColliderCom = { nullptr };
    CCollider*                          m_pTargetCollider = { nullptr };
    CTexture*                           m_pTextureCom = { nullptr };
private:
    _vector                             m_vInitialBarrierPos = {};
    _float                              m_fBarrierDurtaion = {};
    _vector                             m_vPosition = {};

private:
    _bool                               m_bAppearingBarrier = { false };
    _bool                               m_bDissolving = { false };
    _float                              m_fDissolveTime = { 1.0f };

    _float                              m_fHitEffectValue = {};

public:
    static CBoss_Barrier*               Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*                Clone(void* _pArg) override;
    virtual void                        Free() override;
};
END

