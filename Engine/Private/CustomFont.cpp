#include "CustomFont.h"

CCustomFont::CCustomFont(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice{ _pDevice }
    , m_pContext{ _pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CCustomFont::Initialize(const _tchar* _pFontFilePath)
{
    m_pFont = new SpriteFont(m_pDevice, _pFontFilePath);
    m_pBatch = new SpriteBatch(m_pContext);

    return S_OK;
}

HRESULT CCustomFont::Render(const _tchar* _pText, const _float2& _vPosition, _fvector _vColor, _float _fRotation, const _float2& _vOrigin, const _float _fScale)
{
    if (nullptr == m_pFont || nullptr == m_pBatch)
        return E_FAIL;

    m_pBatch->Begin();

    m_pFont->DrawString(m_pBatch, _pText, _vPosition, _vColor, _fRotation, _vOrigin, _fScale);

    m_pBatch->End();

    return S_OK;
}

CCustomFont* CCustomFont::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pFontFilePath)
{
    CCustomFont* pInstance = new CCustomFont(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_pFontFilePath)))
    {
        MSG_BOX("Failed to Created : CCustomFont");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCustomFont::Free()
{
    __super::Free();

    Safe_Delete(m_pBatch);
    Safe_Delete(m_pFont);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
