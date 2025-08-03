#include "pch.h"
#include "Level_MapTool.h"
#include "GameInstance.h"
#include "Level_Loading.h"
//#include "Camera_MapTool.h"
//#include "MapToolTerrain.h"
#include "Monster.h"


CLevel_MapTool::CLevel_MapTool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CLevel(_pDevice, _pContext)
{
    Register_PreviewImage_In_ImGuiWindow(TEXT("../Bin/Resources/MyTextures/ImGui_PreviewImage/Enemies/BoomMonster.dds"), IMG_MONSTER, 1);
    Register_PreviewImage_In_ImGuiWindow(TEXT("../Bin/Resources/MyTextures/ImGui_PreviewImage/Enemies/BillyBoom.dds"), IMG_MONSTER, 1);
    Register_PreviewImage_In_ImGuiWindow(TEXT("../Bin/Resources/MyTextures/ImGui_PreviewImage/Enemies/Robo_Goliath.dds"), IMG_MONSTER, 1);
    Register_PreviewImage_In_ImGuiWindow(TEXT("../Bin/Resources/MyTextures/ImGui_PreviewImage/Enemies/Robo_Larva.dds"), IMG_MONSTER, 1);
    Register_PreviewImage_In_ImGuiWindow(TEXT("../Bin/Resources/MyTextures/ImGui_PreviewImage/Enemies/Robo_Small.dds"), IMG_MONSTER, 1);
    Register_PreviewImage_In_ImGuiWindow(TEXT("../Bin/Resources/MyTextures/ImGui_PreviewImage/Enemies/Wall.png"), IMG_MONSTER, 1);
    Register_PreviewImage_In_ImGuiWindow(TEXT("../Bin/Resources/MyTextures/ImGui_PreviewImage/Enemies/Robo_Boss.dds"), IMG_MONSTER, 1);
    Register_PreviewImage_In_ImGuiWindow(TEXT("../Bin/Resources/MyTextures/ImGui_PreviewImage/Enemies/ItemBox.dds"), IMG_MONSTER, 1);
    Register_PreviewImage_In_ImGuiWindow(TEXT("../Bin/Resources/MyTextures/ImGui_PreviewImage/Enemies/Gate.png"), IMG_MONSTER, 1);


    Register_PreviewImage_In_ImGuiWindow(TEXT("../Bin/Resources/MyTextures/ImGui_PreviewImage/Environment/Wall%d.png"), IMG_BUILDING, 4);

    Register_PreviewImage_In_ImGuiWindow(TEXT("../Bin/Resources/MyTextures/ImGui_PreviewImage/MaskImages/Mask%d.png"), IMG_MASK, 2);

    Register_PreviewImage_In_ImGuiWindow(TEXT("../Bin/Resources/MyTextures/ImGui_PreviewImage/DiffuseImages/Terrain%d.png"), IMG_DIFFUSE, 8);
}

