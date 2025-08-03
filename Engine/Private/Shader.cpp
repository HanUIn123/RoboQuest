#include "Shader.h"

CShader::CShader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CComponent{_pDevice, _pContext}
    , m_pEffect{nullptr}
{
}

CShader::CShader(const CShader& _Prototype)
    :CComponent(_Prototype)
    , m_pEffect{_Prototype.m_pEffect}
    , m_iNumPasses{_Prototype.m_iNumPasses}
    , m_vecInputLayout{_Prototype.m_vecInputLayout}
{
    for (auto& pInputLayout : m_vecInputLayout)
        Safe_AddRef(pInputLayout);

    Safe_AddRef(m_pEffect);
}

HRESULT CShader::Initialize_Prototype(const _tchar* _pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* _pVertexElements, _uint _iNumElements)
{
    _uint			iHlslFlag = {};

#ifdef _DEBUG
    iHlslFlag = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    iHlslFlag = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif 

    ID3DBlob* pErrMsg = { nullptr };

    if (FAILED(D3DX11CompileEffectFromFile(_pShaderFilePath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, iHlslFlag, 0, m_pDevice, &m_pEffect, &pErrMsg)))
        return E_FAIL;

    // 
    ID3DX11EffectTechnique* pTechnique = m_pEffect->GetTechniqueByIndex(0);
    if (nullptr == pTechnique)
        return E_FAIL;

    D3DX11_TECHNIQUE_DESC   TechniqueDesc{};
    // 테크닉의 정보를 가져오기. 
    pTechnique->GetDesc(&TechniqueDesc);

    // 얻어온 테크닉의 pass 갯수.
    m_iNumPasses = TechniqueDesc.Passes;

    // 그 pass 갯수 만큼 루프 돌려서, 최종적으로 pass 의 정보에 접근한다. 
    for (size_t i = 0; i < m_iNumPasses; ++i)
    {
        // 테크닉의 특정 pass 가져왔고,
        ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex((uint32_t)i);
        ID3D11InputLayout* pInputLayout = { nullptr };

        D3DX11_PASS_DESC        PassDesc{};

        // 가져온 특정 pass 에서 pass의 구조체 주소를 가져와서, 채워주기.
        pPass->GetDesc(&PassDesc);

        // 내가 그릴려고 하는 정점은, 어떻게 생겼다. (D3D11_INPUT_ELEMENT_DESC 구조체) / 정점의 멤버 변수는 ~개를 가진다. / 쉐이더가 어떤 정점을 받고있는가, / 쉐이더가 받고있는 정점의 크기 / &pInputlayout
        // pInputLayout 에다가 담기, 그걸 벡터에 집어넣기.
        // PassDesc.pIAInputSignature & PassDesc.IAInputSignatureSize 라고 채우기 위해서, 
        if (FAILED(m_pDevice->CreateInputLayout(_pVertexElements, _iNumElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &pInputLayout)))
            return E_FAIL;

        m_vecInputLayout.push_back(pInputLayout);
    }


    return S_OK;
}

HRESULT CShader::Initialize(void* _pArg)
{
    return S_OK;
}

HRESULT CShader::Begin(_uint _iPassIndex)
{
    if (_iPassIndex >= m_iNumPasses)
        return E_FAIL;

    m_pContext->IASetInputLayout(m_vecInputLayout[_iPassIndex]);

    m_pEffect->GetTechniqueByIndex(0)->GetPassByIndex(_iPassIndex)->Apply(0, m_pContext);

    return S_OK;
}

// 인자로 이름을 받아와서, 그 이름을 가진 전역변수 핸들을 가져오게끔.
// 셰이더에서 행렬 곱하기 연산을 제대로 수행가능하게끔, 
// 셰이더 외부의 값을 셰이더에 어떻게 던져 줄 것인가.
HRESULT CShader::Bind_Matrix(const _char* _pConstantName, const _float4x4* _pMatrix)
{
    if (nullptr == m_pEffect)
        return E_FAIL;

    // 내가 지정한 셰이더 내에 선언된 전역변수의 핸들을 얻어온다.
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
    if (nullptr == pVariable)
        return E_FAIL;

    // 이게 진짜 행렬인가 에 대한 체크 함수가 AsMatrix 함수, 
    ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
    if (nullptr == pMatrixVariable)
        return E_FAIL;

    // SetMatrix : 이 전역 변수에, 내가 넘겨준 행렬을 넣어줄 것이다.
    // 포인터 끼리 형변환이 필요해서 reinterpret_cast으로 float 의 주소로 형변환.
    return pMatrixVariable->SetMatrix(reinterpret_cast<const _float*>(_pMatrix));
}

HRESULT CShader::Bind_SRV(const _char* _pConstantName, ID3D11ShaderResourceView* _pSRV)
{
    // 행렬 과 마찬가지로, 전역 변수의 이름을 가진 핸들을 가져오게끔, 
    ID3DX11EffectVariable*                  pVariable = m_pEffect->GetVariableByName(_pConstantName);
    if (nullptr == pVariable)
        return E_FAIL;

    ID3DX11EffectShaderResourceVariable*    pSRvariable = pVariable->AsShaderResource();
    if (nullptr == pSRvariable)
        return E_FAIL;

    return pSRvariable->SetResource(_pSRV);
}

HRESULT CShader::Bind_SRV_Multi(const _char* _pConstantName, ID3D11ShaderResourceView** _ppSRV, _uint _iNumSRVs)
{
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
    if (nullptr == pVariable)
        return E_FAIL;

    ID3DX11EffectShaderResourceVariable* pSRvariable = pVariable->AsShaderResource();
    if (nullptr == pSRvariable)
        return E_FAIL;

    // 여러 장을 던질 수 있게 되었음.
    return pSRvariable->SetResourceArray(_ppSRV, 0, _iNumSRVs);
}

HRESULT CShader::Bind_RawValue(const _char* _pConstantName, const void* _pData, _uint _iSize)
{
    if (nullptr == m_pEffect)
        return E_FAIL;

    // 내가 지정한 셰이더 내에 선언된 전역변수의 핸들을 얻어온다.
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
    if (nullptr == pVariable)
        return E_FAIL;

    pVariable->SetRawValue(_pData, 0, _iSize);

    return S_OK;
}

HRESULT CShader::Bind_Matrices(const _char* _pConstantName, const _float4x4* _pMatrix, _uint _iNumMatrices)
{
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
    if (nullptr == pVariable)
        return E_FAIL;

    // 이게 진짜 행렬인가 에 대한 체크 함수가 AsMatrix 함수, 
    ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
    if (nullptr == pMatrixVariable)
        return E_FAIL;

    return pMatrixVariable->SetMatrixArray(reinterpret_cast<const _float*>(_pMatrix),0,_iNumMatrices);
}

CShader* CShader::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* _pVertexElements, _uint _iNumElements)
{
    CShader* pInstance = new CShader(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype(_pShaderFilePath, _pVertexElements,_iNumElements)))
    {
        MSG_BOX("Failed To Create : CShader");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CShader::Clone(void* _pArg)
{
    CShader* pInstance = new CShader(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CShader");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CShader::Free()
{
    __super::Free();

    for (auto& pInputLayout : m_vecInputLayout)
        Safe_Release(pInputLayout);

    m_vecInputLayout.clear();

    Safe_Release(m_pEffect);
}
