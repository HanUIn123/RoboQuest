#pragma once
#include "Client_Defines.h"
#include "UIObject.h"
#include "ScopeGauge.h"



BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CScopeScreen final : public CUIObject
{
public:
    struct SCOPE_SCRREN_DESC : public CUIObject::UIOBJECT_DESC
    {

    };

private:
    CScopeScreen(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CScopeScreen(const CScopeScreen& _Prototype);
    virtual ~CScopeScreen() = default;

public:
    virtual HRESULT                 Initialize_Prototype() override;
    virtual HRESULT                 Initialize(void* _pArg)override;

    virtual void                    Priority_Update(_float _fTimeDelta)override;
    virtual void                    Update(_float _fTimeDelta)override;
    virtual void                    Late_Update(_float _fTimeDelta)override;

    virtual HRESULT                 Render()override;

    void                            Set_Alpha(_float _fAlpha) { m_fAlpha = _fAlpha; }

    void                            Activate_ScopeScreen();
    void                            DeActivate_ScopeScreen();
    _bool                           Is_Active() { return m_bIsVisible; }

private:
    CShader*                        m_pShaderCom = { nullptr };
    CVIBuffer_Rect*                 m_pVIBufferCom = { nullptr };
    CTexture*                       m_pTextureCom = { nullptr };

    CScopeGauge*                    m_pGaugeBar = { nullptr };


    _bool                           m_bIsVisible = { false };
    _float                          m_fAlpha = {};



public:
    HRESULT                         Ready_Component();
public:
    static CScopeScreen*            Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*            Clone(void* _pArg) override;
    virtual void                    Free() override;
};
END