HRESULT CLevel_MapTool::Initialize()
{
    if (FAILED(Ready_Layer_MapToolTerrain(TEXT("Layer_MapTool_Terrain"))))
        return E_FAIL;

    if (FAILED(Ready_Light(TEXT("Layer_Light"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_MapToolCamera(TEXT("Layer_MapTool_Camera"))))
        return E_FAIL;

    m_pMapToolCamera = static_cast<CCamera_MapTool*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Camera_MapTool"), LEVEL_MAPTOOL, TEXT("Layer_MapTool_Camera")));
    m_pMapToolTerrain = static_cast<CMapToolTerrain*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_MapToolTerrain"), LEVEL_MAPTOOL, TEXT("Layer_MapTool_Terrain")));
    m_pMapToolBuffer = static_cast<CVIBuffer_MapToolTerrain*>(m_pMapToolTerrain->Find_Component(TEXT("Com_VITerrainBuffer_MapTool")));

    m_iNumMapToolTerrainNumVerticesX = m_pMapToolBuffer->Get_NumVerticesX();
    m_iNumMapToolTerrainNumVerticesZ = m_pMapToolBuffer->Get_NumVerticesZ();

    //m_pTestWall = static_cast<CTestWall*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_")))


    m_pNavigation = static_cast<CNavigation*>(m_pMapToolTerrain->Find_Component(TEXT("Com_Navigation")));


    return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_MapToolMonster(const _tchar* _pLayerTags, CMapToolMonster::MONSTER_TYPE _eMonsterType, _uint _iLoadedDataTypeIndex, void* _pMonsterArg)
{
    if (_iLoadedDataTypeIndex == 1)
    {
        switch (_eMonsterType)
        {
        case CMapToolMonster::MONSTER_TYPE::MT_ROBO_L:
        {
            CTestNorRobo::TEST_NOR_ROBO NorRoboDesc = {};
            NorRoboDesc.vMonsterpos = m_fPickPos;
            NorRoboDesc.vMonsterRotation = m_fRotation;
            NorRoboDesc.vMonsterScale = m_fScale;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestNorRobo"), LEVEL_MAPTOOL, _pLayerTags, &NorRoboDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestNorRobo* pMonster = dynamic_cast<CTestNorRobo*>(pListGameObject->back());
            pMonster->Set_MonsterType(_eMonsterType);
            m_vecTestMonster.push_back(pMonster);
        }
        break;
        case CMapToolMonster::MONSTER_TYPE::MT_ROBO_B:
        {
            CTestMonster::TESTMONSTER_DESC MonsterDesc = {};
            MonsterDesc.vMonsterpos = m_fPickPos;
            MonsterDesc.vMonsterRotation = m_fRotation;
            MonsterDesc.vMonsterScale = m_fScale;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestMonster"), LEVEL_MAPTOOL, _pLayerTags, &MonsterDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestMonster* pMonster = dynamic_cast<CTestMonster*>(pListGameObject->back());
            pMonster->Set_MonsterType(_eMonsterType);
            m_vecTestMonster.push_back(pMonster);
        }
        break;
        case CMapToolMonster::MONSTER_TYPE::MT_ROBO_G:
        {
            CTestGoliath::TESTGOLIATH_DESC MonsterDesc = {};
            MonsterDesc.vMonsterpos = m_fPickPos;
            MonsterDesc.vMonsterRotation = m_fRotation;
            MonsterDesc.vMonsterScale = m_fScale;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestGoliath"), LEVEL_MAPTOOL, _pLayerTags, &MonsterDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestGoliath* pMonster = dynamic_cast<CTestGoliath*>(pListGameObject->back());
            pMonster->Set_MonsterType(_eMonsterType);
            m_vecTestMonster.push_back(pMonster);
        }
        break;
        case CMapToolMonster::MONSTER_TYPE::MT_ROBO_V:
        {
            CTestLarva::TESTLARVA_DESC MonsterDesc = {};
            MonsterDesc.vMonsterpos = m_fPickPos;
            MonsterDesc.vMonsterRotation = m_fRotation;
            MonsterDesc.vMonsterScale = m_fScale;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestLarva"), LEVEL_MAPTOOL, _pLayerTags, &MonsterDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestLarva* pMonster = dynamic_cast<CTestLarva*>(pListGameObject->back());
            pMonster->Set_MonsterType(_eMonsterType);
            m_vecTestMonster.push_back(pMonster);
        }
        break;
        case CMapToolMonster::MONSTER_TYPE::MT_ROBO_S:
        {
            CTestSmall::TESTSMALL_DESC MonsterDesc = {};
            MonsterDesc.vMonsterpos = m_fPickPos;
            MonsterDesc.vMonsterRotation = m_fRotation;
            MonsterDesc.vMonsterScale = m_fScale;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestSmall"), LEVEL_MAPTOOL, _pLayerTags, &MonsterDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestSmall* pMonster = dynamic_cast<CTestSmall*>(pListGameObject->back());
            pMonster->Set_MonsterType(_eMonsterType);
            m_vecTestMonster.push_back(pMonster);
        }
        break;
        case CMapToolMonster::MONSTER_TYPE::MT_DOOR:
        {
            CTestDoor::TESTDOOR_DESC MonsterDesc = {};
            MonsterDesc.vMonsterpos = m_fPickPos;
            MonsterDesc.vMonsterRotation = m_fRotation;
            MonsterDesc.vMonsterScale = m_fScale;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestDoor"), LEVEL_MAPTOOL, _pLayerTags, &MonsterDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestDoor* pMonster = dynamic_cast<CTestDoor*>(pListGameObject->back());
            pMonster->Set_MonsterType(_eMonsterType);
            m_vecTestMonster.push_back(pMonster);
        }
        break;

        case CMapToolMonster::MONSTER_TYPE::MT_ROBO_BOSS:
        {
            CTestBoss::TESTBOSS_DESC MonsterDesc = {};
            MonsterDesc.vMonsterpos = m_fPickPos;
            MonsterDesc.vMonsterRotation = m_fRotation;
            MonsterDesc.vMonsterScale = m_fScale;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestBoss"), LEVEL_MAPTOOL, _pLayerTags, &MonsterDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestBoss* pMonster = dynamic_cast<CTestBoss*>(pListGameObject->back());
            pMonster->Set_MonsterType(_eMonsterType);
            m_vecTestMonster.push_back(pMonster);
        }
        break;

        case CMapToolMonster::MONSTER_TYPE::MT_ITEM_BOX:
        {
            CTestBox::TESTBOX_DESC BoxDesc = {};
            BoxDesc.vMonsterpos = m_fPickPos;
            BoxDesc.vMonsterRotation = m_fRotation;
            BoxDesc.vMonsterScale = m_fScale;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestBox"), LEVEL_MAPTOOL, _pLayerTags, &BoxDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestBox* pMonster = dynamic_cast<CTestBox*>(pListGameObject->back());
            pMonster->Set_MonsterType(_eMonsterType);
            m_vecTestMonster.push_back(pMonster);
        }
        break;

        case CMapToolMonster::MONSTER_TYPE::MT_GATE:
        {
            CTestGate::TESTGATE_DESC GateDesc = {};
            GateDesc.vMonsterpos = m_fPickPos;
            GateDesc.vMonsterRotation = m_fRotation;
            GateDesc.vMonsterScale = m_fScale;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestGate"), LEVEL_MAPTOOL, _pLayerTags, &GateDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestGate* pMonster = dynamic_cast<CTestGate*>(pListGameObject->back());
            pMonster->Set_MonsterType(_eMonsterType);
            m_vecTestMonster.push_back(pMonster);
        }
        break;
        }
    }
    else if (_iLoadedDataTypeIndex == 2)
    {
        CMapToolMonster::GAMEOBJECT_DESC* pLoadedDesc = static_cast<CMapToolMonster::GAMEOBJECT_DESC*>(_pMonsterArg);
        if (!pLoadedDesc)
            return E_FAIL;

        switch (_eMonsterType)
        {
        case CMapToolMonster::MT_ROBO_L:
        {
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestNorRobo"), LEVEL_MAPTOOL, _pLayerTags, pLoadedDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestNorRobo* pMonster = dynamic_cast<CTestNorRobo*>(pListGameObject->back());
            pMonster->Set_MonsterType(_eMonsterType);
            m_vecTestMonster.push_back(pMonster);
        }
        break;

        case CMapToolMonster::MT_ROBO_B:
        {
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestMonster"), LEVEL_MAPTOOL, _pLayerTags, pLoadedDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestMonster* pMonster = dynamic_cast<CTestMonster*>(pListGameObject->back());
            pMonster->Set_MonsterType(_eMonsterType);
            m_vecTestMonster.push_back(pMonster);
        }
        break;

        case CMapToolMonster::MT_ROBO_G:
        {
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestGoliath"), LEVEL_MAPTOOL, _pLayerTags, pLoadedDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestGoliath* pMonster = dynamic_cast<CTestGoliath*>(pListGameObject->back());
            pMonster->Set_MonsterType(_eMonsterType);
            m_vecTestMonster.push_back(pMonster);
        }
        break;
        case CMapToolMonster::MT_ROBO_V:
        {
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestLarva"), LEVEL_MAPTOOL, _pLayerTags, pLoadedDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestLarva* pMonster = dynamic_cast<CTestLarva*>(pListGameObject->back());
            pMonster->Set_MonsterType(_eMonsterType);
            m_vecTestMonster.push_back(pMonster);
        }
        break;
        case CMapToolMonster::MT_ROBO_S:
        {
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestSmall"), LEVEL_MAPTOOL, _pLayerTags, pLoadedDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestSmall* pMonster = dynamic_cast<CTestSmall*>(pListGameObject->back());
            pMonster->Set_MonsterType(_eMonsterType);
            m_vecTestMonster.push_back(pMonster);
        }
        break;
        case CMapToolMonster::MT_DOOR:
        {
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestDoor"), LEVEL_MAPTOOL, _pLayerTags, pLoadedDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestDoor* pMonster = dynamic_cast<CTestDoor*>(pListGameObject->back());
            pMonster->Set_MonsterType(_eMonsterType);
            m_vecTestMonster.push_back(pMonster);
        }
        break;
        case CMapToolMonster::MT_ROBO_BOSS:
        {
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestBoss"), LEVEL_MAPTOOL, _pLayerTags, pLoadedDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestBoss* pMonster = dynamic_cast<CTestBoss*>(pListGameObject->back());
            pMonster->Set_MonsterType(_eMonsterType);
            m_vecTestMonster.push_back(pMonster);
        }
        break;
        case CMapToolMonster::MT_ITEM_BOX:
        {
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestBox"), LEVEL_MAPTOOL, _pLayerTags, pLoadedDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestBox* pMonster = dynamic_cast<CTestBox*>(pListGameObject->back());
            pMonster->Set_MonsterType(_eMonsterType);
            m_vecTestMonster.push_back(pMonster);
        }
        break;
        case CMapToolMonster::MT_GATE:
        {
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestGate"), LEVEL_MAPTOOL, _pLayerTags, pLoadedDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestGate* pMonster = dynamic_cast<CTestGate*>(pListGameObject->back());
            pMonster->Set_MonsterType(_eMonsterType);
            m_vecTestMonster.push_back(pMonster);
        }
        break;
        }
    }

    return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_MapToolBuilding(const _tchar* _pLayerTags, CTestBuilding::BUILDING_TYPE _eBuildingType, _uint _iLoadedDataTypeIndex, void* _pBuildingArg)
{
    if (_iLoadedDataTypeIndex == 1)
    {
        switch (_eBuildingType)
        {
        case CTestBuilding::BUILDING_TYPE::BT_WALL:
        {
            CTestWall::TEST_WALL_DESC WallDesc = {};
            WallDesc.vBigWallPos = m_fPickPos;
            WallDesc.vBigWallRotation = m_fRotation;
            WallDesc.vBigWallScale = m_fScale;
            WallDesc.eWallType = CTestWall::WT_BASIC;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestBigWall"), LEVEL_MAPTOOL, _pLayerTags, &WallDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestWall* pWall = dynamic_cast<CTestWall*>(pListGameObject->back());
            pWall->Set_BuildingType(_eBuildingType);
            m_vecTestBuilding.push_back(pWall);
        }
        break;
        case CTestBuilding::BUILDING_TYPE::BT_BIGWALL:
        {
            CTestWall::TEST_WALL_DESC WallDesc = {};
            WallDesc.vBigWallPos = m_fPickPos;
            WallDesc.vBigWallRotation = m_fRotation;
            WallDesc.vBigWallScale = m_fScale;
            WallDesc.eWallType = CTestWall::WT_BIG;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestBigBigWall"), LEVEL_MAPTOOL, _pLayerTags, &WallDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestWall* pWall = dynamic_cast<CTestWall*>(pListGameObject->back());
            pWall->Set_BuildingType(_eBuildingType);
            m_vecTestBuilding.push_back(pWall);
        }
        break;

        case CTestBuilding::BUILDING_TYPE::BT_PILLAR:
        {
            CTestWall::TEST_WALL_DESC WallDesc = {};
            WallDesc.vBigWallPos = m_fPickPos;
            WallDesc.vBigWallRotation = m_fRotation;
            WallDesc.vBigWallScale = m_fScale;
            WallDesc.eWallType = CTestWall::WT_PILLAR;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestPillar"), LEVEL_MAPTOOL, _pLayerTags, &WallDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestWall* pWall = dynamic_cast<CTestWall*>(pListGameObject->back());
            pWall->Set_BuildingType(_eBuildingType);
            m_vecTestBuilding.push_back(pWall);
        }
        break;

        case CTestBuilding::BUILDING_TYPE::BT_CONCRETE:
        {
            CTestWall::TEST_WALL_DESC WallDesc = {};
            WallDesc.vBigWallPos = m_fPickPos;
            WallDesc.vBigWallRotation = m_fRotation;
            WallDesc.vBigWallScale = m_fScale;
            WallDesc.eWallType = CTestWall::WT_CONCRETE;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestConcrete"), LEVEL_MAPTOOL, _pLayerTags, &WallDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestWall* pWall = dynamic_cast<CTestWall*>(pListGameObject->back());
            pWall->Set_BuildingType(_eBuildingType);
            m_vecTestBuilding.push_back(pWall);
        }
        break;
        }
    }
    else if (_iLoadedDataTypeIndex == 2)
    {
        CTestWall::TEST_WALL_DESC* pLoadedDesc = static_cast<CTestWall::TEST_WALL_DESC*>(_pBuildingArg);
        if (!pLoadedDesc)
            return E_FAIL;

        switch (_eBuildingType)
        {
        case CTestBuilding::BUILDING_TYPE::BT_WALL:
        {
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestBigWall"), LEVEL_MAPTOOL, _pLayerTags, pLoadedDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestWall* pWall = dynamic_cast<CTestWall*>(pListGameObject->back());
            pWall->Set_BuildingType(_eBuildingType);
            m_vecTestBuilding.push_back(pWall);
        }
        break;

        case CTestBuilding::BUILDING_TYPE::BT_BIGWALL:
        {
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestBigBigWall"), LEVEL_MAPTOOL, _pLayerTags, pLoadedDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestWall* pWall = dynamic_cast<CTestWall*>(pListGameObject->back());
            pWall->Set_BuildingType(_eBuildingType);
            m_vecTestBuilding.push_back(pWall);
        }
        break;

        case CTestBuilding::BUILDING_TYPE::BT_PILLAR:
        {
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestPillar"), LEVEL_MAPTOOL, _pLayerTags, pLoadedDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestWall* pWall = dynamic_cast<CTestWall*>(pListGameObject->back());
            pWall->Set_BuildingType(_eBuildingType);
            m_vecTestBuilding.push_back(pWall);
        }
        break;

        case CTestBuilding::BUILDING_TYPE::BT_CONCRETE:
        {
            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestConcrete"), LEVEL_MAPTOOL, _pLayerTags, pLoadedDesc)))
                return E_FAIL;

            list<CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

            CTestWall* pWall = dynamic_cast<CTestWall*>(pListGameObject->back());
            pWall->Set_BuildingType(_eBuildingType);
            m_vecTestBuilding.push_back(pWall);
        }
        break;
        }
    }


    return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_TempCollider(const _tchar* _pLayerTags)
{
    CTempCollider::TEMP_COLLIDER_DESC TempColliderDesc = {};
    TempColliderDesc.vTempColliderpos = m_fPickPos;
    TempColliderDesc.vTempColliderScale = m_fScale;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TempCollider"), LEVEL_MAPTOOL, _pLayerTags, &TempColliderDesc)))
        return E_FAIL;

    list< CGameObject*>* pListGameObject = m_pGameInstance->Get_GameObjectList(LEVEL_MAPTOOL, _pLayerTags);

    if (!pListGameObject->empty())
    {
        CTempCollider* pTempCollider = dynamic_cast<CTempCollider*>(pListGameObject->back());
        m_vecTempCollider.push_back(pTempCollider);
    }

    return S_OK;
}

void CLevel_MapTool::Update(_float _fTimeDelta)
{
    // Imgui Window 창에, Mouser Cursor 있는지 없는지 판단해주는 함수 : IsWindowHovered
    if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) || ImGui::IsAnyItemHovered())
        m_bImGuiHovered = true;
    else
        m_bImGuiHovered = false;

#pragma region Save 기능
    if (m_pGameInstance->Get_DIKeyState(DIK_V) & 0x80)
    {
        //Save_HeightMap();
        return;
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_B) & 0x80)
    {
        //OpenFileDialoge();
        //Load_WholeData();
        return;
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_M) & 0x80)
    {
        //Save_MonsterData();
        return;
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_U) & 0x80)
    {
        //Save_BuildingData();
        return;
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_I) & 0x80)
    {
        m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOGO));
        return;
    }

    //Save_Navi();
