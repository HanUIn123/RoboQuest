#include "VIBuffer_MapToolTerrain.h"


CVIBuffer_MapToolTerrain::CVIBuffer_MapToolTerrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CVIBuffer{ _pDevice, _pContext }
{
}

CVIBuffer_MapToolTerrain::CVIBuffer_MapToolTerrain(const CVIBuffer_MapToolTerrain& _Prototype)
    :CVIBuffer(_Prototype)
    , m_iNumverticesX{ _Prototype.m_iNumverticesX }
    , m_iNumverticesZ{ _Prototype.m_iNumverticesZ }
    , m_VertexPos{_Prototype.m_VertexPos}
{
}

HRESULT CVIBuffer_MapToolTerrain::Initialize_Prototype(const _tchar* _HeightMapTag, VTXNORTEX* _pLoadHeight)
{
    _ulong      dwByte = {};

    HANDLE      hFile = CreateFile(_HeightMapTag, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    if (0 == hFile)
        return E_FAIL;

    BITMAPFILEHEADER    fileHeader;
    ReadFile(hFile, &fileHeader, sizeof(fileHeader), &dwByte, nullptr);

    BITMAPINFOHEADER    infoHeader;
    ReadFile(hFile, &infoHeader, sizeof(infoHeader), &dwByte, nullptr);

    m_iNumverticesX = infoHeader.biWidth;
    m_iNumverticesZ = infoHeader.biHeight;
    m_iNumVertices = m_iNumverticesX * m_iNumverticesZ;


    // _uint* pPixel = new _uint[m_iNumVertices];
    // ReadFile(hFile, pPixel, sizeof(_uint) * m_iNumVertices, &dwByte, nullptr);

    m_pPixel = new _uint[m_iNumVertices];
    ReadFile(hFile, m_pPixel, sizeof(_uint) * m_iNumVertices, &dwByte, nullptr);

    m_iVertexStride = sizeof(VTXNORTEX);

    m_iNumIndices = (m_iNumverticesX - 1) * (m_iNumverticesZ - 1) * 2 * 3;

    m_iIndexStride = 4;

    m_iNumVertexBuffers = 1;

    m_eIndexFormat = DXGI_FORMAT_R32_UINT;

    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    m_VertexPos = new XMVECTOR[m_iNumVertices];



#pragma region VERTEX_BUFFER
    VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

    _uint iIndex = { 0 };

    if (nullptr == _pLoadHeight)
    {
        for (_ushort i = 0; i < m_iNumverticesZ; ++i)
        {
            for (_ushort j = 0; j < m_iNumverticesX; ++j)
            {
                iIndex = i * m_iNumverticesX + j;

                //pVertices[iIndex].vPosition = _float3(((_float)j - 0.5f) * VTXITV, 0.0f, ((_float)i - 0.5f) * VTXITV);
                //pVertices[iIndex].vPosition = _float3(j, (pPixel[iIndex] & 0x000000ff) / 10.0f, i);
                pVertices[iIndex].vPosition = _float3(j, 0.0f, i);
                //pVertices[iIndex].vPosition = _float3(j, , i);
                //pVertices[iIndex].vPosition = _float3(j, (m_pPixel[iIndex] & 0x000000ff) / 10.0f, i);


                pVertices[iIndex].vNormal = _float3(0.0f, 0.0f, 0.0f);
                //pVertices[iIndex].vTexCoord = _float2((_float(j) / (VTXCNTX - 1)), (_float(i) / (VTXCNTZ - 1)));
                pVertices[iIndex].vTexCoord = _float2(j / (m_iNumverticesX - 1.0f), i / (m_iNumverticesZ - 1.0f));


                pVertices[iIndex].vTexIndex = _float3(0.0f, 0.0f, 0.0f);

                m_VertexPos[iIndex] = XMLoadFloat3(&pVertices[iIndex].vPosition);

            }
        }
    }
    else
    {
        for (_ushort i = 0; i < m_iNumverticesZ; ++i)
        {
            for (_ushort j = 0; j < m_iNumverticesX; ++j)
            {
                iIndex = i * m_iNumverticesX + j;

                memcpy(pVertices, _pLoadHeight, sizeof(VTXNORTEX) * m_iNumVertices);

                m_VertexPos[iIndex] = XMLoadFloat3(&pVertices[iIndex].vPosition);
            }
        }
    }


#pragma endregion

#pragma region INDEX_BUFFER

    _uint* pIndices = new _uint[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

    // _ushort		dwTriCnt(0);
    _uint           iNumIndices = { 0 };

    for (_ushort i = 0; i < m_iNumverticesZ - 1; ++i)
    {
        for (_ushort j = 0; j < m_iNumverticesX - 1; ++j)
        {
            iIndex = i * m_iNumverticesX + j;
            _uint iIndices[4] =
            {
                iIndex + m_iNumverticesX,
                iIndex + m_iNumverticesX + 1,
                iIndex + 1,
                iIndex
            };

            //  vNormal °ª Ãß°¡ 
            _vector     vSour, vDest, vNormal;

            pIndices[iNumIndices++] = iIndices[0];
            pIndices[iNumIndices++] = iIndices[1];
            pIndices[iNumIndices++] = iIndices[2];

            vSour = XMLoadFloat3(&pVertices[iIndices[1]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
            vDest = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[1]].vPosition);
            vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

            XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[1]].vNormal, XMLoadFloat3(&pVertices[iIndices[1]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);

            pIndices[iNumIndices++] = iIndices[0];
            pIndices[iNumIndices++] = iIndices[2];
            pIndices[iNumIndices++] = iIndices[3];

            vSour = XMLoadFloat3(&pVertices[iIndices[2]].vPosition) - XMLoadFloat3(&pVertices[iIndices[0]].vPosition);
            vDest = XMLoadFloat3(&pVertices[iIndices[3]].vPosition) - XMLoadFloat3(&pVertices[iIndices[2]].vPosition);
            vNormal = XMVector3Normalize(XMVector3Cross(vSour, vDest));

            XMStoreFloat3(&pVertices[iIndices[0]].vNormal, XMLoadFloat3(&pVertices[iIndices[0]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[2]].vNormal, XMLoadFloat3(&pVertices[iIndices[2]].vNormal) + vNormal);
            XMStoreFloat3(&pVertices[iIndices[3]].vNormal, XMLoadFloat3(&pVertices[iIndices[3]].vNormal) + vNormal);
        }
    }

    for (size_t i = 0; i < m_iNumVertices; ++i)
        XMStoreFloat3(&pVertices[i].vNormal, XMVector3Normalize(XMLoadFloat3(&pVertices[i].vNormal)));
#pragma endregion
    ZeroMemory(&m_tagBufferDesc, sizeof(m_tagBufferDesc));
    m_tagBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    m_tagBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    m_tagBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_tagBufferDesc.StructureByteStride = m_iVertexStride;
    m_tagBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_tagBufferDesc.MiscFlags = 0;

    ZeroMemory(&m_tagInitialData, sizeof(m_tagInitialData));
    m_tagInitialData.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    ZeroMemory(&m_tagBufferDesc, sizeof(m_tagBufferDesc));
    m_tagBufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
    m_tagBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_tagBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_tagBufferDesc.StructureByteStride = m_iIndexStride;
    m_tagBufferDesc.CPUAccessFlags = 0;
    m_tagBufferDesc.MiscFlags = 0;

    ZeroMemory(&m_tagInitialData, sizeof(m_tagInitialData));
    m_tagInitialData.pSysMem = pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);
    Safe_Delete_Array(_pLoadHeight);
    Safe_Delete_Array(pIndices);
    Safe_Delete_Array(m_pPixel);

    CloseHandle(hFile);

    return S_OK;
}

HRESULT CVIBuffer_MapToolTerrain::Initialize(void* _pArg)
{
    return S_OK;
}

void CVIBuffer_MapToolTerrain::Mapping_Buffer(XMVECTOR* _pVertices)
{
    //D3D11_MAPPED_SUBRESOURCE tagSubResource = {};
    //m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &tagSubResource);
    //memcpy(tagSubResource.pData, _pVertices, sizeof(XMVECTOR) * m_iNumVertices);
    //m_pContext->Unmap(m_pVB, 0);

}

void CVIBuffer_MapToolTerrain::Set_MapToolTerrainShape()
{

}


CVIBuffer_MapToolTerrain* CVIBuffer_MapToolTerrain::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _HeightMapTag, VTXNORTEX* _pLoadHeight)
{
    CVIBuffer_MapToolTerrain* pInstance = new CVIBuffer_MapToolTerrain(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype(_HeightMapTag, _pLoadHeight)))
    {
        MSG_BOX("Failed To Created : CVIBuffer_MapToolTerrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_MapToolTerrain::Clone(void* _pArg)
{
    CVIBuffer_MapToolTerrain* pInstance = new CVIBuffer_MapToolTerrain(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CVIBuffer_MapToolTerrain");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CVIBuffer_MapToolTerrain::Free()
{
    __super::Free();

    if(false == m_bClone)
        Safe_Delete_Array(m_VertexPos);
}
