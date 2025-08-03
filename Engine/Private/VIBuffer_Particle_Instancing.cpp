#include "VIBuffer_Particle_Instancing.h"

CVIBuffer_Particle_Instancing::CVIBuffer_Particle_Instancing(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CVIBuffer{ _pDevice, _pContext }
{
}

CVIBuffer_Particle_Instancing::CVIBuffer_Particle_Instancing(const CVIBuffer_Particle_Instancing& _Prototype)
    :CVIBuffer(_Prototype)
    /*
     m_tagInstanceBufferDesc = {};
     m_InstanceInitialData = {};
     m_iInstanceVertexStride = {};
     m_iNumIndexPerInstance = {};
     m_iNumInstance = {};

     m_pInstanceVertices = { nullptr };
     m_pVBInstance = { nullptr };
    */
    , m_tagInstanceBufferDesc{ _Prototype.m_tagInstanceBufferDesc }
    , m_InstanceInitialData{ _Prototype.m_InstanceInitialData }
    , m_iInstanceVertexStride{ _Prototype.m_iInstanceVertexStride }
    , m_iNumIndexPerInstance{ _Prototype.m_iNumIndexPerInstance }
    , m_iNumInstance{ _Prototype.m_iNumInstance }
    , m_pVBInstance{ _Prototype.m_pVBInstance }
    , m_pInstanceVertices{ _Prototype.m_pInstanceVertices }
    , m_pSpeeds{ _Prototype.m_pSpeeds }
    , m_bLoop{ _Prototype.m_bLoop }
    , m_vPivot{ _Prototype.m_vPivot }
{
    Safe_AddRef(m_pVBInstance);
}

HRESULT CVIBuffer_Particle_Instancing::Initialize_Prototype(const PARTICLE_INSTANCING_DESC* _pDesc)
{
    return S_OK;
}

HRESULT CVIBuffer_Particle_Instancing::Initialize(void* _pArg)
{
    return m_pDevice->CreateBuffer(&m_tagInstanceBufferDesc, &m_InstanceInitialData, &m_pVBInstance);
}

HRESULT CVIBuffer_Particle_Instancing::Bind_InputAssembler()
{
    // Stride �� 2���� �ǰ�, ���� Particle Instancing Ŭ������ ��ӹ޴� �ڽĵ���,
    // ���� Ŭ������, Bind_InputAssembler �Լ��� ȣ�� �ϰ� ��.
    if (nullptr == m_pContext)
        return E_FAIL;

    ID3D11Buffer* pVertexBuffer[] = {
        m_pVB,
        m_pVBInstance
    };

    _uint           iVertexStrides[] = {
        m_iVertexStride,
        m_iInstanceVertexStride
    };

    _uint               iOffsets[] = { 0, 0 };

    // IASetVertexBuffers �� ȣ���� ��, �� ��ƼŬ �ν��Ͻ� ���۴�, ������ ���� ���� �޸�, �ν��Ͻ� ���� ������ �ϳ��� �� ���� ���Ե�.
    // ( ���� �׸��� �뵵 ) + ( �ν��Ͻ� �뵵 ) 
    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffer, iVertexStrides, iOffsets);
    m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

    return S_OK;
}

HRESULT CVIBuffer_Particle_Instancing::Render()
{
    if (nullptr == m_pContext)
        return E_FAIL;

    // �ν��Ͻ� �ϳ� �׸� �� ��� �ʿ����� / �ν��Ͻ� ���� / 0 / 0 
    m_pContext->DrawIndexedInstanced(m_iNumIndexPerInstance, m_iNumInstance, 0, 0, 0);
    return S_OK;
}