#pragma endregion

    static int m_iPickingType = PICKING_TYPE::PT_MONSTER;

    ImGui::Begin("Picking Type");
    if (ImGui::RadioButton("Monster Picking", &m_iPickingType, PICKING_TYPE::PT_MONSTER))
    {
        m_bPickingMonsterActiveOn = true;
        m_bPickingBuildingActiveOn = false;
    }
    if (ImGui::RadioButton("Building Picking", &m_iPickingType, PICKING_TYPE::PT_BUILDING))
    {
        m_bPickingMonsterActiveOn = false;
        m_bPickingBuildingActiveOn = true;
    }
    if (ImGui::RadioButton("Brush Texture", &m_iPickingType, PICKING_TYPE::PT_SPLATTING_TEXTURE))
    {
    }
    if (ImGui::RadioButton("Raise Terrain", &m_iPickingType, PICKING_TYPE::PT_TERRAIN_PART))
    {
    }
    if (ImGui::RadioButton("Choose Diffuse", &m_iPickingType, PICKING_TYPE::PT_CHOOSE_DIFFUSETEXTURE))
    {
    }
    if (ImGui::RadioButton("Choose Mask", &m_iPickingType, PICKING_TYPE::PT_CHOOSE_MASKTEXTURE))
    {
    }
    if (ImGui::RadioButton("Create Cell", &m_iPickingType, PICKING_TYPE::PT_CELL))
    {
        m_bPickingMonsterActiveOn = false;
        m_bPickingBuildingActiveOn = false;
    }
    if (ImGui::RadioButton("Collider Picking", &m_iPickingType, PICKING_TYPE::PT_COLLIDER))
    {
        m_bPickingMonsterActiveOn = false;
        m_bPickingBuildingActiveOn = false;
    }
    ImGui::End();

    if (m_iPickingType == PICKING_TYPE::PT_TERRAIN_PART)
    {
        if (ImGui::Button("Save Terrain"))
        {
            if (FAILED(Save_HeightMap()))
            {
                ImGui::Text("Failed To Save Terrain Data!!");
            }
            else
            {
                ImGui::Text("Succed To Save Terrain Data!");
            }
        }


    }

    if (m_iPickingType == PICKING_TYPE::PT_MONSTER)
    {
        if (ImGui::Button("Save Monster"))
        {
            if (FAILED(Save_MonsterData()))
            {
                ImGui::Text("Failed To Save MonsterData!!");
            }
            else
            {
                ImGui::Text("Succed To Save Monster Data!");
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Load Monster"))
        {
            if (FAILED(Load_MonsterData()))
            {
                ImGui::Text("Failed To Load Monster Data !");
            }
            else
            {
                ImGui::Text("Succeded To Load Monster Data !");
            }
        }
    }

    if (m_iPickingType == PICKING_TYPE::PT_BUILDING)
    {
        if (ImGui::Button("Save Building"))
        {
            if (FAILED(Save_BuildingData()))
            {
                ImGui::Text("Failed To Save Building Data!!");
            }
            else
            {
                ImGui::Text("Succed To Save Building Data!");
            }
        }
        ImGui::SameLine();

        if (ImGui::Button("Load Building"))
        {
            if (FAILED(Load_BuildingData()))
            {
                ImGui::Text("Failed To Load Building Data !");
            }
            else
            {
                ImGui::Text("Succeded To Load Building Data !");
            }
        }
    }

    if (m_iPickingType == PICKING_TYPE::PT_CELL)
    {
        if (ImGui::Button("Save Navi"))
        {
            if (FAILED(Save_Navi()))
            {
                ImGui::Text("Failed To Save Navigation Data!!");
            }
            else
            {
                ImGui::Text("Succed To Save Navigation Data!");
            }
        }
        ImGui::SameLine();

        if (ImGui::Button("Load Navi"))
        {
            if (FAILED(Load_Navi()))
            {
                ImGui::Text("Failed To Load NaviData !");
            }
            else
            {
                ImGui::Text("Succeded To Load NaviData !");
            }
        }
    }


    ImGui::Begin("Object List");

    if (m_iPickingType == PICKING_TYPE::PT_MONSTER)
    {
        Setting_MonsterList();
    }
    else if (m_iPickingType == PICKING_TYPE::PT_BUILDING)
    {
        Setting_BuildingList();
    }
    else if (m_iPickingType == PICKING_TYPE::PT_TERRAIN_PART)
    {
        ImGui::Text("Terrain Editing Mode");
    }
    else if (m_iPickingType == PICKING_TYPE::PT_SPLATTING_TEXTURE)
    {
        ImGui::Text("Splatting Texture Mode");
    }
    else if (m_iPickingType == PICKING_TYPE::PT_CHOOSE_DIFFUSETEXTURE)
    {
        Setting_DiffuseImages();
    }
    else if (m_iPickingType == PICKING_TYPE::PT_CHOOSE_MASKTEXTURE)
    {
        Setting_MaskImages();
    }

    ImGui::End();

    if (!m_bImGuiHovered)
    {
        if (m_iPickingType == PICKING_TYPE::PT_MONSTER)
        {
            m_pGameInstance->Click_Once([&]() { Pick_Monster(PICKING_TYPE::PT_MONSTER); });
        }
        else if (m_iPickingType == PICKING_TYPE::PT_BUILDING)
        {
            m_pGameInstance->Click_Once([&]() { Pick_Building(PICKING_TYPE::PT_BUILDING); });
        }
        else if (m_iPickingType == PICKING_TYPE::PT_SPLATTING_TEXTURE)
        {
            Show_MouseRange(PICKING_TYPE::PT_SPLATTING_TEXTURE, _fTimeDelta);
        }
        else if (m_iPickingType == PICKING_TYPE::PT_TERRAIN_PART)
        {
            Show_MouseRange(PICKING_TYPE::PT_TERRAIN_PART, _fTimeDelta);
        }
        else if (m_iPickingType == PICKING_TYPE::PT_CHOOSE_DIFFUSETEXTURE)
        {
            Select_TerrainTexture(m_iDiffuseImageFirstIndex);
        }
        else if (m_iPickingType == PICKING_TYPE::PT_CHOOSE_MASKTEXTURE)
        {
            Adjust_MaskImage();
        }
        else if (m_iPickingType == PICKING_TYPE::PT_CELL)
        {
            m_pGameInstance->Click_Once([&]() {Picking_Points(); });
        }
    }

#ifdef _DEBUG 
    ImGui::Begin("Device Settings", NULL, ImGuiWindowFlags_MenuBar);
    ImGui::SliderFloat("Mouse Range", &(m_fRange), 0.0f, 10.0f);
    ImGui::End();
#endif
    if (m_vecTestMonster.size() > 0)
        Show_MonsterList();
    if (m_vecTestBuilding.size() > 0)
        Show_BuildingList();

    Delete_Cell_Mode();
}

HRESULT CLevel_MapTool::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("맵 툴 레벨입니다."));
#endif

    return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_MapToolTerrain(const _tchar* _pLayerTags)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_MapToolTerrain"), LEVEL_MAPTOOL, _pLayerTags, nullptr)))
    {
        return E_FAIL;
    }

    return S_OK;
}


