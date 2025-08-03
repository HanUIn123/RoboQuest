#include "pch.h"
#include "Load_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLoad_Manager)

CLoad_Manager::CLoad_Manager()
{
}

CLoad_Manager::~CLoad_Manager()
{
}

VTXNORTEX* CLoad_Manager::Load_Terrain_Height(_uint _iTerrainIndex)
{
    const _tchar* strFilePath{};
    if (_iTerrainIndex == 0)
    {
        //strFilePath = L"../Bin/DataFiles/TerrainData/Terrain01.txt";
        strFilePath = L"../Bin/DataFiles/TerrainData/ReadlTerrain.txt";
    }
    else if (_iTerrainIndex == 1)
        strFilePath = L"../Bin/DataFiles/TerrainData/Terrain02.txt";
    //const _tchar* strFilePath = L"";

    _ulong  dwByte = {};
    HANDLE  hFile = CreateFile(strFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX("Failed To Open Terrain File!!");
        return nullptr;
    }

    _uint numVertices = {};
    ReadFile(hFile, &numVertices, sizeof(_uint), &dwByte, nullptr);

    pBasicTerrainVertices = new VTXNORTEX[numVertices];
    ReadFile(hFile, pBasicTerrainVertices, sizeof(VTXNORTEX) * numVertices, &dwByte, nullptr);

    CloseHandle(hFile);

    return pBasicTerrainVertices;
}

VTXNORTEX* CLoad_Manager::Load_MapToolTerrain_Height()
{
    const _tchar* strFilePath = L"../Bin/DataFiles/TerrainHeight.txt";

    if (nullptr == strFilePath)
        return nullptr;

    _ulong dwByte = {};
    HANDLE  hFile = CreateFile(strFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        //MSG_BOX("Failed To Open Terrain File!!");
        return nullptr;
    }

    _uint numVertices = {};
    ReadFile(hFile, &numVertices, sizeof(_uint), &dwByte, nullptr);

    pMapToolTerrainVertices = new VTXNORTEX[numVertices];
    ReadFile(hFile, pMapToolTerrainVertices, sizeof(VTXNORTEX) * numVertices, &dwByte, nullptr);

    CloseHandle(hFile);

    return pMapToolTerrainVertices;
}

void CLoad_Manager::Load_Monster(const wstring& filePath)
{
    if (filePath.empty())
    {
        MSG_BOX("Invalid file path!");
        return;
    }

    HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX("Failed To Open Monster File!!");
        return;
    }

    _ulong dwByte = 0;
    ReadFile(hFile, &m_iMonsterCount, sizeof(_uint), &dwByte, nullptr);

    m_vecMonsterType.resize(m_iMonsterCount);
    m_vecMonsterPoisition.resize(m_iMonsterCount);
    m_vecMonsterRotation.resize(m_iMonsterCount);
    m_vecMonsterScale.resize(m_iMonsterCount);
    for (_uint i = 0; i < m_iMonsterCount; ++i)
    {
        CMapToolMonster::MONSTER_TYPE eMonsterType;
        ReadFile(hFile, &eMonsterType, sizeof(CMapToolMonster::MONSTER_TYPE), &dwByte, nullptr);

        CMonster::INGAME_MONSTER_TYPE eIGMonsterType = {};
        switch (eMonsterType)
        {
        case CMapToolMonster::MT_ROBO_L:
            eIGMonsterType = CMonster::MT_BOOM;
            break;
        case CMapToolMonster::MT_ROBO_B:
            eIGMonsterType = CMonster::MT_BILLY;
            break;
        case CMapToolMonster::MT_ROBO_G:
            eIGMonsterType = CMonster::MT_GOLIATH;
            break;
        case CMapToolMonster::MT_ROBO_V:
            eIGMonsterType = CMonster::MT_LARVA;
            break;
        case CMapToolMonster::MT_ROBO_S:
            eIGMonsterType = CMonster::MT_SMALL;
            break;
        case CMapToolMonster::MT_DOOR:
            eIGMonsterType = CMonster::MT_DOOR;
            break;
        case CMapToolMonster::MT_ROBO_BOSS:
            eIGMonsterType = CMonster::MT_BOSS;
            break;
        case CMapToolMonster::MT_ITEM_BOX:
            eIGMonsterType = CMonster::MT_ITEM_BOX;
            break;
        case CMapToolMonster::MT_GATE:
            eIGMonsterType = CMonster::MT_GATE;
            break;
        }
        m_vecMonsterType[i] = eIGMonsterType;

        _float3 vPosition;
        ReadFile(hFile, &vPosition, sizeof(_float3), &dwByte, nullptr);
        m_vecMonsterPoisition[i] = vPosition;

        _float3 vRotation;
        ReadFile(hFile, &vRotation, sizeof(_float3), &dwByte, nullptr);
        m_vecMonsterRotation[i] = vRotation;

        _float3 vScale;
        ReadFile(hFile, &vScale, sizeof(_float3), &dwByte, nullptr);
        m_vecMonsterScale[i] = vScale;
    }
    CloseHandle(hFile);
}

void CLoad_Manager::Load_Building(const wstring& filePath)
{
    //const _tchar* strFilePath = L"../Bin/DataFiles/BuildingData.txt";
    if (filePath.empty())
    {
        MSG_BOX("Invalid file path!");
        return;
    }

    HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX("Failed To Open Monster File!!");
        return;
    }

    _ulong  dwByte = {};
    ReadFile(hFile, &m_iBuildingCount, sizeof(_uint), &dwByte, nullptr);

    m_vecBuildingType.resize(m_iBuildingCount);
    m_vecBuildingPoisition.resize(m_iBuildingCount);
    m_vecBuildingRotation.resize(m_iBuildingCount);
    m_vecBuildingScale.resize(m_iBuildingCount);
    for (_uint i = 0; i < m_iBuildingCount; ++i)
    {
        CTestBuilding::BUILDING_TYPE eBuildingType;
        ReadFile(hFile, &eBuildingType, sizeof(CTestBuilding::BUILDING_TYPE), &dwByte, nullptr);

        CBuilding::INGAME_BUILDING_TYPE eIgBuildingType = {};
        switch (eBuildingType)
        {
        case CTestBuilding::BT_WALL:
            eIgBuildingType = CBuilding::BT_WALL;
            break;
        case CTestBuilding::BT_BIGWALL:
            eIgBuildingType = CBuilding::BT_BIG_WALL;
            break;
        case CTestBuilding::BT_PILLAR:
            eIgBuildingType = CBuilding::BT_PILLAR;
            break;
        case CTestBuilding::BT_CONCRETE:
            eIgBuildingType = CBuilding::BT_CONCRETE;
            break;
        }
        m_vecBuildingType[i] = eIgBuildingType;

        _float3 vPosition;
        ReadFile(hFile, &vPosition, sizeof(_float3), &dwByte, nullptr);
        m_vecBuildingPoisition[i] = vPosition;

        _float3 vRotation;
        ReadFile(hFile, &vRotation, sizeof(_float3), &dwByte, nullptr);
        m_vecBuildingRotation[i] = vRotation;

        _float3 vScale;
        ReadFile(hFile, &vScale, sizeof(_float3), &dwByte, nullptr);
        m_vecBuildingScale[i] = vScale;
    }
    CloseHandle(hFile);

}

void CLoad_Manager::Free()
{
    __super::Free();

    Safe_Delete_Array(pBasicTerrainVertices);
}
