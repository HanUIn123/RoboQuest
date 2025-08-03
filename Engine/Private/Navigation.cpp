#include "../Public/Navigation.h"
#include "Cell.h"

#ifdef _DEBUG
#include "Shader.h"
#include "GameInstance.h"
#endif


_float4x4           CNavigation::m_WorldMatrix = {};
_float4x4           CNavigation::m_WorldMatrixInverse = {};


CNavigation::CNavigation(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CComponent{ _pDevice, _pContext }
{
}

CNavigation::CNavigation(const CNavigation& _Prototype)
    :CComponent(_Prototype)
    , m_vecCells{ _Prototype.m_vecCells }
#ifdef _DEBUG
    , m_pShader{ _Prototype.m_pShader }
#endif
{
#ifdef _DEBUG
    Safe_AddRef(m_pShader);
#endif
    //for (auto& pCell : m_vecCells)
    //{
    //    Safe_AddRef(pCell);
    //}

    for (auto& pCell : m_vecCells)
        Safe_AddRef(pCell);


}

HRESULT CNavigation::Initialize_Prototype(const _tchar* _pNavigationDataFile)
{
    _ulong      dwByte = {};
    HANDLE hFile = CreateFile(_pNavigationDataFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    _uint iCellCountNum = {};
    ReadFile(hFile, &iCellCountNum, sizeof(_uint), &dwByte, nullptr);

    //while (true)
    for (_uint i = 0; i < iCellCountNum; ++i)
    {
        _float3         vPoints[CCell::POINT_END] = {};

        ReadFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

        // ���� Ż�⹮ ����
        if (0 == dwByte)
            break;

        // size �� �༭ ó���� �ϳ� ���� �뷮 1��, �ϳ� �� ���� 2��.~~ 
        CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_uint)(m_vecCells.size()));
        //CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, iCellCountNum);
        if (nullptr == pCell)
            return E_FAIL;

        m_vecCells.push_back(pCell);
    }
    CloseHandle(hFile);

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_WorldMatrixInverse, XMMatrixIdentity());

#ifdef _DEBUG
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../EngineSDK/Hlsl/Shader_Cell.hlsl"), VTXPOS::tagElements, VTXPOS::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;
#endif

    // ���� �̰� ȣ������� ���������� ���� �� 4�� �� �ٳ�� �ְ� �Ǿ���. 
    if (FAILED(SetUp_NeighborCells()))
        return E_FAIL;


    return S_OK;















//    _ulong dwByte = 0;
//    HANDLE hFile = CreateFile(_pNavigationDataFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
//
//    // ������ ������ �⺻ �ʱ�ȭ�� ����
//    if (hFile == INVALID_HANDLE_VALUE)
//    {
//        XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
//        XMStoreFloat4x4(&m_WorldMatrixInverse, XMMatrixIdentity());
//
//#ifdef _DEBUG
//        m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../EngineSDK/Hlsl/Shader_Cell.hlsl"), VTXPOS::tagElements, VTXPOS::iNumElements);
//        if (nullptr == m_pShader)
//            return E_FAIL;
//#endif
//
//        if (FAILED(SetUp_NeighborCells()))
//            return E_FAIL;
//
//        return S_OK;
//    }
//
//    // ���� ������ �ʱ�ȭ
//    for (auto& pCell : m_vecCells)
//    {
//        Safe_Release(pCell);
//    }
//    m_vecCells.clear();
//
//    // Cell ���� �б�
//    _uint iCellCountNum = {};
//    if (!ReadFile(hFile, &iCellCountNum, sizeof(_uint), &dwByte, nullptr) || dwByte == 0)
//    {
//        MSG_BOX("Failed to read cell count from navigation file!");
//        CloseHandle(hFile);
//        return E_FAIL;
//    }
//
//    // Cell ������ �б� �� �߰�
//    for (_uint i = 0; i < iCellCountNum; ++i)
//    {
//        _float3 vPoints[CCell::POINT_END] = {};
//        if (!ReadFile(hFile, vPoints, sizeof(_float3) * CCell::POINT_END, &dwByte, nullptr) || dwByte == 0)
//        {
//            MSG_BOX("Failed to read cell data from navigation file!");
//            CloseHandle(hFile);
//            return E_FAIL;
//        }
//
//        CCell* pCell = CCell::Create(m_pDevice, m_pContext, vPoints, (_uint)m_vecCells.size());
//        if (!pCell)
//        {
//            MSG_BOX("Failed to create cell!");
//            CloseHandle(hFile);
//            return E_FAIL;
//        }
//
//        m_vecCells.push_back(pCell);
//    }
//
//    // m_bFirstPick ���� �б�
//    if (!ReadFile(hFile, &m_bFirstPick, sizeof(bool), &dwByte, nullptr) || dwByte == 0)
//    {
//        MSG_BOX("Failed to read m_bFirstPick from navigation file!");
//        CloseHandle(hFile);
//        return E_FAIL;
//    }
//
//    CloseHandle(hFile);
//
//    // ���� ��� �ʱ�ȭ
//    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
//    XMStoreFloat4x4(&m_WorldMatrixInverse, XMMatrixIdentity());
//
//#ifdef _DEBUG
//    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../EngineSDK/Hlsl/Shader_Cell.hlsl"), VTXPOS::tagElements, VTXPOS::iNumElements);
//    if (!m_pShader)
//    {
//        MSG_BOX("Failed to create debug shader!");
//        return E_FAIL;
//    }
//#endif
//
//    if (FAILED(SetUp_NeighborCells()))
//    {
//        MSG_BOX("Failed to set up neighbor cells!");
//        return E_FAIL;
//    }
//
//    return S_OK;
}

