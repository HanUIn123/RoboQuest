#pragma once

#include "Client_Defines.h"
#include "MapToolMonster.h"
#include "Monster.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CModel;
END

BEGIN(Client)
class CImGui_Manager : public CBase
{
    DECLARE_SINGLETON(CImGui_Manager)

public:
    CImGui_Manager();
    virtual ~CImGui_Manager();

private:
    CGameInstance*                          m_pGameInstance = { nullptr };
private:
    ID3D11RasterizerState*                  m_pFrameStateRS[2] = { nullptr };
    D3D11_RASTERIZER_DESC                   m_tagRasterizeDesc[2] = {};
    void                                    InitRenderStates();
public:
    void                                    SwitchFrameMode();

//#pragma region Terrain·Îµå
//    VTXNORTEX*                              Load_Terrain_Height();
//    VTXNORTEX*                              Load_MapToolTerrain_Height();
//    VTXNORTEX*                              pBasicTerrainVertices = { nullptr };
//    VTXNORTEX*                              pMapToolTerrainVertices = { nullptr };
//#pragma endregion
//
//#pragma region Monster Load
//public:
//    void                                    Load_Monster();
//private:
//    _uint                                   m_iMonsterCount = {};
//    vector<CMonster::INGAME_MONSTER_TYPE>   m_vecMonsterType;
//    vector<_float3>                         m_vecMonsterPoisition;
//    vector<_float3>                         m_vecMonsterRotation;
//    vector<_float3>                         m_vecMonsterScale;
//public:
//    vector<CMonster::INGAME_MONSTER_TYPE>   Get_MonsterType() { return m_vecMonsterType; }
//    _uint                                   Get_MonsterCount() { return m_iMonsterCount; }
//    vector<_float3>                         Get_MonsterPosition() { return m_vecMonsterPoisition; }
//    vector<_float3>                         Get_MonsterRotation() { return m_vecMonsterRotation; }
//    vector<_float3>                         Get_MonsterScale() { return m_vecMonsterScale; }
//#pragma endregion

private:
    _bool                                   m_bWireFrameMode = { false };
public:
    void                                    ImGui_SetUp(CGameInstance* _pGameInstance);
    void                                    ImGui_Tick();
    void                                    ImGui_Render();
    void                                    ImGui_ShutDown();

    virtual void Free() override;
};
END