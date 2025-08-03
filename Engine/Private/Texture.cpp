#include "../Public/Texture.h"
#include "Shader.h"


CTexture::CTexture(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CComponent{_pDevice, _pContext}
{
}

CTexture::CTexture(const CTexture& _Prototype)
    : CComponent(_Prototype)
    , m_iNumSRVs{_Prototype.m_iNumSRVs}
    , m_vecSRVs{_Prototype.m_vecSRVs}
{
    for (auto& pSRV : m_vecSRVs)
        Safe_AddRef(pSRV);
}

HRESULT CTexture::Initialize_Prototype(const _tchar* _pTextureFilePath, _uint _iNumTexture)
{
    m_iNumSRVs = _iNumTexture;

    // .Ȯ���� 
    _tchar      szEXT[MAX_PATH] = {};

    _wsplitpath_s(_pTextureFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

    for (size_t i = 0; i < m_iNumSRVs; ++i)
    {
        ID3D11Texture2D*                    pTexture2D = { nullptr };
        ID3D11ShaderResourceView*           pSRV = { nullptr };

        _tchar                              szTextureFilePath[MAX_PATH] = TEXT("");

        // _pTextureFilePath (���Ĺ���) 
        wsprintf(szTextureFilePath, _pTextureFilePath, i);

        HRESULT         hr = {};

        // CreateDDSTextureFromFile : ������ Texture2D �� �޾ƿͼ� ShaderResourceView ����� �ϴµ�, �� �Լ���,
        // �˾Ƽ� ShaderResourceView ������ ����. �׷��� 3��° ���ڿ� nullptr �ְ�,
        // 4��°�� �׳� ShaderResourceView �ּ� ����. ->�Լ� �ϳ��� ��ģ����.
        if (false == lstrcmp(szEXT, TEXT(".dds")))
        {
            // Ȯ���ڰ� .dds �� ��,
            hr = CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
        }
        else if (false == lstrcmp(szEXT, TEXT(".tga")))
        {
            hr = E_FAIL;
        }
        else
        {
            // Ȯ���ڰ� .jpg / .png ��� �� ��, 
            hr = CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
        }

        if (FAILED(hr))
            return E_FAIL;

        m_vecSRVs.push_back(pSRV);
    }

    return S_OK;
}

HRESULT CTexture::Initialize(void* _pArg)
{
    return S_OK;
}

// ~�� ���̴� Ŭ������, ~�� �̸��� �°�, ~�� ���� �ؽ��� 
HRESULT CTexture::Bind_ShaderResource(CShader* _pShader, const _char* _pConstantName, _uint _iIndex)
{
    if (_iIndex >= m_iNumSRVs)
        return E_FAIL;

     return _pShader->Bind_SRV(_pConstantName, m_vecSRVs[_iIndex]);
}

HRESULT CTexture::Bind_ShaderResource_Multi(CShader* _pShader, const _char* _pConstantName)
{
    return _pShader->Bind_SRV_Multi(_pConstantName, &m_vecSRVs.front(), m_iNumSRVs);
}


CTexture* CTexture::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pTextureFilePath, _uint _iNumTexture)
{
    CTexture* pInstance = new CTexture(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype(_pTextureFilePath, _iNumTexture)))
    {
        MSG_BOX("Failed To Created : CTexture");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CTexture::Clone(void* _pArg)
{
    CTexture* pInstance = new CTexture(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CTexture");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTexture::Free()
{
    __super::Free();

    for (auto& pSRV : m_vecSRVs)
        Safe_Release(pSRV);

    m_vecSRVs.clear();
}
