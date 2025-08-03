#include "VIBuffer_Cube.h"

CVIBuffer_Cube::CVIBuffer_Cube(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CVIBuffer{ _pDevice, _pContext }
{
}

CVIBuffer_Cube::CVIBuffer_Cube(const CVIBuffer_Cube& _Prototype)
    :CVIBuffer(_Prototype)
{
}

HRESULT CVIBuffer_Cube::Initialize_Prototype()
{
    m_iNumVertices = 8;
    m_iVertexStride = sizeof(VTXCUBE);
    m_iNumIndices = 36;
    m_iIndexStride = 2;
    m_iNumVertexBuffers = 1;
    m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
    ZeroMemory(&m_tagBufferDesc, sizeof m_tagBufferDesc);
    m_tagBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    m_tagBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_tagBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_tagBufferDesc.StructureByteStride = m_iVertexStride;
    m_tagBufferDesc.CPUAccessFlags = 0;
    m_tagBufferDesc.MiscFlags = 0;

    VTXCUBE* pVertices = new VTXCUBE[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXCUBE) * m_iNumVertices);

    _float fOffsetX = 0.0f;

    pVertices[0].vPosition = _float3(-1.f * 0.5f + fOffsetX, 1.f * 0.5f, -1.f * 0.5f);
    pVertices[0].vTexcoord = pVertices[0].vPosition;

    pVertices[1].vPosition = _float3(1.f * 0.5f + fOffsetX, 1.f * 0.5f, -1.f * 0.5f);
    pVertices[1].vTexcoord = pVertices[1].vPosition;

    pVertices[2].vPosition = _float3(1.f * 0.5f + fOffsetX, -1.f * 0.5f, -1.f * 0.5f);
    pVertices[2].vTexcoord = pVertices[2].vPosition;

    pVertices[3].vPosition = _float3(-1.f * 0.5f + fOffsetX, -1.f * 0.5f, -1.f * 0.5f);
    pVertices[3].vTexcoord = pVertices[3].vPosition;

    // ÈÄ¸é
    pVertices[4].vPosition = _float3(-1.f * 0.5f + fOffsetX, 1.f * 0.5f, 1.f * 0.5f);
    pVertices[4].vTexcoord = pVertices[4].vPosition;

    pVertices[5].vPosition = _float3(1.f * 0.5f + fOffsetX, 1.f * 0.5f, 1.f * 0.5f);
    pVertices[5].vTexcoord = pVertices[5].vPosition;

    pVertices[6].vPosition = _float3(1.f * 0.5f + fOffsetX, -1.f * 0.5f, 1.f * 0.5f);
    pVertices[6].vTexcoord = pVertices[6].vPosition;

    pVertices[7].vPosition = _float3(-1.f * 0.5f + fOffsetX, -1.f * 0.5f, 1.f * 0.5f);
    pVertices[7].vTexcoord = pVertices[7].vPosition;

    ZeroMemory(&m_tagInitialData, sizeof m_tagInitialData);
    m_tagInitialData.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region INDEX_BUFFER
    ZeroMemory(&m_tagBufferDesc, sizeof(m_tagBufferDesc));
    m_tagBufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
    m_tagBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_tagBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_tagBufferDesc.StructureByteStride = m_iIndexStride;
    m_tagBufferDesc.CPUAccessFlags = 0;
    m_tagBufferDesc.MiscFlags = 0;

    _ushort* pIndices = new _ushort[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

    pIndices[0] = 1; pIndices[1] = 5; pIndices[2] = 6;
    pIndices[3] = 1; pIndices[4] = 6; pIndices[5] = 2;

    pIndices[6] = 4; pIndices[7] = 0; pIndices[8] = 3;
    pIndices[9] = 4; pIndices[10] = 3; pIndices[11] = 7;


    pIndices[12] = 4; pIndices[13] = 5; pIndices[14] = 1;
    pIndices[15] = 4; pIndices[16] = 1; pIndices[17] = 0;


    pIndices[18] = 3; pIndices[19] = 2; pIndices[20] = 6;
    pIndices[21] = 3; pIndices[22] = 6; pIndices[23] = 7;


    pIndices[24] = 7; pIndices[25] = 6; pIndices[26] = 5;
    pIndices[27] = 7; pIndices[28] = 5; pIndices[29] = 4;


    pIndices[30] = 0; pIndices[31] = 1; pIndices[32] = 2;
    pIndices[33] = 0; pIndices[34] = 2; pIndices[35] = 3;

    ZeroMemory(&m_tagInitialData, sizeof m_tagInitialData);
    m_tagInitialData.pSysMem = pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);
#pragma endregion



    return S_OK;
}

HRESULT CVIBuffer_Cube::Initialize(void* _pArg)
{
    return S_OK;
}

CVIBuffer_Cube* CVIBuffer_Cube::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CVIBuffer_Cube");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_Cube::Clone(void* _pArg)
{
    CVIBuffer_Cube* pInstance = new CVIBuffer_Cube(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Created : CVIBuffer_Cube");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVIBuffer_Cube::Free()
{
    __super::Free();
}
