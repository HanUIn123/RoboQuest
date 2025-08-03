#pragma once
#include "VIBuffer.h"
BEGIN(Engine)
class ENGINE_DLL CVIBuffer_MapToolTerrain final : public CVIBuffer
{
private:
    CVIBuffer_MapToolTerrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CVIBuffer_MapToolTerrain(const CVIBuffer_MapToolTerrain& _Prototype);
    virtual ~CVIBuffer_MapToolTerrain() = default;

public:
    XMVECTOR* Get_VertexPos() const 
    { 
        return m_VertexPos; 
    }
public:
    virtual HRESULT             Initialize_Prototype(const _tchar* _HeightMapTag, VTXNORTEX* _pLoadHeight = nullptr);
    virtual HRESULT             Initialize(void* _pArg) override;

public:
    ID3D11Buffer* Get_VertexBuffer() { return m_pVB; }
    ID3D11Buffer* Get_IndexBuffer() { return m_pIB; }

private:
    _uint                       m_iNumverticesX, m_iNumverticesZ;
private:
    //XMFLOAT3*                   m_VertexPos = { nullptr };
    _uint*                      m_pPixel = { nullptr };
public:
    _uint* Get_Pixel() { return m_pPixel; }
    void Mapping_Buffer(XMVECTOR* _pVertices);

public:
    _uint               Get_NumVerticesX() const    {
        return m_iNumverticesX;
    }

    _uint               Get_NumVerticesZ() const {
        return m_iNumverticesZ;
    }

    void                   Set_MapToolTerrainShape();

    XMVECTOR*               m_VertexPos = { nullptr };

private:
    _uint				m_iNumVerticesX = 0, m_iNumVerticesZ = 0;

public:
    void        SetTextureIndex(_uint _iIndex, _uint _iTextureIndex);

public:
    static CVIBuffer_MapToolTerrain* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _HeightMapTag, VTXNORTEX* _pLoadHeight = nullptr);
    virtual CComponent* Clone(void* _pArg) override;
    virtual void Free() override;
};

END