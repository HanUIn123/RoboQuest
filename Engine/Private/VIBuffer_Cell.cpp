#include "../Public/VIBuffer_Cell.h"


CVIBuffer_Cell::CVIBuffer_Cell(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CVIBuffer{ _pDevice, _pContext }
{
}

CVIBuffer_Cell::CVIBuffer_Cell(const CVIBuffer_Cell& _Prototype)
    :CVIBuffer(_Prototype)
{
}

HRESULT CVIBuffer_Cell::Initialize_Prototype(const _float3* _pPoints)
{
    // 정점은 3개,
    m_iNumVertices = 3;
    m_iVertexStride = sizeof(VTXPOS);

    // 인덱스는 4개. : 01 - 12 - 23 - 30 이런식으로, 라인스트립할꺼임. 
    m_iNumIndices = 4;
    m_iIndexStride = 2;
    m_iNumVertexBuffers = 1;
    m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;


#pragma region VERTEX_BUFFER
    ZeroMemory(&m_tagBufferDesc, sizeof m_tagBufferDesc);
    m_tagBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    m_tagBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_tagBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_tagBufferDesc.StructureByteStride = m_iVertexStride;
    m_tagBufferDesc.CPUAccessFlags = 0;
    m_tagBufferDesc.MiscFlags = 0;

    VTXPOS* pVertices = new VTXPOS[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXPOS) * m_iNumVertices);

    _float3 offset = _float3(-0.0f, 0.0f, 0.0f);

    for (_uint i = 0; i < m_iNumVertices; ++i)
    {
        pVertices[i].vPosition = _float3(_pPoints[i].x + offset.x, _pPoints[i].y + offset.y, _pPoints[i].z + offset.z);
    }

    //memcpy(pVertices, _pPoints, sizeof(_float3) * 3);

    ZeroMemory(&m_tagInitialData, sizeof m_tagInitialData);
    m_tagInitialData.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region INDEX_BUFFER
    ZeroMemory(&m_tagBufferDesc, sizeof m_tagBufferDesc);
    m_tagBufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
    m_tagBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_tagBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_tagBufferDesc.StructureByteStride = m_iIndexStride;
    m_tagBufferDesc.CPUAccessFlags = 0;
    m_tagBufferDesc.MiscFlags = 0;

    _ushort* pIndices = new _ushort[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

    pIndices[0] = 0;
    pIndices[1] = 1;
    pIndices[2] = 2;
    pIndices[3] = 0;

    ZeroMemory(&m_tagInitialData, sizeof m_tagInitialData);
    m_tagInitialData.pSysMem = pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);
#pragma endregion

    return S_OK;
}

HRESULT CVIBuffer_Cell::Initialize(void* _pArg)
{
    return S_OK;
}

CVIBuffer_Cell* CVIBuffer_Cell::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _float3* _pPoints)
{
    CVIBuffer_Cell* pInstance = new CVIBuffer_Cell(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype(_pPoints)))
    {
        MSG_BOX("Failed To Created : CVIBuffer_Cell");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_Cell::Clone(void* _pArg)
{
    CVIBuffer_Cell* pInstance = new CVIBuffer_Cell(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Created : CVIBuffer_Cell");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Cell::Free()
{
    __super::Free();
}
