#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
//class CTexture;
END

BEGIN(Client)
class CBody_Gate final : public CPartObject
{
public:
    struct BODY_GATE_DESC : public CPartObject::PARTOBJECT_DESC
    {
        const _uint* pParentState = { nullptr };
    };

private:
    CBody_Gate(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CBody_Gate(const CBody_Gate& _Prototype);
    virtual ~CBody_Gate() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;
    virtual void                Priority_Update(_float _fTimeDelta) override;
    virtual void                Update(_float _fTimeDelta) override;
    virtual void                Late_Update(_float _fTimeDelta) override;
    virtual HRESULT             Render() override;
    virtual HRESULT                     Render_Glow() override;

public:
    _bool                       IsAnimationFinished() const;

private:
    CShader* m_pShaderCom = { nullptr };
    CModel* m_pModelCom = { nullptr };
    //CTexture* m_pTextureCom = { nullptr };

private:
    const _uint* m_pParentState = { nullptr };

public:
    HRESULT                     Ready_Components();
    HRESULT                     Bind_ShaderResources();

private:
    _uint                       m_iPreAnimationState = {};
    _bool                       m_isStop = { false };
public:
    static CBody_Gate* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void                Free() override;
};
END