HRESULT CLevel_MapTool::Ready_Light(const _tchar* _pLayerTags)
{
    LIGHT_DESC          tagDesc{};

    ZeroMemory(&tagDesc, sizeof tagDesc);

    tagDesc.eState = LIGHT_DESC::LT_DIRECTIONAL;

    tagDesc.vDirection = _float4(1.0f, -1.0f, 1.0f, 0.0f);

    tagDesc.vDiffuse = _float4(1.0f, 1.0f, 1.0f, 1.0f);

    tagDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.0f);

    tagDesc.vSpecular = _float4(1.0f, 1.0f, 1.0f, 1.0f);

    if (FAILED(m_pGameInstance->Add_Light(tagDesc)))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_MapTool::Ready_Layer_MapToolCamera(const _tchar* _pLayerTags)
{
    CCamera_MapTool::CAMERA_MAPTOOL_DESC Desc = {};

    Desc._vEye = _float3(0.0f, 10.0f, -7.0f);
    Desc._vAt = _float3(0.0f, 0.0f, 0.0f);

    Desc.fFov = XMConvertToRadians(60.0f);
    Desc.fNear = 0.1f;
    Desc.fFar = 1000.0f;
    Desc.fMouseSensity = 0.1f;
    Desc.fSpeedPerSec = 10.0f;
    Desc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_Camera_MapTool"), LEVEL_MAPTOOL, _pLayerTags, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_MapTool::Register_PreviewImage_In_ImGuiWindow(const _tchar* _pPreviewImageFilePath, IMGUI_TEXTURE_TYPE _eImguiTextureType, _uint _iTextureNumber)
{
    for (_uint i = 0; i < _iTextureNumber; ++i)
    {
        _tchar      szEXT[MAX_PATH] = {};

        _wsplitpath_s(_pPreviewImageFilePath, nullptr, 0, nullptr, 0, nullptr, 0, szEXT, MAX_PATH);

        for (_uint i = 0; i < _iTextureNumber; ++i)
        {
            ID3D11Texture2D* pTexture2D = { nullptr };
            ID3D11ShaderResourceView* pSRV = { nullptr };

            _tchar                      szTextureFilePath[MAX_PATH] = TEXT("");

            wsprintf(szTextureFilePath, _pPreviewImageFilePath, i);

            HRESULT     hr = {};

            if (false == lstrcmp(szEXT, TEXT(".dds")))
            {
                hr = CreateDDSTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
            }
            else if (false == lstrcmp(szEXT, TEXT(".tga")))
            {
                hr = E_FAIL;
            }
            else
            {
                hr = CreateWICTextureFromFile(m_pDevice, szTextureFilePath, nullptr, &pSRV);
            }

            if (FAILED(hr))
                return E_FAIL;

            switch (_eImguiTextureType)
            {
            case IMG_MONSTER:
                m_vecMonsterSRVs.push_back(pSRV);
                break;
            case IMG_BUILDING:
                m_vecBuildingSRVs.push_back(pSRV);
                break;
            case IMG_DIFFUSE:
                m_vecDiffuseImageSRVs.push_back(pSRV);
                break;
            case IMG_MASK:
                m_vecMaksImageSRVs.push_back(pSRV);
                break;
            }
        }
    }
    return S_OK;
}

void CLevel_MapTool::Setting_MonsterList()
{
    if (ImGui::CollapsingHeader("Monster List"))
        return;

    const char* items[] = { "Monster List" };

    static int nCurrentItem = 0;
    ImGui::Combo("##3", &nCurrentItem, items, IM_ARRAYSIZE(items));

    for (_int i = 0; i < 9; ++i)
    {
        int textureIndex = nCurrentItem * 3 + i;

        if (textureIndex < m_vecMonsterSRVs.size())
        {
            if (ImGui::ImageButton(("Monster" + std::to_string(textureIndex)).c_str(), m_vecMonsterSRVs[textureIndex], ImVec2(50.0f, 50.0f)))
            {
                m_iMonsterTypeIndex = textureIndex;
            }

            if ((i + 1) % 3 != 0)
            {
                ImGui::SameLine();
            }
        }
    }
}

void CLevel_MapTool::Setting_BuildingList()
{
    if (ImGui::CollapsingHeader("Building List"))
        return;

    const char* items[] = { "Building List" };

    static int nCurrentItem = 0;
    ImGui::Combo("##2", &nCurrentItem, items, IM_ARRAYSIZE(items));

    for (_int i = 0; i < 4; ++i)
    {
        int textureIndex = nCurrentItem * 3 + i;

        if (textureIndex < m_vecBuildingSRVs.size())
        {
            if (ImGui::ImageButton(("Wall" + std::to_string(textureIndex)).c_str(), m_vecBuildingSRVs[textureIndex], ImVec2(50.0f, 50.0f)))
            {
                m_iBuildingTypeIndex = textureIndex;
            }

            if ((i + 1) % 3 != 0)
            {
                ImGui::SameLine();
            }
        }
    }
}

void CLevel_MapTool::Setting_MaskImages()
{
    if (ImGui::CollapsingHeader("MaskImage List"))
        return;

    const char* items[] = { "MaskImage List" };

    static int nCurrentItem = 0;
    ImGui::Combo("##4", &nCurrentItem, items, IM_ARRAYSIZE(items));

    for (_int i = 0; i < 2; ++i)
    {
        int textureIndex = nCurrentItem * 3 + i;

        if (textureIndex < m_vecMaksImageSRVs.size())
        {
            if (ImGui::ImageButton(("MaskImages" + std::to_string(textureIndex)).c_str(), m_vecMaksImageSRVs[textureIndex], ImVec2(50.0f, 50.0f)))
            {
                m_iMaskImageIndex = textureIndex;
            }

            if ((i + 1) % 3 != 0)
            {
                ImGui::SameLine();
            }
        }
    }
}

void CLevel_MapTool::Setting_DiffuseImages()
{
    if (ImGui::CollapsingHeader("DiffuseImage List"))
        return;

    const char* items[] = { "DiffuseImage List" };

    static int nCurrentItem = 0;
    ImGui::Combo("##4", &nCurrentItem, items, IM_ARRAYSIZE(items));

    for (_int i = 0; i < 7; ++i)
    {
        int textureIndex = nCurrentItem * 3 + i;

        if (textureIndex < m_vecDiffuseImageSRVs.size())
        {
            if (ImGui::ImageButton(("DiffuseImages" + std::to_string(textureIndex)).c_str(), m_vecDiffuseImageSRVs[textureIndex], ImVec2(50.0f, 50.0f)))
            {
                m_iDiffuseImageFirstIndex = textureIndex;
            }

            if ((i + 1) % 3 != 0)
            {
                ImGui::SameLine();
            }
        }
    }

}

HRESULT CLevel_MapTool::Picking_Points()
{
    m_fPickPos = m_pMapToolCamera->Picking_Terrain(g_hWnd, static_cast<CVIBuffer_MapToolTerrain*>(m_pMapToolTerrain->Find_Component(TEXT("Com_VITerrainBuffer_MapTool"))));

    if (m_fPickPos.y == -0.5f)
        return S_OK;

    m_fPickPos.y += 0.1f;

    if (m_bDeleteMode)
    {
        m_vecPickedPoints.push_back(m_fPickPos);
        return S_OK;
    }

    if (FAILED(Ready_Layer_TempCollider(TEXT("Layer_MapTool_TempCollider"))))
        return E_FAIL;

    m_vecPickedPoints.push_back(m_fPickPos);

    if (m_vecPickedPoints.size() == 3 && bFirstPick)
    {
        tagWholeCellPoints.fCellPoints[0] = m_vecPickedPoints[0];
        tagWholeCellPoints.fCellPoints[1] = m_vecPickedPoints[1];
        tagWholeCellPoints.fCellPoints[2] = m_vecPickedPoints[2];

        m_pNavigation->Create_Cell(tagWholeCellPoints.fCellPoints);

        m_vecWholeCellPoints.push_back(tagWholeCellPoints);

        tagWholeCellPoints.fPrevPoints[0] = tagWholeCellPoints.fCellPoints[0];
        tagWholeCellPoints.fPrevPoints[1] = tagWholeCellPoints.fCellPoints[1];

        m_iNumCellCount++;
        bFirstPick = false;

        m_vecPickedPoints.clear();
    }
    else if (m_vecPickedPoints.size() < 3 && bFirstPick == false)
    {
        // m_vecPickedPoints[0] 가 이제 고정적으로, 내가 "새로" 찍는 한 점을 의미함.
        // m_vecWholeCellPoints 에 있는 모든 cell points 들이랑 계산해서 나온 가장 가까운 두점이 
        // Compute_NearPoints 라는 <xmfloat3, xmfloat3> 라고 pair 타입으로 담고 있는 NearPoints로 처리함.
        auto NearPoints = Compute_NearPoints(m_vecWholeCellPoints, m_vecPickedPoints[0]);

        // 이제 내가 딱 새로 찍은 점하고 NearPoints의 첫 번째 원소 랑 두 번째 원소가, 어우러져서 Cell을 만듬!! 
        tagWholeCellPoints.fCellPoints[0] = NearPoints.first;
        tagWholeCellPoints.fCellPoints[1] = NearPoints.second;
        tagWholeCellPoints.fCellPoints[2] = m_vecPickedPoints[0];

        // 그리고 그 어우러져 만들어진 Cell 이 시계방향으로 된건지 아닌지, IsCWTriangle 함수로 판단하자.
        // vNewCellpoint1 가 내가 찍은 점과 가장 가까운 점의 첫 번째 원소의 point.
        // vNewCellpoint2 가 내가 찍은 점과 가장 가까운 점의 두 번째 원소의 point.
        // vNewCellpoint3 가 내가 찍은 점 point.
        XMVECTOR vNewCellpoint1 = XMLoadFloat3(&tagWholeCellPoints.fCellPoints[0]);
        XMVECTOR vNewCellpoint2 = XMLoadFloat3(&tagWholeCellPoints.fCellPoints[1]);
        XMVECTOR vNewCellpoint3 = XMLoadFloat3(&tagWholeCellPoints.fCellPoints[2]);

        if (Is_CWTriangle(vNewCellpoint1, vNewCellpoint2, vNewCellpoint3))
        {
            // 강제적으로 그냥 순서 바꿔버림ㅋㅋ 이거안하면 겹쳐짐. 
            // 시발 아무리 해도 찍다보면 [2] - [0] - [1] 해야하는데 씨발놈이, [2] - [1] - [0] 이렇게 되버림;;
            // 그래서 바꿔버림 여기서 강제적으로 위 순서로 되게 와 시발 이러니까 되대?
            swap(tagWholeCellPoints.fCellPoints[1], tagWholeCellPoints.fCellPoints[2]);

            m_pNavigation->Create_Cell(tagWholeCellPoints.fCellPoints);

            m_vecWholeCellPoints.push_back(tagWholeCellPoints);

            tagWholeCellPoints.fPrevPoints[0] = tagWholeCellPoints.fCellPoints[1];
            tagWholeCellPoints.fPrevPoints[1] = tagWholeCellPoints.fCellPoints[2];

            m_iNumCellCount++;
        }
        m_vecPickedPoints.clear();
    }

    return S_OK;
}

_float CLevel_MapTool::Compute_Cell_Distance(const XMFLOAT3& _NewPickingPoint, const XMFLOAT3& _PrevPickedPoint)
{
    // 루트 (  x^ 2 + y^ 2 + z^ 2 ) 내가 찍은 점 xyz 랑 기존의 점 xyz 의 거리를 구하는 식임 ㅇㅅㅇ
    return sqrtf(powf(_PrevPickedPoint.x - _NewPickingPoint.x, 2) + powf(_PrevPickedPoint.y - _NewPickingPoint.y, 2) + powf(_PrevPickedPoint.z - _NewPickingPoint.z, 2));
}

pair<XMFLOAT3, XMFLOAT3> CLevel_MapTool::Compute_NearPoints(const vector<CELL_POINTS>& _vecTagCells, const XMFLOAT3& _newPoints)
{
    _float fMinDistance = FLT_MAX;
    pair<XMFLOAT3, XMFLOAT3> NearestPoints;

    for (const auto& cell : _vecTagCells)
    {
        for (_uint i = 0; i < 3; ++i)
        {
            for (_uint j = i + 1; j < 3; ++j)
            {
                // fCellPoints[0] 하고 fCellPoints[1] 하고 각각의 거리 fDist1, fDist2 를 구해가지고,
                _float fDist1 = Compute_Cell_Distance(_newPoints, cell.fCellPoints[i]);
                _float fDist2 = Compute_Cell_Distance(_newPoints, cell.fCellPoints[j]);

                // 만약에 처음 fMinDistance가 float_max로 되어있는데, 그거보다 작으면, 
                // 이제는 지금 찍은 점과 기존의 두 점의 dist가 최소가 된다. 
                // 그리고 가장 근접한 두 점은 이 때의 cell.fCellpoints[i] 와 cell.fcellpoints[j]가 된다. 
                // cell.fCellPoints[0] ~~ cell.fCellPoints[1],cell.fCellPoints[2] 랑.
                // cell.fCellPoints[1] ~ cell.fCellPoints[2] 랑,
                // cell.fCellPoints[2] ~ 는 없음. 
                // 3번 검사해서, 
                if (fDist1 + fDist2 < fMinDistance)
                {
                    fMinDistance = fDist1 + fDist2;
                    NearestPoints = { cell.fCellPoints[i], cell.fCellPoints[j] };
                }
            }
        }
    }
    return NearestPoints;
}

//pair<XMFLOAT3, _bool> CLevel_MapTool::Find_NearestPoint(const vector<CELL_POINTS>& _vecTagCells, const XMFLOAT3& _newPoint)
//{
//    _float fMinDistance = FLT_MAX;
//    XMFLOAT3 nearestPoint;
//    bool found = false;
//
//    for (const auto& cell : _vecTagCells)
//    {
//        for (_uint i = 0; i < 3; ++i)
//        {
//            _float fDist = Compute_Cell_Distance(_newPoint, cell.fCellPoints[i]);
//
//            if (fDist < fMinDistance)
//            {
//                fMinDistance = fDist;
//                nearestPoint = cell.fCellPoints[i];
//                found = true;
//            }
//        }
//    }
//    return { nearestPoint, found };
//}

_bool CLevel_MapTool::Is_CWTriangle(const XMVECTOR& _NearestCellPoint1, const XMVECTOR& _NearestCellPoint2, const XMVECTOR& _PickedNewPoint)
{
    // 가장 가까운 point 1 과 point 2의 방향벡터! 
    XMVECTOR vDir1 = XMVectorSubtract(_NearestCellPoint2, _NearestCellPoint1);

    // 내가 찍은 점하고 point1 하고의 방향벡터.
    XMVECTOR vDir2 = XMVectorSubtract(_PickedNewPoint, _NearestCellPoint1);

    // 이제 그 둘의 방향벡터를 외적해서 뽑아내내 결과값 하나가 0 보다 크면 삼각형이 된거임. 
    // 0보다 작으면 삼각형이 안되는거임 말이안됨ㅋㅋ
    // Cell 클래스의 IsIn 함수 좀 참고했음
    _float fTriangleValue = XMVectorGetX(XMVector3Length(XMVector3Cross(vDir1, vDir2)));

    // 외적하고나고나서 값을 보는데, 만약에 0 이면, 삼각형을 만드는데 필요한 세 점이 한 선에 있거나 그런거임
    // 그래서 무적권 양수가 되야 삼각형이 되었구나 라고 알 수 있셈.
    if (fTriangleValue > 0.0f)
        return true;
    else
        return false;
}

_bool CLevel_MapTool::Is_Point_InTriangle(const XMVECTOR& _Point, const XMVECTOR& _VertexPoint0, const XMVECTOR& _VertexPoint1, const XMVECTOR& _VertexPoint2)
{
    //내적 ( 외적 ( ab, at ) , 외적 ( at, ac ) ) > 0 
    //[출처] - 삼각형 안에 점이 있는지 확인하는 방법 | 작성자 콜라곰

    XMVECTOR v0v1 = XMVectorSubtract(_VertexPoint1, _VertexPoint0);
    XMVECTOR v1v2 = XMVectorSubtract(_VertexPoint2, _VertexPoint1);
    XMVECTOR v2v0 = XMVectorSubtract(_VertexPoint0, _VertexPoint2);

    XMVECTOR v0p = XMVectorSubtract(_Point, _VertexPoint0);
    XMVECTOR v1p = XMVectorSubtract(_Point, _VertexPoint1);
    XMVECTOR v2p = XMVectorSubtract(_Point, _VertexPoint2);

    XMVECTOR vCrossValue0 = XMVector3Cross(v0v1, v0p);
    XMVECTOR vCrossValue1 = XMVector3Cross(v1v2, v1p);
    XMVECTOR vCrossValue2 = XMVector3Cross(v2v0, v2p);

    XMVECTOR vNormal = XMVector3Cross(v0v1, XMVectorSubtract(_VertexPoint2, _VertexPoint0));

    _float  fDotValue0 = XMVectorGetX(XMVector3Dot(vCrossValue0, vNormal));
    _float  fDotValue1 = XMVectorGetX(XMVector3Dot(vCrossValue1, vNormal));
    _float  fDotValue2 = XMVectorGetX(XMVector3Dot(vCrossValue2, vNormal));

    return (fDotValue0 >= 0.0f && fDotValue1 >= 0.0f && fDotValue2 >= 0.0f);
}

HRESULT CLevel_MapTool::Delete_Cell()
{
    if (m_vecPickedPoints.empty())
        return E_FAIL;

    XMFLOAT3 vPickedPoint = m_vecPickedPoints[0];

    for (auto iter = m_vecWholeCellPoints.begin(); iter != m_vecWholeCellPoints.end(); ++iter)
    {
        const auto& pCell = *iter;

        if (Is_Point_InTriangle(XMLoadFloat3(&vPickedPoint), XMLoadFloat3(&pCell.fCellPoints[0]), XMLoadFloat3(&pCell.fCellPoints[1]), XMLoadFloat3(&pCell.fCellPoints[2])))
        {
            m_pNavigation->Delete_Cell(pCell.fCellPoints);

            m_vecWholeCellPoints.erase(iter);

            m_iNumCellCount--;
            return S_OK;
        }
    }

    return E_FAIL;
}

HRESULT CLevel_MapTool::Delete_Cell_Mode()
{
    if (m_pGameInstance->Get_DIKeyState(DIK_F) & 0x80)
    {
        m_bDeleteMode = true;

        if (!m_vecPickedPoints.empty())
        {
            Delete_Cell();

            if (!m_vecTempCollider.empty())
            {
                CGameObject* pTempCollider = m_vecTempCollider.back();
                if (nullptr != pTempCollider)
                {
                    m_pGameInstance->Delete_GameObject(LEVEL_MAPTOOL, TEXT("Layer_MapTool_TempCollider"), pTempCollider);
                    m_vecTempCollider.pop_back();
                }
            }
            m_vecPickedPoints.clear();
        }
    }
    else
    {
        m_bDeleteMode = false;
    }

    return S_OK;
}


HRESULT CLevel_MapTool::Save_Navi()
{
#pragma region 네비저장 수정(다른이름으로저장으로)
    wstring fileName;
    OpenFileDialoge(L"NavigationData.txt", L"Text Files\0*.TXT\0All Files\0*.*\0", fileName);
    if (fileName.empty())
        return E_FAIL;


    HANDLE hFile = CreateFile(fileName.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX("Failed To Create NavigationData File!");
        return E_FAIL;
    }

    DWORD dwByte = 0;
    WriteFile(hFile, &m_iNumCellCount, sizeof(_uint), &dwByte, nullptr);

    for (auto& pCellPoints : m_vecWholeCellPoints)
        WriteFile(hFile, pCellPoints.fCellPoints, sizeof(_float3) * 3, &dwByte, nullptr);

    // m_bFirstPick 상태 저장
    //WriteFile(hFile, &bFirstPick, sizeof(bool), &dwByte, nullptr);

    MSG_BOX("Success Save");

    CloseHandle(hFile);

    return S_OK;
#pragma endregion
}

HRESULT CLevel_MapTool::Load_MonsterData()
{
    wstring fileName;
    OpenFileDialoge(L"Select Monster Data", L"Text Files\0*.TXT\0All Files\0*.*\0", fileName);

    if (fileName.empty())
    {
        MSG_BOX("No file selected!");
        return E_FAIL;
    }

    wcout << L"Selected file: " << fileName << endl;


    for (auto& pMonster : m_vecTestMonster)
    {
        if (pMonster)
        {
            m_pGameInstance->Delete_GameObject(LEVEL_MAPTOOL, TEXT("Layer_MapTool_Monster"), pMonster);

            Safe_Release(pMonster);
            pMonster = nullptr;
        }
    }
    m_vecTestMonster.clear();
    m_vecCheckPosition.clear();


    CLoad_Manager::GetInstance()->Load_Monster(fileName);

    MSG_BOX("Monster Data Load Success!!");

    _uint iMonsterCount = CLoad_Manager::GetInstance()->Get_MonsterCount();
    for (_uint i = 0; i < iMonsterCount; ++i)
    {
        CMapToolMonster::MONSTER_TYPE eMonsterType = static_cast<CMapToolMonster::MONSTER_TYPE>(CLoad_Manager::GetInstance()->Get_MonsterType()[i]);
        _float3 vPosition = CLoad_Manager::GetInstance()->Get_MonsterPosition()[i];
        _float3 vRotation = CLoad_Manager::GetInstance()->Get_MonsterRotation()[i];
        _float3 vScale = CLoad_Manager::GetInstance()->Get_MonsterScale()[i];

        switch (eMonsterType)
        {
        case CMapToolMonster::MT_ROBO_L:
        {
            CMapToolMonster::MAPTOOL_MONSTER_DESC MonsterDesc = {};
            MonsterDesc.vMonsterpos = vPosition;
            MonsterDesc.vMonsterRotation = vRotation;
            MonsterDesc.vMonsterScale = vScale;

            if (FAILED(Ready_Layer_MapToolMonster(TEXT("Layer_MapTool_Monster"), eMonsterType, 2, &MonsterDesc)))
            {
                MSG_BOX("Failed to Robo_Boom!");
                return E_FAIL;
            }
        }
        break;

        case CMapToolMonster::MT_ROBO_B:
        {
            CMapToolMonster::MAPTOOL_MONSTER_DESC MonsterDesc = {};
            MonsterDesc.vMonsterpos = vPosition;
            MonsterDesc.vMonsterRotation = vRotation;
            MonsterDesc.vMonsterScale = vScale;

            if (FAILED(Ready_Layer_MapToolMonster(TEXT("Layer_MapTool_Monster"), eMonsterType, 2, &MonsterDesc)))
            {
                MSG_BOX("Failed to Robo_Billy!");
                return E_FAIL;
            }
        }
        break;

        case CMapToolMonster::MT_ROBO_G:
        {
            CMapToolMonster::MAPTOOL_MONSTER_DESC MonsterDesc = {};
            MonsterDesc.vMonsterpos = vPosition;
            MonsterDesc.vMonsterRotation = vRotation;
            MonsterDesc.vMonsterScale = vScale;

            if (FAILED(Ready_Layer_MapToolMonster(TEXT("Layer_MapTool_Monster"), eMonsterType, 2, &MonsterDesc)))
            {
                MSG_BOX("Failed To Robo_Goliath!");
                return E_FAIL;
            }
        }
        break;

        case CMapToolMonster::MT_ROBO_V:
        {
            CMapToolMonster::MAPTOOL_MONSTER_DESC MonsterDesc = {};
            MonsterDesc.vMonsterpos = vPosition;
            MonsterDesc.vMonsterRotation = vRotation;
            MonsterDesc.vMonsterScale = vScale;

            if (FAILED(Ready_Layer_MapToolMonster(TEXT("Layer_MapTool_Monster"), eMonsterType, 2, &MonsterDesc)))
            {
                MSG_BOX("Failed To Robo_Larva!");
                return E_FAIL;
            }
        }
        break;

        case CMapToolMonster::MT_ROBO_S:
        {
            CMapToolMonster::MAPTOOL_MONSTER_DESC MonsterDesc = {};
            MonsterDesc.vMonsterpos = vPosition;
            MonsterDesc.vMonsterRotation = vRotation;
            MonsterDesc.vMonsterScale = vScale;

            if (FAILED(Ready_Layer_MapToolMonster(TEXT("Layer_MapTool_Monster"), eMonsterType, 2, &MonsterDesc)))
            {
                MSG_BOX("Failed To Robo_Small!");
                return E_FAIL;
            }
        }
        break;

        case CMapToolMonster::MT_DOOR:
        {
            CMapToolMonster::MAPTOOL_MONSTER_DESC MonsterDesc = {};
            MonsterDesc.vMonsterpos = vPosition;
            MonsterDesc.vMonsterRotation = vRotation;
            MonsterDesc.vMonsterScale = vScale;

            if (FAILED(Ready_Layer_MapToolMonster(TEXT("Layer_MapTool_Monster"), eMonsterType, 2, &MonsterDesc)))
            {
                MSG_BOX("Failed To Robo_Door!");
                return E_FAIL;
            }
        }
        break;

        case CMapToolMonster::MT_ROBO_BOSS:
        {
            CMapToolMonster::MAPTOOL_MONSTER_DESC MonsterDesc = {};
            MonsterDesc.vMonsterpos = vPosition;
            MonsterDesc.vMonsterRotation = vRotation;
            MonsterDesc.vMonsterScale = vScale;

            if (FAILED(Ready_Layer_MapToolMonster(TEXT("Layer_MapTool_Monster"), eMonsterType, 2, &MonsterDesc)))
            {
                MSG_BOX("Failed To Robo_Boss!");
                return E_FAIL;
            }
        }
        break;

        case CMapToolMonster::MT_ITEM_BOX:
        {
            CMapToolMonster::MAPTOOL_MONSTER_DESC MonsterDesc = {};
            MonsterDesc.vMonsterpos = vPosition;
            MonsterDesc.vMonsterRotation = vRotation;
            MonsterDesc.vMonsterScale = vScale;

            if (FAILED(Ready_Layer_MapToolMonster(TEXT("Layer_MapTool_Monster"), eMonsterType, 2, &MonsterDesc)))
            {
                MSG_BOX("Failed To Item_Box!");
                return E_FAIL;
            }
        }
        break;

        case CMapToolMonster::MT_GATE:
        {
            CMapToolMonster::MAPTOOL_MONSTER_DESC MonsterDesc = {};
            MonsterDesc.vMonsterpos = vPosition;
            MonsterDesc.vMonsterRotation = vRotation;
            MonsterDesc.vMonsterScale = vScale;

            if (FAILED(Ready_Layer_MapToolMonster(TEXT("Layer_MapTool_Monster"), eMonsterType, 2, &MonsterDesc)))
            {
                MSG_BOX("Failed To Gate!");
                return E_FAIL;
            }
        }
        break;
        }
        m_vecCheckPosition.push_back(vPosition);
    }

    return S_OK;
}

HRESULT CLevel_MapTool::Load_BuildingData()
{
    wstring fileName;
    OpenFileDialoge(L"Select Building Data", L"Text Files\0*.TXT\0All Files\0*.*\0", fileName);

    if (fileName.empty())
    {
        MSG_BOX("No file selected!");
        return E_FAIL;
    }

    wcout << L"Selected file: " << fileName << endl;

    for (auto& pBuilding : m_vecTestBuilding)
    {
        if (pBuilding)
        {
            m_pGameInstance->Delete_GameObject(LEVEL_MAPTOOL, TEXT("Layer_MapTool_Building"), pBuilding);

            Safe_Release(pBuilding);
            pBuilding = nullptr;
        }
    }
    m_vecTestBuilding.clear();
    m_vecCheckBuildingPosition.clear();

    CLoad_Manager::GetInstance()->Load_Building(fileName);

    _uint iBuildingCount = CLoad_Manager::GetInstance()->Get_BuildingCount();
    if (iBuildingCount == 0)
    {
        MSG_BOX("Failed Load Building Data");
        return S_OK;
    }

    for (_uint i = 0; i < iBuildingCount; ++i)
    {
        CTestBuilding::BUILDING_TYPE eBuildingType = static_cast<CTestBuilding::BUILDING_TYPE>(CLoad_Manager::GetInstance()->Get_BuildingType()[i]);

        _float3 vPosition = CLoad_Manager::GetInstance()->Get_BuildingPosition()[i];
        _float3 vRotation = CLoad_Manager::GetInstance()->Get_BuildingRotation()[i];
        _float3 vScale = CLoad_Manager::GetInstance()->Get_BuildingScale()[i];

        switch (eBuildingType)
        {
        case CTestBuilding::BT_WALL:
        {
            CTestWall::TEST_WALL_DESC WallDesc = {};
            WallDesc.vBigWallPos = vPosition;
            WallDesc.vBigWallRotation = vRotation;
            WallDesc.vBigWallScale = vScale;
            WallDesc.eWallType = CTestWall::WT_BASIC;

            if (FAILED(Ready_Layer_MapToolBuilding(TEXT("Layer_MapTool_Building"), eBuildingType, 2, &WallDesc)))
            {
                MSG_BOX("Failed to Load Basic Wall!");
                return E_FAIL;
            }
        }
        break;

        case CTestBuilding::BT_BIGWALL:
        {
            CTestWall::TEST_WALL_DESC WallDesc = {};
            WallDesc.vBigWallPos = vPosition;
            WallDesc.vBigWallRotation = vRotation;
            WallDesc.vBigWallScale = vScale;
            WallDesc.eWallType = CTestWall::WT_BIG;

            if (FAILED(Ready_Layer_MapToolBuilding(TEXT("Layer_MapTool_Building"), eBuildingType, 2, &WallDesc)))
            {
                MSG_BOX("Failed to Load Big Wall!");
                return E_FAIL;
            }
        }
        break;

        case CTestBuilding::BT_PILLAR:
        {
            CTestWall::TEST_WALL_DESC WallDesc = {};
            WallDesc.vBigWallPos = vPosition;
            WallDesc.vBigWallRotation = vRotation;
            WallDesc.vBigWallScale = vScale;
            WallDesc.eWallType = CTestWall::WT_PILLAR;

            if (FAILED(Ready_Layer_MapToolBuilding(TEXT("Layer_MapTool_Building"), eBuildingType, 2, &WallDesc)))
            {
                MSG_BOX("Failed to Load Big Wall!");
                return E_FAIL;
            }
        }
        break;

        case CTestBuilding::BT_CONCRETE:
        {
            CTestWall::TEST_WALL_DESC WallDesc = {};
            WallDesc.vBigWallPos = vPosition;
            WallDesc.vBigWallRotation = vRotation;
            WallDesc.vBigWallScale = vScale;
            WallDesc.eWallType = CTestWall::WT_CONCRETE;

            if (FAILED(Ready_Layer_MapToolBuilding(TEXT("Layer_MapTool_Building"), eBuildingType, 2, &WallDesc)))
            {
                MSG_BOX("Failed to Load Big Wall!");
                return E_FAIL;
            }
        }
        break;
        }

        m_vecCheckBuildingPosition.push_back(vPosition);
    }

    MSG_BOX("Building Data Load Success");
    return S_OK;
#pragma endregion

    return S_OK;
}

HRESULT CLevel_MapTool::Load_Navi()
{
    wstring fileName;
    OpenFileDialoge(L"Select Navigation Data", L"Text Files\0*.TXT\0All Files\0*.*\0", fileName);

    if (fileName.empty())
    {
        MSG_BOX("No file selected!");
        return E_FAIL;
    }

    HANDLE hFile = CreateFile(fileName.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX("Failed To Open NavigationData File!");
        return E_FAIL;
    }

    DWORD dwByte = 0;
    _uint iLoadedCellCount = 0;

    ReadFile(hFile, &iLoadedCellCount, sizeof(_uint), &dwByte, nullptr);

    for (_uint i = 0; i < iLoadedCellCount; ++i)
    {
        CELL_POINTS cellPoints;
        ReadFile(hFile, cellPoints.fCellPoints, sizeof(_float3) * 3, &dwByte, nullptr);

        _bool bIsDuplicate = false;
        for (const auto& existingCell : m_vecWholeCellPoints)
        {
            if (memcmp(existingCell.fCellPoints, cellPoints.fCellPoints, sizeof(_float3) * 3) == 0)
            {
                bIsDuplicate = true;
                break;
            }
        }

        if (!bIsDuplicate)
        {
            m_pNavigation->Create_Cell(cellPoints.fCellPoints);
            m_vecWholeCellPoints.push_back(cellPoints);
            m_iNumCellCount++;
        }
    }

    CloseHandle(hFile);

    MSG_BOX("Navigation Data Loaded Successfully!");

    return S_OK;
}


void CLevel_MapTool::Show_MonsterList()
{
    if (m_bPickingBuildingActiveOn)
        return;

    if (ImGui::CollapsingHeader("Monster List"))
    {
        ImGui::Text("Placed Monsters:");

        for (size_t i = 0; i < m_vecCheckPosition.size(); ++i)
        {
            char szObjectName[128];

            CMapToolMonster* pMonster = dynamic_cast<CMapToolMonster*>(m_vecTestMonster[i]);
            if (nullptr != pMonster)
            {
                auto eMonsterType = pMonster->Get_MonsterType();
                if (eMonsterType == CMapToolMonster::MT_ROBO_L)
                    sprintf_s(szObjectName, "Nor_Monster #%zu", i + 1);
                if (eMonsterType == CMapToolMonster::MT_ROBO_B)
                    sprintf_s(szObjectName, "Boss_Monster #%zu", i + 1);
                if (eMonsterType == CMapToolMonster::MT_ROBO_G)
                    sprintf_s(szObjectName, "Goliath_Monster #%zu", i + 1);
                if (eMonsterType == CMapToolMonster::MT_ROBO_V)
                    sprintf_s(szObjectName, "Larva_Monster #%zu", i + 1);
                if (eMonsterType == CMapToolMonster::MT_ROBO_S)
                    sprintf_s(szObjectName, "Small_Monster #%zu", i + 1);
                if (eMonsterType == CMapToolMonster::MT_DOOR)
                    sprintf_s(szObjectName, "Door #%zu", i + 1);
                if (eMonsterType == CMapToolMonster::MT_ROBO_BOSS)
                    sprintf_s(szObjectName, "Boss_Monster #%zu", i + 1);
                if (eMonsterType == CMapToolMonster::MT_ITEM_BOX)
                    sprintf_s(szObjectName, "ItemBox #%zu", i + 1);
                if (eMonsterType == CMapToolMonster::MT_GATE)
                    sprintf_s(szObjectName, "Gate #%zu", i + 1);

            }

            if (ImGui::Button(szObjectName))
            {
                m_SelectedObjectIndex = static_cast<_uint>(i);
                ImGui::Text("Selected Monster Index: %d", m_SelectedObjectIndex);
            }

            ImGui::SameLine();
            if (ImGui::Button(("Delete##" + std::to_string(i)).c_str()))
            {
                m_vecCheckPosition.erase(m_vecCheckPosition.begin() + i);

                if (m_SelectedObjectIndex == static_cast<_uint>(i))
                    m_SelectedObjectIndex = -1;

                m_pMonsterGameObject = m_vecTestMonster[i];

                if (m_pMonsterGameObject)
                {
                    if (FAILED(m_pGameInstance->Delete_GameObject(LEVEL_MAPTOOL, TEXT("Layer_MapTool_Monster"), m_pMonsterGameObject)))
                    {
                        MSG_BOX("삭제 실패");
                    }
                    Safe_Release(m_pMonsterGameObject);
                }
                m_vecTestMonster.erase(m_vecTestMonster.begin() + i);

                if (m_SelectedObjectIndex == i)
                {
                    m_SelectedObjectIndex = -1;
                }
                else if (m_SelectedObjectIndex > i)
                {
                    m_SelectedObjectIndex--;
                }

                break;
            }
        }

        if (m_SelectedObjectIndex >= 0 && m_SelectedObjectIndex < m_vecTestMonster.size())
        {
            m_pMapToolMonster = dynamic_cast<CMapToolMonster*>(m_vecTestMonster[m_SelectedObjectIndex]);

            if (m_pMapToolMonster)
            {
                CTransform* pTransform = dynamic_cast<CTransform*>(m_pMapToolMonster->Find_Component(TEXT("Com_Transform")));

                if (pTransform)
                {
                    ImGui::Text("Selected Monster Properties:");

                    m_fRotation = m_pMapToolMonster->Get_TestMonsterRotation();

                    if (ImGui::SliderFloat3("Rotation (XYZ)", reinterpret_cast<_float*>(&m_fRotation), 0.0f, 20.0f))
                    {
                        m_pMapToolMonster->Set_TestMonsterRotation(m_fRotation);

                        pTransform->Rotation(m_fRotation.x, m_fRotation.y, m_fRotation.z);
                    }

                    m_fScale = pTransform->Compute_Scaled();
                    if (ImGui::SliderFloat3("Scale", reinterpret_cast<_float*>(&m_fScale), 1.0f, 5.0f))
                    {
                        m_pMapToolMonster->Set_TestMonsterScale(m_fScale);

                        pTransform->Scaling(m_fScale);
                    }

                    m_fTranslation = m_pMapToolMonster->Get_TestMonsterPosition();
                    if (ImGui::SliderFloat3("Translation", reinterpret_cast<_float*>(&m_fTranslation), -10.0f, 257.0f))
                    {
                        m_pMapToolMonster->Set_TestMonsterPosition(m_fTranslation);

                        pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_fTranslation), 1.0f));
                    }

                    _float3 position = m_pMapToolMonster->Get_TestMonsterPosition();
                    ImGui::Text("Position: X=%f, Y=%f, Z=%f", position.x, position.y, position.z);
                }
            }
            else
            {
                m_SelectedObjectIndex = -1;
            }
        }
    }
}

