#include "pch.h"
#include "ImGui_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
{
}

CImGui_Manager::~CImGui_Manager()
{
    for (size_t i = 0; i <= 1; ++i)
        m_pFrameStateRS[i]->Release();

    ImGui_ShutDown();
}

void CImGui_Manager::ImGui_SetUp(CGameInstance* _pGameInstance)
{
    m_pGameInstance = _pGameInstance;

    InitRenderStates();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(_pGameInstance->Get_Device(), _pGameInstance->Get_Context());

    // XMPlaneFromPointNormal
}

void CImGui_Manager::ImGui_Tick()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void CImGui_Manager::ImGui_Render()
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    ImGui::EndFrame();

    if (m_bWireFrameMode)
        m_pGameInstance->Get_Context()->RSSetState(m_pFrameStateRS[0]);
    else
        m_pGameInstance->Get_Context()->RSSetState(m_pFrameStateRS[1]);
}

void CImGui_Manager::ImGui_ShutDown()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void CImGui_Manager::InitRenderStates()
{
    for (size_t i = 0; i <= 1; ++i)
    {
        if (i == 0)
            m_tagRasterizeDesc[i].FillMode = D3D11_FILL_WIREFRAME;
        else
            m_tagRasterizeDesc[1].FillMode = D3D11_FILL_SOLID;

        m_tagRasterizeDesc[i].CullMode = D3D11_CULL_NONE;
        m_tagRasterizeDesc[i].DepthClipEnable = true;
        m_pGameInstance->Get_Device()->CreateRasterizerState(&m_tagRasterizeDesc[i], &m_pFrameStateRS[i]);
    }
}

void CImGui_Manager::SwitchFrameMode()
{
    ImGui::Begin("Switch FrameMode", NULL, ImGuiWindowFlags_MenuBar);
    ImGui::Checkbox("Switching To WireFrame", &m_bWireFrameMode);
    if (ImGui::Button("WireFrame On"))
        m_bWireFrameMode = true;
    if (ImGui::Button("WireFrame Off"))
        m_bWireFrameMode = false;
    ImGui::End();
}

//VTXNORTEX* CImGui_Manager::Load_Terrain_Height()
//{
//    //const _tchar* strFilePath = L"../Bin/DataFiles/TerrainData.txt";
//    const _tchar* strFilePath = L"../Bin/DataFiles/TerrainHeight.txt";
//
//    _ulong  dwByte = {};
//    HANDLE  hFile = CreateFile(strFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
//
//    if (hFile == INVALID_HANDLE_VALUE)
//    {
//        MSG_BOX("Failed To Open Terrain File!!");
//        return nullptr;
//    }
//
//    _uint numVertices = {};
//    ReadFile(hFile, &numVertices, sizeof(_uint), &dwByte, nullptr);
//
//    pBasicTerrainVertices = new VTXNORTEX[numVertices];
//    ReadFile(hFile, pBasicTerrainVertices, sizeof(VTXNORTEX) * numVertices, &dwByte, nullptr);
//
//    CloseHandle(hFile);
//
//    return pBasicTerrainVertices;
//}
//
//VTXNORTEX* CImGui_Manager::Load_MapToolTerrain_Height()
//{
//    const _tchar* strFilePath = L"../Bin/DataFiles/TerrainHeight.txt";
//
//    if (nullptr == strFilePath)
//        return nullptr;
//
//    _ulong dwByte = {};
//    HANDLE  hFile = CreateFile(strFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
//
//    if (hFile == INVALID_HANDLE_VALUE)
//    {
//        //MSG_BOX("Failed To Open Terrain File!!");
//        return nullptr;
//    }
//
//    _uint numVertices = {};
//    ReadFile(hFile, &numVertices, sizeof(_uint), &dwByte, nullptr);
//
//    pMapToolTerrainVertices = new VTXNORTEX[numVertices];
//    ReadFile(hFile, pMapToolTerrainVertices, sizeof(VTXNORTEX) * numVertices, &dwByte, nullptr);
//
//    CloseHandle(hFile);
//
//    return pMapToolTerrainVertices;
//}
//
//void CImGui_Manager::Load_Monster()
//{
//    const _tchar* strFilePath = L"../Bin/DataFiles/MonsterData.txt";
//
//    _ulong  dwByte = {};
//    HANDLE  hFile = CreateFile(strFilePath, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
//
//    if (hFile == INVALID_HANDLE_VALUE)
//    {
//        MSG_BOX("Failed To Open Monster File!!");
//        return;
//    }
//
//    ReadFile(hFile, &m_iMonsterCount, sizeof(_uint), &dwByte, nullptr);
//
//    m_vecMonsterType.resize(m_iMonsterCount);
//    m_vecMonsterPoisition.resize(m_iMonsterCount);
//    m_vecMonsterRotation.resize(m_iMonsterCount);
//    m_vecMonsterScale.resize(m_iMonsterCount);
//    for (_uint i = 0; i < m_iMonsterCount; ++i)
//    {
//        CMapToolMonster::MONSTER_TYPE eMonsterType;
//        ReadFile(hFile, &eMonsterType, sizeof(CMapToolMonster::MONSTER_TYPE), &dwByte, nullptr);
//
//        CMonster::INGAME_MONSTER_TYPE eIGMonsterType = {};
//        switch (eMonsterType)
//        {
//        case CMapToolMonster::MT_ROBO_L:
//            eIGMonsterType = CMonster::MT_BOOM;
//            break;
//        case CMapToolMonster::MT_ROBO_B:
//            eIGMonsterType = CMonster::MT_BILLY;
//            break;
//        }
//        m_vecMonsterType[i] = eIGMonsterType;
//
//        _float3 vPosition;
//        ReadFile(hFile, &vPosition, sizeof(_float3), &dwByte, nullptr);
//        m_vecMonsterPoisition[i] = vPosition;
//
//        _float3 vRotation;
//        ReadFile(hFile, &vRotation, sizeof(_float3), &dwByte, nullptr);
//        m_vecMonsterRotation[i] = vRotation;
//
//        _float3 vScale;
//        ReadFile(hFile, &vScale, sizeof(_float3), &dwByte, nullptr);
//        m_vecMonsterScale[i] = vScale;
//    }
//    CloseHandle(hFile);
//}

void CImGui_Manager::Free()
{
    __super::Free();
    //Safe_Delete_Array(pMapToolTerrainVertices);
    //Safe_Delete_Array(pBasicTerrainVertices);
}
