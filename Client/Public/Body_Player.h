#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CBody_Player final : public CPartObject
{
    

public:
    struct BODY_PLAYER_DESC : public CPartObject::PARTOBJECT_DESC
    {
        const _uint* pParentState = { nullptr };
    };

private:
    CBody_Player(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CBody_Player(const CBody_Player& _Prototype);
    virtual ~CBody_Player() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* _pArg) override;
    virtual void Priority_Update(_float _fTimeDelta) override;
    virtual void Update(_float _fTimeDelta) override;
    virtual void Late_Update(_float _fTimeDelta) override;
    virtual HRESULT Render() override;
    virtual HRESULT Render_Shadow() override;

public:
    _bool IsAnimationFinished() const;

private:
    CShader* m_pShaderCom = { nullptr };
    CModel* m_pModelCom = { nullptr };

private:    
    const _uint* m_pParentState = { nullptr };

public:
    HRESULT Ready_Components();
    HRESULT Bind_ShaderResources();


private :
    _uint           m_iTempCheckNum = {};
private:
    _uint           m_iPreAnimationState = {};


public:
    static CBody_Player* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void Free() override;
};

END