void CLevel_MapTool::Show_BuildingList()
{
    if (m_bPickingMonsterActiveOn)
        return;

    if (ImGui::CollapsingHeader("Building List"))
    {
        ImGui::Text("Placed Buildings:");

        for (size_t i = 0; i < m_vecCheckBuildingPosition.size(); ++i)
        {
            char szObjectName[128];

            CTestBuilding* pBuilding = dynamic_cast<CTestBuilding*>(m_vecTestBuilding[i]);
            if (nullptr != pBuilding)
            {
                auto eBuildingType = pBuilding->Get_BuildingType();
                if (eBuildingType == CTestBuilding::BT_WALL)
                    sprintf_s(szObjectName, "Basic Wall #%zu", i + 1);
                if (eBuildingType == CTestBuilding::BT_BIGWALL)
                    sprintf_s(szObjectName, "Big Wall #%zu", i + 1);
                if (eBuildingType == CTestBuilding::BT_PILLAR)
                    sprintf_s(szObjectName, "Pillar #%zu", i + 1);
                if (eBuildingType == CTestBuilding::BT_CONCRETE)
                    sprintf_s(szObjectName, "Concrete #%zu", i + 1);
            }

            if (ImGui::Button(szObjectName))
            {
                m_SelectedObjectIndex = static_cast<_uint>(i);
                ImGui::Text("Selected Building Index: %d", m_SelectedObjectIndex);
            }

            ImGui::SameLine();
            if (ImGui::Button(("Delete##" + std::to_string(i)).c_str()))
            {
                m_vecCheckBuildingPosition.erase(m_vecCheckBuildingPosition.begin() + i);

                if (m_SelectedObjectIndex == static_cast<_uint>(i))
                    m_SelectedObjectIndex = -1;

                m_pBuildingGameObject = m_vecTestBuilding[i];

                if (m_pBuildingGameObject)
                {
                    if (FAILED(m_pGameInstance->Delete_GameObject(LEVEL_MAPTOOL, TEXT("Layer_MapTool_Building"), m_pBuildingGameObject)))
                    {
                        MSG_BOX("삭제 실패");
                    }
                    Safe_Release(m_pBuildingGameObject);
                }
                m_vecTestBuilding.erase(m_vecTestBuilding.begin() + i);

                if (m_SelectedObjectIndex == i)
                {
                    m_SelectedObjectIndex = -1;
                }
                else if (m_SelectedObjectIndex > i)
                {
                    m_SelectedObjectIndex--;
                }
                break;
            }
        }

        if (m_SelectedObjectIndex >= 0 && m_SelectedObjectIndex < m_vecTestBuilding.size())
        {
            m_pMapToolBuilding = dynamic_cast<CTestBuilding*>(m_vecTestBuilding[m_SelectedObjectIndex]);

            if (m_pMapToolBuilding)
            {
                CTransform* pTransform = dynamic_cast<CTransform*>(m_pMapToolBuilding->Find_Component(TEXT("Com_Transform")));

                if (pTransform)
                {
                    ImGui::Text("Selected Monster Properties:");

                    m_fRotation = m_pMapToolBuilding->Get_TestBuildingRotation();

                    // 추후에, 키보드 <- -> 로 돌릴 수 있게 끔.... 
                    if (ImGui::SliderFloat3("Rotation (XYZ)", reinterpret_cast<_float*>(&m_fRotation), 0.0f, 10.0f))
                    {
                        m_pMapToolBuilding->Set_TestBuildingRotation(m_fRotation);

                        pTransform->Rotation(m_fRotation.x, m_fRotation.y, m_fRotation.z);
                    }

                    m_fScale = pTransform->Compute_Scaled();
                    if (ImGui::SliderFloat3("Scale", reinterpret_cast<_float*>(&m_fScale), 1.0f, 5.0f))
                    {
                        m_pMapToolBuilding->Set_TestBuildingScale(m_fScale);

                        pTransform->Scaling(m_fScale);
                    }

                    m_fTranslation = m_pMapToolBuilding->Get_TestBuildingPosition();
                    if (ImGui::SliderFloat3("Translation", reinterpret_cast<_float*>(&m_fTranslation), -20.0f, 259.0f))
                    {
                        m_pMapToolBuilding->Set_TestBuildingPosition(m_fTranslation);

                        pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_fTranslation), 1.0f));
                    }

                    _float3 position = m_pMapToolBuilding->Get_TestBuildingPosition();
                    ImGui::Text("Position: X=%f, Y=%f, Z=%f", position.x, position.y, position.z);
                }
            }
            else
            {
                m_SelectedObjectIndex = -1;
            }
        }
    }
}

