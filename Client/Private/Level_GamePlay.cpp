#include "pch.h"
#include "../Public/Level_GamePlay.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Level_Boss.h"

#include "Camera_Free.h"
//#include "Player.h"

#include "FadeObject.h"

#include "PlayerHpbar.h"
#include "HpBarBackGround.h"
#include "MouseCursor.h"
#include "CrossHair.h"
#include "BulletCount.h"
#include "UIWeapon.h"

//#include "Bullet.h"
#include "Bullet_HandGun.h"
#include "Bullet_Rifle.h"
#include "Bullet_Sniper.h"

#include "Missile_Boom.h"

#include "Laser_Larva.h"

#include "Particle_MuzzleFire.h"
#include "Effect_Impact.h"

#include "BasicTerrain.h"

#include "Wall.h"

#include "Robo_Billy.h"
#include "Robo_Boom.h"
#include "Robo_Goliath.h"
#include "Robo_Larva.h"
#include "Robo_Small.h"
#include "Boss_Tower.h"
#include "Door.h"
#include "ItemBox.h"
#include "Gate.h"


#include "UINumber_Manager.h"
#include "Fade_Manager.h"

#include "BossHpBar.h"
#include "BossHpBarBackGround.h"

#include "UILetters.h"
#include "DashEffect.h"
#include "ScopeScreen.h"
#include "ScopeGauge.h"
#include "BossIntroScreen.h"
#include "BreakEffect.h"
#include "HealEffect.h"


#include "Dying_Explode.h"


CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_GamePlay::Initialize()
{
    m_pGameInstance->PlayBGM(L"bgm.wav", 0.5f);
    // 
    // 이게 같은 채널 끊고 지금 나올라는거
    //m_pGameInstance->PlaySelfish(L"BGM_MainMenu.wav". /* 채널 */, 볼륨);


    if (FAILED(Ready_Light()))
        return E_FAIL;

    if (FAILED(Ready_Layer_BasicTerrain(TEXT("Layer_BasicTerrain"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Building(TEXT("Layer_Buildings"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera_Free(TEXT("Layer_Camera_Free"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_PlayerEffect(TEXT("Layer_PlayerEffect"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_PlayerHpBar(TEXT("Layer_PlayerHpBar"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_BossHpBar(TEXT("Layer_BossHpBar"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_FrameEffect(TEXT("Layer_FrameEffect"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
        return E_FAIL;

    //if (FAILED(Ready_Layer_AnimObject(TEXT("Layer_AnimObject"))))
    //    return E_FAIL;

   

    if (FAILED(Ready_Layer_Fade(TEXT("Layer_Fade"))))
        return E_FAIL;


    if (FAILED(Ready_Layer_Bullet(TEXT("Layer_Bullet"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_LCCollider(TEXT("Layer_LCCollider"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Item(TEXT("Layer_Item"))))
        return E_FAIL;

    //if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
    //    return E_FAIL;
    //m_pGameInstance->add_layer

    return S_OK;
}

void CLevel_GamePlay::Update(_float _fTimeDelta)
{
    //_bool bIsCollided = m_pPlayer->Collide_With_Collider();
    //
    //CFade_Manager::GetInstance()->Set_Collide(bIsCollided);
    //
    CFade_Manager::GetInstance()->Update(_fTimeDelta);

    _uint iPlayerHp = m_pPlayer->Get_CurrentHp();
    _uint iPlayerMaxHp = m_pPlayer->Get_MaxHp();
    _uint iCurrentAmmo = m_pPlayer->Get_CurrentAmmoInMagazine();
    _uint iTotalAmmo = m_pPlayer->Get_TotalAmmo();
    //_uint iSlashIndex = 10;

    CUINumber_Manager* pNumberManager = CUINumber_Manager::GetInstance();
    if (pNumberManager)
    {
        pNumberManager->Set_DigitAt(1, iPlayerHp);
        pNumberManager->Set_DigitAt(2, iPlayerMaxHp);

        pNumberManager->Set_DigitAt(3, iCurrentAmmo);
        pNumberManager->Set_DigitAt(4, iTotalAmmo);

        pNumberManager->Set_DigitAt(5, 10);
        pNumberManager->Set_DigitAt(6, 10);
    }
}

HRESULT CLevel_GamePlay::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("게임 플레이 레벨입니다."));
    //ImGui::SliderFloat("##1", &m_fCameraSpeed, 10.0f, 40.0f);
#endif

    CUINumber_Manager* pNumberManager = CUINumber_Manager::GetInstance();
    if (pNumberManager)
        pNumberManager->Render();



    CFade_Manager::GetInstance()->Render();


    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Light()
{
    if (FAILED(m_pGameInstance->SetUp_ShadowLight(XMVectorSet(-10.f, 10.f, -10.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 1.f),
        XMConvertToRadians(60.f), g_iWinSizeX / static_cast<_float>(g_iWinSizeY), 0.1f, 1000.f)))
        return E_FAIL;

    LIGHT_DESC          tagDesc{};

#pragma region 방향성광원_Directional
    ZeroMemory(&tagDesc, sizeof tagDesc);

    tagDesc.eState = LIGHT_DESC::LT_DIRECTIONAL;

    tagDesc.vDirection = _float4(1.0f, -1.0f, 1.0f, 0.0f);

    tagDesc.vDiffuse = _float4(1.0f, 1.0f, 1.0f, 1.0f);

    tagDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.0f);
    //tagDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
    tagDesc.vSpecular = _float4(1.0f, 1.0f, 1.0f, 1.0f);

    if (FAILED(m_pGameInstance->Add_Light(tagDesc)))
        return E_FAIL;
#pragma endregion

    //#pragma region 점 광원 Point
    //    ZeroMemory(&tagDesc, sizeof tagDesc);
    //
    //    tagDesc.eState = LIGHT_DESC::LT_POINT;
    //
    //    tagDesc.vPosition = _float4(10.0f, 3.0f, 10.0f, 1.0f);
    //    tagDesc.fRange = 10.0f;
    //
    //    tagDesc.vDiffuse = _float4(1.0f, 0.0f, 0.0f, 1.0f);
    //
    //    tagDesc.vAmbient = _float4(0.4f, 0.1f, 0.1f, 1.0f);
    //
    //    tagDesc.vSpecular = tagDesc.vDiffuse;
    //
    //    if (FAILED(m_pGameInstance->Add_Light(tagDesc)))
    //        return E_FAIL;
    //#pragma endregion

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BasicTerrain(const _tchar* _pLayerTags)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BasicTerrain"), LEVEL_GAMEPLAY, _pLayerTags, nullptr)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Sky"), LEVEL_GAMEPLAY, _pLayerTags, nullptr)))
        return E_FAIL;

    return S_OK;
}

// 여기다가, skybox 등등 다 나중에 추가할거임.
HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const _tchar* _pLayerTags)
{
    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Building(const _tchar* _pLayerTags)
{
    //wstring filePath = L"../Bin/DataFiles/BuildingFiles/BuildingData09.txt";
    //wstring filePath = L"../Bin/DataFiles/BuildingFiles/TestBuilding.txt";
    wstring filePath = L"../Bin/DataFiles/BuildingFiles/ReadData.txt";
    //wstring filePath = L"../Bin/DataFiles/BuildingFiles/Test.txt";

    CLoad_Manager::GetInstance()->Load_Building(filePath);

    for (_uint i = 0; i < CLoad_Manager::GetInstance()->Get_BuildingCount(); ++i)
    {
        auto BuildingType = CLoad_Manager::GetInstance()->Get_BuildingType()[i];

        switch (BuildingType)
        {
        case Client::CBuilding::BT_WALL:
        {
            CWall::INGAME_WALL_DESC    WallDesc = {};
            WallDesc.vWallPos = CLoad_Manager::GetInstance()->Get_BuildingPosition()[i];
            WallDesc.vWallRotation = CLoad_Manager::GetInstance()->Get_BuildingRotation()[i];
            WallDesc.vWallScale = CLoad_Manager::GetInstance()->Get_BuildingScale()[i];
            WallDesc.eWallType = CWall::IG_BASIC_WALL;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Walls"), LEVEL_GAMEPLAY, _pLayerTags, &WallDesc)))
                return E_FAIL;
        }
        break;
        case Client::CBuilding::BT_BIG_WALL:
        {
            CWall::INGAME_WALL_DESC    WallDesc = {};
            WallDesc.vWallPos = CLoad_Manager::GetInstance()->Get_BuildingPosition()[i];
            WallDesc.vWallRotation = CLoad_Manager::GetInstance()->Get_BuildingRotation()[i];
            WallDesc.vWallScale = CLoad_Manager::GetInstance()->Get_BuildingScale()[i];
            WallDesc.eWallType = CWall::IG_BIG_WALL;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Walls"), LEVEL_GAMEPLAY, _pLayerTags, &WallDesc)))
                return E_FAIL;
        }
        break;

        case Client::CBuilding::BT_PILLAR:
        {
            CWall::INGAME_WALL_DESC    WallDesc = {};
            WallDesc.vWallPos = CLoad_Manager::GetInstance()->Get_BuildingPosition()[i];
            WallDesc.vWallRotation = CLoad_Manager::GetInstance()->Get_BuildingRotation()[i];
            WallDesc.vWallScale = CLoad_Manager::GetInstance()->Get_BuildingScale()[i];
            WallDesc.eWallType = CWall::IG_PILLAR;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Pillar"), LEVEL_GAMEPLAY, _pLayerTags, &WallDesc)))
                return E_FAIL;
        }
        break;

        case Client::CBuilding::BT_CONCRETE:
        {
            CWall::INGAME_WALL_DESC    WallDesc = {};
            WallDesc.vWallPos = CLoad_Manager::GetInstance()->Get_BuildingPosition()[i];
            WallDesc.vWallRotation = CLoad_Manager::GetInstance()->Get_BuildingRotation()[i];
            WallDesc.vWallScale = CLoad_Manager::GetInstance()->Get_BuildingScale()[i];
            WallDesc.eWallType = CWall::IG_CONCRETE;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Concrete"), LEVEL_GAMEPLAY, _pLayerTags, &WallDesc)))
                return E_FAIL;
        }
        break;
        }
    }

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera_Free(const _tchar* _pLayerTags)
{
    CCamera_Free::CAMERA_FREE_DESC          Desc = {};

    Desc._vEye = _float3(0.0f, 10.0f, -7.0f);
    Desc._vAt = _float3(0.0f, 0.0f, 0.0f);

    Desc.fFov = XMConvertToRadians(60.0f);
    Desc.fNear = 0.1f;
    Desc.fFar = 1000.0f;
    Desc.fMouseSensity = 0.1f;
    Desc.fSpeedPerSec = 10.0f;
    Desc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera_Free"), LEVEL_GAMEPLAY, _pLayerTags, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_PlayerEffect(const _tchar* _pLayerTags)
{
    CDashEffect::DASH_EFFECT_DESC   DashDesc = {};
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_DashEffect"), LEVEL_GAMEPLAY, _pLayerTags, &DashDesc)))
        return E_FAIL;

    CHurtEffect::HURT_EFFECT_DESC HurtDesc = {};
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_HurtEffect"), LEVEL_GAMEPLAY, _pLayerTags, &HurtDesc)))
        return E_FAIL;

    CBurnEffect::BURN_EFFECT_DESC BurnDesc = {};
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BurnEffect"), LEVEL_GAMEPLAY, _pLayerTags, &BurnDesc)))
        return E_FAIL;

    CScopeGauge::SCOPE_GAUGE_DESC GaugeDesc = {};
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_ScopeGauge"), LEVEL_GAMEPLAY, _pLayerTags, &GaugeDesc)))
        return E_FAIL;

    CScopeScreen::SCOPE_SCRREN_DESC ScopeDesc = {};
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_ScopeScreen"), LEVEL_GAMEPLAY, _pLayerTags, &ScopeDesc)))
        return E_FAIL;



    CBossIntroScreen::INTRO_DESC IntroDesc = {};
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BossIntro"), LEVEL_GAMEPLAY, _pLayerTags, &IntroDesc)))
        return E_FAIL;

    CBreakEffect::BREAK_EFFECT_DESC BreakDesc = {};
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Break"), LEVEL_GAMEPLAY, _pLayerTags, &BreakDesc)))
        return E_FAIL;

    CHealEffect::HEAL_EFFECT_DESC HealDesc = {};
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_HealEffect"), LEVEL_GAMEPLAY, _pLayerTags, &HealDesc)))
        return E_FAIL;
    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const _tchar* _pLayerTags)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, _pLayerTags, nullptr)))
        return E_FAIL;

    m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, TEXT("Layer_Player")));

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Fade(const _tchar* _pLayerTags)
{
    CFadeObject::FADE_DESC      Desc = {};

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Black"), LEVEL_GAMEPLAY, _pLayerTags, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_PlayerHpBar(const _tchar* _pLayerTags)
{
    CPlayerHpbar::HP_BAR        Desc = {};
    CHpBarBackGround::HPBACK_DESC GroundDesc = {};

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_PlayerHpBar"), LEVEL_GAMEPLAY, _pLayerTags, &Desc)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_HpBarBackGround"), LEVEL_GAMEPLAY, _pLayerTags, &GroundDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Monster(const _tchar* _pLayerTags)
{
    //wstring filePath = L"../Bin/DataFiles/UnitFiles/MonsterData12.txt";
    //wstring filePath = L"../Bin/DataFiles/UnitFiles/MonsterData11.txt";
    wstring filePath = L"../Bin/DataFiles/UnitFiles/TestMon.txt";

    CLoad_Manager::GetInstance()->Load_Monster(filePath);

    for (_uint i = 0; i < CLoad_Manager::GetInstance()->Get_MonsterCount(); ++i)
    {
        auto monsterType = CLoad_Manager::GetInstance()->Get_MonsterType()[i];

        switch (monsterType)
        {
        case CMonster::MT_BOOM:
        {
            CRobo_Boom::ROBO_BOOM_DESC             BoomDesc = {};
            BoomDesc.vMonsterpos = CLoad_Manager::GetInstance()->Get_MonsterPosition()[i];
            BoomDesc.vMonsterRotation = CLoad_Manager::GetInstance()->Get_MonsterRotation()[i];
            BoomDesc.vMonsterScale = CLoad_Manager::GetInstance()->Get_MonsterScale()[i];
            //BoomDesc.vMonsterScale.x *= 0.01f;
            //BoomDesc.vMonsterScale.y *= 0.01f;
            //BoomDesc.vMonsterScale.z *= 0.01f;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_RoboBoom"), LEVEL_GAMEPLAY, _pLayerTags, &BoomDesc)))
                return E_FAIL;
        }
        break;

        case CMonster::MT_BILLY:
        {
            CRobo_Billy::ROBO_BILLY_DESC           BillyDesc = {};
            BillyDesc.vMonsterpos = CLoad_Manager::GetInstance()->Get_MonsterPosition()[i];
            BillyDesc.vMonsterRotation = CLoad_Manager::GetInstance()->Get_MonsterRotation()[i];
            BillyDesc.vMonsterScale = CLoad_Manager::GetInstance()->Get_MonsterScale()[i];
            BillyDesc.vMonsterScale.x *= 0.01f;
            BillyDesc.vMonsterScale.y *= 0.01f;
            BillyDesc.vMonsterScale.z *= 0.01f;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_RoboBilly"), LEVEL_GAMEPLAY, _pLayerTags, &BillyDesc)))
                return E_FAIL;
        }

        break;

        case CMonster::MT_GOLIATH:
        {
            CRobo_Goliath::ROBO_GOLIATH_DESC        GoliathDesc = {};
            GoliathDesc.vMonsterpos = CLoad_Manager::GetInstance()->Get_MonsterPosition()[i];
            GoliathDesc.vMonsterRotation = CLoad_Manager::GetInstance()->Get_MonsterRotation()[i];
            GoliathDesc.vMonsterScale = CLoad_Manager::GetInstance()->Get_MonsterScale()[i];

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_RoboGoliath"), LEVEL_GAMEPLAY, _pLayerTags, &GoliathDesc)))
                return E_FAIL;
        }
        break;

        case CMonster::MT_LARVA:
        {
            CRobo_Larva::ROBO_LARVA_DESC        LarvaDesc = {};
            LarvaDesc.vMonsterpos = CLoad_Manager::GetInstance()->Get_MonsterPosition()[i];
            LarvaDesc.vMonsterRotation = CLoad_Manager::GetInstance()->Get_MonsterRotation()[i];
            LarvaDesc.vMonsterScale = CLoad_Manager::GetInstance()->Get_MonsterScale()[i];

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_RoboLarva"), LEVEL_GAMEPLAY, _pLayerTags, &LarvaDesc)))
                return E_FAIL;
        }
        break;

        case CMonster::MT_SMALL:
        {
            CRobo_Small::ROBO_SMALL_DESC        SmallDesc = {};
            SmallDesc.vMonsterpos = CLoad_Manager::GetInstance()->Get_MonsterPosition()[i];
            SmallDesc.vMonsterRotation = CLoad_Manager::GetInstance()->Get_MonsterRotation()[i];
            SmallDesc.vMonsterScale = CLoad_Manager::GetInstance()->Get_MonsterScale()[i];

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_RoboSmall"), LEVEL_GAMEPLAY, _pLayerTags, &SmallDesc)))
                return E_FAIL;
        }
        break;

        case CMonster::MT_DOOR:
        {
            CDoor::DOOR_DESC        DoorDesc = {};
            DoorDesc.vMonsterpos = CLoad_Manager::GetInstance()->Get_MonsterPosition()[i];
            DoorDesc.vMonsterRotation = CLoad_Manager::GetInstance()->Get_MonsterRotation()[i];
            DoorDesc.vMonsterScale = CLoad_Manager::GetInstance()->Get_MonsterScale()[i];

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Door"), LEVEL_GAMEPLAY, _pLayerTags, &DoorDesc)))
                return E_FAIL;
        }
        break;

        case CMonster::MT_BOSS:
        {
            CBoss_Tower::ROBO_BOSS_DESC        BossDesc = {};
            BossDesc.vMonsterpos = CLoad_Manager::GetInstance()->Get_MonsterPosition()[i];
            BossDesc.vMonsterRotation = CLoad_Manager::GetInstance()->Get_MonsterRotation()[i];
            BossDesc.vMonsterScale = CLoad_Manager::GetInstance()->Get_MonsterScale()[i];

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Boss"), LEVEL_GAMEPLAY, _pLayerTags, &BossDesc)))
                return E_FAIL;
        }
        break;

        case CMonster::MT_ITEM_BOX:
        {
            static _uint iItemBoxIndex = 1;

            CItemBox::ITEM_BOX_DESC BoxDesc = {};
            BoxDesc.vMonsterpos = CLoad_Manager::GetInstance()->Get_MonsterPosition()[i];
            BoxDesc.vMonsterRotation = CLoad_Manager::GetInstance()->Get_MonsterRotation()[i];
            BoxDesc.vMonsterScale = CLoad_Manager::GetInstance()->Get_MonsterScale()[i];
            BoxDesc.iWeaponTypeIndex = (iItemBoxIndex % 2 == 1) ? 1 : 2;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_ItemBox"), LEVEL_GAMEPLAY, _pLayerTags, &BoxDesc)))
                return E_FAIL;

            iItemBoxIndex++;
        }
        break;

        case CMonster::MT_GATE:
        {
            //static _uint iItemBoxIndex = 1;

            CGate::GATE_DESC GateDesc= {};
            GateDesc.vMonsterpos = CLoad_Manager::GetInstance()->Get_MonsterPosition()[i];
            GateDesc.vMonsterRotation = CLoad_Manager::GetInstance()->Get_MonsterRotation()[i];
            GateDesc.vMonsterScale = CLoad_Manager::GetInstance()->Get_MonsterScale()[i];
            //GateDesc.iWeaponTypeIndex = (iItemBoxIndex % 2 == 1) ? 1 : 2;

            if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Gate"), LEVEL_GAMEPLAY, _pLayerTags, &GateDesc)))
                return E_FAIL;

            //iItemBoxIndex++;
        }
        break;
        }
    }
    return S_OK;
}

//HRESULT CLevel_GamePlay::Ready_Layer_AnimObject(const _tchar* _pLayerTags)
//{
//    wstring filePath = L"../Bin/DataFiles/UnitFiles/TestMon.txt";
//
//    CLoad_Manager::GetInstance()->Load_Monster(filePath);
//
//    for (_uint i = 0; i < CLoad_Manager::GetInstance()->Get_MonsterCount(); ++i)
//    {
//
//    }
//
//    return E_NOTIMPL;
//}

HRESULT CLevel_GamePlay::Ready_Layer_MouseCursor(const _tchar* _pLayerTags)
{
    CMouseCursor::CURSOR_DESC       Desc = {};

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_MouseCursor"), LEVEL_GAMEPLAY, _pLayerTags, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_UI(const _tchar* _pLayerTags)
{
    CCrossHair::CROSS_HAIR_DESC     Desc = {};

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_CrossHair"), LEVEL_GAMEPLAY, _pLayerTags, &Desc)))
        return E_FAIL;

    CUINumber_Manager* pNumberManager = CUINumber_Manager::GetInstance();
    if (!pNumberManager)
        return E_FAIL;

    pNumberManager->Initialize();

    CUINumber::UINUMBER_DESC healthCurrentDesc = {};
    healthCurrentDesc.fSizeX = 50.0f;
    healthCurrentDesc.fSizeY = 50.0f;
    healthCurrentDesc.fX = 260.0f;
    healthCurrentDesc.fY = 600.0f;
    healthCurrentDesc.fZ = 0.01f;
    healthCurrentDesc.fNear = 0.0f;
    healthCurrentDesc.fFar = 1.0f;

    for (int i = 0; i < 3; ++i) {
        if (FAILED(pNumberManager->Add_NumberUI(1, healthCurrentDesc)))
            return E_FAIL;

        healthCurrentDesc.fX += 20.0f;
    }

    // 현재 체력바 숫자 / 최대 체력바 숫자 순이라서 그 사이 간격 값 조절하는 데임. 
    CUINumber::UINUMBER_DESC healthMaxDesc = healthCurrentDesc;
    healthMaxDesc.fX += 20.0f;

    for (int i = 0; i < 3; ++i) {
        if (FAILED(pNumberManager->Add_NumberUI(2, healthMaxDesc)))
            return E_FAIL;

        healthMaxDesc.fX += 20.0f;
    }

    CUINumber::UINUMBER_DESC slashDesc = {};
    slashDesc.fSizeX = 35.0f;
    slashDesc.fSizeY = 35.0f;
    slashDesc.fX = 320.0f;
    slashDesc.fY = 600.0f;
    slashDesc.fZ = 0.01f;
    slashDesc.fNear = 0.0f;
    slashDesc.fFar = 1.0f;
    if (FAILED(pNumberManager->Add_NumberUI(5, slashDesc)))
        return E_FAIL;


    // 총알 UI 설정
    CUINumber::UINUMBER_DESC bulletCurrentDesc = {};
    bulletCurrentDesc.fSizeX = 50.0f;
    bulletCurrentDesc.fSizeY = 50.0f;
    bulletCurrentDesc.fX = 800.0f;
    bulletCurrentDesc.fY = 650.0f;
    bulletCurrentDesc.fZ = 0.1f;
    bulletCurrentDesc.fNear = 0.0f;
    bulletCurrentDesc.fFar = 1.0f;

    for (int i = 0; i < 3; ++i) {
        if (FAILED(pNumberManager->Add_NumberUI(3, bulletCurrentDesc)))
            return E_FAIL;

        bulletCurrentDesc.fX += 20.0f;
    }

    CUINumber::UINUMBER_DESC bulletTotalDesc = bulletCurrentDesc;
    bulletTotalDesc.fX += 20.0f;  // 전체 총알 위치 X (현재 탄창 오른쪽에 배치)

    for (int i = 0; i < 3; ++i) {
        if (FAILED(pNumberManager->Add_NumberUI(4, bulletTotalDesc)))  // ID 4 : 전체 총알
            return E_FAIL;

        bulletTotalDesc.fX += 20.0f;
    }

    CUINumber::UINUMBER_DESC slashDesc2 = {};
    slashDesc2.fSizeX = 35.0f;
    slashDesc2.fSizeY = 35.0f;
    slashDesc2.fX = 860.0f;
    slashDesc2.fY = 650.0f;
    slashDesc2.fZ = 0.01f;
    slashDesc2.fNear = 0.0f;
    slashDesc2.fFar = 1.0f;
    if (FAILED(pNumberManager->Add_NumberUI(6, slashDesc2)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Bullet(const _tchar* _pLayerTags)
{
    CBullet_HandGun::BULLET_HANDGUN_DESC    Desc = {};

    for (_uint i = 0; i < 50; ++i)
    {
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Bullet_HandGun"), LEVEL_GAMEPLAY, _pLayerTags, &Desc)))
            return E_FAIL;
    }


    CBullet_Rifle::BULLET_RIFLE_DESC    RifleDesc = {};

    for (_uint i = 0; i < 200; ++i)
    {
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Bullet_Rifle"), LEVEL_GAMEPLAY, _pLayerTags, &RifleDesc)))
            return E_FAIL;
    }

    CBullet_Sniper::BULLET_SNIPER_DESC    SniperDesc = {};

    for (_uint i = 0; i < 100; ++i)
    {
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Bullet_Sniper"), LEVEL_GAMEPLAY, _pLayerTags, &SniperDesc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Missile(const _tchar* _pLayerTags)
{
    CMissile_Boom::MISSILE_BOOM_DESC    Desc = {};

    for (_uint i = 0; i < 100; ++i)
    {
        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Missile_Boom"), LEVEL_GAMEPLAY, _pLayerTags, &Desc)))
            return E_FAIL;
    }

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Laser(const _tchar* _pLayerTag)
{
    CLaser_Larva::LASER_DESC Desc = {};

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Laser_Larva"), LEVEL_GAMEPLAY, _pLayerTag, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Item(const _tchar* _pLayerTag)
{
    CWeaponItem::WEAPON_ITEM_DESC WeaponDesc = {};
    WeaponDesc.eItemType = CItem::ITEM_WEAPON;
    WeaponDesc.iItemID = 1;
    WeaponDesc.bCollected = false;
    WeaponDesc.iUseCount = 1;
    //WeaponDesc.vPosition = { 5.0f, 0.0f, 5.0f };
    WeaponDesc.vPosition = { 0.f, -1000.f, 0.f };

    WeaponDesc.iMaxAmmo = 30;
    WeaponDesc.fDamage = 10.0f;
    WeaponDesc.fRange = 50.0f;

    WeaponDesc.pPlayer = m_pPlayer;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_WeaponItemRifle"), LEVEL_GAMEPLAY, _pLayerTag, &WeaponDesc)))
        return E_FAIL;

    CWeaponItem::WEAPON_ITEM_DESC WeaponDesc2 = {};
    WeaponDesc2.eItemType = CItem::ITEM_WEAPON;
    WeaponDesc2.iItemID = 2;
    WeaponDesc2.bCollected = false;
    WeaponDesc2.iUseCount = 1;
    //WeaponDesc2.vPosition = { 5.0f, 0.0f, 5.0f };
    WeaponDesc2.vPosition = { 0.f, -1000.f, 0.f };
    WeaponDesc2.iMaxAmmo = 30;
    WeaponDesc2.fDamage = 10.0f;
    WeaponDesc2.fRange = 50.0f;
    WeaponDesc2.pPlayer = m_pPlayer;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_WeaponItemSniper"), LEVEL_GAMEPLAY, _pLayerTag, &WeaponDesc2)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_FrameEffect(const _tchar* _pLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Dying_Explosion"), LEVEL_GAMEPLAY, _pLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_LCCollider(const _tchar* _pLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_LC_Collider"), LEVEL_GAMEPLAY, _pLayerTag)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_DashCube(const _tchar* _pLayerTag)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_DashCube"), LEVEL_GAMEPLAY, _pLayerTag)))
        return E_FAIL;


    return S_OK;
}

//HRESULT CLevel_GamePlay::Ready_Layer_Effect(const _tchar* pLayerTag)
//{
//    for (size_t i = 0; i < 30; i++)
//    {
//        if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Effect_Explosion"), LEVEL_GAMEPLAY, pLayerTag)))
//            return E_FAIL;
//    }
//
//    return S_OK;
//}

HRESULT CLevel_GamePlay::Ready_Layer_BossHpBar(const _tchar* _pLayerTag)
{
    CBossHpBar::BOSS_HPBAR_DESC     BossHpDesc = {};
    CBossHpBarBackGround::BOSS_HPBARCGROUND_DESC HpBackDesc = {};

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BossHpBar"), LEVEL_GAMEPLAY, _pLayerTag, &BossHpDesc)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BossHpBarBackGround"), LEVEL_GAMEPLAY, _pLayerTag, &HpBackDesc)))
        return E_FAIL;

    return S_OK;
}


CLevel_GamePlay* CLevel_GamePlay::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CLevel_GamePlay* pInstance = new CLevel_GamePlay(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_GamePlay");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_GamePlay::Free()
{
    __super::Free();
}
