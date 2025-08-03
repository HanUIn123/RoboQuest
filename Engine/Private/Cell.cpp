#include "../Public/Cell.h"
#include "VIBuffer_Cell.h"


CCell::CCell(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice{_pDevice}
    , m_pContext{_pContext}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CCell::Initialize(const _float3* _pPoints, _int _iIndex)
{
    memcpy(m_vPoints, _pPoints, sizeof(_float3) * POINT_END);

    m_iIndex = _iIndex;

    // XMPlaneFromPoints 함수에 의해서 이렇게 점 3개를 넣어주면, 평면을 구하게 된거임. 
    // 이 함수는 진짜 시계방향 기준으로 점을 찍어놔야 왼손 법칙에 의해서, 이의 수직 벡터는 위쪽으로 향하게 됨 .
    XMStoreFloat4(&m_vPlane, XMPlaneFromPoints(XMLoadFloat3(&_pPoints[POINT_A]), XMLoadFloat3(&_pPoints[POINT_B]), XMLoadFloat3(&_pPoints[POINT_C])));



#ifdef _DEBUG
    m_pVIBuffer = CVIBuffer_Cell::Create(m_pDevice, m_pContext, m_vPoints);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;
#endif

    return S_OK;
}

_bool CCell::bIsIn(_fvector _vPosition, _int* _pNeighborIndex)
{
    for (_uint i = 0; i < LINE_END; i++)
    {
        // 방향 벡터 구하기 .
        _vector vDir = XMVector3Normalize(_vPosition - XMLoadFloat3(&m_vPoints[i]));

        // 법선 벡터 구하기 위해서, 해당 라인.  삼각형의 01 라인인지, 12 라인인지, 20 라인인지.
        _vector vLine = XMLoadFloat3(&m_vPoints[(i + 1) % 3]) - XMLoadFloat3(&m_vPoints[i]);

        // 법선 벡터 구하기 
        _vector vNormal = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vLine) * -1.0f, 0.0f, XMVectorGetX(vLine), 0.0f));

        // 이제 방향벡터와 법선 벡터 내적을 해가지구, 깂이, 음수이면 안에있다.  양수이면 바깥에 있다.
        if (0 < XMVectorGetX(XMVector3Dot(vDir, vNormal)))
        {
            *_pNeighborIndex = m_iNeighborIndices[i];
            return false;
        }
    }

    return true;
}

_bool CCell::Compare_Points(_fvector _vNeighborsSourPoint, _fvector _vNeighborsDestPoint)
{
    // 처음에 A점과 A점 검사 
  // XMVectorEqual : 이라는 함수도 있음. ->각 벡터의 성분을 비교하고, 즉 각각의 벡터 의 x 끼리 서로 비교해서 둘 중 true 인 걸로 벡터의 x y z 를 채워준다는 함수임 (좀 다름)
    if (true == XMVector3Equal(_vNeighborsSourPoint, XMLoadFloat3(&m_vPoints[POINT_A])))
    {
        if (true == XMVector3Equal(_vNeighborsDestPoint, XMLoadFloat3(&m_vPoints[POINT_B])))
            return true;
        if (true == XMVector3Equal(_vNeighborsDestPoint, XMLoadFloat3(&m_vPoints[POINT_C])))
            return true;
    }

    if (true == XMVector3Equal(_vNeighborsSourPoint, XMLoadFloat3(&m_vPoints[POINT_B])))
    {
        if (true == XMVector3Equal(_vNeighborsDestPoint, XMLoadFloat3(&m_vPoints[POINT_C])))
            return true;
        if (true == XMVector3Equal(_vNeighborsDestPoint, XMLoadFloat3(&m_vPoints[POINT_A])))
            return true;
    }

    if (true == XMVector3Equal(_vNeighborsSourPoint, XMLoadFloat3(&m_vPoints[POINT_C])))
    {
        if (true == XMVector3Equal(_vNeighborsDestPoint, XMLoadFloat3(&m_vPoints[POINT_A])))
            return true;
        if (true == XMVector3Equal(_vNeighborsDestPoint, XMLoadFloat3(&m_vPoints[POINT_B])))
            return true;
    }

    return false;
}

_float CCell::Compute_Height(_fvector _vPosition)
{
    return (-m_vPlane.x * XMVectorGetX(_vPosition) - (m_vPlane.z * XMVectorGetZ(_vPosition)) - m_vPlane.w) / m_vPlane.y;
}

void CCell::Setting_ShrinkPoints(_float _vShrinkRatio)
{
    _float3 vCellCenterPoint =
    {
        (m_vPoints[POINT_A].x + m_vPoints[POINT_B].x + m_vPoints[POINT_C].x) / 3.0f,
        (m_vPoints[POINT_A].y + m_vPoints[POINT_B].y + m_vPoints[POINT_C].y) / 3.0f,
        (m_vPoints[POINT_A].z + m_vPoints[POINT_B].z + m_vPoints[POINT_C].z) / 3.0f
    };

    for (int i = 0; i < POINT_END; ++i)
    {
        m_vShrinkedPoints[i].x = vCellCenterPoint.x + (m_vPoints[i].x - vCellCenterPoint.x) * _vShrinkRatio;
        m_vShrinkedPoints[i].y = vCellCenterPoint.y + (m_vPoints[i].y - vCellCenterPoint.y) * _vShrinkRatio;
        m_vShrinkedPoints[i].z = vCellCenterPoint.z + (m_vPoints[i].z - vCellCenterPoint.z) * _vShrinkRatio;
    }
}

_bool CCell::bIsIn_ShrinkedCell(_fvector _vPosition)
{
    for (_uint i = 0; i < LINE_END; ++i)
    {
        _vector vDir = XMVector3Normalize(_vPosition - XMLoadFloat3(&m_vShrinkedPoints[i]));
        _vector vLine = XMLoadFloat3(&m_vShrinkedPoints[(i + 1) % 3]) - XMLoadFloat3(&m_vShrinkedPoints[i]);
        _vector vNormal = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vLine) * -1.0f, 0.0f, XMVectorGetX(vLine), 0.0f));

        if (0 < XMVectorGetX(XMVector3Dot(vDir, vNormal)))
            return false;
    }
    return true;
}

#ifdef _DEBUG
HRESULT CCell::Render()
{
    m_pVIBuffer->Bind_InputAssembler();

    m_pVIBuffer->Render();

    return S_OK;
}
#endif // _DEBUG

CCell* CCell::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _float3* _pPoints, _int _iIndex)
{
    CCell* pInstance = new CCell(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_pPoints, _iIndex)))
    {
        MSG_BOX("Failed To Created : CCell");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCell::Free()
{
    __super::Free();
#ifdef _DEBUG
    Safe_Release(m_pVIBuffer);
#endif
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
