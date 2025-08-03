#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CBody_Box final: public CPartObject
{
public:
    struct BODY_BOX_DESC : public CPartObject::PARTOBJECT_DESC
    {
        const _uint* pParentState = { nullptr };
    };

private:
    CBody_Box(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CBody_Box(const CBody_Box& _Prototype);
    virtual ~CBody_Box() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;
    virtual void                Priority_Update(_float _fTimeDelta) override;
    virtual void                Update(_float _fTimeDelta) override;
    virtual void                Late_Update(_float _fTimeDelta) override;
    virtual HRESULT             Render() override;

public:
    _bool                       IsAnimationFinished() const;
    _uint                       Get_BoxNumber() { return m_iBoxNumber; }


private:
    CShader*                    m_pShaderCom = { nullptr };
    CModel*                     m_pModelCom = { nullptr };

private:
    const _uint*                m_pParentState = { nullptr };

public:
    HRESULT                     Ready_Components();
    HRESULT                     Bind_ShaderResources();

private:
    _uint                       m_iPreAnimationState = {};
    _bool                       m_isStop = { false };


    _uint                       m_iBoxNumber = {};
public:
    static CBody_Box*           Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*        Clone(void* _pArg) override;
    virtual void                Free() override;
};
END
