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
    // Rect Buffer ��, ������ �߰��� Cube �� Terrain �� �ʿ��� ���� ����, ������ ũ�� ���� �������� �θ�Ÿ����, VIBuffer Ŭ������ �����صΰ�,
    // ������ Ŭ�������� �� �����鿡 ���� �ʱⰪ�� ��������.
    
    // �簢���� �׸��� ���� ���� ���� 4��.
    m_iNumVertices = 4;

    // ������ ũ�� 
    m_iVertexStride = sizeof(VTXPOSTEX);

    // ���� 4�� ������ ǥ���ϱ�����? 0-1-2 / 2-3-0 �׸��� ������ ����, �� �� ��° �� ���� �ε��� ����.
    m_iNumIndices = 6;

    // �ε����� 2����Ʈ�� �׸� ��. (�������� �޿�� ���η� ä�� ��, ��ü�� �� ����Ʈ�ΰ�) 
    // signed short ���� 2����Ʈ�ε�, ǥ���� �� �ִ� ���� 65535 �� . ���� ���� �׸� �������� �� ���� �Ѿ��~ �� ������ ���� 
    // ǥ���� �ε����� ũ��
    m_iIndexStride = 2;

    // �� ���� ���ؽ� ���۸� ���ε� �� ���ΰ�.
    m_iNumVertexBuffers = 1;

    // �ε��� ũ�Ⱑ 2����Ʈ, �� 16��Ʈ, �׸��� �ε����� ���� Ÿ��.
    m_eIndexFormat = DXGI_FORMAT_R16_UINT;

    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEX_BUFFER
    ZeroMemory(&m_tagBufferDesc, sizeof(m_tagBufferDesc));

    // ���� ������� �ϴ� ������ ũ�� * ���� ���� 
    m_tagBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    // ���۸� ���� ���۷� ����ڴ�.
    m_tagBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    // ��� ������ � ���ε��� �� ���ΰ�.
    m_tagBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    // ������ ũ�� 
    m_tagBufferDesc.StructureByteStride = m_iVertexStride;
    // ���� ������ ��쿡�� CPUAccessFlags �� MiscFlags �� 0 .
    m_tagBufferDesc.CPUAccessFlags = 0;
    m_tagBufferDesc.MiscFlags = 0;

    VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

    // pVertices ���� 4���� ��ġ ����.
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
