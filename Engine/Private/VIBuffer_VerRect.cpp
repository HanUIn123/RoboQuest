#include "VIBuffer_VerRect.h"

CVIBuffer_VerRect::CVIBuffer_VerRect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CVIBuffer{ _pDevice, _pContext }
{
}

CVIBuffer_VerRect::CVIBuffer_VerRect(const CVIBuffer_VerRect& _Prototype)
    :CVIBuffer(_Prototype)
{
}

HRESULT CVIBuffer_VerRect::Initialize_Prototype()
{
    // Rect Buffer 나, 앞으로 추가할 Cube 나 Terrain 이 필요한 정점 갯수, 정점의 크기 등의 변수들을 부모타입인, VIBuffer 클래스에 선언해두고,
     // 각각의 클래스에서 이 변수들에 대한 초기값을 세팅하자.

     // 사각형을 그리기 위한 정점 갯수 4개.
    m_iNumVertices = 4;

    // 정점의 크기 
    m_iVertexStride = sizeof(VTXPOSTEX);

    // 정점 4개 가지고 표현하기위한? 0-1-2 / 2-3-0 그리기 순서에 대한, 그 몇 번째 에 대한 인덱스 갯수.
    m_iNumIndices = 6;

    // 인덱스는 2바이트로 그릴 것. (정점들을 쭈우욱 가로로 채울 때, 전체가 몇 바이트인가) 
    // signed short 형이 2바이트인데, 표현할 수 있는 수가 65535 로 . 내가 만약 그릴 정점들이 몇 만개 넘어간다~ 그 갯수에 대한 
    // 표현할 인덱스의 크기
    m_iIndexStride = 2;

    // 몇 개의 버텍스 버퍼를 바인딩 할 것인가.
    m_iNumVertexBuffers = 1;

    // 인덱스 크기가 2바이트, 즉 16비트, 그리고 인덱스는 정수 타입.
    m_eIndexFormat = DXGI_FORMAT_R16_UINT;

    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEX_BUFFER
    ZeroMemory(&m_tagBufferDesc, sizeof(m_tagBufferDesc));

    // 내가 만들고자 하는 정점의 크기 * 정점 갯수 
    m_tagBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    // 버퍼를 정적 버퍼로 만들겠다.
    m_tagBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    // 어떠한 정보로 어떤 바인딩이 될 것인가.
    m_tagBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    // 정점의 크기 
    m_tagBufferDesc.StructureByteStride = m_iVertexStride;
    // 정적 버퍼인 경우에는 CPUAccessFlags 와 MiscFlags 는 0 .
    m_tagBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_tagBufferDesc.MiscFlags = 0;

    VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

    // pVertices 정점 4개의 위치 설정.
    pVertices[0].vPosition = _float3(-0.5f, 0.0f, 0.5f);
    pVertices[0].vTexCoord = _float2(0.f, 0.0f);

    pVertices[1].vPosition = _float3(0.5f, 0.0f, 0.5f);
    pVertices[1].vTexCoord = _float2(1.0f, 0.0f);

    pVertices[2].vPosition = _float3(0.5f, 0.0f, -0.5f);
    pVertices[2].vTexCoord = _float2(1.0f, 1.0f);

    pVertices[3].vPosition = _float3(-0.5f,0.0f, -0.5f);
    pVertices[3].vTexCoord = _float2(0.0f, 1.0f);

    ZeroMemory(&m_tagInitialData, sizeof(m_tagInitialData));
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

    pIndices[0] = 0;
    pIndices[1] = 1;
    pIndices[2] = 2;

    pIndices[3] = 0;
    pIndices[4] = 2;
    pIndices[5] = 3;

    ZeroMemory(&m_tagInitialData, sizeof(m_tagInitialData));
    m_tagInitialData.pSysMem = pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);
#pragma endregion
    return S_OK;
}

HRESULT CVIBuffer_VerRect::Initialize(void* _pArg)
{
    return S_OK;
}

void CVIBuffer_VerRect::Update_Vertex(_vector vTopLeft, _vector vTopRight, _vector vBottomRight, _vector vBottomLeft)
{    
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (FAILED(m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &mappedResource)))
        return;

    VTXPOSTEX* pVertices = reinterpret_cast<VTXPOSTEX*>(mappedResource.pData);

 
    XMStoreFloat3(&pVertices[0].vPosition, vTopLeft);     // 좌상단 (좁은 쪽)
    XMStoreFloat3(&pVertices[1].vPosition, vTopRight);    // 우상단 (좁은 쪽)
    XMStoreFloat3(&pVertices[2].vPosition, vBottomRight); // 우하단 (넓은 쪽)
    XMStoreFloat3(&pVertices[3].vPosition, vBottomLeft);  // 좌하단 (넓은 쪽)

 
    pVertices[0].vTexCoord = _float2(0.f, 0.0f);
    pVertices[1].vTexCoord = _float2(1.0f, 0.0f);
    pVertices[2].vTexCoord = _float2(1.0f, 1.0f);
    pVertices[3].vTexCoord = _float2(0.0f, 1.0f);

   
    m_pContext->Unmap(m_pVB, 0);
}

CVIBuffer_VerRect* CVIBuffer_VerRect::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CVIBuffer_VerRect* pInstance = new CVIBuffer_VerRect(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CVIBuffer_VerRect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_VerRect::Clone(void* _pArg)
{
    CVIBuffer_VerRect* pInstance = new CVIBuffer_VerRect(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CVIBuffer_VerRect");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CVIBuffer_VerRect::Free()
{
    __super::Free();
}
