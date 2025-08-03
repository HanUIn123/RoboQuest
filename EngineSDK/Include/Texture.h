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
    // 얘는 부모 Component 와 다르게, 원형객체를 복제화 하는 식으로 만들것임. override 해제 ㄱㄱ 
    virtual HRESULT Initialize_Prototype(const _tchar* _pTextureFilePath, _uint _iNumTexture);
    virtual HRESULT Initialize(void* _pArg) override;

public:
    HRESULT Bind_ShaderResource(class CShader* _pShader, const _char* _pConstantName, _uint _iIndex);
    // 여러개의 텍스쳐를 셰이더에 던질 용도의 함수.
    HRESULT Bind_ShaderResource_Multi(class CShader* _pShader, const _char* _pConstantName);
private:
    _uint                                       m_iNumSRVs = { 0 };
    vector<ID3D11ShaderResourceView*>           m_vecSRVs;
    vector<ID3D11Texture2D*>                    m_vecTextureResource;



public:
    // 부모 Component에서는, 
    static CTexture*                Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pTextureFilePath, _uint _iNumTexture);
    virtual CComponent*             Clone(void* _pArg) override;
    virtual void                    Free() override;

};

END