HRESULT CNavigation::Initialize_Prototype_CreateOnly()
{
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_WorldMatrixInverse, XMMatrixIdentity());

#ifdef _DEBUG
    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../EngineSDK/Hlsl/Shader_Cell.hlsl"), VTXPOS::tagElements, VTXPOS::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;
#endif

    // ���� �̰� ȣ������� ���������� ���� �� 4�� �� �ٳ�� �ְ� �Ǿ���. 
    if (FAILED(SetUp_NeighborCells()))
        return E_FAIL;

    return S_OK;
}


HRESULT CNavigation::Initialize(void* _pArg)
{
    if (nullptr != _pArg)
    {
        NAVIGATION_DESC* pDesc = static_cast<NAVIGATION_DESC*>(_pArg);

        m_iCurrentCellIndex = pDesc->iCurrentCellIndex;
    }

    return S_OK;
}

void CNavigation::Set_CurrentNaviIndex(_vector _vWorldPos)
{
    _int                iNeighborIndex = { -1 };

    for (_uint i = 0; i < m_vecCells.size(); ++i)
    {
        if (true == m_vecCells[i]->bIsIn(_vWorldPos, &iNeighborIndex))
        {
            m_iCurrentCellIndex = i;
            return;
        }
    }
}

_bool CNavigation::bIsMove(_fvector _vWorldPos)
{
    _vector             vPosition = XMVector3TransformCoord(_vWorldPos, XMLoadFloat4x4(&m_WorldMatrixInverse));

    _int                iNeighborIndex = { -1 };
    m_iNeighborIndex = iNeighborIndex;

    if (false == m_vecCells[m_iCurrentCellIndex]->bIsIn(vPosition, &iNeighborIndex))
    {
        if (-1 != iNeighborIndex)
        {
            while (true)
            {
                // ���ʷ� ���� ��쿡 �ε����� �� ������ �з��� �� ��������� ��찡 �־ ���� ó����. 
                if (-1 == iNeighborIndex)
                    return false;

                if (true == m_vecCells[iNeighborIndex]->bIsIn(vPosition, &iNeighborIndex))
                {
                    //m_iCurrentCellIndex = iNeighborIndex;
                    //return true;
                    break;
                }
            }

            // �̿��ﰢ������ �� ��쿡 ���� �ε����� ���� �̿��ﰢ���� �ε����� �ٲ���. 
            m_iCurrentCellIndex = iNeighborIndex;
            m_iNeighborIndex = iNeighborIndex;
            return true;
        }
        else
        {
            // ������ ���� ������. 
            m_iNeighborIndex = -1;
            return false;
        }
    }
    else
    {
        // �� �ȿ� �׳� �ִ�. 
        m_iNeighborIndex = iNeighborIndex;
        return true;
    }
}


