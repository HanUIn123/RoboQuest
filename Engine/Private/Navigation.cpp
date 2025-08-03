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

        // 루프 탈출문 조건
        if (0 == dwByte)
            break;

        // size 로 줘서 처음에 하나 들어가면 용량 1개, 하나 또 들어가면 2개.~~ 
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

    // 이제 이걸 호출해줘야 최종적으로 현재 셀 4개 를 뛰놀수 있게 되었음. 
    if (FAILED(SetUp_NeighborCells()))
        return E_FAIL;


    return S_OK;















//    _ulong dwByte = 0;
//    HANDLE hFile = CreateFile(_pNavigationDataFile, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
//
//    // 파일이 없으면 기본 초기화만 진행
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
//    // 기존 데이터 초기화
//    for (auto& pCell : m_vecCells)
//    {
//        Safe_Release(pCell);
//    }
//    m_vecCells.clear();
//
//    // Cell 개수 읽기
//    _uint iCellCountNum = {};
//    if (!ReadFile(hFile, &iCellCountNum, sizeof(_uint), &dwByte, nullptr) || dwByte == 0)
//    {
//        MSG_BOX("Failed to read cell count from navigation file!");
//        CloseHandle(hFile);
//        return E_FAIL;
//    }
//
//    // Cell 데이터 읽기 및 추가
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
//    // m_bFirstPick 상태 읽기
//    if (!ReadFile(hFile, &m_bFirstPick, sizeof(bool), &dwByte, nullptr) || dwByte == 0)
//    {
//        MSG_BOX("Failed to read m_bFirstPick from navigation file!");
//        CloseHandle(hFile);
//        return E_FAIL;
//    }
//
//    CloseHandle(hFile);
//
//    // 월드 행렬 초기화
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

    // 이제 이걸 호출해줘야 최종적으로 현재 셀 4개 를 뛰놀수 있게 되었음. 
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
                // 최초로 나간 경우에 인덱스가 한 프레임 밀려서 좀 못따라오는 경우가 있어서 예외 처리함. 
                if (-1 == iNeighborIndex)
                    return false;

                if (true == m_vecCells[iNeighborIndex]->bIsIn(vPosition, &iNeighborIndex))
                {
                    //m_iCurrentCellIndex = iNeighborIndex;
                    //return true;
                    break;
                }
            }

            // 이웃삼각형으로 간 경우에 현재 인덱스가 이제 이웃삼각형의 인덱스로 바꿔짐. 
            m_iCurrentCellIndex = iNeighborIndex;
            m_iNeighborIndex = iNeighborIndex;
            return true;
        }
        else
        {
            // 현재의 셀을 나갔다. 
            m_iNeighborIndex = -1;
            return false;
        }
    }
    else
    {
        // 셀 안에 그냥 있다. 
        m_iNeighborIndex = iNeighborIndex;
        return true;
    }
}


_float CNavigation::Compute_Height(_fvector _vWorldPos)
{
    // 그냥 지형이라면, 내 현재 셀 인덱스가, 
    if (-1 == m_iCurrentCellIndex)
    {
        return 0.0f;
    }

    // 이 함수를 사용할 객체의 _vWorldPos 즉 월드 행렬을 가져오기 때문에, 
    // 그런데 평면의 방정식을 통한 셀의 높이 를 구하는 좌표는 로컬 영역이기 때문에, 
    // 여기서 한 번 월드 행렬의 역행렬을 통해 로컬 vPosition 을 구해주고, 

    _vector     vPosition = XMVector3TransformCoord(_vWorldPos, XMLoadFloat4x4(&m_WorldMatrixInverse));

    // 그 로컬을 Cell 클래스의 Comput_Hegiht에 인자로 넘겨주어서, 받아온 로컬 좌표를 다시 vPosition 에 넣어준다. 
    vPosition = XMVectorSetY(vPosition, m_vecCells[m_iCurrentCellIndex]->Compute_Height(vPosition));

    // 그럼 그 로컬 vPosition 을 다시 한 번 월드로 바꿔주어서,
    vPosition = XMVector3TransformCoord(vPosition, XMLoadFloat4x4(&m_WorldMatrix));

    // 그 vPosition 의 최종 월드 Y 좌표를 뽑아내줌 .
    return XMVectorGetY(vPosition);
}

_vector CNavigation::Setting_SlidingMove(_fvector _vWorldPos)
{
    if (m_iNeighborIndex == -1)
    {
        _vector vPosition = XMVector3TransformCoord(_vWorldPos, XMLoadFloat4x4(&m_WorldMatrixInverse));

        for (_uint i = 0; i < CCell::LINE_END; ++i)
        {
            // 방향 벡터 구하기 .
            _vector vDir = (vPosition - m_vecCells[m_iCurrentCellIndex]->Get_Point((CCell::POINT)i));

            // 법선 벡터 구하기 위해서, 해당 라인.  삼각형의 01 라인인지, 12 라인인지, 20 라인인지.
            _vector vLine = (m_vecCells[m_iCurrentCellIndex]->Get_Point((CCell::POINT)((i + 1) % 3)) - m_vecCells[m_iCurrentCellIndex]->Get_Point((CCell::POINT)i));

            // 법선 벡터 구하기 
            _vector vNormal = XMVector3Normalize(XMVectorSet(XMVectorGetZ(vLine) * -1.0f, 0.0f, XMVectorGetX(vLine), 0.0f));

            // 이제 방향벡터와 법선 벡터 내적을 해가지구, 깂이, 음수이면 안에있다.  양수이면 바깥에 있다.
            if (0.0f <  XMVectorGetX(XMVector3Dot(vDir, vNormal)))
            {
                // 투영벡터 플레이어 위치에서 충돌할 선분으로 내려꽂아버릴, 방향벡터.
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
        
        // 음수이면 안에있다.양수이면 바깥에 있다.
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

    // m_iCurrentCellIndex 가, -1 이라면, 초록색, 그 외라면 빨간색. 

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
        // - 1 이 아니었다면, 일단 월드 행렬을 하나 받아오고, 
        _float4x4           WorldMatrix = m_WorldMatrix;

        // 행렬의 깊이를 [3][1] 를 올려보기 
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
    // pSourCell : 받아올 셀, pDescCell : 현재 나(객체)가 있는 셀.
    for (auto& pSourCell : m_vecCells)
    {
        for (auto& pDescCell : m_vecCells)
        {
            // 첫 검사 부터 같다 라면, ( 두 셀의 시작점은 같으니, ) continue 로 넘기고, 다시 검사 시작 
            if (pSourCell == pDescCell)
                continue;

            // 받아온 셀의 점 A 와 B 가 같다는게, true 라면, 받아와서 검사실시한 sourcCell 이 ab 라인을 공유하는구나! 
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

    cout << "셀 하나 생성! 후 push_back 완료!" << endl;

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
