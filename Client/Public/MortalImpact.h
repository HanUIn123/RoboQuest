#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_VerRect;
END

BEGIN(Client)
class CMortalImpact final : public CGameObject
{
public:
    struct MORTAL_IMPACT_DESC : public CGameObject::GAMEOBJECT_DESC
    {
    };

private:
    CMortalImpact(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CMortalImpact(const CMortalImpact& _Prototype);
    virtual ~CMortalImpact() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;
    virtual void                Priority_Update(_float _fTimeDelta) override;
    virtual void                Update(_float _fTimeDelta) override;
    virtual void                Late_Update(_float _fTimeDelta) override;
    virtual HRESULT             Render() override;

    _bool                       IsActive() { return m_bActive; }
    void                        Set_Active(_bool _bActive);
    void                        DeActivate_Impact();

    void                        Set_Position(_vector _vPosition, _vector _vDirection);

public:
    HRESULT                     Ready_Components();
    HRESULT                     Bind_ShaderResource();

private:
    CShader*                    m_pShaderCom = { nullptr };
    CTexture*                   m_pTextureCom = { nullptr };
    CVIBuffer_VerRect*          m_pVIBufferCom = { nullptr };

private:
    _bool                       m_bActive = { false };
    _float                      m_fTimer = {};

    _float                      m_fResistTime = {};
    _vector                     m_vPosition = {};

    _bool                       m_bIncreasing = { true };
    _float                      m_fLerpProgress = {};


public:
    static CMortalImpact*       Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*        Clone(void* _pArg) override;
    virtual void                Free() override;
};
END
