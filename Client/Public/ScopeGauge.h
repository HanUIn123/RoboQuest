#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CScopeGauge final : public CUIObject
{
public:
    struct SCOPE_GAUGE_DESC : public CUIObject::UIOBJECT_DESC
    {

    };

private:
    CScopeGauge(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CScopeGauge(const CScopeGauge& _Prototype);
    virtual ~CScopeGauge() = default;

public:
    virtual HRESULT                 Initialize_Prototype() override;
    virtual HRESULT                 Initialize(void* _pArg)override;

    virtual void                    Priority_Update(_float _fTimeDelta)override;
    virtual void                    Update(_float _fTimeDelta)override;
    virtual void                    Late_Update(_float _fTimeDelta)override;

    virtual HRESULT                 Render()override;

    void                            Set_Alpha(_float _fAlpha) { m_fAlpha = _fAlpha; }

    void                            Activate_Gauge();
    void                            DeActivate_Gauge();
    _bool                           Is_Active() { return m_bIsVisible; }

    _bool                           Is_FullCharge() { return m_bIsFullCharge; }
    void                            Initialilze_FullCharge(_bool _bFullCharge) { m_bIsFullCharge = _bFullCharge; }


    _float                          Compute_GaugePercent();
    void                            Initialize_GaugeBar(_float _fCurrentBar) { m_fCurrentBarSize = _fCurrentBar; }

private:
    CShader* m_pShaderCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CTexture* m_pBarTextureCom = { nullptr };

    _bool                           m_bIsVisible = { false };
    _float                          m_fAlpha = {};

private:
    _float                          m_fCurrentBarSize = {};
    _float                          m_fMaxBarSize = {};
    _float                          m_fBackGroundSize = {};

    _bool                           m_bIsFullCharge = { false };

public:
    HRESULT                         Ready_Component();
public:
    static CScopeGauge* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void                    Free() override;
};
END

