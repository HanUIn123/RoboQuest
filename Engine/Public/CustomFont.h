#pragma once
#include "Base.h"

BEGIN(Engine)

class CCustomFont final : public CBase
{
private:
    CCustomFont(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CCustomFont() = default;


public:
    HRESULT     Initialize(const _tchar* _pFontFilePath);
    HRESULT     Render(const _tchar* _pText, const _float2& _vPosition, _fvector _vColor = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f), _float _fRotation = 0.0f, const _float2& _vOrigin = _float2(0.f, 0.f), const _float _fScale = 1.0f);

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    SpriteBatch* m_pBatch = { nullptr };
    SpriteFont* m_pFont = { nullptr };

public:
    static CCustomFont* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pFontFilePath);
    virtual void Free() override;
};

END