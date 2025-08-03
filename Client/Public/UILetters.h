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
class CUILetters final : public CUIObject
{
public:
    struct LETTER_DESC : public CUIObject::UIOBJECT_DESC
    {

    };

private:
    CUILetters(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CUILetters(const CUILetters& _Prototype);
    virtual ~CUILetters() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* _pArg)override;

    virtual void Priority_Update(_float _fTimeDelta)override;
    virtual void Update(_float _fTimeDelta)override;
    virtual void Late_Update(_float _fTimeDelta)override;

    virtual HRESULT Render()override;

    _bool Is_Visible() { return m_bVisible; }
    void Set_Visible(_bool _bVisible);

    void Set_TextureIndex(_uint _iTextureIndex) { m_iLetterTextureIndex = _iTextureIndex; }
    void Set_Position(_vector _vPos);

private:
    CShader*            m_pShaderCom = { nullptr };
    CVIBuffer_Rect*     m_pVIBufferCom = { nullptr };
    CTexture*           m_pTextureCom = { nullptr };

    _bool               m_bVisible = { false };
    _uint               m_iLetterTextureIndex = {  };

    _float              m_fLifeTime = {1.0f};

    _float              m_fTime = {};
public:
    HRESULT             Ready_Component();

public:
    static CUILetters* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void Free() override;
};

END