#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CVIBuffer_Particle_Instancing abstract : public CVIBuffer
{
public:
    struct PARTICLE_INSTANCING_DESC
    {
        // 인스턴스의 갯수를 부모 구조체 PARTICLE_INSTANCING_DESC 에서 제공.
        _uint iNumInstance = {};
        _float3     vRange = {};
        _float3     vCenter = {};
        _float2     vSize = {};
        _float2     vSpeed = {};
        _float3     vPivot = {};
        _float2     vLifeTime = {};
        _bool       isLoop = {};
    };

protected:
    CVIBuffer_Particle_Instancing(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CVIBuffer_Particle_Instancing(const CVIBuffer_Particle_Instancing& _Prototype);
    virtual ~CVIBuffer_Particle_Instancing() = default;

public:
    virtual HRESULT                 Initialize_Prototype(const PARTICLE_INSTANCING_DESC* _pDesc);
    virtual HRESULT                 Initialize(void* _pArg) override;
    virtual HRESULT                 Bind_InputAssembler() override;
    virtual HRESULT                 Render() override;

public:
    void Drop(_float _fTimeDelta);
    void Rise(_float fTimeDelta);
    void Following_Smoke(_float _fTimeDelta, _vector vDirection);

    void Explosion(_float _fTimeDetla);
    void    MissileSmoke(_float _fTimeDelta);
    void FireThrower(_float _fTimeDelta, _vector _vFireDirection);
    void FireThorwer_Reset();

    void Fire_FrameUpdate(_float _fTimeDelta);


    /* VI_Buffer 부문 최상위 클래스에서 제공해주는 변수들.
    protected:
    DXGI_FORMAT					m_eIndexFormat = {};
    _uint                       m_iNumVertexBuffers = {};
    _uint                       m_iNumVertices = {};
    _uint                       m_iVertexStride = {};
    _uint                       m_iNumIndices = {};
    _uint                       m_iIndexStride = {};
    D3D11_PRIMITIVE_TOPOLOGY    m_ePrimitiveTopology = {};
    */


protected:
    // 인스턴스 버퍼 용 구조체 하나 변수로 추가.
    D3D11_BUFFER_DESC               m_tagInstanceBufferDesc = {};
    D3D11_SUBRESOURCE_DATA          m_InstanceInitialData = {};
    _uint                           m_iInstanceVertexStride = {};
    _uint                           m_iNumIndexPerInstance = {};
    _uint                           m_iNumInstance = {};

    VTX_PARTICLE_INSTANCE* m_pInstanceVertices = { nullptr };
    ID3D11Buffer* m_pVBInstance = { nullptr };

    _float* m_pSpeeds = { nullptr };
    //_float* m_pRandomSpeeds = { nullptr };
    _bool                           m_bLoop = { false };
    _float3							m_vPivot = {};

public:
    virtual CComponent* Clone(void* _pArg) = 0;
    virtual void Free();

};

END