#include "TrailBuffer.h"

CTrailBuffer::CTrailBuffer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CVIBuffer{ _pDevice, _pContext }
{
}

CTrailBuffer::CTrailBuffer(const CTrailBuffer& _Prototype)
    :CVIBuffer(_Prototype)
{
}

HRESULT CTrailBuffer::Initialize_Prototype()
{
    m_iNumVertices = 4 * 16;

    m_iVertexStride = sizeof(VTXPOSTEX);

    //m_iNumIndices = 8 * 2 * 3;
    m_iNumIndices = (_uint)(m_iNumVertices * 0.5 * 3);

    m_iIndexStride = 2;

    m_iNumVertexBuffers = 1;

    m_eIndexFormat = DXGI_FORMAT_R16_UINT;

    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

#pragma region VERTEX_BUFFER
    ZeroMemory(&m_tagBufferDesc, sizeof(m_tagBufferDesc));

    m_tagBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    m_tagBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    m_tagBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_tagBufferDesc.StructureByteStride = m_iVertexStride;
    m_tagBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_tagBufferDesc.MiscFlags = 0;

    VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);


    pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
    pVertices[0].vTexCoord = _float2(0.f, 0.f);

    pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
    pVertices[1].vTexCoord = _float2(1.f, 0.f);

    pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
    pVertices[2].vTexCoord = _float2(1.f, 1.f);

    pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
    pVertices[3].vTexCoord = _float2(0.f, 1.f);

    ZeroMemory(&m_tagInitialData, sizeof(m_tagInitialData));
    m_tagInitialData.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region INDEX_BUFFER
    ZeroMemory(&m_tagBufferDesc, sizeof(m_tagBufferDesc));

    m_tagBufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
    m_tagBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    m_tagBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_tagBufferDesc.StructureByteStride = m_iIndexStride;
    m_tagBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_tagBufferDesc.MiscFlags = 0;

    _ushort* pIndices = new _ushort[m_iNumIndices ];
    ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

    for (_uint i = 0; i < 16; ++i)
    {
        pIndices[6 * i] = 2 * i;
        pIndices[6 * i + 1] = 2 * i + 2;
        pIndices[6 * i + 2] = 2 * i + 3;

        pIndices[6 * i + 3] = 2 * i;
        pIndices[6 * i + 4] = 2 * i + 3;
        pIndices[6 * i + 5] = 2 * i + 1;

    }

    ZeroMemory(&m_tagInitialData, sizeof(m_tagInitialData));
    m_tagInitialData.pSysMem = pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;

    Safe_Delete_Array(pIndices);
#pragma endregion


    return S_OK;
}

HRESULT CTrailBuffer::Initialize(void* _pArg)
{
    return S_OK;
}

void CTrailBuffer::Update_Trail_Vertex(const deque<_float3>& _vBulletPos, const _float3& _vUp, _matrix _mBulletMatrix)
{
    D3D11_MAPPED_SUBRESOURCE tagSubResource = {};
    m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &tagSubResource);

    ZeroMemory(&m_tagBufferDesc, sizeof(m_tagBufferDesc));
    m_tagBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    m_tagBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    m_tagBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_tagBufferDesc.StructureByteStride = m_iVertexStride;
    m_tagBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    m_tagBufferDesc.MiscFlags = 0;

    VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXPOSTEX) * m_iNumVertices);

    for (_uint i = 0; i <= _vBulletPos.size() - 1; ++i)
    {
        _float3 vPos = {};
        _float fTexCoord = { 1.0f };
        _vector vLocalPos = XMVector3TransformCoord(XMLoadFloat3(&_vBulletPos[i]), XMMatrixInverse(nullptr, _mBulletMatrix));

        XMStoreFloat3(&vPos, vLocalPos);
        _vector vUp = (XMLoadFloat3(&_vUp));

        //vPos.y += 1.0f;
        vPos.y += vUp.m128_f32[1];
        XMStoreFloat3(&pVertices[2 * i].vPosition, XMLoadFloat3(&vPos));

        //vPos.y -= 2.0f;
        vPos.y -= vUp.m128_f32[1] * 2.0f;
        XMStoreFloat3(&pVertices[2 * i + 1].vPosition, XMLoadFloat3(&vPos));

        pVertices[2 * i].vTexCoord.x = (_float)((i / (_vBulletPos.size() - 1)));
        pVertices[2 * i].vTexCoord.y = 0.0f;

        pVertices[2 * i + 1].vTexCoord.x = (_float)((i / (_vBulletPos.size() - 1)));
        pVertices[2 * i + 1].vTexCoord.y = 1.0f;
    }

    ZeroMemory(&m_tagInitialData, sizeof(m_tagInitialData));
    m_tagInitialData.pSysMem = pVertices;


    m_pContext->Unmap(m_pVB, 0);

    Safe_Release(m_pVB);

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return;

    Safe_Delete_Array(pVertices);
}

CTrailBuffer* CTrailBuffer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTrailBuffer* pInstance = new CTrailBuffer(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CTrailBuffer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CTrailBuffer::Clone(void* _pArg)
{
    CTrailBuffer* pInstance = new CTrailBuffer(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CTrailBuffer");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CTrailBuffer::Free()
{
    __super::Free();
}
