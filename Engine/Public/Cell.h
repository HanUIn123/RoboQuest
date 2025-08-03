#pragma once
#include "Base.h"

BEGIN(Engine)

class CCell final : public CBase
{
public:
    enum        POINT           { POINT_A, POINT_B, POINT_C, POINT_END };
    enum        LINE            { LINE_AB, LINE_BC, LINE_CA, LINE_END };

private:
    CCell(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CCell() = default;

public:
    void Set_Neighbor(LINE _eLine, CCell* _pNeighbor) {
        m_iNeighborIndices[_eLine] = _pNeighbor->m_iIndex;
    }

public:
    _vector Get_Point(POINT _ePoint)
    {
        return XMLoadFloat3(&m_vPoints[_ePoint]);
    }

public:
    HRESULT         Initialize(const _float3* _pPoints, _int _iIndex);
    _bool           bIsIn(_fvector _vPosition, _int* _pNeighborIndex);
    _bool           Compare_Points(_fvector _vNeighborsSourPoint, _fvector _vNeighborsDestPoint);
    _float          Compute_Height(_fvector _vPosition);

public:
    void            Setting_ShrinkPoints(_float _vShrinkRatio);
    _bool           bIsIn_ShrinkedCell(_fvector _vPosition);

#ifdef _DEBUG
public:
    HRESULT         Render();
#endif

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

    _float3         m_vPoints[POINT_END] = {};
    _float3         m_vShrinkedPoints[POINT_END] = {};

    _int            m_iNeighborIndices[LINE_END] = { -1, -1, -1 };
    _int            m_iIndex = {};
    _float4         m_vPlane = {};

#ifdef _DEBUG
private:
    class CVIBuffer_Cell* m_pVIBuffer = { nullptr };
#endif

public:
    static CCell* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _float3* _pPoints, _int _iIndex);
    virtual void Free() override;
};

END