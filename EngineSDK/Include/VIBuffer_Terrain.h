#pragma once
#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
private:
    CVIBuffer_Terrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CVIBuffer_Terrain(const CVIBuffer_Terrain& _Prototype);
    virtual ~CVIBuffer_Terrain() = default;

public:
    virtual HRESULT             Initialize_Prototype(const _tchar* _HeightMapTag, VTXNORTEX* _pLoadData = nullptr);
    virtual HRESULT             Initialize(void* _pArg) override;

//public:
//    _uint                       Get_NumverticesX() { return m_iNumverticesX;  }
//    _uint                       Get_NumverticesZ() { return m_iNumverticesZ;  }

public:
    XMVECTOR*                   Get_VertexPos() const{return m_VertexPos;}

public:
    _float                      Get_Height(const XMFLOAT3& _vPos);

private:
    _uint                       m_iNumverticesX, m_iNumverticesZ;
    XMVECTOR*                   m_VertexPos = { nullptr };
    _float4                     m_vPlane = {};



public:
    static CVIBuffer_Terrain* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _HeightMapTag, VTXNORTEX* _pLoadData = nullptr);
    virtual CComponent* Clone(void* _pArg) override;
    virtual void Free() override;
};

END