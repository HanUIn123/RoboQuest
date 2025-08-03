#pragma once
#include "Base.h"

BEGIN(Engine)
class CMaterial final: public CBase
{
private:
    CMaterial(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CMaterial() = default;

public:
    HRESULT Initialize(const aiMaterial* _pAIMaterial, const _char* _pModelFilePath);
    HRESULT Initialize_SaveData(const aiMaterial* _pAIMaterial, HANDLE _pSaveFile, const _char* _pModelFilePath);
    HRESULT Initialize_LoadData(HANDLE _pLoadFile, const _char* _pModelFilePath);
    HRESULT Bind_Material(class CShader* _pShader, aiTextureType _eTextureType, const _char* _pConstantName, _uint _iTextureIndex);

private:
    ID3D11Device* m_pDevice;
    ID3D11DeviceContext* m_pContext;
    vector<ID3D11ShaderResourceView*>       m_vecMtrl_Textures[AI_TEXTURE_TYPE_MAX];

public:
    static CMaterial* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const aiMaterial* _pAIMaterial, const _char* _pModelFilePath);
    static CMaterial* Create_SaveData(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const aiMaterial* _pAIMaterial, HANDLE _pSaveFile, const _char* _pModelFilePath);
    static CMaterial* Create_LoadData(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, HANDLE _pLoadFile, const _char* _pModelFilePath);
    virtual void Free() override;

};

END