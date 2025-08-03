#include "VIBuffer_Rect.h"

CVIBuffer_Rect::CVIBuffer_Rect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CVIBuffer{_pDevice, _pContext}
{
}

CVIBuffer_Rect::CVIBuffer_Rect(const CVIBuffer_Rect& _Prototype)
    :CVIBuffer(_Prototype)
{
}

HRESULT CVIBuffer_Rect::Initialize_Prototype()
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
    m_tagBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    // 어떠한 정보로 어떤 바인딩이 될 것인가.
    m_tagBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    // 정점의 크기 
    m_tagBufferDesc.StructureByteStride = m_iVertexStride;
    // 정적 버퍼인 경우에는 CPUAccessFlags 와 MiscFlags 는 0 .
    m_tagBufferDesc.CPUAccessFlags = 0;
    m_tagBufferDesc.MiscFlags = 0;

    VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

    // pVertices 정점 4개의 위치 설정.
    pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.0f);
    pVertices[0].vTexCoord = _float2(0.f, 0.0f);

    pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.0f);
    pVertices[1].vTexCoord = _float2(1.0f, 0.0f);

    pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.0f);
    pVertices[2].vTexCoord = _float2(1.0f, 1.0f);

    pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.0f);
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

HRESULT CVIBuffer_Rect::Initialize(void* _pArg)
{
    return S_OK;
}

CVIBuffer_Rect* CVIBuffer_Rect::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CVIBuffer_Rect");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CVIBuffer_Rect::Clone(void* _pArg)
{
    CVIBuffer_Rect* pInstance = new CVIBuffer_Rect(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CVIBuffer_Rect");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CVIBuffer_Rect::Free()
{
    __super::Free();
}
