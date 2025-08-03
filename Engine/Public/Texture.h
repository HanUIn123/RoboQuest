#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
private:
    CTexture(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CTexture(const CTexture& _Prototype);
    virtual ~CTexture() = default;

public:
    // ��� �θ� Component �� �ٸ���, ������ü�� ����ȭ �ϴ� ������ �������. override ���� ���� 
    virtual HRESULT Initialize_Prototype(const _tchar* _pTextureFilePath, _uint _iNumTexture);
    virtual HRESULT Initialize(void* _pArg) override;

public:
    HRESULT Bind_ShaderResource(class CShader* _pShader, const _char* _pConstantName, _uint _iIndex);
    // �������� �ؽ��ĸ� ���̴��� ���� �뵵�� �Լ�.
    HRESULT Bind_ShaderResource_Multi(class CShader* _pShader, const _char* _pConstantName);
private:
    _uint                                       m_iNumSRVs = { 0 };
    vector<ID3D11ShaderResourceView*>           m_vecSRVs;
    vector<ID3D11Texture2D*>                    m_vecTextureResource;



public:
    // �θ� Component������, 
    static CTexture*                Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pTextureFilePath, _uint _iNumTexture);
    virtual CComponent*             Clone(void* _pArg) override;
    virtual void                    Free() override;

};

END