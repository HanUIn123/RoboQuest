#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CUIInteraction final : public CUIObject
{
public:
    struct UIINTERACTION_DESC : public CUIObject::UIOBJECT_DESC
    {
        _uint iTextureIndex;
    };

private:
    CUIInteraction(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CUIInteraction(const CUIInteraction& _Prototype);
    virtual ~CUIInteraction() = default;

public:

    HRESULT                         Initialize_Prototype();
    HRESULT                         Initialize(void* _pArg);

    void                            Priority_Update(_float _fTimeDelta) override;
    void                            Update(_float _fTimeDelta) override;
    void                            Late_Update(_float _fTimeDelta) override;
    HRESULT                         Render() override;


    _uint                           Get_WeaponUIIndex() { return m_iTextureIndex; }
    void                            Set_TextureIndex(_uint _iTextureIndex) { m_iTextureIndex = _iTextureIndex; }
    void                            Set_Visible(_bool _bVisible) { m_bVisible = _bVisible; }
    void                            Set_HighLight(_bool _bHighlight);
    void                            Set_Opacity(_float _fOpacity) { m_fOpacity = _fOpacity; }

private:
    HRESULT                         Ready_Component();

public:
    static CUIInteraction*          Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*            Clone(void* _pArg) override;
    virtual void                    Free() override;

private:
    CShader*                        m_pShaderCom = { nullptr };
    CVIBuffer_Rect*                 m_pVIBufferCom = { nullptr };
    CTexture*                       m_pTextureCom = { nullptr };
private:
    _uint                           m_iTextureIndex = {};
    _bool                           m_bHighlighted = { false };
    _bool                           m_bScaling = { false };
    _float                          m_fLerpProgress = {};
    _bool                           m_bIncreasing = { true };
private:
    _float                          m_fOpacity = {};
    _bool                           m_bVisible = true;
};
END
