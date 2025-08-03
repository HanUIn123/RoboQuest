#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CBossHpBar final : public CUIObject
{
public:
    struct BOSS_HPBAR_DESC : public CUIObject::UIOBJECT_DESC
    {

    };

private:
    CBossHpBar(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CBossHpBar(const CBossHpBar& _Prototype);
    virtual ~CBossHpBar() = default;

public:
    virtual HRESULT                 Initialize_Prototype() override;
    virtual HRESULT                 Initialize(void* _pArg)override;

    virtual void                    Priority_Update(_float _fTimeDelta)override;
    virtual void                    Update(_float _fTimeDelta)override;
    virtual void                    Late_Update(_float _fTimeDelta)override;

    virtual HRESULT                 Render()override;

    void                            Activate_HpBar();
    void                            Set_Visible(_bool _bIsVisible) { m_bIsVisible = _bIsVisible; }

private:
    CShader* m_pShaderCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CTexture* m_pTextureCom = { nullptr };

public:
    HRESULT                         Ready_Component();
    void                            Decrease_Hp(_uint _iDamageValue);

private:
    _float                          m_fCurrentHp = {};
    _float                          m_fMaxHp = {};
    _float                          m_fHpBarWidth = {};

    _bool                           m_bIsVisible = { false };
public:
    static CBossHpBar* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void                    Free() override;
};
END
