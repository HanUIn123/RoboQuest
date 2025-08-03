#include "VIBuffer_Terrain.h"

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CVIBuffer{_pDevice, _pContext}
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain& _Prototype)
    :CVIBuffer(_Prototype)
    ,  m_iNumverticesX{_Prototype.m_iNumverticesX}
    ,  m_iNumverticesZ{_Prototype.m_iNumverticesZ}
    , m_VertexPos{_Prototype.m_VertexPos }
{
}

HRESULT CVIBuffer_Terrain::Initialize_Prototype(const _tchar* _HeightMapTag, VTXNORTEX* _pLoadData)
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

    _uint* pPixel = new _uint[m_iNumVertices];
    ReadFile(hFile, pPixel, sizeof(_uint) * m_iNumVertices, &dwByte, nullptr);

    m_iVertexStride = sizeof(VTXNORTEX);

    m_iNumIndices = (m_iNumverticesX - 1) * (m_iNumverticesZ - 1) * 2 * 3;

    m_iIndexStride = 4;

    m_iNumVertexBuffers = 1;

    m_eIndexFormat = DXGI_FORMAT_R32_UINT;

    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // m_VertexPos 메모리 할당
    m_VertexPos = new XMVECTOR[m_iNumVertices];

#pragma region VERTEX_BUFFER
    VTXNORTEX* pVertices = new VTXNORTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

    _uint iIndex = { 0 };

    for (_ushort i = 0; i < m_iNumverticesZ; ++i)
    {
        for (_ushort j = 0; j < m_iNumverticesX; ++j)
        {
            iIndex = i * m_iNumverticesX + j;

            //pVertices[iIndex].vPosition = _float3(((_float)j - 0.5f) * VTXITV, 0.0f, ((_float)i - 0.5f) * VTXITV);
            //pVertices[iIndex].vPosition = _float3(j, (pPixel[iIndex] & 0x000000ff) / 10.0f, i);
            //pVertices[iIndex].vPosition = _float3(j, _fLoadedHeightData, i);
            //pVertices[iIndex].vNormal = _float3(0.0f, 0.0f, 0.0f);
            //pVertices[iIndex].vTexCoord = _float2((_float(j) / (VTXCNTX - 1)), (_float(i) / (VTXCNTZ - 1)));
            //pVertices[iIndex].vPosition = _pLoadData[iIndex].vPosition;
            //pVertices[iIndex].vNormal = _float3(0.0f, 0.0f, 0.0f);
            //pVertices[iIndex].vTexCoord = _float2(j / (m_iNumverticesX - 1.0f), i / (m_iNumverticesZ - 1.0f));

            //pVertices[iIndex].vTexIndex = _float3(0.0f, 0.0f, 0.0f);
            memcpy(pVertices, _pLoadData, sizeof(VTXNORTEX) * m_iNumVertices);

            // m_VertexPos 배열에 정점의 위치 저장
            m_VertexPos[iIndex] = XMVectorSet(pVertices[iIndex].vPosition.x, pVertices[iIndex].vPosition.y, pVertices[iIndex].vPosition.z, 1.0f); // w 성분 설정
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

            //  vNormal 값 추가 
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
    m_tagBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_tagBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_tagBufferDesc.StructureByteStride = m_iVertexStride;
    m_tagBufferDesc.CPUAccessFlags = 0;
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
    Safe_Delete_Array(pIndices);
    Safe_Delete_Array(pPixel);
    CloseHandle(hFile);

    return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void* _pArg)
{
    return S_OK;
}

_float CVIBuffer_Terrain::Get_Height(const XMFLOAT3& _vPos)
{
    if (!m_VertexPos || m_iNumverticesX <= 1 || m_iNumverticesZ <= 1)
        return 0.0f;

    _ulong uCurrentX = static_cast<_ulong>(_vPos.x);
    _ulong uCurrentZ = static_cast<_ulong>(_vPos.z);

    if (uCurrentX >= m_iNumverticesX - 1 || uCurrentZ >= m_iNumverticesZ - 1)
        return 0.0f;

    _ulong dwIndex = uCurrentZ * m_iNumverticesX + uCurrentX;

    _float fRatioX = _vPos.x - XMVectorGetX(m_VertexPos[dwIndex]);
    _float fRatioZ = XMVectorGetZ(m_VertexPos[dwIndex]) - _vPos.z;

    XMVECTOR        v0, v1, v2;

    // 우상단
    if (fRatioX > fRatioZ)
    {
        v0 = m_VertexPos[dwIndex];
        v1 = m_VertexPos[dwIndex + 1];
        v2 = m_VertexPos[dwIndex + m_iNumverticesX + 1];
    }
    else
    {
        // 좌하단
        v0 = m_VertexPos[dwIndex];
        v1 = m_VertexPos[dwIndex + m_iNumverticesX];
        v2 = m_VertexPos[dwIndex + m_iNumverticesX + 1];
    }

    XMVECTOR    vDir0 = v1 - v0;
    XMVECTOR    vDir1 = v2 - v0;
    XMVECTOR    vNormal = XMVector3Normalize(XMVector3Cross(vDir0, vDir1));

    // Ax + By + Cz + D = 0 에서 
    // A B C 는 법선 벡터의 x y z 성분임. 
    m_vPlane.x = XMVectorGetX(vNormal);
    m_vPlane.y = XMVectorGetY(vNormal);
    m_vPlane.z = XMVectorGetZ(vNormal);
    m_vPlane.w = -( m_vPlane.x * XMVectorGetX(v0) + m_vPlane.y * XMVectorGetY(v0) + m_vPlane.z * XMVectorGetZ(v0));

    //  return (-m_vPlane.x * XMVectorGetX(_vPosition) - (m_vPlane.z * XMVectorGetZ(_vPosition)) - m_vPlane.w) / m_vPlane.y;
    return (-m_vPlane.x * _vPos.x - m_vPlane.z * _vPos.z - m_vPlane.w) / m_vPlane.y;
}

CVIBuffer_Terrain* CVIBuffer_Terrain::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _HeightMapTag, VTXNORTEX* _pLoadData)
{
    CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype(_HeightMapTag, _pLoadData)))
    {
        MSG_BOX("Failed To Created : CVIBuffer_Terrain");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_Terrain::Clone(void* _pArg)
{
    CVIBuffer_Terrain* pInstance = new CVIBuffer_Terrain(*this);


    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CVIBuffer_Terrain");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CVIBuffer_Terrain::Free()
{
    __super::Free();

    if (false == m_bClone)
        Safe_Delete_Array(m_VertexPos);
}
