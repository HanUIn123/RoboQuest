#pragma once
#include "VIBuffer.h"
BEGIN(Engine)
class ENGINE_DLL CVIBuffer_MapToolTerrainSec final : public CVIBuffer
{
private:
    CVIBuffer_MapToolTerrainSec(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CVIBuffer_MapToolTerrainSec(const CVIBuffer_MapToolTerrainSec& _Prototype);
    virtual ~CVIBuffer_MapToolTerrainSec() = default;

public:
    XMVECTOR* Get_VertexPos() const
    {
        return m_VertexPos;
    }
public:
    virtual HRESULT             Initialize_Prototype(const _tchar* _HeightMapTag);
    //virtual HRESULT             Initialize_Prototype(const _tchar* _)
    virtual HRESULT             Initialize(void* _pArg) override;

public:
    ID3D11Buffer* Get_VertexBuffer() { return m_pVB; }
    ID3D11Buffer* Get_IndexBuffer() { return m_pIB; }

private:
    _uint                       m_iNumverticesX, m_iNumverticesZ;
private:
    //XMFLOAT3*                   m_VertexPos = { nullptr };
    _uint* m_pPixel = { nullptr };
public:
    _uint* Get_Pixel() { return m_pPixel; }
    void Mapping_Buffer(XMVECTOR* _pVertices);

public:
    _uint               Get_NumVerticesX() const {
        return m_iNumverticesX;
    }

    _uint               Get_NumVerticesZ() const {
        return m_iNumverticesZ;
    }

    void                   Set_MapToolTerrainShape();

    XMVECTOR* m_VertexPos = { nullptr };


public:
    void Set_Height(_float _y) { m_y = _y; };
private:
    _float m_y;

private:
    _uint				m_iNumVerticesX = 0, m_iNumVerticesZ = 0;

public:
    static CVIBuffer_MapToolTerrainSec* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _HeightMapTag);
    virtual CComponent* Clone(void* _pArg) override;
    virtual void Free() override;
};

END