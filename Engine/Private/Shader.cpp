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
    // ��ũ���� ������ ��������. 
    pTechnique->GetDesc(&TechniqueDesc);

    // ���� ��ũ���� pass ����.
    m_iNumPasses = TechniqueDesc.Passes;

    // �� pass ���� ��ŭ ���� ������, ���������� pass �� ������ �����Ѵ�. 
    for (size_t i = 0; i < m_iNumPasses; ++i)
    {
        // ��ũ���� Ư�� pass �����԰�,
        ID3DX11EffectPass* pPass = pTechnique->GetPassByIndex((uint32_t)i);
        ID3D11InputLayout* pInputLayout = { nullptr };

        D3DX11_PASS_DESC        PassDesc{};

        // ������ Ư�� pass ���� pass�� ����ü �ּҸ� �����ͼ�, ä���ֱ�.
        pPass->GetDesc(&PassDesc);

        // ���� �׸����� �ϴ� ������, ��� �����. (D3D11_INPUT_ELEMENT_DESC ����ü) / ������ ��� ������ ~���� ������. / ���̴��� � ������ �ް��ִ°�, / ���̴��� �ް��ִ� ������ ũ�� / &pInputlayout
        // pInputLayout ���ٰ� ���, �װ� ���Ϳ� ����ֱ�.
        // PassDesc.pIAInputSignature & PassDesc.IAInputSignatureSize ��� ä��� ���ؼ�, 
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

// ���ڷ� �̸��� �޾ƿͼ�, �� �̸��� ���� �������� �ڵ��� �������Բ�.
// ���̴����� ��� ���ϱ� ������ ����� ���డ���ϰԲ�, 
// ���̴� �ܺ��� ���� ���̴��� ��� ���� �� ���ΰ�.
HRESULT CShader::Bind_Matrix(const _char* _pConstantName, const _float4x4* _pMatrix)
{
    if (nullptr == m_pEffect)
        return E_FAIL;

    // ���� ������ ���̴� ���� ����� ���������� �ڵ��� ���´�.
    ID3DX11EffectVariable* pVariable = m_pEffect->GetVariableByName(_pConstantName);
    if (nullptr == pVariable)
        return E_FAIL;

    // �̰� ��¥ ����ΰ� �� ���� üũ �Լ��� AsMatrix �Լ�, 
    ID3DX11EffectMatrixVariable* pMatrixVariable = pVariable->AsMatrix();
    if (nullptr == pMatrixVariable)
        return E_FAIL;

    // SetMatrix : �� ���� ������, ���� �Ѱ��� ����� �־��� ���̴�.
    // ������ ���� ����ȯ�� �ʿ��ؼ� reinterpret_cast���� float �� �ּҷ� ����ȯ.
    return pMatrixVariable->SetMatrix(reinterpret_cast<const _float*>(_pMatrix));
}

HRESULT CShader::Bind_SRV(const _char* _pConstantName, ID3D11ShaderResourceView* _pSRV)
{
    // ��� �� ����������, ���� ������ �̸��� ���� �ڵ��� �������Բ�, 
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

    // ���� ���� ���� �� �ְ� �Ǿ���.
    return pSRvariable->SetResourceArray(_ppSRV, 0, _iNumSRVs);
}

HRESULT CShader::Bind_RawValue(const _char* _pConstantName, const void* _pData, _uint _iSize)
{
    if (nullptr == m_pEffect)
        return E_FAIL;

    // ���� ������ ���̴� ���� ����� ���������� �ڵ��� ���´�.
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

    // �̰� ��¥ ����ΰ� �� ���� üũ �Լ��� AsMatrix �Լ�, 
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