_float CNavigation::Compute_Height(_fvector _vWorldPos)
{
    // �׳� �����̶��, �� ���� �� �ε�����, 
    if (-1 == m_iCurrentCellIndex)
    {
        return 0.0f;
    }

    // �� �Լ��� ����� ��ü�� _vWorldPos �� ���� ����� �������� ������, 
    // �׷��� ����� �������� ���� ���� ���� �� ���ϴ� ��ǥ�� ���� �����̱� ������, 
    // ���⼭ �� �� ���� ����� ������� ���� ���� vPosition �� �����ְ�, 

    _vector     vPosition = XMVector3TransformCoord(_vWorldPos, XMLoadFloat4x4(&m_WorldMatrixInverse));

    // �� ������ Cell Ŭ������ Comput_Hegiht�� ���ڷ� �Ѱ��־, �޾ƿ� ���� ��ǥ�� �ٽ� vPosition �� �־��ش�. 
    vPosition = XMVectorSetY(vPosition, m_vecCells[m_iCurrentCellIndex]->Compute_Height(vPosition));

    // �׷� �� ���� vPosition �� �ٽ� �� �� ����� �ٲ��־,
    vPosition = XMVector3TransformCoord(vPosition, XMLoadFloat4x4(&m_WorldMatrix));

    // �� vPosition �� ���� ���� Y ��ǥ�� �̾Ƴ��� .
    return XMVectorGetY(vPosition);
}

_vector CNavigation::Setting_SlidingMove(_fvector _vWorldPos)
{
    if (m_iNeighborIndex == -1)
    {
        _vector vPosition = XMVector3TransformCoord(_vWorldPos, XMLoadFloat4x4(&m_WorldMatrixInverse));

        for (_uint i = 0; i < CCell::LINE_END; ++i)
        {
            // ���� ���� ���ϱ� .
            _vector vDir = (vPosition - m_vecCells[m_iCurrentCellIndex]->Get_Point((CCell::POINT)i));

            // ���� ���� ���ϱ� ���ؼ�, �ش� ����.  �ﰢ���� 01 ��������, 12 ��������, 20 ��������.
            _vector vLine = (m_vecCells[m_iCurrentCellIndex]->Get_Point((CCell::POINT)((i + 1) % 3)) - m_vecCells[m_iCurrentCellIndex]->Get_Point((CCell::POINT)i));

            // ���� ���� ���ϱ� 
            _vector vNormal = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vLine) * -1.0f, 0.0f, XMVectorGetX(vLine), 0.0f));

            // ���� ���⺤�Ϳ� ���� ���� ������ �ذ�����, ����, �����̸� �ȿ��ִ�.  ����̸� �ٱ��� �ִ�.
            if (0.0f <  XMVectorGetX(XMVector3Dot(vDir, vNormal)))
            {
                // �������� �÷��̾� ��ġ���� �浹�� �������� �����Ⱦƹ���, ���⺤��.
                _vector vLineCollisionPoint = m_vecCells[m_iCurrentCellIndex]->Get_Point((CCell::POINT)i) + XMVector3Dot(vDir, vLine) * vLine / XMVector3Dot(vLine, vLine);
                _vector vSlideDir = XMVector3Normalize(vLineCollisionPoint - vPosition);
                return vPosition + (vSlideDir * 0.1f);
            }
        }
    }

    return _vWorldPos;
}

void CNavigation::Setting_ShrinkCell(_float _fShrinkRatio)
{ 
    for (auto& pCell : m_vecCells)
    {
        pCell->Setting_ShrinkPoints(_fShrinkRatio);
    }
}

_bool CNavigation::bIsIn_ShrinkedCell(_fvector _vWorldPos)
{
    _vector vPosition = XMVector3TransformCoord(_vWorldPos, XMLoadFloat4x4(&m_WorldMatrixInverse));

    for (auto& pCell : m_vecCells)
    {
        if (pCell->bIsIn_ShrinkedCell(vPosition))
            return true;
    }

    return false;
}

_bool CNavigation::bIsOn_Line(_fvector _vWorldPos)
{
    _vector vPosition = XMVector3TransformCoord(_vWorldPos, XMLoadFloat4x4(&m_WorldMatrixInverse));

    for (_uint i = 0; i < CCell::LINE_END; ++i)
    {
        _vector vDir = (vPosition - m_vecCells[m_iCurrentCellIndex]->Get_Point((CCell::POINT)i));
        _vector vLine = (m_vecCells[m_iCurrentCellIndex]->Get_Point((CCell::POINT)((i + 1) % 3)) - m_vecCells[m_iCurrentCellIndex]->Get_Point((CCell::POINT)i));
        _vector vNormal = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vLine) * -1.0f, 0.0f, XMVectorGetX(vLine), 0.0f));
        
        // �����̸� �ȿ��ִ�.����̸� �ٱ��� �ִ�.
        if (0.0f < XMVectorGetX(XMVector3Dot(vDir, vNormal))) 
        {
            return true; 
        }
    }

    return false; 
}

