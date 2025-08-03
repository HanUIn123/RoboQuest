#pragma once
#include "Client_Defines.h"
#include "PartObject.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_VerRect;
class CVIBuffer_Point_Instancing;
class CCollider;
END

BEGIN(Client)
class CFireRoad final : public CGameObject
{
public:
    struct FIRE_ROAD_DESC : public CGameObject::GAMEOBJECT_DESC
    {
    };

public:
    //void Set_WorldMatrix(_float4x4* _pWorldMatrix) { m_CombinedWorldMatrix = *_pWorldMatrix; }

private:
    CFireRoad(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CFireRoad(const CFireRoad& _Prototype);
    virtual ~CFireRoad() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;
    virtual void                Priority_Update(_float _fTimeDelta) override;
    virtual void                Update(_float _fTimeDelta) override;
    virtual void                Late_Update(_float _fTimeDelta) override;
    virtual HRESULT             Render() override;
    virtual HRESULT             Render_Glow() override;

    _bool                       IsActive() { return m_bActive; }
    void                        Set_Active(_bool _bActive);
    void                        DeActivateFlow();

    void                        Set_Position(_vector _vPosition, _vector _vDirection);
    void                                Check_Collision_With_Player();
private:
    CShader*                    m_pShaderCom = { nullptr };
    CShader*                    m_pShaderFireFrame = { nullptr };
    CTexture*                   m_pTextureCom = { nullptr };
    CTexture*                   m_pFrameTextureCom = { nullptr };
    CVIBuffer_VerRect*          m_pVIBufferCom = { nullptr };
    CVIBuffer_Point_Instancing*          m_pFrameBufferCom = { nullptr };
    CCollider*                  m_pColliderCom = { nullptr };
    CCollider* m_pTargetCollider = { nullptr };
    CPlayer* m_pPlayer = { nullptr };

    _float                      m_fTimer = {};
    _bool                       m_bActive = { false };
    _float                      m_fFlowTime = {};
    _vector                     m_vPosition = {};

    _bool                       m_bIncreasing = { true };
    _float                      m_fLerpProgress = {};

    _float						m_fFrame = {};
    _float                      m_fAnimationTime = 0.0f;
public:
    HRESULT                     Ready_Components();
    HRESULT                     Bind_ShaderResource();

public:
    static CFireRoad*           Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*        Clone(void* _pArg) override;
    virtual void                Free() override;
};
END
