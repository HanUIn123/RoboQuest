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
    // Stride 가 2개가 되고, 현재 Particle Instancing 클래스를 상속받는 자식들은,
    // 현재 클래스의, Bind_InputAssembler 함수를 호출 하게 됨.
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

    // IASetVertexBuffers 를 호출할 때, 이 파티클 인스턴스 버퍼는, 기존의 버퍼 때와 달리, 인스턴스 전용 정보가 하나씩 더 들어나서 들어가게됨.
    // ( 원래 그리기 용도 ) + ( 인스턴싱 용도 ) 
    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffer, iVertexStrides, iOffsets);
    m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
    m_pContext->IASetPrimitiveTopology(m_ePrimitiveTopology);

    return S_OK;
}

HRESULT CVIBuffer_Particle_Instancing::Render()
{
    if (nullptr == m_pContext)
        return E_FAIL;

    // 인스턴스 하나 그릴 때 몇개가 필요한지 / 인스턴스 갯수 / 0 / 0 
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

    // 방향 벡터 정규화 (길이 1로 만들어서 속도 곱할 때 안정적으로 이동)
    _vDirection = XMVector3Normalize(_vDirection);

    for (size_t i = 0; i < m_iNumInstance; i++)
    {
        // 연기를 미사일 진행 반대 방향으로 이동시키기
        _vector vMoveDir = _vDirection * m_pSpeeds[i] * _fTimeDelta;

        // 파티클 위치 업데이트
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
        vMoveDir *= -1.0f;  // 반대 방향으로 이동 (미사일 방향으로)

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

        // 불꽃의 생명 주기 업데이트
        pVertices[i].vLifeTime.y += _fTimeDelta;

        // 불꽃이 일정 시간 지속되면 다시 생성하여 연속적인 효과 유지
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
        // 불꽃이 일정 시간 지속되면 다시 생성하여 연속적인 효과 유지
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
        // 불꽃의 생명 주기 업데이트
        pVertices[i].vLifeTime.y += _fTimeDelta;

        // 불꽃이 일정 시간 지속되면 다시 생성하여 연속적인 효과 유지
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

// 현재 이 Particle Instance 버퍼는, 실제 인스턴스로 띄우고자 하는 버퍼를 복제로 만들 때! 할당할 것이다. 
// 원형으로 만들 때에는 m_pVB, 사본으로 만들 때에는 m_pVBInstance ,
// 애초에 버퍼 할당을 할 때에, 초기 상태를 원형 만들 때 만들어두고, 실제로 따라가면서 움직이는 실질적인 데이터는, 
// m_VBInstance 를 복제할 때 생성한다. 


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