#pragma region SAVE
void CLevel_MapTool::OpenFileDialoge(const _tchar* _pDefaultFileName, const _tchar* _pFilter, std::wstring& outFileName)
{
#pragma region 기존꺼

#pragma endregion

    OPENFILENAME ofn;
    _tchar szFile[MAX_PATH] = {};

    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = _pFilter;
    ofn.nFilterIndex = 1;
    ofn.lpstrDefExt = L"txt";
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    wcscpy_s(szFile, MAX_PATH, _pDefaultFileName);

    if (GetSaveFileName(&ofn))
    {
        outFileName = szFile;
    }
}

HRESULT CLevel_MapTool::Save_HeightMap(/*const _tchar* _strMapFileName*/)
{
#pragma region Terrain 높이저장 기존.

#pragma endregion

    wstring fileName;
    OpenFileDialoge(L"HeightMap.txt", L"Text Files\0*.TXT\0All Files\0*.*\0", fileName);

    if (fileName.empty())
        return E_FAIL;

    HANDLE hFile = CreateFile(fileName.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX("Failed To Create File!!");
        return E_FAIL;
    }

    DWORD dwByte = 0;
    _uint iTotalBufferNum = m_iNumMapToolTerrainNumVerticesX * m_iNumMapToolTerrainNumVerticesZ;
    WriteFile(hFile, &iTotalBufferNum, sizeof(_uint), &dwByte, nullptr);
    WriteFile(hFile, m_pVertices, sizeof(VTXNORTEX) * (iTotalBufferNum), &dwByte, nullptr);

    MSG_BOX("HeightMap data saved successfully.");
    CloseHandle(hFile);

    return S_OK;
}

