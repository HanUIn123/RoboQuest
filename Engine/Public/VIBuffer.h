#pragma once
#include "Component.h"

// 기타 구체적인 정점 인덱스 버퍼(CVIBuffer_Rect 등)의 부모가 되는 클래스 

BEGIN(Engine)

class ENGINE_DLL CVIBuffer abstract : public CComponent
{
protected:
    CVIBuffer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CVIBuffer(const CVIBuffer& _Prototype);
    virtual ~CVIBuffer() = default;



public:
    virtual HRESULT         Initialize_Prototype() override;
    virtual HRESULT         Initialize(void* _pArg) override;
    virtual HRESULT         Bind_InputAssembler();
    virtual HRESULT         Render();


protected:
    ID3D11Buffer*               m_pVB = { nullptr };
    ID3D11Buffer*               m_pIB = { nullptr };

    D3D11_BUFFER_DESC           m_tagBufferDesc = {};
    D3D11_SUBRESOURCE_DATA      m_tagInitialData = {};

protected:
    DXGI_FORMAT					m_eIndexFormat = {};
    _uint                       m_iNumVertexBuffers = {};
    _uint                       m_iNumVertices = {};
    _uint                       m_iVertexStride = {};
    _uint                       m_iNumIndices = {};
    _uint                       m_iIndexStride = {};
    D3D11_PRIMITIVE_TOPOLOGY    m_ePrimitiveTopology = {};

protected:
    HRESULT                     Create_Buffer(ID3D11Buffer** _ppOut);


public:
    virtual CComponent*         Clone(void* _pArg) = 0;
    virtual void                Free();

};

END