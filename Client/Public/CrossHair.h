#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
class CTransform;
END

BEGIN(Client)
class CCrossHair final : public CUIObject
{
public:
    struct CROSS_HAIR_DESC : public CUIObject::UIOBJECT_DESC
    {

    };

public:
    void Set_CrossHair_TextureIndex(_uint _iIndex) { m_iTextureIndex = _iIndex; }
private:
    CCrossHair(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CCrossHair(const CCrossHair& _Prototype);
    virtual ~CCrossHair() = default;

public:
    virtual HRESULT Initialize_Prototype() override;            
    virtual HRESULT Initialize(void* _pArg)override;            

    virtual void Priority_Update(_float _fTimeDelta)override;
    virtual void Update(_float _fTimeDelta)override;
    virtual void Late_Update(_float _fTimeDelta)override;

    virtual HRESULT Render()override;

    void Set_Visible(_bool _bVisible) { m_bIsVisible = _bVisible; }

private:
    CShader* m_pShaderCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CTexture* m_pTextureCom = { nullptr };

public:
    HRESULT                 Ready_Component();

private:    
    _uint                   m_iTextureIndex = {};
    _bool                   m_bIsVisible = { true };

private:
    //CROSS_HAIR_DESC*        m_pDesc = {nullptr};

public:
    static CCrossHair* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void Free() override;
};

END