HRESULT CLevel_MapTool::Save_Level(const _tchar* _strMapFileName)
{
    const _tchar* strMapFileName = L"";
    HANDLE hFile = CreateFile(_strMapFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX("Failed To Create File!!");
        return E_FAIL;
    }

    DWORD dwByte = 0;
    _uint iTotalBufferNum = m_iNumMapToolTerrainNumVerticesX * m_iNumMapToolTerrainNumVerticesZ;
    WriteFile(hFile, &iTotalBufferNum, sizeof(_uint), &dwByte, nullptr);
    WriteFile(hFile, m_pVertices, sizeof(VTXNORTEX) * (iTotalBufferNum), &dwByte, nullptr);

    MSG_BOX("Success Save");

    CloseHandle(hFile);

    return S_OK;
}

HRESULT CLevel_MapTool::Save_MonsterData()
{
#pragma region 몬스터 세이브 수정
    wstring fileName;
    OpenFileDialoge(L"MonsterData.txt", L"Text Files\0*.TXT\0All Files\0*.*\0", fileName);
    if (fileName.empty())
        return E_FAIL;

    HANDLE hFile = CreateFile(fileName.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX("Failed To Create MonsterData File!");
        return E_FAIL;
    }

    DWORD dwByte = 0;
    _uint iMonsterCount = static_cast<_uint>(m_vecTestMonster.size());
    WriteFile(hFile, &iMonsterCount, sizeof(_uint), &dwByte, nullptr);

    for (auto& pMonsterObject : m_vecTestMonster)
    {
        if (nullptr != pMonsterObject)
        {
            CMapToolMonster* pMonster = dynamic_cast<CMapToolMonster*>(pMonsterObject);
            if (pMonster != nullptr)
            {
                auto eMonsterType = pMonster->Get_MonsterType();
                WriteFile(hFile, &eMonsterType, sizeof(CMapToolMonster::MONSTER_TYPE), &dwByte, nullptr);

                _float3 vPosition = pMonster->Get_TestMonsterPosition();
                WriteFile(hFile, &vPosition, sizeof(_float3), &dwByte, nullptr);

                _float3 vRotation = pMonster->Get_TestMonsterRotation();
                WriteFile(hFile, &vRotation, sizeof(_float3), &dwByte, nullptr);

                _float3 vScale = pMonster->Get_TestMonsterScale();
                WriteFile(hFile, &vScale, sizeof(_float3), &dwByte, nullptr);
            }
        }
    }

    MSG_BOX("Save Monster Complete");
    CloseHandle(hFile);
#pragma endregion
    return S_OK;
}
HRESULT CLevel_MapTool::Save_BuildingData()
{
#pragma region 빌딩 수정후 
    wstring fileName;
    OpenFileDialoge(L"BuildingData.txt", L"Text Files\0*.TXT\0All Files\0*.*\0", fileName);
    if (fileName.empty())
        return E_FAIL;

    HANDLE hFile = CreateFile(fileName.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX("Failed To Create BuildingData File!");
        return E_FAIL;
    }

    DWORD dwByte = 0;
    _uint iBuildingCount = static_cast<_uint>(m_vecTestBuilding.size());
    WriteFile(hFile, &iBuildingCount, sizeof(_uint), &dwByte, nullptr);

    for (auto& pBuildingObject : m_vecTestBuilding)
    {
        if (nullptr != pBuildingObject)
        {
            CTestBuilding* pBuilding = dynamic_cast<CTestBuilding*>(pBuildingObject);
            if (pBuilding != nullptr)
            {
                auto eBuildingType = pBuilding->Get_BuildingType();
                WriteFile(hFile, &eBuildingType, sizeof(CTestBuilding::BUILDING_TYPE), &dwByte, nullptr);

                _float3 vPosition = pBuilding->Get_TestBuildingPosition();
                WriteFile(hFile, &vPosition, sizeof(_float3), &dwByte, nullptr);

                _float3 vRotation = pBuilding->Get_TestBuildingRotation();
                WriteFile(hFile, &vRotation, sizeof(_float3), &dwByte, nullptr);

                _float3 vScale = pBuilding->Get_TestBuildingScale();
                WriteFile(hFile, &vScale, sizeof(_float3), &dwByte, nullptr);
            }
        }
    }

    MSG_BOX("Save Building Complete");
    CloseHandle(hFile);
#pragma endregion

    return S_OK;
}

HRESULT CLevel_MapTool::Pick_Monster(PICKING_TYPE _ePTType)
{
    m_fPickPos = m_pMapToolCamera->Picking_Terrain(g_hWnd, static_cast<CVIBuffer_MapToolTerrain*>(m_pMapToolTerrain->Find_Component(TEXT("Com_VITerrainBuffer_MapTool"))));

    if (m_fPickPos.y < 0)
        return E_FAIL;

    for (size_t i = 0; i < m_vecCheckPosition.size(); i++)
    {
        if ((m_vecCheckPosition[i].x == m_fPickPos.x) && (m_vecCheckPosition[i].y == m_fPickPos.y) && (m_vecCheckPosition[i].z == m_fPickPos.z))
            return E_FAIL;
    }

    CMapToolMonster::MONSTER_TYPE eMonsterType{};

    switch (m_iMonsterTypeIndex)
    {
    case 0:
        eMonsterType = CMapToolMonster::MONSTER_TYPE::MT_ROBO_L;
        break;
    case 1:
        eMonsterType = CMapToolMonster::MONSTER_TYPE::MT_ROBO_B;
        break;
    case 2:
        eMonsterType = CMapToolMonster::MONSTER_TYPE::MT_ROBO_G;
        break;
    case 3:
        eMonsterType = CMapToolMonster::MONSTER_TYPE::MT_ROBO_V;
        break;
    case 4:
        eMonsterType = CMapToolMonster::MONSTER_TYPE::MT_ROBO_S;
        break;
    case 5: 
        eMonsterType = CMapToolMonster::MONSTER_TYPE::MT_DOOR;
        break;
    case 6:
        eMonsterType = CMapToolMonster::MONSTER_TYPE::MT_ROBO_BOSS;
        break;
    case 7:
        eMonsterType = CMapToolMonster::MONSTER_TYPE::MT_ITEM_BOX;
        break;
    case 8:
        eMonsterType = CMapToolMonster::MONSTER_TYPE::MT_GATE;
        break;
    }

    if (FAILED(Ready_Layer_MapToolMonster(TEXT("Layer_MapTool_Monster"), eMonsterType, 1)))
        return E_FAIL;

    m_vecCheckPosition.push_back(m_fPickPos);

    return S_OK;
}

HRESULT CLevel_MapTool::Pick_Building(PICKING_TYPE _ePTType)
{
    m_fPickPos = m_pMapToolCamera->Picking_Terrain(g_hWnd, static_cast<CVIBuffer_MapToolTerrain*>(m_pMapToolTerrain->Find_Component(TEXT("Com_VITerrainBuffer_MapTool"))));

    if (m_fPickPos.y < 0)
        return E_FAIL;

    for (size_t i = 0; i < m_vecCheckBuildingPosition.size(); i++)
    {
        if ((m_vecCheckBuildingPosition[i].x == m_fPickPos.x) && (m_vecCheckBuildingPosition[i].y == m_fPickPos.y) && (m_vecCheckBuildingPosition[i].z == m_fPickPos.z))
            return E_FAIL;
    }

    CTestBuilding::BUILDING_TYPE eBuildingType{};

    switch (m_iBuildingTypeIndex)
    {
    case 0:
        eBuildingType = CTestBuilding::BUILDING_TYPE::BT_WALL;
        break;
    case 1:
        eBuildingType = CTestBuilding::BUILDING_TYPE::BT_BIGWALL;
        break;
    case 2:
        eBuildingType = CTestBuilding::BUILDING_TYPE::BT_PILLAR;
        break;
    case 3:
        eBuildingType = CTestBuilding::BUILDING_TYPE::BT_CONCRETE;
        break;
    }

    if (FAILED(Ready_Layer_MapToolBuilding(TEXT("Layer_MapTool_Building"), eBuildingType, 1)))
        return E_FAIL;

    m_vecCheckBuildingPosition.push_back(m_fPickPos);

    return S_OK;
}



