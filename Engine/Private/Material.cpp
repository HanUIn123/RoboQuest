#include "Material.h"
#include "Shader.h"

CMaterial::CMaterial(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :m_pDevice{_pDevice}
    ,m_pContext{_pContext}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CMaterial::Initialize(const aiMaterial* _pAIMaterial, const _char* _pModelFilePath)
{
    for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
    {
        // AI_TEXTURE_TYPE_MAX 속 텍스쳐 타입 중 (i) 에서, 
        // 만약에 Diffuse 용 texture 이면, 그 Diffuse Texture 가 몇 장인지.
        _uint iNumTextures = _pAIMaterial->GetTextureCount(aiTextureType(i));

        m_vecMtrl_Textures[i].reserve(iNumTextures);

        for (size_t j = 0; j < iNumTextures; ++j)
        {
            aiString        strTextureFilePath;
            ID3D11ShaderResourceView* pSRV = { nullptr };

            if (FAILED(_pAIMaterial->GetTexture(aiTextureType(i), (_uint)j, &strTextureFilePath)))
                return E_FAIL;

            _char szDrive[MAX_PATH] = "";
            _char szDirectory[MAX_PATH] = "";
            _char szFileName[MAX_PATH] = "";
            _char szEXT[MAX_PATH] = "";

            _splitpath_s(_pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
            _splitpath_s(strTextureFilePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

            _char szFullPath[MAX_PATH] = "";
            strcpy_s(szFullPath, szDrive);
            strcat_s(szFullPath, szDirectory);
            strcat_s(szFullPath, szFileName);
            strcat_s(szFullPath, szEXT);

            _tchar szLastPath[MAX_PATH] = {};
            MultiByteToWideChar(CP_ACP, 0, szFullPath, (_uint)(strlen(szFullPath)), szLastPath, MAX_PATH);

            HRESULT hr = {};

            if (false == strcmp(szEXT, ".dds"))
                hr = CreateDDSTextureFromFile(m_pDevice, szLastPath, nullptr, &pSRV);
            else if (false == strcmp(szEXT, ".tga"))
                return E_FAIL;
            else
                hr = CreateWICTextureFromFile(m_pDevice, szLastPath, nullptr, &pSRV);

            if (FAILED(hr))
                return E_FAIL;

            m_vecMtrl_Textures[i].push_back(pSRV);
        }
    }
    return S_OK;
}

HRESULT CMaterial::Initialize_SaveData(const aiMaterial* _pAIMaterial, HANDLE _pSaveFile, const _char* _pModelFilePath)
{
    DWORD       dwByte = {};

    for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
    {
        // AI_TEXTURE_TYPE_MAX 속 텍스쳐 타입 중 (i) 에서, 
        // 만약에 Diffuse 용 texture 이면, 그 Diffuse Texture 가 몇 장인지.
        _uint iNumTextures = _pAIMaterial->GetTextureCount(aiTextureType(i));
        WriteFile(_pSaveFile, &iNumTextures, sizeof(_uint), &dwByte, nullptr);

        m_vecMtrl_Textures[i].reserve(iNumTextures);

        for (size_t j = 0; j < iNumTextures; ++j)
        {
            aiString        strTextureFilePath;
            ID3D11ShaderResourceView* pSRV = { nullptr };

            if (FAILED(_pAIMaterial->GetTexture(aiTextureType(i), (_uint)j, &strTextureFilePath)))
                return E_FAIL;

            _char szDrive[MAX_PATH] = "";
            _char szDirectory[MAX_PATH] = "";
            _char szFileName[MAX_PATH] = "";
            _char szEXT[MAX_PATH] = "";

            _splitpath_s(_pModelFilePath, szDrive, MAX_PATH, szDirectory, MAX_PATH, nullptr, 0, nullptr, 0);
            _splitpath_s(strTextureFilePath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szEXT, MAX_PATH);

            _uint iExtLength = (_uint)strlen(szEXT);
            WriteFile(_pSaveFile, &iExtLength, sizeof(_uint), &dwByte, nullptr);

            for (_uint k = 0; k < iExtLength; ++k)
            {
                WriteFile(_pSaveFile, &szEXT, sizeof(_char), &dwByte, nullptr);
            }

            _char szFullPath[MAX_PATH] = "";
            strcpy_s(szFullPath, szDrive);
            strcat_s(szFullPath, szDirectory);
            strcat_s(szFullPath, szFileName);
            strcat_s(szFullPath, szEXT);

            _tchar szLastPath[MAX_PATH] = {};
            MultiByteToWideChar(CP_ACP, 0, szFullPath, (_uint)(strlen(szFullPath)), szLastPath, MAX_PATH);

            _uint iLength = (_uint)wcslen(szLastPath);
            WriteFile(_pSaveFile, &iLength, sizeof(_uint), &dwByte, nullptr);

            for (_uint k = 0; k < iLength; ++k)
            {
                WriteFile(_pSaveFile, &szLastPath[k], sizeof(_tchar), &dwByte, nullptr);
            }

            HRESULT hr = {};

            if (false == strcmp(szEXT, ".dds"))
                hr = CreateDDSTextureFromFile(m_pDevice, szLastPath, nullptr, &pSRV);
            else if (false == strcmp(szEXT, ".tga"))
                return E_FAIL;
            else
                hr = CreateWICTextureFromFile(m_pDevice, szLastPath, nullptr, &pSRV);

            if (FAILED(hr))
                return E_FAIL;

            m_vecMtrl_Textures[i].push_back(pSRV);
        }
    }

    return S_OK;
}

HRESULT CMaterial::Initialize_LoadData(HANDLE _pLoadFile, const _char* _pModelFilePath)
{
    DWORD       dwByte = {};

    for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
    {
        _uint iNumTextures = 0;
        ReadFile(_pLoadFile, &iNumTextures, sizeof(_uint), &dwByte, nullptr);

        m_vecMtrl_Textures[i].reserve(iNumTextures);

        for (size_t j = 0; j < iNumTextures; ++j)
        {
            ID3D11ShaderResourceView* pSRV = { nullptr };

            _uint iExtLength = 0;
            ReadFile(_pLoadFile, &iExtLength, sizeof(_uint), &dwByte, nullptr);

            _char szExtName[MAX_PATH] = {};
            for (_uint k = 0; k < iExtLength; ++k)
            {
                ReadFile(_pLoadFile, szExtName, sizeof(_char), &dwByte, nullptr);
            }


            _uint iLastPathLength = 0;
            _tchar szLstPathFileName[MAX_PATH] = {};
            ReadFile(_pLoadFile, &iLastPathLength, sizeof(_uint), &dwByte, nullptr);

            for (_uint k = 0; k < iLastPathLength; ++k)
            {
                ReadFile(_pLoadFile, &szLstPathFileName[k], sizeof(_tchar), &dwByte, nullptr);
            }

            HRESULT hr = {};

            if (false == strcmp(szExtName, ".dds"))
                hr = CreateDDSTextureFromFile(m_pDevice, szLstPathFileName, nullptr, &pSRV);
            else if (false == strcmp(szExtName, ".tga"))
                return E_FAIL;
            else
                hr = CreateWICTextureFromFile(m_pDevice, szLstPathFileName, nullptr, &pSRV);

            if (FAILED(hr))
                return E_FAIL;

            m_vecMtrl_Textures[i].push_back(pSRV);
        }
    }

    return S_OK;
}

HRESULT CMaterial::Bind_Material(CShader* _pShader, aiTextureType _eTextureType, const _char* _pConstantName, _uint _iTextureIndex)
{
    if (_iTextureIndex >= m_vecMtrl_Textures[_eTextureType].size())
        return E_FAIL;

    return _pShader->Bind_SRV(_pConstantName, m_vecMtrl_Textures[_eTextureType][_iTextureIndex]);
}

CMaterial* CMaterial::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const aiMaterial* _pAIMaterial, const _char* _pModelFilePath)
{
    CMaterial* pInstance = new CMaterial(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize(_pAIMaterial, _pModelFilePath)))
    {
        MSG_BOX("Failed To Created : CMaterial");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CMaterial* CMaterial::Create_SaveData(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const aiMaterial* _pAIMaterial, HANDLE _pSaveFile, const _char* _pModelFilePath)
{
    CMaterial* pInstance = new CMaterial(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_SaveData(_pAIMaterial, _pSaveFile, _pModelFilePath)))
    {
        MSG_BOX("Failed To Created : CMaterial Saved");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CMaterial* CMaterial::Create_LoadData(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, HANDLE _pLoadFile, const _char* _pModelFilePath)
{
    CMaterial* pInstance = new CMaterial(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_LoadData(_pLoadFile, _pModelFilePath)))
    {
        MSG_BOX("Failed To Created : CMaterial Loaded");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CMaterial::Free()
{
    __super::Free();

    for (auto& SRVs: m_vecMtrl_Textures)
    {
        for (auto& pSRV : SRVs)
        {
            Safe_Release(pSRV);
        }
        SRVs.clear();
    }

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

}
