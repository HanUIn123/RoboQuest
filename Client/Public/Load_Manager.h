#pragma once
#include "Client_Defines.h"
#include "MapToolMonster.h"
#include "Monster.h"

#include "TestBuilding.h"
#include "Building.h"

#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CModel;
END

BEGIN(Client)
class CLoad_Manager : public CBase
{
    DECLARE_SINGLETON(CLoad_Manager)

public:
    CLoad_Manager();
    virtual ~CLoad_Manager();

private:
    CGameInstance* m_pGameInstance = { nullptr };

#pragma region Terrain Load
public:
    VTXNORTEX*                                  Load_Terrain_Height(_uint _iTerrainIndex = 0);
    VTXNORTEX*                                  Load_MapToolTerrain_Height();
    VTXNORTEX*                                  pBasicTerrainVertices = { nullptr };
    VTXNORTEX*                                  pMapToolTerrainVertices = { nullptr };
#pragma endregion

#pragma region Monster Load
public:
    void                                        Load_Monster(const wstring& filePath);
private:
    _uint                                       m_iMonsterCount = {};
    vector<CMonster::INGAME_MONSTER_TYPE>       m_vecMonsterType;
    vector<_float3>                             m_vecMonsterPoisition;
    vector<_float3>                             m_vecMonsterRotation;
    vector<_float3>                             m_vecMonsterScale;
public:
    vector<CMonster::INGAME_MONSTER_TYPE>       Get_MonsterType() { return m_vecMonsterType; }
    _uint                                       Get_MonsterCount() { return m_iMonsterCount; }
    vector<_float3>                             Get_MonsterPosition() { return m_vecMonsterPoisition; }
    vector<_float3>                             Get_MonsterRotation() { return m_vecMonsterRotation; }
    vector<_float3>                             Get_MonsterScale() { return m_vecMonsterScale; }
#pragma endregion

#pragma region Building Load
public:
    void                                        Load_Building(const wstring& filePath);
private:
    _uint                                       m_iBuildingCount = {};
    vector<CBuilding::INGAME_BUILDING_TYPE>     m_vecBuildingType;
    vector<_float3>                             m_vecBuildingPoisition;
    vector<_float3>                             m_vecBuildingRotation;
    vector<_float3>                             m_vecBuildingScale;
public:
    vector<CBuilding::INGAME_BUILDING_TYPE>     Get_BuildingType() { return m_vecBuildingType; }
    _uint                                       Get_BuildingCount() { return m_iBuildingCount; }
    vector<_float3>                             Get_BuildingPosition() { return m_vecBuildingPoisition; }
    vector<_float3>                             Get_BuildingRotation() { return m_vecBuildingRotation; }
    vector<_float3>                             Get_BuildingScale() { return m_vecBuildingScale; }
#pragma endregion

    virtual void Free() override;
};

END