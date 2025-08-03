#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CBossIntroScreen final : public CUIObject
{
public:
    struct INTRO_DESC : public CUIObject::UIOBJECT_DESC
    {

    };

private:
    CBossIntroScreen(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CBossIntroScreen(const CBossIntroScreen& _Prototype);
    virtual ~CBossIntroScreen() = default;

public:
    virtual HRESULT                 Initialize_Prototype() override;
    virtual HRESULT                 Initialize(void* _pArg)override;

    virtual void                    Priority_Update(_float _fTimeDelta)override;
    virtual void                    Update(_float _fTimeDelta)override;
    virtual void                    Late_Update(_float _fTimeDelta)override;

    virtual HRESULT                 Render()override;
    //virtual HRESULT                 Render_Glow();

    void                            Set_Alpha(_float _fAlpha) { m_fAlpha = _fAlpha; }

    void                            Activate_BossIntroScreen();
    void                            DeActivate_BossIntroScreen();
    _bool                           Is_Active() { return m_bIsVisible; }

private:
    CShader*                        m_pShaderCom = { nullptr };
    CVIBuffer_Rect*                 m_pVIBufferCom = { nullptr };
    CTexture*                       m_pTextureCom = { nullptr };
    CTexture*                       m_pTextureCom2 = { nullptr };

    CTexture*                       m_pNameTexturCom = { nullptr };


    _bool                           m_bIsVisible = { false };
    _float                          m_fAlpha = {};

private:


    _bool                               m_bDissolving = { false };
    _float                              m_fDissolveTime = { 1.0f };

public:
    HRESULT                         Ready_Component();
public:
    static CBossIntroScreen*        Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*            Clone(void* _pArg) override;
    virtual void                    Free() override;
};

END