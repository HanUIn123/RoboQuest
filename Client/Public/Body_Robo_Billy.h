#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CBody_Robo_Billy final : public CPartObject
{
public:
    struct BODY_BILLY_DESC : public CPartObject::PARTOBJECT_DESC
    {
        const _uint* pParentState = { nullptr };
    };

private:
    CBody_Robo_Billy(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CBody_Robo_Billy(const CBody_Robo_Billy& _Prototype);
    virtual ~CBody_Robo_Billy() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;
    virtual void                Priority_Update(_float _fTimeDelta) override;
    virtual void                Update(_float _fTimeDelta) override;
    virtual void                Late_Update(_float _fTimeDelta) override;
    virtual HRESULT             Render() override;



public:
    _bool                       IsAnimationFinished() const;

private:
    CShader* m_pShaderCom = { nullptr };
    CModel* m_pModelCom = { nullptr };

private:
    const _uint* m_pParentState = { nullptr };

public:
    HRESULT                     Ready_Components();
    HRESULT                     Bind_ShaderResources();


private:
    _uint           m_iTempCheckNum = {};
private:
    _uint           m_iPreAnimationState = {};


public:
    static CBody_Robo_Billy* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void Free() override;
};
END

