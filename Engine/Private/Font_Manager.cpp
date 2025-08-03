#include "Font_Manager.h"
#include "CustomFont.h"
#include "GameInstance.h"


CFont_Manager::CFont_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :m_pDevice{ _pDevice }
    , m_pContext{ _pContext }
    ,m_pGameInstance{CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}


HRESULT CFont_Manager::Initialize()
{
    return S_OK;
}

HRESULT CFont_Manager::Add_Font(const _wstring& _strFontTag, const _tchar* _pFontFilePath)
{
    if (nullptr != Find_Font(_strFontTag))
        return E_FAIL;

    CCustomFont* pFont = CCustomFont::Create(m_pDevice, m_pContext, _pFontFilePath);

    if (nullptr == pFont)
        return E_FAIL;

    m_mapFonts.emplace(_strFontTag, pFont);

    return S_OK;
}

HRESULT CFont_Manager::Render(const _wstring& _strFontTag, const _tchar* _pText, const _float2& _vPosition, _fvector _vColor, _float _fRotation, const _float2& _vOrigin, const _float _fScale)
{
    CCustomFont* pFont = Find_Font(_strFontTag);
    if (nullptr == pFont)
        return E_FAIL;

    return pFont->Render(_pText, _vPosition, _vColor, _fRotation, _vOrigin, _fScale);
}

_float2 CFont_Manager::Convert_WorldToScreen(const _float3& worldPosition)
{
    XMMATRIX viewMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW); 
    XMMATRIX projMatrix = m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ);

    XMVECTOR worldPos = XMLoadFloat3(&worldPosition);

    XMVECTOR clipSpacePos = XMVector3TransformCoord(worldPos, viewMatrix * projMatrix);

    _uint2 viewportSize = m_pGameInstance->Get_ViewportSize();
    _float2 screenPosition;
    screenPosition.x = (XMVectorGetX(clipSpacePos) + 1.0f) * 0.5f * viewportSize.x; 
    screenPosition.y = (1.0f - XMVectorGetY(clipSpacePos)) * 0.5f * viewportSize.y; 

    return screenPosition;
}

CCustomFont* CFont_Manager::Find_Font(const _wstring& _strFontTag)
{
    auto iter = m_mapFonts.find(_strFontTag);

    if (iter == m_mapFonts.end())
        return nullptr;

    return iter->second;
}


CFont_Manager* CFont_Manager::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CFont_Manager* pInstance = new CFont_Manager(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CFont_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFont_Manager::Free()
{
    __super::Free();

    for (auto& Pair : m_mapFonts)
        Safe_Release(Pair.second);

    m_mapFonts.clear();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
