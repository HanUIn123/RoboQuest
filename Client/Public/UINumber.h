#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CUINumber final : public CUIObject
{
public:
    struct UINUMBER_DESC : public CUIObject::UIOBJECT_DESC
    {

    };


private:
    CUINumber(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CUINumber(const CUINumber& _Prototype);
    virtual ~CUINumber() = default;

public:

    HRESULT Initialize_Prototype();
    HRESULT Initialize(void* pArg);

    void Priority_Update(_float _fTimeDelta) override;
    void Update(_float _fTimeDelta) override;
    void Late_Update(_float _fTimeDelta) override;
    HRESULT Render() override;

    _int Get_SlaschDigit() { return m_iDigit; }

public:

    void Set_Digit(_int _iRealNum);
    void Set_Visible(_bool _bVisible) { m_bVisible = _bVisible; }

public:
    static CUINumber* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void Free() override;

private:
    CShader* m_pShaderCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CTexture* m_pTextureCom = { nullptr };

    _bool       m_bVisible = { true };

private:
    HRESULT Ready_Component();

private:
    _int m_iDigit = 0; 
};
END