#ifdef _DEBUG
HRESULT CNavigation::Render()
{
    _float4x4               ViewMatrix, ProjMatrix = {};
    ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
    ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;

    // m_iCurrentCellIndex ��, -1 �̶��, �ʷϻ�, �� �ܶ�� ������. 

    _float4 vColor = -1 == m_iCurrentCellIndex ? _float4(0.0f, 1.0f, 0.0f, 1.0f) : _float4(1.0f, 0.0f, 0.0f, 1.0f);

    if (FAILED(m_pShader->Bind_RawValue("g_vColor", &vColor, sizeof(_float4))))
        return E_FAIL;

    if (-1 == m_iCurrentCellIndex)
    {
        m_pShader->Begin(0);
        for (auto& pCell : m_vecCells)
        {
            //if(nullptr != pCell)
            pCell->Render();
        }
    }
    else
    {
        // - 1 �� �ƴϾ��ٸ�, �ϴ� ���� ����� �ϳ� �޾ƿ���, 
        _float4x4           WorldMatrix = m_WorldMatrix;

        // ����� ���̸� [3][1] �� �÷����� 
        WorldMatrix.m[3][1] += 0.1f;

        if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &WorldMatrix)))
            return E_FAIL;
        m_pShader->Begin(0);

        m_vecCells[m_iCurrentCellIndex]->Render();
    }

    return S_OK;
}
#endif

HRESULT CNavigation::SetUp_NeighborCells()
{
    // pSourCell : �޾ƿ� ��, pDescCell : ���� ��(��ü)�� �ִ� ��.
    for (auto& pSourCell : m_vecCells)
    {
        for (auto& pDescCell : m_vecCells)
        {
            // ù �˻� ���� ���� ���, ( �� ���� �������� ������, ) continue �� �ѱ��, �ٽ� �˻� ���� 
            if (pSourCell == pDescCell)
                continue;

            // �޾ƿ� ���� �� A �� B �� ���ٴ°�, true ���, �޾ƿͼ� �˻�ǽ��� sourcCell �� ab ������ �����ϴ±���! 
            if (true == pDescCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_A), pSourCell->Get_Point(CCell::POINT_B)))
                pSourCell->Set_Neighbor(CCell::LINE_AB, pDescCell);
            if (true == pDescCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_B), pSourCell->Get_Point(CCell::POINT_C)))
                pSourCell->Set_Neighbor(CCell::LINE_BC, pDescCell);
            if (true == pDescCell->Compare_Points(pSourCell->Get_Point(CCell::POINT_C), pSourCell->Get_Point(CCell::POINT_A)))
                pSourCell->Set_Neighbor(CCell::LINE_CA, pDescCell);

        }
    }

    return S_OK;
}

CNavigation* CNavigation::CreateOnly(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CNavigation* pInstance = new CNavigation(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype_CreateOnly()))
    {
        MSG_BOX("Failed To Created : CNavigation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CNavigation* CNavigation::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _tchar* _pNavigationDataFile)
{
    CNavigation* pInstance = new CNavigation(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype(_pNavigationDataFile)))
    {
        MSG_BOX("Failed To Created : CNavigation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

HRESULT CNavigation::Create_Cell(const _float3 _vPoints[3])
{
    CCell* pCell = CCell::Create(m_pDevice, m_pContext, _vPoints, (_uint)(m_vecCells.size()));
    if (nullptr == pCell)
        return E_FAIL;

    m_vecCells.push_back(pCell);

    cout << "�� �ϳ� ����! �� push_back �Ϸ�!" << endl;

    return S_OK;
}

HRESULT CNavigation::Delete_Cell(const _float3 _vPoints[3])
{
    for (auto iter = m_vecCells.begin(); iter != m_vecCells.end(); ++iter)
    {
        if ((*iter)->Compare_Points(XMLoadFloat3(&_vPoints[0]), XMLoadFloat3(&_vPoints[1])) &&
            (*iter)->Compare_Points(XMLoadFloat3(&_vPoints[1]), XMLoadFloat3(&_vPoints[2])) &&
            (*iter)->Compare_Points(XMLoadFloat3(&_vPoints[2]), XMLoadFloat3(&_vPoints[0])))
        {
            Safe_Release(*iter);
            m_vecCells.erase(iter);

            return S_OK;
        }
    }

    return E_FAIL;
}

CComponent* CNavigation::Clone(void* _pArg)
{
    CNavigation* pInstance = new CNavigation(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CNavigation");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNavigation::Free()
{
    __super::Free();


    for (auto& pCell : m_vecCells)
    {
        Safe_Release(pCell);
    }
    m_vecCells.clear();


#ifdef _DEBUG
    Safe_Release(m_pShader);
#endif

}
