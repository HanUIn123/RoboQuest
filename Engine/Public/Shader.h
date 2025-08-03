#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
    CShader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CShader(const CShader& _Prototype);
    virtual ~CShader() = default;

public:
    virtual HRESULT Initialize_Prototype(const _tchar* _pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* _pVertexElements, _uint _iNumElements);
    virtual HRESULT Initialize(void* _pArg);
public:
    HRESULT Begin(_uint _iPassIndex);
    // BackGround 객체에서, 뷰 & 투영행렬을 만들어서 쓸 수 있게, 셰이더 파일을 바인딩하겠다.
    HRESULT Bind_Matrix(const _char* _pConstantName, const _float4x4* _pMatrix);

    // Texture를 셰이더에 던져주는 기능이 필요하다.
    HRESULT Bind_SRV(const _char* _pConstantName, ID3D11ShaderResourceView* _pSRV);

    HRESULT Bind_SRV_Multi(const _char* _pConstantName, ID3D11ShaderResourceView** _ppSRV, _uint _iNumSRVs);


    HRESULT Bind_RawValue(const _char* _pConstantName, const void* _pData, _uint _iSize);

    HRESULT Bind_Matrices(const _char* _pConstantName, const _float4x4* _pMatrix, _uint _iNumMatrices);

private:
    ID3DX11Effect*                  m_pEffect = { nullptr };
    vector<ID3D11InputLayout*>      m_vecInputLayout;
    _uint                           m_iNumPasses = {};

public:
    static CShader* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* _pVertexElements, _uint _iNumElements);
    virtual CComponent* Clone(void* _pArg) override;
    virtual void Free() override;

};

END