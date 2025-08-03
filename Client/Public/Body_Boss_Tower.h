#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTexture;
END

BEGIN(Client)
class CBody_Boss_Tower final : public CPartObject
{
public:
    struct BODY_BOSS_DESC : public CPartObject::PARTOBJECT_DESC
    {
        const _uint* pParentState = { nullptr };
    };

private:
    CBody_Boss_Tower(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CBody_Boss_Tower(const CBody_Boss_Tower& _Prototype);
    virtual ~CBody_Boss_Tower() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;
    virtual void                Priority_Update(_float _fTimeDelta) override;
    virtual void                Update(_float _fTimeDelta) override;
    virtual void                Late_Update(_float _fTimeDelta) override;
    virtual HRESULT             Render() override;


public:
    _bool                       IsAnimationFinished() const;
    _bool                       IsDissolving() { return m_bDissolving; }
    _float                      Get_DissolveTime() { return m_fDissolveTime; }
    void                        Activate_HittedEffect();
    void                        Activate_DissolveEffect();

private:
    CShader*                    m_pShaderCom = { nullptr };
    CModel*                     m_pModelCom = { nullptr };
    CTexture*                   m_pTextureCom = { nullptr };

private:
    const _uint*                m_pParentState = { nullptr };

public:
    HRESULT                     Ready_Components();
    HRESULT                     Bind_ShaderResources();

private:
    _uint                       m_iPreAnimationState = {};

private:
    _float                      m_fHitEffectValue = {};
private:
    _bool                        m_bDissolving = { false };     // 디졸브 활성화 여부
    _float                       m_fDissolveTime = {};          // 디졸브 진행 시간
    

public:
    static CBody_Boss_Tower*    Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*        Clone(void* _pArg) override;
    virtual void                Free() override;

};
END