_bool CLevel_MapTool::Adjust_MaskImage()
{
    static _uint        iMaskTextureIndex = -1;

    if (m_iMaskImageIndex == iMaskTextureIndex)
        return false;

    switch (m_iMaskImageIndex)
    {
    case 0:
        m_pMapToolTerrain->Set_MaskTextureIndex(0);
        break;
    case 1:
        m_pMapToolTerrain->Set_MaskTextureIndex(1);
        break;
    }
    MSG_BOX("Change_MaskImage Complete!");
    iMaskTextureIndex = m_iMaskImageIndex;

    return true;
}

_bool CLevel_MapTool::Select_TerrainTexture(_uint _iFirstDiffuseIndex)
{
    static _uint            iDiffuseTextureIndex = -1;

    if (m_iDiffuseImageFirstIndex == iDiffuseTextureIndex)
        return false;

    switch (m_iDiffuseImageFirstIndex)
    {
    case 0:
        m_pMapToolTerrain->Bind_DiffuseTextureIndex(0, _iFirstDiffuseIndex);
        break;
    case 1:
        m_pMapToolTerrain->Bind_DiffuseTextureIndex(1, _iFirstDiffuseIndex);
        break;
    case 2:
        m_iDiffuseImageSecondIndex = 2;
        m_pMapToolTerrain->Bind_DiffuseTextureIndex(m_iDiffuseImageFirstIndex, m_iDiffuseImageSecondIndex);
        break;
    case 3:
        m_iDiffuseImageSecondIndex = 3;
        m_pMapToolTerrain->Bind_DiffuseTextureIndex(m_iDiffuseImageFirstIndex, m_iDiffuseImageSecondIndex);
        break;
    case 4:
        m_iDiffuseImageSecondIndex = 4;
        m_pMapToolTerrain->Bind_DiffuseTextureIndex(m_iDiffuseImageFirstIndex, m_iDiffuseImageSecondIndex);
        break;
    case 5:
        m_iDiffuseImageSecondIndex = 5;
        m_pMapToolTerrain->Bind_DiffuseTextureIndex(m_iDiffuseImageFirstIndex, m_iDiffuseImageSecondIndex);
        break;
    case 6:
        m_iDiffuseImageSecondIndex = 6;
        m_pMapToolTerrain->Bind_DiffuseTextureIndex(m_iDiffuseImageFirstIndex, m_iDiffuseImageSecondIndex);
        break;
    }

    MSG_BOX("Change_DiffuseImage Complete!");
    iDiffuseTextureIndex = m_iDiffuseImageFirstIndex;

    return true;
}

HRESULT CLevel_MapTool::Show_MouseRange(PICKING_TYPE _ePTType, _float _fTimeDelta)
{
    m_fPickPos = m_pMapToolCamera->Picking_Terrain(g_hWnd, static_cast<CVIBuffer_MapToolTerrain*>(m_pMapToolTerrain->Find_Component(TEXT("Com_VITerrainBuffer_MapTool"))));

    m_bPicked = true;

    // 피킹된 좌표를 셰이더로 전달
    m_pMapToolTerrain->Set_TerrainPickPos(m_fPickPos, m_fRange, m_bPicked);

    // 브러시 중~ 이라는 bool 변수를 셰이더로 전달.
    m_pMapToolTerrain->Set_TextureBrush(m_bBrushed);

    switch (_ePTType)
    {
    case Client::CLevel_MapTool::PT_TERRAIN_PART:
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
        {
            RaiseUp_Terrain(_fTimeDelta);
        }
        else if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
        {
            RaiseDown_Terrain(_fTimeDelta);
        }
        break;
    case Client::CLevel_MapTool::PT_SPLATTING_TEXTURE:
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
        {
            m_bBrushed = true;
            Brushing_Terrain();
        }
        else
        {
            m_bBrushed = false;
        }
        break;

    case Client::CLevel_MapTool::PT_CHOOSE_MASKTEXTURE:
        if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
        {
            m_bBrushed = true;
        }
        else
        {
            m_bBrushed = false;
        }
        break;

    }

    if (m_fPickPos.y < 0)
        return E_FAIL;

    return S_OK;
}

void CLevel_MapTool::RaiseUp_Terrain(_float _fTimeDelta)
{
    _uint iMinRangeX = _uint(m_fPickPos.x - m_fRange);
    _uint iMaxRangeX = _uint(m_fPickPos.x + m_fRange);
    _uint iMinRangeZ = _uint(m_fPickPos.z - m_fRange);
    _uint iMaxRangeZ = _uint(m_fPickPos.z + m_fRange);

    if (iMinRangeX < 0)
        iMinRangeX = 0;
    else
        iMinRangeX = iMinRangeX;

    if (iMaxRangeX >= m_iNumMapToolTerrainNumVerticesX)
        iMaxRangeX = m_iNumMapToolTerrainNumVerticesX - 1;
    else
        iMaxRangeX = iMaxRangeX;

    if (iMinRangeZ < 0)
        iMinRangeZ = 0;
    else
        iMinRangeZ = iMinRangeZ;

    if (iMaxRangeZ >= m_iNumMapToolTerrainNumVerticesZ)
        iMaxRangeZ = m_iNumMapToolTerrainNumVerticesZ - 1;
    else
        iMaxRangeX = iMaxRangeX;

    D3D11_MAPPED_SUBRESOURCE tagSubResource = {};
    m_pContext->Map(m_pMapToolBuffer->Get_VertexBuffer(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &tagSubResource);
    m_pVertices = static_cast<VTXNORTEX*>(tagSubResource.pData);

    for (_uint iTerrainZ = iMinRangeZ; iTerrainZ <= iMaxRangeZ; ++iTerrainZ)
    {
        for (_uint iTerrainX = iMinRangeX; iTerrainX <= iMaxRangeX; ++iTerrainX)
        {
            _uint iIndex = iTerrainZ * m_iNumMapToolTerrainNumVerticesX + iTerrainX;

            m_pVertices[iIndex].vPosition.y += 0.01f;

            m_fHeightY = m_pVertices[iIndex].vPosition.y;
        }
    }
    m_pContext->Unmap(m_pMapToolBuffer->Get_VertexBuffer(), 0);
}

void CLevel_MapTool::RaiseDown_Terrain(_float _fTimeDelta)
{
    _uint iMinRangeX = _uint(m_fPickPos.x - m_fRange);
    _uint iMaxRangeX = _uint(m_fPickPos.x + m_fRange);
    _uint iMinRangeZ = _uint(m_fPickPos.z - m_fRange);
    _uint iMaxRangeZ = _uint(m_fPickPos.z + m_fRange);

    if (iMinRangeX < 0)
        iMinRangeX = 0;
    if (iMaxRangeX >= m_iNumMapToolTerrainNumVerticesX)
        iMaxRangeX = m_iNumMapToolTerrainNumVerticesX - 1;

    if (iMinRangeZ < 0)
        iMinRangeZ = 0;
    if (iMaxRangeZ >= m_iNumMapToolTerrainNumVerticesZ)
        iMaxRangeZ = m_iNumMapToolTerrainNumVerticesZ - 1;

    D3D11_MAPPED_SUBRESOURCE tagSubResource = {};
    m_pContext->Map(m_pMapToolBuffer->Get_VertexBuffer(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &tagSubResource);
    m_pVertices = static_cast<VTXNORTEX*>(tagSubResource.pData);

    for (_uint iTerrainZ = iMinRangeZ; iTerrainZ <= iMaxRangeZ; ++iTerrainZ)
    {
        for (_uint iTerrainX = iMinRangeX; iTerrainX <= iMaxRangeX; ++iTerrainX)
        {
            _uint iIndex = iTerrainZ * m_iNumMapToolTerrainNumVerticesX + iTerrainX;

            m_pVertices[iIndex].vPosition.y -= 0.01f;

            m_fHeightY = m_pVertices[iIndex].vPosition.y;
        }
    }

    m_pContext->Unmap(m_pMapToolBuffer->Get_VertexBuffer(), 0);
}

void CLevel_MapTool::Brushing_Terrain()
{
    _uint iMinRangeX = _uint(m_fPickPos.x - m_fRange);
    _uint iMaxRangeX = _uint(m_fPickPos.x + m_fRange);
    _uint iMinRangeZ = _uint(m_fPickPos.z - m_fRange);
    _uint iMaxRangeZ = _uint(m_fPickPos.z + m_fRange);

    if (iMinRangeX < 0)
        iMinRangeX = 0;
    else
        iMinRangeX = iMinRangeX;

    if (iMaxRangeX >= m_iNumMapToolTerrainNumVerticesX)
        iMaxRangeX = m_iNumMapToolTerrainNumVerticesX - 1;
    else
        iMaxRangeX = iMaxRangeX;

    if (iMinRangeZ < 0)
        iMinRangeZ = 0;
    else
        iMinRangeZ = iMinRangeZ;

    if (iMaxRangeZ >= m_iNumMapToolTerrainNumVerticesZ)
        iMaxRangeZ = m_iNumMapToolTerrainNumVerticesZ - 1;
    else
        iMaxRangeX = iMaxRangeX;

    D3D11_MAPPED_SUBRESOURCE tagSubResource = {};
    m_pContext->Map(m_pMapToolBuffer->Get_VertexBuffer(), 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &tagSubResource);
    m_pVertices = static_cast<VTXNORTEX*>(tagSubResource.pData);

#pragma region 지형색칠
    for (_uint iTerrainZ = iMinRangeZ; iTerrainZ <= iMaxRangeZ; ++iTerrainZ)
    {
        for (_uint iTerrainX = iMinRangeX; iTerrainX <= iMaxRangeX; ++iTerrainX)
        {
            _uint iIndex = iTerrainZ * m_iNumMapToolTerrainNumVerticesX + iTerrainX;

            m_pVertices[iIndex].vTexCoord.x = static_cast<float>(iTerrainX) / (m_iNumMapToolTerrainNumVerticesX - 1);
            m_pVertices[iIndex].vTexCoord.y = static_cast<float>(iTerrainZ) / (m_iNumMapToolTerrainNumVerticesZ - 1);

            m_pVertices[iIndex].vTexIndex.x = 0.5f;
        }
    }
    m_pContext->Unmap(m_pMapToolBuffer->Get_VertexBuffer(), 0);
#pragma endregion
}

void CLevel_MapTool::Set_TerrainTexture(_float _fX, _float _fY)
{
    m_pMapToolTerrain->Bind_Terrain_Texture(_fX, _fY);
}


CLevel_MapTool* CLevel_MapTool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CLevel_MapTool* pInstance = new CLevel_MapTool(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_MapTool");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_MapTool::Free()
{
    __super::Free();

    //for (auto& pTestMonster : m_vecTestMonster)
    //    Safe_Release(pTestMonster);
    //m_vecTestMonster.clear();

    for (auto& pSRV : m_vecMonsterSRVs)
        Safe_Release(pSRV);

    m_vecMonsterSRVs.clear();

    for (auto& pSRV : m_vecBuildingSRVs)
        Safe_Release(pSRV);

    m_vecBuildingSRVs.clear();

    for (auto& pSRV : m_vecMaksImageSRVs)
        Safe_Release(pSRV);

    m_vecMaksImageSRVs.clear();


    for (auto& pSRV : m_vecDiffuseImageSRVs)
        Safe_Release(pSRV);

    m_vecDiffuseImageSRVs.clear();

    Safe_Release(m_pNavigation);
    //Safe_Release(m_pcollider)
}
