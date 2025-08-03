#include "../Public/VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CComponent{_pDevice, _pContext}
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& _Prototype)
    :CComponent(_Prototype)
    // 얕은 복사로 해도 괜찮은 이유 : 로컬 영역 기준, 버퍼는 항상 원점을 기준으로 한 초기값이 바뀌지 않고, 유지된 상태로 생성되야 해서, 얕은복사로도 괜찮다.
    , m_pVB{ _Prototype.m_pVB }
    , m_pIB{_Prototype.m_pIB}
    , m_iNumVertices{_Prototype.m_iNumVertices}
    , m_iVertexStride{_Prototype.m_iVertexStride}
    , m_iNumIndices{_Prototype.m_iNumIndices}
    , m_iIndexStride{_Prototype.m_iIndexStride}
    , m_iNumVertexBuffers{_Prototype.m_iNumVertexBuffers}
    , m_eIndexFormat{_Prototype.m_eIndexFormat}
    , m_ePrimitiveTopology{ _Prototype.m_ePrimitiveTopology }
{
    Safe_AddRef(m_pVB);
    Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CVIBuffer::Initialize(void* _pArg)
{
    return S_OK;
}

HRESULT CVIBuffer::Bind_InputAssembler()
{
    if (nullptr == m_pContext)
        return E_FAIL;

    ID3D11Buffer* pVertexBuffer[] = {
        m_pVB,
    };

    _uint				iVertexStrides[] = {
        m_iVertexStride,
    };

    _uint				iOffsets[] = {
        0
    };


    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffer, iVertexStrides, iOffsets);
    m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

    return S_OK;
}

HRESULT CVIBuffer::Render()
{
    if (nullptr == m_pContext)
        return E_FAIL;

    m_pContext->DrawIndexed(m_iNumIndices, 0, 0);

    return S_OK;
}

HRESULT CVIBuffer::Create_Buffer(ID3D11Buffer** _ppOut)
{
    // D3D11_BUFFER_DESC : 내가 지금 만들고자 하는 버퍼의 속성을 설정. (할당하고자 하는 크기, 동적 or 정적) 
    // D3D11_SUBRESOURCE_DATA : 내가 지금 만드는 공간에 채워줄 초기 값.
    return m_pDevice->CreateBuffer(&m_tagBufferDesc, &m_tagInitialData, _ppOut);
}

void CVIBuffer::Free()
{
    __super::Free();

    Safe_Release(m_pVB);
    Safe_Release(m_pIB);
}
