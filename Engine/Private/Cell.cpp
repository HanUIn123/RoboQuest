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

    // XMPlaneFromPoints �Լ��� ���ؼ� �̷��� �� 3���� �־��ָ�, ����� ���ϰ� �Ȱ���. 
    // �� �Լ��� ��¥ �ð���� �������� ���� ������ �޼� ��Ģ�� ���ؼ�, ���� ���� ���ʹ� �������� ���ϰ� �� .
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
        // ���� ���� ���ϱ� .
        _vector vDir = XMVector3Normalize(_vPosition - XMLoadFloat3(&m_vPoints[i]));

        // ���� ���� ���ϱ� ���ؼ�, �ش� ����.  �ﰢ���� 01 ��������, 12 ��������, 20 ��������.
        _vector vLine = XMLoadFloat3(&m_vPoints[(i + 1) % 3]) - XMLoadFloat3(&m_vPoints[i]);

        // ���� ���� ���ϱ� 
        _vector vNormal = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vLine) * -1.0f, 0.0f, XMVectorGetX(vLine), 0.0f));

        // ���� ���⺤�Ϳ� ���� ���� ������ �ذ�����, ����, �����̸� �ȿ��ִ�.  ����̸� �ٱ��� �ִ�.
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
    // ó���� A���� A�� �˻� 
  // XMVectorEqual : �̶�� �Լ��� ����. ->�� ������ ������ ���ϰ�, �� ������ ���� �� x ���� ���� ���ؼ� �� �� true �� �ɷ� ������ x y z �� ä���شٴ� �Լ��� (�� �ٸ�)
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