void CVIBuffer_Particle_Instancing::Drop(_float _fTimeDelta)
{
    D3D11_MAPPED_SUBRESOURCE SubResource{};

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    VTX_PARTICLE_INSTANCE* pVertices = static_cast<VTX_PARTICLE_INSTANCE*>(SubResource.pData);

    for (size_t i = 0; i < m_iNumInstance; i++)
    {
        pVertices[i].vTranslation.y -= m_pSpeeds[i] * _fTimeDelta;
        pVertices[i].vLifeTime.y += _fTimeDelta;
        if (pVertices[i].vLifeTime.x <= pVertices[i].vLifeTime.y)
        {
            if (true == m_bLoop)
            {
                pVertices[i].vTranslation.y = m_pInstanceVertices[i].vTranslation.y;
                pVertices[i].vLifeTime.y = 0.f;
            }
        }
    }

    m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Particle_Instancing::Rise(_float _fTimeDelta)
{
    D3D11_MAPPED_SUBRESOURCE SubResource{};

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    VTX_PARTICLE_INSTANCE* pVertices = static_cast<VTX_PARTICLE_INSTANCE*>(SubResource.pData);

    for (size_t i = 0; i < m_iNumInstance; i++)
    {
        pVertices[i].vTranslation.y += m_pSpeeds[i] * _fTimeDelta;
        pVertices[i].vLifeTime.y += _fTimeDelta;
        if (pVertices[i].vLifeTime.x <= pVertices[i].vLifeTime.y)
        {
            if (true == m_bLoop)
            {
                pVertices[i].vTranslation.y = m_pInstanceVertices[i].vTranslation.y;
                pVertices[i].vLifeTime.y = 0.f;
            }
        }
    }

    m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Particle_Instancing::Following_Smoke(_float _fTimeDelta, _vector _vDirection)
{
    D3D11_MAPPED_SUBRESOURCE SubResource{};

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    VTX_PARTICLE_INSTANCE* pVertices = static_cast<VTX_PARTICLE_INSTANCE*>(SubResource.pData);

    // ���� ���� ����ȭ (���� 1�� ���� �ӵ� ���� �� ���������� �̵�)
    _vDirection = XMVector3Normalize(_vDirection);

    for (size_t i = 0; i < m_iNumInstance; i++)
    {
        // ���⸦ �̻��� ���� �ݴ� �������� �̵���Ű��
        _vector vMoveDir = _vDirection * m_pSpeeds[i] * _fTimeDelta;

        // ��ƼŬ ��ġ ������Ʈ
        XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vMoveDir);

        pVertices[i].vLifeTime.y += _fTimeDelta;
        if (pVertices[i].vLifeTime.x <= pVertices[i].vLifeTime.y)
        {
            if (m_bLoop)
            {
                pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
                pVertices[i].vLifeTime.y = 0.f;
            }
        }
    }

    m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Particle_Instancing::Explosion(_float _fTimeDetla)
{
    D3D11_MAPPED_SUBRESOURCE SubResource{};

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    VTX_PARTICLE_INSTANCE* pVertices = static_cast<VTX_PARTICLE_INSTANCE*>(SubResource.pData);

    for (size_t i = 0; i < m_iNumInstance; i++)
    {
        _vector		vMoveDir = XMVectorSetW(XMVector3Normalize(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivot)), 0.f);

        XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vMoveDir * m_pSpeeds[i] * _fTimeDetla);

        pVertices[i].vLifeTime.y += _fTimeDetla;
        if (pVertices[i].vLifeTime.x <= pVertices[i].vLifeTime.y)
        {
            if (true == m_bLoop)
            {
                pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
                pVertices[i].vLifeTime.y = 0.f;
            }
        }
    }

    m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Particle_Instancing::MissileSmoke(_float _fTimeDelta)
{
    D3D11_MAPPED_SUBRESOURCE SubResource{};

    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
    VTX_PARTICLE_INSTANCE* pVertices = static_cast<VTX_PARTICLE_INSTANCE*>(SubResource.pData);

    for (size_t i = 0; i < m_iNumInstance; i++)
    {
        _vector vMoveDir = XMLoadFloat3(&m_vPivot) - XMLoadFloat4(&pVertices[i].vTranslation);
        vMoveDir = XMVector3Normalize(vMoveDir);
        vMoveDir *= -1.0f;  // �ݴ� �������� �̵� (�̻��� ��������)

        XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vMoveDir * m_pSpeeds[i] * _fTimeDelta);

        pVertices[i].vLifeTime.y += _fTimeDelta;
        if (pVertices[i].vLifeTime.x <= pVertices[i].vLifeTime.y)
        {
            if (true == m_bLoop)
            {
                pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
                pVertices[i].vLifeTime.y = 0.f;
            }
        }
    }

    m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Particle_Instancing::FireThrower(_float _fTimeDelta, _vector _vFireDirection)
{
    D3D11_MAPPED_SUBRESOURCE SubResource{};
    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    VTX_PARTICLE_INSTANCE* pVertices = static_cast<VTX_PARTICLE_INSTANCE*>(SubResource.pData);

    for (size_t i = 0; i < m_iNumInstance; i++)
    {
        _vector vMoveDir = _vFireDirection;

        XMStoreFloat4(&pVertices[i].vTranslation,
            XMLoadFloat4(&pVertices[i].vTranslation) + vMoveDir * m_pSpeeds[i] * _fTimeDelta);

        // �Ҳ��� ���� �ֱ� ������Ʈ
        pVertices[i].vLifeTime.y += _fTimeDelta;

        // �Ҳ��� ���� �ð� ���ӵǸ� �ٽ� �����Ͽ� �������� ȿ�� ����
        if (pVertices[i].vLifeTime.x <= pVertices[i].vLifeTime.y)
        {
            if (true == m_bLoop)
            {
                pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
                pVertices[i].vLifeTime.y = 0.f;
            }
        }
    }

    m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Particle_Instancing::FireThorwer_Reset()
{
    D3D11_MAPPED_SUBRESOURCE SubResource{};
    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    VTX_PARTICLE_INSTANCE* pVertices = static_cast<VTX_PARTICLE_INSTANCE*>(SubResource.pData);

    for (size_t i = 0; i < m_iNumInstance; i++)
    {
        // �Ҳ��� ���� �ð� ���ӵǸ� �ٽ� �����Ͽ� �������� ȿ�� ����
        pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
        pVertices[i].vLifeTime.y = 0.f;
    }
    m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Particle_Instancing::Fire_FrameUpdate(_float _fTimeDelta)
{
    D3D11_MAPPED_SUBRESOURCE SubResource{};
    m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

    VTX_PARTICLE_INSTANCE* pVertices = static_cast<VTX_PARTICLE_INSTANCE*>(SubResource.pData);

    for (size_t i = 0; i < m_iNumInstance; i++)
    {
        // �Ҳ��� ���� �ֱ� ������Ʈ
        pVertices[i].vLifeTime.y += _fTimeDelta;

        // �Ҳ��� ���� �ð� ���ӵǸ� �ٽ� �����Ͽ� �������� ȿ�� ����
        if (pVertices[i].vLifeTime.x <= pVertices[i].vLifeTime.y)
        {
            if (true == m_bLoop)
            {
                pVertices[i].vTranslation = m_pInstanceVertices[i].vTranslation;
                pVertices[i].vLifeTime.y = 0.f;
            }
        }
    }

    m_pContext->Unmap(m_pVBInstance, 0);
}

// ���� �� Particle Instance ���۴�, ���� �ν��Ͻ��� ������ �ϴ� ���۸� ������ ���� ��! �Ҵ��� ���̴�. 
// �������� ���� ������ m_pVB, �纻���� ���� ������ m_pVBInstance ,
// ���ʿ� ���� �Ҵ��� �� ����, �ʱ� ���¸� ���� ���� �� �����ΰ�, ������ ���󰡸鼭 �����̴� �������� �����ʹ�, 
// m_VBInstance �� ������ �� �����Ѵ�. 


void CVIBuffer_Particle_Instancing::Free()
{
    __super::Free();

    if (false == m_bClone)
    {
        Safe_Delete_Array(m_pSpeeds);
        Safe_Delete_Array(m_pInstanceVertices);
    }

    Safe_Release(m_pVBInstance);
}
