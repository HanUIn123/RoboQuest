#include "pch.h"
#include "../Public/Loader.h"
#include "GameInstance.h"
#include "Camera_Free.h"
#include "Camera_MapTool.h"
#include "BackGround.h"
#include "BasicTerrain.h"

#include "FadeObject.h"



#include "Sky.h"

#include "LevelChangeCollider.h"

#pragma region PLAYER
#include "Player.h"
#include "Body_Player.h"
//#include "Gun.h"
#include "HandGun.h"
#include "Rifle.h"
#include "Sniper.h"

#include "Player_Camera.h"
#pragma endregion

#pragma region ROBO_GOLIATH
#include "Robo_Goliath.h"
#include "Body_Robo_Goliath.h"
#include "Mortal.h"
#include "Mortal_Explosion.h"
#include "HpBar_Robo_Goliath.h"

#include "MortalImpact.h"
#pragma endregion


#pragma region ROBO_BILLY
#include "Robo_Billy.h"
#include "Body_Robo_Billy.h"
#pragma endregion

#pragma region ROBO_BOOM
#include "Robo_Boom.h"
#include "Body_Robo_Boom.h"
#include "HpBar_Robo_Boom.h"

#pragma endregion


#pragma region ROBO_LARVA
#include "Robo_Larva.h"
#include "Body_Robo_Larva.h"
#include "HpBar_Robo_Larva.h"
#pragma endregion

#pragma region ROBO_SMALL
#include "Robo_Small.h"
#include "Body_Robo_Small.h"
#include "HpBar_Robo_Small.h"
#pragma endregion

#pragma region ETC
#include "Door.h"
#include "Body_Door.h"
#include "ItemBox.h"
#include "Body_Box.h"

#include "Gate.h"
#include "Body_Gate.h"

#pragma endregion

#pragma region ROBO_BOSS
#include "Boss_Tower.h"
#include "Body_Boss_Tower.h"
#include "Boss_Mortal.h"
#include "Boss_Fire.h"

#include "FireRoad.h"
#include "Fire_Frame.h"

#include "Boss_Explosion.h"

#include "BossFinger.h"
#pragma endregion


//#include "Bullet.h"
#include "Bullet_HandGun.h"
#include "Bullet_Rifle.h"
#include "Bullet_Sniper.h"

#include "Missile_Boom.h"
#include "Missile_Goliath.h"
#include "Missile_Boss.h"
#include "Missile_Mortal.h"
#include "Laser_Larva.h"
#include "Laser_Boss.h"

#include "Boss_Barrier.h"

#pragma region BUILDING
#include "Wall.h"
#pragma endregion


#pragma region ITEM
#include "WeaponItem.h"
#include "Item.h"
#pragma endregion


#pragma region UI
#include "CrossHair.h"
#include "PlayerHpbar.h"
#include "HpBarBackGround.h"
#include "MouseCursor.h"
#include "BulletCount.h"

#include "BossHpBar.h"
#include "BossHpBarBackGround.h"

#include "UIWeapon.h"

#include "UIInteraction.h"

#include "DamageNumber.h"

#include "UILetters.h"

#include "ScopeScreen.h"
#include "ScopeGauge.h"

#include "BossIntroScreen.h"

#pragma endregion

#include "MapToolTerrain.h"


#pragma region MAPTOOL_MONSTER
#include "TestMonster.h"
#include "TestNorRobo.h"
#include "TestGoliath.h"
#include "TestLarva.h"
#include "TestSmall.h"
#include "TestDoor.h"
#include "TestBoss.h"
#include "TestBox.h"
#include "TestGate.h"
#pragma endregion

#pragma region MAPTOOL_BUILDING
#include "TestWall.h"
#include "TempCollider.h"
#pragma endregion

#pragma region BOSSMAP
#include "BossTerrain.h"

#pragma endregion


#include "Particle_MuzzleFire.h"
#include "Effect_Explosion.h"
#include "Small_Explosion.h"
#include "Dying_Explode.h"

#include "Particle_Fire.h"

#include "Effect_Impact.h"
#include "DashEffect.h"
#include "HurtEffect.h"
#include "BurnEffect.h"
#include "BreakEffect.h"
#include "HealEffect.h"

#include "MissileSmokeParticle.h"


// Texture class 를 사용하기 위해, 여기다가 그냥 해도 되겠으나, 
// Gameinstance 를 통한 -> Prototype Manager class 를 통한, 

CLoader::CLoader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice)
    , m_pContext(_pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

// _beginthread의 3번 째 인자로, 진입점 함수가 필요하기 때문에, 근데 이거는 멤버변수를 사용할 수 없기 때문에, 전역으로 처리를 해준다.
// APIENTRY : Client.cpp 의 wWinAPI~~ 함수에도 적용되어 있는 진입점 함수라는 의미이다, __stdcall 형식에 맞춰서 써줘야 해서 __stdcall::Initialize() ~ 라고
// 써도 되지만, 그냥 APIENTRY라고 해도 상관없음.
_uint APIENTRY Thread_Main(void* _pArg)
{
    CLoader* pLoader = static_cast<CLoader*>(_pArg);

    // 이 진입점 함수 속에서, 추가적으로 생성한 스레드가, 스레드메인을 호출해주고,
    // 스레드 메인에서 start_loading을 호출한다.
    pLoader->Start_Loading();

    return 0;
}

HRESULT CLoader::Initialize(LEVELID _eNextLevelID)
{
    m_eNextLevelID = _eNextLevelID;

    InitializeCriticalSection(&m_CriticalSection);

    // m_eNextLevelID 가 필요로 하는 자원을 로드해주기 위한, Thread를 만들어낸다.
    m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);
    if (0 == m_hThread)
        return E_FAIL;

    return S_OK;
}

HRESULT CLoader::Start_Loading()
{
    EnterCriticalSection(&m_CriticalSection);

    CoInitializeEx(nullptr, 0);

    HRESULT     hr = {};

    switch (m_eNextLevelID)
    {
    case LEVEL_LOGO:
        hr = Loading_For_Level_Logo();
        break;
    case LEVEL_GAMEPLAY:
        hr = Loading_For_Level_GamePlay();
        break;
        //case LEVEL_BOSS:
        //    hr = Loading_For_Level_Boss();
        //    break;
    case LEVEL_MAPTOOL:
        hr = Loading_For_Level_MapTool();
        break;
    }

    if (FAILED(hr))
        return E_FAIL;

    LeaveCriticalSection(&m_CriticalSection);

    return S_OK;
}

void CLoader::SetUp_WindowText()
{
    SetWindowText(g_hWnd, m_szLoadingText);
}

HRESULT CLoader::Loading_For_Level_Logo()
{
    lstrcpy(m_szLoadingText, TEXT("Texture원형을 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Texture_BackGround"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Level_ImageBackGround/Logo_Level/LogoBackGround.dds"), 1))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("Shader 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Shader_VtxPosTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::tagElements, VTXPOSTEX::iNumElements))))
        return E_FAIL;
    lstrcpy(m_szLoadingText, TEXT("BackGround Rect Buffer 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    // BackGround 원형 객체를 추가하려고 한다.(원형 객체 생성) 
    // 무슨 레벨에, 무슨 키, 무슨 오브젝트 원형인지.
    lstrcpy(m_szLoadingText, TEXT("BackGround 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_GameObject_BackGround"), CBackGround::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    lstrcpy(m_szLoadingText, TEXT("로딩 끝"));
    m_bIsFinished = true;

    return S_OK;
}

HRESULT CLoader::Loading_For_Level_GamePlay()
{
#pragma region Navigation
    lstrcpyW(m_szLoadingText, TEXT("네비게이션 원형을 생성한다."));
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.txt")))))
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NavigationFiles/NavigationData01.txt")))))
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NavigationFiles/TestNavigation.txt")))))
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/NavigationFiles/RealNavi.txt")))))
        return E_FAIL;
#pragma endregion


#pragma region COLLIDER
    /* For.Prototype_Component_Collider_AABB */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), CCollider::Create(m_pDevice, m_pContext, CCollider::CT_AABB))))
        return E_FAIL;
    /* For.Prototype_Component_Collider_SPHERE */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), CCollider::Create(m_pDevice, m_pContext, CCollider::CT_SPHERE))))
        return E_FAIL;
#pragma endregion



#pragma region SKY
    /* For.Prototype_Component_Texture_Sky */
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/SkyBox/Sky_%d.dds"), 4))))
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Sky"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/SkyBox/MySky.dds"), 1))))
        return E_FAIL;

    /* For.Prototype_Component_VIBuffer_SkyCube */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_SkyCube"), CSkyBoxCube::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_Component_Shader_VtxSkyCube */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxSkyCube"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxSkyCube.hlsl"), VTXCUBE::tagElements, VTXCUBE::iNumElements))))
        return E_FAIL;

    /* For.Prototype_GameObject_Sky */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Sky"), CSky::Create(m_pDevice, m_pContext))))
        return E_FAIL;

#pragma endregion


#pragma region BLACK_FADE
    lstrcpy(m_szLoadingText, TEXT("Black Fade Texture원형을 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Black"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/UI/Black/Fade2.png"), 1))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("Black Fade Shader 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Black"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxFadeTex.hlsl"), VTXPOSTEX::tagElements, VTXPOSTEX::iNumElements))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("Black Fade Rect Buffer 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_Black"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    lstrcpyW(m_szLoadingText, TEXT("Black Fade 원형 객체를 생성하는 중..."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Black"), CFadeObject::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion



#pragma region BasicTerrain
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    lstrcpy(m_szLoadingText, TEXT("Terrain Texture원형을 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BasicTerrain"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/MyTerrain/Terrain/Terrain%d.png"), 8))))
        return E_FAIL;
    lstrcpy(m_szLoadingText, TEXT("Shader 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxNorTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::tagElements, VTXNORTEX::iNumElements))))
        return E_FAIL;
    lstrcpy(m_szLoadingText, TEXT("Terrain_Buffer 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Terrain"), CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height2.bmp"), CLoad_Manager::GetInstance()->Load_Terrain_Height()))))
        return E_FAIL;
    lstrcpy(m_szLoadingText, TEXT("BaiscTerrain 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BasicTerrain"), CBasicTerrain::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion 

#pragma region Camera Free
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    lstrcpyW(m_szLoadingText, TEXT("Camera_Free 원형 객체를 생성하는 중..."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Camera_Free"), CCamera_Free::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion 


#pragma region Building
    _matrix		PreTransformMatrix = XMMatrixIdentity();

    /* Prototype_GameObject_BasicWall */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BasicWall"), CTestWall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_GameObject_BigWall */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BigWall"), CTestWall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_Component_Model_BasicWall */
    PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BasicWall"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/NonModel/Environment/Wall/Wall2.fbx", g_hWnd, TEXT("../Bin/DataFiles/BuildingFiles/BigWall.bin"), CModel::MODEL_NONANIM, PreTransformMatrix))))
        return E_FAIL;

    /* Prototype_Component_Model_BigWall */
    PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BigWall"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/NonModel/Environment/BigWall/BigWall3.fbx", g_hWnd, TEXT("../Bin/DataFiles/BuildingFiles/BigBigWall.bin"), CModel::MODEL_NONANIM, PreTransformMatrix))))
        return E_FAIL;

    /* Prototype_Component_Model_Pillar */
    PreTransformMatrix = XMMatrixScaling(0.00001f, 0.00001f, 0.00001f);
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Pillar"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/NonModel/Environment/Pillar/Pillar.fbx", g_hWnd, TEXT("../Bin/DataFiles/BuildingFiles/Pillar.bin"), CModel::MODEL_NONANIM, PreTransformMatrix))))
        return E_FAIL;

    /* Prototype_Component_Model_Concrete */
    PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Concrete"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/NonModel/Environment/ConcreteWall/ConcreteWall2.fbx", g_hWnd, TEXT("../Bin/DataFiles/BuildingFiles/Concrete.bin"), CModel::MODEL_NONANIM, PreTransformMatrix))))
        return E_FAIL;

    /* For.Prototype_GameObject_Walls */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Walls"), CWall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Pillar */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Pillar"), CWall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Concrete */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Concrete"), CWall::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion




#pragma region PLAYER 
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    lstrcpyW(m_szLoadingText, TEXT("Player AnimMesh Shader 생성하는 중.. "));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPlayerAnimMesh"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPlayerAnimMesh.hlsl"), VTXANIMMESH::tagElements, VTXANIMMESH::iNumElements))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::tagElements, VTXANIMMESH::iNumElements))))
        return E_FAIL;

    lstrcpyW(m_szLoadingText, TEXT("Player Non_AnimMesh Shader 생성하는 중.. "));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::tagElements, VTXMESH::iNumElements))))
        return E_FAIL;


    lstrcpyW(m_szLoadingText, TEXT("Player Model 원형 생성하는 중.."));
    PreTransformMatrix = XMMatrixScaling(0.00005f, 0.00005f, 0.00005f) * XMMatrixRotationY(XMConvertToRadians(225.0f));

    /* For.Prototype_Component_Model_Player */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Player"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/Player/Player4.fbx", g_hWnd, TEXT("../Bin/DataFiles/UnitFiles/Player.bin"), CModel::MODEL_ANIM, PreTransformMatrix))))
        return E_FAIL;
    /* For.Prototype_Component_Model_HandGun */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HandGun"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/Gun/HandGun/HandGun6.fbx", g_hWnd, TEXT("../Bin/DataFiles/GunFiles/HandGun.bin"), CModel::MODEL_ANIM))))
        return E_FAIL;

    /* For.Prototype_Component_Model_RifleModel */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RifleModel"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/Gun/Rifle/Rifle.fbx", g_hWnd, TEXT("../Bin/DataFiles/GunFiles/Rifle.bin"), CModel::MODEL_ANIM))))
        return E_FAIL;

    /* For.Prototype_Component_Model_RifleModel */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SniperModel"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/Gun/Sniper/Sniper.fbx", g_hWnd, TEXT("../Bin/DataFiles/GunFiles/Sniper.bin"), CModel::MODEL_ANIM))))
        return E_FAIL;





    /* For.Prototype_GameObject_Player */
    lstrcpyW(m_szLoadingText, TEXT("Player Object 객체 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Player"), CPlayer::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Body_Player */
    lstrcpyW(m_szLoadingText, TEXT("Player Body Part 객체 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Body_Player"), CBody_Player::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_HandGun */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_HandGun"), CHandGun::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Rifle */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Rifle"), CRifle::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Sniper */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Sniper"), CSniper::Create(m_pDevice, m_pContext))))
        return E_FAIL;



    /* For.Prototype_GameObject_PlayerCamera */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_PlayerCamera"), CPlayer_Camera::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#pragma endregion

#pragma region PlayerHpBar
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    lstrcpy(m_szLoadingText, TEXT("Player HpBar Texture원형을 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_PlayerHpBar"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/UI/HpBar/T_HealthBar_ProgressBar.png"), 1))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("Player HpBar Shader 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPlayerHpBarTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPlayerHpBarTex.hlsl"), VTXPOSTEX::tagElements, VTXPOSTEX::iNumElements))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("Player HpBar Rect Buffer 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_HpBackBar"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    lstrcpyW(m_szLoadingText, TEXT("Player HpBar 원형 객체를 생성하는 중..."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_PlayerHpBar"), CPlayerHpbar::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    lstrcpy(m_szLoadingText, TEXT("Player HpBarBackGround Texture원형을 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_HpBarBackGround"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/UI/HpBar/T_Player_Background.png"), 1))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("Player HpBarBackGround Shader 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPosTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::tagElements, VTXPOSTEX::iNumElements))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("Player HpBarBackGround Rect Buffer 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_HpBackGround"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    lstrcpyW(m_szLoadingText, TEXT("Player HpBarBackGround 원형 객체를 생성하는 중..."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_HpBarBackGround"), CHpBarBackGround::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_UILetters"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Letters/Letter%d.png"), 5))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxUILetters"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxLetterTex.hlsl"), VTXPOSTEX::tagElements, VTXPOSTEX::iNumElements))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_UILetters"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Letter"), CUILetters::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion 



#pragma region BOSS HPBAR
    lstrcpy(m_szLoadingText, TEXT("Boss HpBar Texture원형을 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossHpBar"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/UI/HpBar/T_HealthBar_ProgressBar.png"), 1))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("Boss HpBar Shader 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_BossHpBar"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPlayerHpBarTex.hlsl"), VTXPOSTEX::tagElements, VTXPOSTEX::iNumElements))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("Boss HpBar Rect Buffer 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_BossHpBar"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    lstrcpyW(m_szLoadingText, TEXT("Boss HpBar 원형 객체를 생성하는 중..."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BossHpBar"), CBossHpBar::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    lstrcpy(m_szLoadingText, TEXT("Boss HpBarBackGround Texture원형을 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossBarHpBackGround"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/UI/HpBar/T_Player_Background.png"), 1))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("Boss HpBarBackGround Shader 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_BossBarHpShader"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::tagElements, VTXPOSTEX::iNumElements))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("Boss HpBarBackGround Rect Buffer 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_BossHpBackGround"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    lstrcpyW(m_szLoadingText, TEXT("Boss HpBarBackGround 원형 객체를 생성하는 중..."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BossHpBarBackGround"), CBossHpBarBackGround::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion

#pragma region 아이템
    lstrcpy(m_szLoadingText, TEXT("Weapon Item 원형을 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Rifle"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/Gun/Rifle/Rifle2.fbx", g_hWnd, TEXT("../Bin/ModelFiles/Gun/Item/Rifle.bin"), CModel::MODEL_NONANIM))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sniper"), CModel::Create_ForSave(m_pDevice, m_pContext, "../Bin/Resources/Models/Gun/Sniper/Sniper2.fbx", g_hWnd, TEXT("../Bin/ModelFiles/Gun/Item/Sniper.bin"), CModel::MODEL_NONANIM))))
        return E_FAIL;



    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_WeaponItem"), CWeaponItem::Create(m_pDevice, m_pContext))))
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_WeaponItemRifle"), CWeaponItem::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_WeaponItemSniper"), CWeaponItem::Create(m_pDevice, m_pContext))))
        return E_FAIL;




#pragma endregion

    ///* For.Prototype_GameObject_HandGun */
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_HandGun"), CHandGun::Create(m_pDevice, m_pContext))))
    //    return E_FAIL;





#pragma region CrossHair
    lstrcpy(m_szLoadingText, TEXT("CrossHair Texture원형을 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_CrossHair"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/UI/Crosshair/BasicCrossHair%d.png"), 2))))
        return E_FAIL;

    //lstrcpy(m_szLoadingText, TEXT("CrossHair Shader 원형 객체를 생성하는 중.."));
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPosTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::tagElements, VTXPOSTEX::iNumElements))))
    //    return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("CrossHair Rect Buffer 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_CrossHair"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    lstrcpyW(m_szLoadingText, TEXT("CrossHair 원형 객체를 생성하는 중..."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_CrossHair"), CCrossHair::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion

#pragma region UI_WEAPONIMAGE
    lstrcpy(m_szLoadingText, TEXT("WEAPON UIIMAGE 원형을 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_WeaponIcon"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/WeaponIcon/Weapon%d.png"), 3))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_WeaponIcon"), CUIWeapon::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion

#pragma region UI_INTERACTION
    lstrcpy(m_szLoadingText, TEXT("WEAPON UIIMAGE 원형을 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Interaction"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/UI/KeyBoard/Key%d.png"), 5))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Interaction"), CUIInteraction::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion



#pragma region BUILLET_COUNT
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BulletCount"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/UI/BulletCount/BC%d.dds"), 10))))
    //    return E_FAIL;

    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_BulletCount"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
    //    return E_FAIL;

    //lstrcpyW(m_szLoadingText, TEXT("CrossHair 원형 객체를 생성하는 중..."));
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BulletCount"), CBulletCount::Create(m_pDevice, m_pContext))))
    //    return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Number"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/UI/BulletCount/BC%d.dds"), 11))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DamageNum"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/UI/BulletCount/BC%d.dds"), 11))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    //lstrcpyW(m_szLoadingText, TEXT("CrossHair 원형 객체를 생성하는 중..."));
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BulletCount"), CBulletCount::Create(m_pDevice, m_pContext))))
    //    return E_FAIL;

#pragma endregion

#pragma region BULLET
    lstrcpyW(m_szLoadingText, TEXT("Basic Bullet 원형 객체 생성하는 중.."));
    PreTransformMatrix = XMMatrixScaling(0.00001f, 0.00001f, 0.00001f);
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Bullet"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/NonAnimModels/Bullet/Bullet.fbx", CModel::MODEL_NONANIM, PreTransformMatrix))))
        return E_FAIL;

    /* For.Prototype_GameObject_Bullet_HandGun */
    lstrcpyW(m_szLoadingText, TEXT("Bullet Object 객체 생성하는 중.."));

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Bullet_HandGun"), CBullet_HandGun::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    // m_pTransformCom->Scaling(_float3(0.00003f, 0.00003f, 0.00003f));
    PreTransformMatrix = XMMatrixScaling(0.00003f, 0.00003f, 0.00003f) * XMMatrixRotationY(XMConvertToRadians(80.0f));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RifleBullet"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/NonAnimModels/BulletRifle/RifleBullet.fbx", CModel::MODEL_NONANIM, PreTransformMatrix))))
        return E_FAIL;

    /* For.Prototype_GameObject_Bullet_Rifle */
    lstrcpyW(m_szLoadingText, TEXT("Bullet Object 객체 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Bullet_Rifle"), CBullet_Rifle::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    //     m_pTransformCom->Scaling(_float3(0.00003f, 0.00003f, 0.00003f));
    PreTransformMatrix = XMMatrixScaling(0.00003f, 0.00003f, 0.00003f) * XMMatrixRotationY(XMConvertToRadians(80.0f));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SniperBullet"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/NonAnimModels/BulletRifle/RifleBullet.fbx", CModel::MODEL_NONANIM, PreTransformMatrix))))
        return E_FAIL;

    /* For.Prototype_GameObject_Bullet_Rifle */
    lstrcpyW(m_szLoadingText, TEXT("Bullet Object 객체 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Bullet_Sniper"), CBullet_Sniper::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion

#pragma region MISSILE
    PreTransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Missile"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/NonAnimModels/Missile/SmallMissile/SmallMissile.fbx", CModel::MODEL_NONANIM, PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Missile_Goliath"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/NonAnimModels/BigMissile/BigMissile.fbx", CModel::MODEL_NONANIM, PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Missile_Mortal"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/NonAnimModels/BigMissile/BigMissile.fbx", CModel::MODEL_NONANIM, PreTransformMatrix))))
        return E_FAIL;

    PreTransformMatrix = XMMatrixScaling(0.0003f, 0.0003f, 0.0003f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Missile_Boss"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/NonAnimModels/BigMissile/BigMissile.fbx", CModel::MODEL_NONANIM, PreTransformMatrix))))
        return E_FAIL;

    PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationX(XMConvertToRadians(90.0f));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_LaserBoss"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/NonAnimModels/Cylinder/Cylinder2.fbx", CModel::MODEL_NONANIM, PreTransformMatrix))))
        return E_FAIL;

    //PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationZ(XMConvertToRadians(90.0f)) * XMMatrixRotationX(XMConvertToRadians(180.0f));
    PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f) * XMMatrixRotationY(XMConvertToRadians(-90.0f)) * XMMatrixRotationZ(XMConvertToRadians(90.0f));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Barrier"), CModel::Create(m_pDevice, m_pContext, "../Bin/Resources/NonAnimModels/Barrier/Barrier.fbx", CModel::MODEL_NONANIM, PreTransformMatrix))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_BarrierTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxBarreirTex.hlsl"), VTXMESH::tagElements, VTXMESH::iNumElements))))
        return E_FAIL;


    /* For.Prototype_GameObject_Missile_Boom */
    lstrcpyW(m_szLoadingText, TEXT("Missile Object 객체 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Missile_Boom"), CMissile_Boom::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Missile_Goliath"), CMissile_Goliath::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Missile_Boss"), CMissile_Boss::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Missile_Mortal"), CMissile_Mortal::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Boss_Laser"), CLaser_Boss::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Barrier"), CBoss_Barrier::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_Component_Shader_VtxCube*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxLaser"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxLaserMesh.hlsl"), VTXMESH::tagElements, VTXMESH::iNumElements))))
        return E_FAIL;

    //Prototype_GameObject_Boss_Laser

#pragma endregion

#pragma region LASER

    /* Prototype_Component_Shader_VtxCube*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxCube"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::tagElements, VTXCUBE::iNumElements))))
        return E_FAIL;

    /* Prototype_Component_Texture_Laser*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Laser"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Laser/MyLaser.dds"), 1))))
        return E_FAIL;

    /* For.Prototype_Component_VIBuffer_RectangleCube */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_RectangleCube"), CRectangle_Cube::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Laser_Larva */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Laser_Larva"), CLaser_Larva::Create(m_pDevice, m_pContext))))
        return E_FAIL;

#pragma endregion


#pragma region MUZZLE_FIRE_EFFECT
    // Prototype_GameObject_Particle_MuzzleFire
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Particle_MuzzleFire"), CParticle_MuzzleFire::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_Component_Texture_Snow */
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Snow/Snow2.png"), 1))))
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MuzzleFlash"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/MuzzleFlash/Impact016Yellow.png"), 1))))
        return E_FAIL;

    /* For.Prototype_Component_Shader_VtxPointInstance */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTX_POINT_INSTANCE::tagElements, VTX_POINT_INSTANCE::iNumElements))))
        return E_FAIL;

    CVIBuffer_Point_Instancing::PARTICLE_POINT_INSTANCING_DESC ExplosionDesc{};
    //ExplosionDesc.iNumInstance = 300;
    ExplosionDesc.iNumInstance = 1;
    ExplosionDesc.vCenter = _float3(0.f, 0.f, 0.f);
    ExplosionDesc.vRange = _float3(0.0002f, 0.0002f, 0.0002f);
    //ExplosionDesc.vRange = _float3(0.2f, 0.2f, 0.2f);
    //ExplosionDesc.vSize = _float2(0.01f, 0.02f);
    ExplosionDesc.vSize = _float2(0.1f, 0.2f);
    ExplosionDesc.vSpeed = _float2(0.2f, 0.5f);
    ExplosionDesc.vLifeTime = _float2(0.2f, 0.4f);
    //ExplosionDesc.vLifeTime = _float2(0.1f, 0.3f);
    ExplosionDesc.isLoop = true;
    ExplosionDesc.vPivot = _float3(0.f, 0.f, 0.f);

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Point_Instancing"), CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, &ExplosionDesc))))
        return E_FAIL;
#pragma endregion


#pragma region  BOSS_FIRE
    lstrcpyW(m_szLoadingText, TEXT("Boss Fire 객체 생성하는 중.."));
    // Prototype_GameObject_Particle_BossFire
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Particle_BossFire"), CParticle_Fire::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_Component_Texture_Fire */
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Fire"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Fire/Fire%d.png"), 2))))
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Fire"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Fire/Fire_01_output.png"), 1))))
        return E_FAIL;

    /* For.Prototype_Component_Shader_VtxPointInstance_Fire */
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance_Fire"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTX_POINT_INSTANCE::tagElements, VTX_POINT_INSTANCE::iNumElements))))
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance_Fire"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxFireParticleEffect.hlsl"), VTX_POINT_INSTANCE::tagElements, VTX_POINT_INSTANCE::iNumElements))))
        return E_FAIL;

    CVIBuffer_Point_Instancing::PARTICLE_POINT_INSTANCING_DESC FireDesc{};
    FireDesc.iNumInstance = 100;
    FireDesc.vCenter = _float3(0.f, 0.f, 0.f);
    //FireDesc.vRange = _float3(0.5f, 0.5f, 0.5f);
    FireDesc.vRange = _float3(2.0f, 2.0f, 2.0f);
    //FireDesc.vSize = _float2(0.05f, 0.1f);
    FireDesc.vSize = _float2(1.0f,1.0f);
    FireDesc.vSpeed = _float2(5.f, 10.f);
    FireDesc.vLifeTime = _float2(0.1f, 5.0f);
    FireDesc.isLoop = true;
    FireDesc.vPivot = _float3(0.f, 0.f, 0.f);

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Point_Instancing_Fire"), CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, &FireDesc))))
        return E_FAIL;
#pragma endregion

#pragma region FireFrame
    ///* For.Prototype_Component_Texture_Snow */
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Fire"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Fire/Fire%d.png"), 2))))
    //    return E_FAIL;

    ///* For.Prototype_Component_Shader_VtxPointInstance_Fire */
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPointInstance_Fire"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTX_POINT_INSTANCE::tagElements, VTX_POINT_INSTANCE::iNumElements))))
    //    return E_FAIL;

    //CVIBuffer_Point_Instancing::PARTICLE_POINT_INSTANCING_DESC FireDesc{};
    FireDesc.iNumInstance = 250;
    FireDesc.vCenter = _float3(0.f, 0.f, 0.f);
    FireDesc.vRange = _float3(1.0f, 0.0f, 1.0f);
    FireDesc.vSize = _float2(0.5f, 1.2f);
    FireDesc.vSpeed = _float2(3.0f, 6.0f);
    //FireDesc.vLifeTime = _float2(1.0f, 2.0f);
    FireDesc.vLifeTime = _float2(0.1f, 5.0f);
    FireDesc.isLoop = true;
    FireDesc.vPivot = _float3(0.f, 0.f, 0.f);

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_FramePoint"), CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, &FireDesc))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_FireFrame"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPointInstance.hlsl"), VTX_POINT_INSTANCE::tagElements, VTX_POINT_INSTANCE::iNumElements))))
        return E_FAIL;
#pragma endregion


#pragma region  MISSILE_SMOKE
    lstrcpyW(m_szLoadingText, TEXT("Missile Smoke 객체 생성하는 중.."));
    // Prototype_GameObject_Particle_Smoke
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Particle_Smoke"), CMissileSmokeParticle::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_Component_Texture_Smoke */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Smoke"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Smoke/smoke_sheet.png"), 1))))
        return E_FAIL;

    /* For.Prototype_Component_Shader_SmokeEffect */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_SmokeEffect"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxSmokeParticleEffect.hlsl"), VTX_POINT_INSTANCE::tagElements, VTX_POINT_INSTANCE::iNumElements))))
        return E_FAIL;

    CVIBuffer_Point_Instancing::PARTICLE_POINT_INSTANCING_DESC SmokeDesc{};
   SmokeDesc.iNumInstance = 150;
   SmokeDesc.vCenter = _float3(0.f, 0.f, 0.f);
   SmokeDesc.vRange = _float3(0.5f, 0.5f, 0.5f);
   //SmokeDesc.vSize = _float2(0.7f, 0.7f);
   SmokeDesc.vSize = _float2(3.0f,3.0f);
   SmokeDesc.vSpeed = _float2(2.f, 5.f);
   SmokeDesc.vLifeTime = _float2(0.5f, 3.0f);
   SmokeDesc.isLoop = true;
   SmokeDesc.vPivot = _float3(0.f, 0.f, 0.f);

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Smoke"), CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, &SmokeDesc))))
        return E_FAIL;
#pragma endregion




#pragma region EFFECT_EXPLOSION

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxExplosionTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxExplosionTex.hlsl"), VTXPOSTEX::tagElements, VTXPOSTEX::iNumElements))))
        return E_FAIL;

    /* For.Prototype_Component_Texture_Explosion */
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Flame/Fire_%d.png"), 33))))
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Explosion/Explosion_%d.png"), 13))))
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Explosion/Explo_%d.png"), 17))))
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Explosion/SmallExplo/T_Mole_Death.png"), 1))))
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossExplosion"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Explosion/Explo_%d.png"), 17))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SmallExplosion"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DyingExplosion"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Explosion/SmallExplo/T_FX_Flare_0001.dds"), 1))))
        return E_FAIL;


    CVIBuffer_Point_Instancing::PARTICLE_POINT_INSTANCING_DESC MortalExplosionDesc{};
    MortalExplosionDesc.iNumInstance = 50;
    MortalExplosionDesc.vCenter = _float3(0.f, 0.f, 0.f);
    //FireDesc.vRange = _float3(0.5f, 0.5f, 0.5f);
    MortalExplosionDesc.vRange = _float3(2.0f, 2.0f, 2.0f);
    //FireDesc.vSize = _float2(0.05f, 0.1f);
    MortalExplosionDesc.vSize = _float2(1.0f, 1.0f);
    MortalExplosionDesc.vSpeed = _float2(0.5f, 1.0f);
    MortalExplosionDesc.vLifeTime = _float2(1.0f, 1.5f);
    MortalExplosionDesc.isLoop = true;
    MortalExplosionDesc.vPivot = _float3(0.f, 0.f, 0.f);

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Point_MortalExplosion"), CVIBuffer_Point_Instancing::Create(m_pDevice, m_pContext, &MortalExplosionDesc))))
        return E_FAIL;




    /* For. Prototype_GameObject_Effect_Explosion */
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Effect_Explosion"), CEffect_Explosion::Create(m_pDevice, m_pContext))))
    //    return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BossExplosion"), CBoss_Explosion::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Mortal_Explosion"), CMortal_Explosion::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Small_Explosion"), CSmall_Explosion::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_FireFrame"), CFire_Frame::Create(m_pDevice, m_pContext))))
    //    return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Dying_Explosion"), CDying_Explode::Create(m_pDevice, m_pContext))))
        return E_FAIL;



#pragma endregion

    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FireFrame"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Flame/Fire_%d.png"), 33))))
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FireFrame"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Fire/Fire_01_output.png"), 1))))
        return E_FAIL;

#pragma region EFFECT_IMPACT
    /* For.Prototype_Component_Texture_Impact */
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Impact"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Explosion/Explosion%d.png"), 90))))
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Impact"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Effect/Impact%d.png"), 5))))
        return E_FAIL;

    /* For. Prototype_GameObject_Effect_Impact */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Effect_Impact"), CEffect_Impact::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion



#pragma region DASH_EFFECT
    lstrcpy(m_szLoadingText, TEXT("DashEffect Texture원형을 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Dash"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/UI/Dash/T_SprintLayer_3C.png"), 1))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("DashEffect Shader 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Dash"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxFadeTex.hlsl"), VTXPOSTEX::tagElements, VTXPOSTEX::iNumElements))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("DashEffect Rect Buffer 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_Dash"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    lstrcpyW(m_szLoadingText, TEXT("DashEffect 원형 객체를 생성하는 중..."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_DashEffect"), CDashEffect::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion


#pragma region HURT_EFFECT
    lstrcpy(m_szLoadingText, TEXT("HurtEffect Texture원형을 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Hurt"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/UI/Hurt/T_VignetSpeed.png"), 1))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Break"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/UI/Hurt/ScreenBreak.png"), 1))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Burn"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/UI/Hurt/T_Fire_SpriteSheet_Fade.png"), 1))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Heal"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/UI/Hurt/T_VignetSpeed.png"), 1))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("HurtEffect Shader 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_Hurt"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxFadeTex.hlsl"), VTXPOSTEX::tagElements, VTXPOSTEX::iNumElements))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("HurtEffect Rect Buffer 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_Hurt"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    lstrcpyW(m_szLoadingText, TEXT("HurtEffect 원형 객체를 생성하는 중..."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_HurtEffect"), CHurtEffect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BurnEffect"), CBurnEffect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_HealEffect"), CHealEffect::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion


#pragma region SCOPE_SCREEN
    lstrcpy(m_szLoadingText, TEXT("DashEffect Texture원형을 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ScopeScreen"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Scope/T_PH_ScopeCenter.png"), 1))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("DashEffect Shader 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_ScopeScreen"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxFadeTex.hlsl"), VTXPOSTEX::tagElements, VTXPOSTEX::iNumElements))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("DashEffect Rect Buffer 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_ScopeScreen"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    lstrcpyW(m_szLoadingText, TEXT("DashEffect 원형 객체를 생성하는 중..."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_ScopeScreen"), CScopeScreen::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_ScopeBar"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Scope/T_StunBar_Background.png"), 1))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_ScopeGauge"), CScopeGauge::Create(m_pDevice, m_pContext))))
        return E_FAIL;



    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BossIntro"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/BossIntro/T_DrTurret_Background2.png"), 1))))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_IntroDissolve"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/BossIntro/T_Bubble_Noise.png"), 1))))
        return E_FAIL;


    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BossIntro"), CBossIntroScreen::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Break"), CBreakEffect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

#pragma endregion


#pragma region Monster
    lstrcpyW(m_szLoadingText, TEXT("Monster Model 원형 객체 생성하는 중.."));
    //PreTransformMatrix = XMMatrixScaling(0.0000001f, 0.0000001f, 0.0000001f);
    //PreTransformMatrix = XMMatrixScaling(0.0000001f, 0.0000001f, 0.0000001f);
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RoboBilly"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/BillyBoom/BillyBoom2.fbx", g_hWnd, TEXT("../Bin/ModelFiles/Monster/Robo_Billy/Robo_Billy.bin"), CModel::MODEL_ANIM /*PreTransformMatrix*/))))
        return E_FAIL;

    //PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
    PreTransformMatrix = XMMatrixScaling(0.00005f, 0.00005f, 0.00005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RoboBoom"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/BoomRobo/Robo_B.fbx", g_hWnd, TEXT("../Bin/ModelFiles/Monster/Robo_Boom/Robo_Boom.bin"), CModel::MODEL_ANIM, PreTransformMatrix))))
        return E_FAIL;

    //PreTransformMatrix = XMMatrixScaling()
    //PreTransformMatrix = XMMatrixScaling(0.00005f, 0.00005f, 0.00005f);
    PreTransformMatrix = XMMatrixScaling(0.00005f, 0.00005f, 0.00005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RoboGoliath"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/GoliathRobo/Goliath.fbx", g_hWnd, TEXT("../Bin/ModelFiles/Monster/Robo_Goliath/Robo_Goliath.bin"), CModel::MODEL_ANIM, PreTransformMatrix))))
        return E_FAIL;

    //PreTransformMatrix = XMMatrixScaling()
    PreTransformMatrix = XMMatrixScaling(0.00005f, 0.00005f, 0.00005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RoboLarva"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/LarvaRobo/Larva.fbx", g_hWnd, TEXT("../Bin/ModelFiles/Monster/Robo_Larva/Robo_Larva.bin"), CModel::MODEL_ANIM, PreTransformMatrix))))
        return E_FAIL;

    //PreTransformMatrix = XMMatrixScaling()
    PreTransformMatrix = XMMatrixScaling(0.00005f, 0.00005f, 0.00005f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RoboSmall"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/SmallRobo/SmallBot.fbx", g_hWnd, TEXT("../Bin/ModelFiles/Monster/Robo_Small/Robo_Small.bin"), CModel::MODEL_ANIM, PreTransformMatrix))))
        return E_FAIL;

    PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Door"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Door/Door.fbx", g_hWnd, TEXT("../Bin/ModelFiles/Monster/Door/Door.bin"), CModel::MODEL_ANIM, PreTransformMatrix))))
        return E_FAIL;

    PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Gate"), CModel::Create_ForSave(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Gate/Gate3.fbx", g_hWnd, TEXT("../Bin/ModelFiles/Monster/Gate/Gate.bin"), CModel::MODEL_ANIM, PreTransformMatrix))))
        return E_FAIL;

    PreTransformMatrix = XMMatrixScaling(0.0015f, 0.0015f, 0.0015f) * XMMatrixRotationY(XMConvertToRadians(180.0f));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_RoboBoss"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/BossRobo/BossTower.fbx", g_hWnd, TEXT("../Bin/ModelFiles/Monster/Robo_Boss/BossTower.bin"), CModel::MODEL_ANIM, PreTransformMatrix))))
        return E_FAIL;

    PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ItemBox"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/ItemBox/ItemBox.fbx", g_hWnd, TEXT("../Bin/ModelFiles/Monster/ItemBox/ItemBox.bin"), CModel::MODEL_ANIM, PreTransformMatrix))))
        return E_FAIL;

 /*   if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ItemBox2"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/ItemBox/ItemBox.fbx", g_hWnd, TEXT("../Bin/ModelFiles/Monster/ItemBox/ItemBox2.bin"), CModel::MODEL_ANIM, PreTransformMatrix))))
        return E_FAIL;*/

    lstrcpyW(m_szLoadingText, TEXT("Monster Object 객체 생성하는 중.."));
    /* For.Prototype_GameObject_RoboBoom */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_RoboBoom"), CRobo_Boom::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_RoboBilly */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_RoboBilly"), CRobo_Billy::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_RoboGoliath */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_RoboGoliath"), CRobo_Goliath::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_RoboLarva */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_RoboLarva"), CRobo_Larva::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_RoboSmall */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_RoboSmall"), CRobo_Small::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Door */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Door"), CDoor::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Gate */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Gate"), CGate::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Boss */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Boss"), CBoss_Tower::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Boss */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_ItemBox"), CItemBox::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Body_RoboBilly */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Body_RoboBilly"), CBody_Robo_Billy::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Body_RoboBoom */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Body_RoboBoom"), CBody_Robo_Boom::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Body_RoboGoliath */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Body_RoboGoliath"), CBody_Robo_Goliath::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Mortal */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Mortal"), CMortal::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Body_RoboLarva */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Body_RoboLarva"), CBody_Robo_Larva::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Body_RoboSmall */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Body_RoboSmall"), CBody_Robo_Small::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Body_Door */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Body_Door"), CBody_Door::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Body_Gate */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Body_Gate"), CBody_Gate::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Body_Boss */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Body_RoboBoss"), CBody_Boss_Tower::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_Body_Boss */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Body_ItemBox"), CBody_Box::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_BossMortal */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BossMortal"), CBoss_Mortal::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_BossFire */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BossFire"), CBoss_Fire::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_BossFinger */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_BossFinger"), CBossFinger::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    // Dissolve Texture 
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Dissolve"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Dissolve/T_Pixel_Noise.png"), 1))))
        return E_FAIL;

    // Dissolve Prototype_Component_Texture_BoomDissolve 
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BoomDissolve"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Dissolve/T_Noise_Small.png"), 1))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_GoliathDissolve"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Dissolve/T_Noise_Liquid.png"), 1))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_LarvaDissolve"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Dissolve/T_Noise_Small.png"), 1))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SmallDissolve"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Dissolve/T_Noise_Small.png"), 1))))
        return E_FAIL;

    // Dissolve Texture - Barrier 
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_BarrierDissolve"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Dissolve/T_Pixel_Noise.png"), 1))))
        return E_FAIL;

    // Trail Texture 
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_TrailTexture"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Trail/TrailsPatterrns_04_Out_01.png"), 1))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_RifleTrailTexture"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Trail/TrailsPatterrns_04_Out_03.png"), 1))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_SniperTrailTexture"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Trail/T_Trails_Gen1.png"), 1))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_TrailBuffer"), CTrailBuffer::Create(m_pDevice, m_pContext))))
        return E_FAIL;


    // Mask Texture 
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DoorMask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Mask/T_VFX_Caustic_01_MSK.png"), 1))))
        return E_FAIL;

    /* For.Prototype_GameObject_FireRoad */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_FireRoad"), CFireRoad::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_MortalImpact */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_MortalImpact"), CMortalImpact::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_Component_Texture_FireRoad */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FireRoad"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Fire/T_Fire_Lava.dds"), 1))))
        return E_FAIL;

    /* For.Prototype_Component_Texture_MortalImpact */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_MortalImpact"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/MortalImpact/T_ElitePawn_Eye.png"), 1))))
        return E_FAIL;

    /* For.Prototype_Component_Shader_PosVerRect */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_PosVerRect"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosVerTex.hlsl"), VTXPOSTEX::tagElements, VTXPOSTEX::iNumElements))))
        return E_FAIL;

    /* For.Prototype_Component_VIBuffer_Rect_Ver */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_Ver"), CVIBuffer_VerRect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

#pragma endregion

#pragma region 몬스터들 체력바
    /* For.Protoype_GameObject_HpBar_RoboBoom */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_HpBar_RoboBoom"), CHpBar_Robo_Boom::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_DamageNumber"), CDamageNumber::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("Player HpBar Texture원형을 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RoboBoomHpBar"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/UI/HpBar/T_HealthBar_ProgressBar.png"), 1))))
        return E_FAIL;

    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_RoboBoomHpBar_Background"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/UI/HpBar/T_HealthBar_ProgressBar.png"), 1))))
    //    return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("Player HpBar Shader 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxBoomHpBarTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxRoboHpBarTex.hlsl"), VTXPOSTEX::tagElements, VTXPOSTEX::iNumElements))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("Player HpBar Rect Buffer 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_RoboBoomHpBar"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;



    /* For.Protoype_GameObject_HpBar_RoboBoom */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_HpBar_RoboGoliath"), CHpBar_Robo_Goliath::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_HpBar_RoboLarva */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_HpBar_RoboLarva"), CHpBar_Robo_Larva::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* For.Prototype_GameObject_HpBar_RoboSmall */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_HpBar_RoboSmall"), CHpBar_Robo_Small::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion 



#pragma region LEVEL_CHANGE_COLLIDER
    /* Prototype_GameObject_LC_Collider*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_LC_Collider"), CLevelChangeCollider::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion


#pragma region DASH_CUBE
    /* Prototype_GameObject_DashCube */
    //if(FAILED(m_pGameInstance->Add_Prototype(LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_DashCube"), CDashCube::Create(m_pDevice, m_pContext))))
    //    return E_FAIL;

#pragma endregion

    // 게임 플레이 레벨 자원 로딩 종료
    lstrcpy(m_szLoadingText, TEXT("로딩 끝"));
    m_bIsFinished = true;

    return S_OK;
}

HRESULT CLoader::Loading_For_Level_Boss()
{
    lstrcpy(m_szLoadingText, TEXT("로딩 끝"));
    m_bIsFinished = true;

    return S_OK;
}

HRESULT CLoader::Loading_For_Level_MapTool()
{
#pragma region Navigation
    lstrcpyW(m_szLoadingText, TEXT("네비게이션 원형을 생성한다."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Navigation"), CNavigation::CreateOnly(m_pDevice, m_pContext))))
        return E_FAIL;

    //lstrcpyW(m_szLoadingText, TEXT("네비게이션 원형을 생성한다."));
    //if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Navigation_FromLoad"), CNavigation::Create(m_pDevice, m_pContext, TEXT("../Bin/DataFiles/Navigation.txt")))))
    //    return E_FAIL;
#pragma endregion

#pragma region MapToolCamera
    lstrcpyW(m_szLoadingText, TEXT("Camera_MapTool 원형 객체를 생성하는 중..."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_Camera_MapTool"), CCamera_MapTool::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion 

#pragma region MapTool Terrain
    lstrcpy(m_szLoadingText, TEXT("MapToolTerrain Texture원형을 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_MapToolTerrain"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/MyTerrain/Terrain/Terrain%d.png"), 8))))
        return E_FAIL;
    /* For.Prototype_Component_Texture_Terrain_Mask*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_MapToolTerrain_Mask"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/MyTerrain/MaskImages/Mask%d.png"), 2))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Texture_MapToolTerrain_Mouse"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Terrain/MouseRange.png"), 1))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("Shader 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxNorTex_MapToolTerrain"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::tagElements, VTXNORTEX::iNumElements))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("MapTool 용 Terrain_Buffer 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_VIBuffer_MapToolTerrain"), CVIBuffer_MapToolTerrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height2.bmp"), CLoad_Manager::GetInstance()->Load_MapToolTerrain_Height()))))
        return E_FAIL;

    /*   if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_VIBuffer_MapToolTerrain"), CVIBuffer_MapToolTerrain::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
           return E_FAIL;*/

    lstrcpy(m_szLoadingText, TEXT("MapToolTerrain 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_MapToolTerrain"), CMapToolTerrain::Create(m_pDevice, m_pContext))))
        return E_FAIL;
#pragma endregion
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    /* Prototype_GameObject_TestMonster*/
    _matrix         PreTransformMatrix = XMMatrixIdentity();

    /* Prototype_GameObject_TestNorRobo*/
    lstrcpyW(m_szLoadingText, TEXT("Monster 원형을 생성하는 중.. "));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestNorRobo"), CTestNorRobo::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_GameObject_TestMonster*/
    lstrcpyW(m_szLoadingText, TEXT("Monster 원형을 생성하는 중.. "));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestMonster"), CTestMonster::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_GameObject_TestGoliath*/
    lstrcpyW(m_szLoadingText, TEXT("Monster 원형을 생성하는 중.. "));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestGoliath"), CTestGoliath::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_GameObject_TestLarva*/
    lstrcpyW(m_szLoadingText, TEXT("Monster 원형을 생성하는 중.. "));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestLarva"), CTestLarva::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_GameObject_TestSmall*/
    lstrcpyW(m_szLoadingText, TEXT("Monster 원형을 생성하는 중.. "));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestSmall"), CTestSmall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_GameObject_TestDoor*/
    lstrcpyW(m_szLoadingText, TEXT("Monster 원형을 생성하는 중.. "));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestDoor"), CTestDoor::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_GameObject_TestGate*/
    lstrcpyW(m_szLoadingText, TEXT("Monster 원형을 생성하는 중.. "));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestGate"), CTestGate::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_GameObject_TestBoss*/
    lstrcpyW(m_szLoadingText, TEXT("Monster 원형을 생성하는 중.. "));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestBoss"), CTestBoss::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_GameObject_TestBoss*/
    lstrcpyW(m_szLoadingText, TEXT("Monster 원형을 생성하는 중.. "));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestBox"), CTestBox::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_Component_Shader_VtxAnimMesh*/
    lstrcpyW(m_szLoadingText, TEXT("Monster AnimMesh Shader 생성하는 중.. "));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxAnimMesh"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimMesh.hlsl"), VTXANIMMESH::tagElements, VTXANIMMESH::iNumElements))))
        return E_FAIL;

    /* Prototype_Component_Model_BillyBoom*/
    PreTransformMatrix = XMMatrixScaling(0.001f, 0.001f, 0.001f);
    lstrcpyW(m_szLoadingText, TEXT("Monster Model 원형 객체 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_BillyBoom"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/BillyBoom/BillyBoom2.fbx", g_hWnd, TEXT("../Bin/ModelFiles/Monster/Robo_Billy/Robo_Billy.bin"), CModel::MODEL_ANIM, PreTransformMatrix))))
        return E_FAIL;

    /* Prototype_Component_Model_BoomMonster*/
    PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
    lstrcpyW(m_szLoadingText, TEXT("Monster Model 원형 객체 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_BoomMonster"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/BoomRobo/Robo_B.fbx", g_hWnd, TEXT("../Bin/ModelFiles/Monster/Robo_Boom/Robo_Boom.bin"), CModel::MODEL_ANIM, PreTransformMatrix))))
        return E_FAIL;

    /* Prototype_Component_Model_TestGoliath*/
    PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
    lstrcpyW(m_szLoadingText, TEXT("Monster Model 원형 객체 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_TestGoliath"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/GoliathRobo/Goliath2.fbx", g_hWnd, TEXT("../Bin/ModelFiles/Monster/Robo_Goliath/Robo_Goliath.bin"), CModel::MODEL_ANIM, PreTransformMatrix))))
        return E_FAIL;

    /* Prototype_Component_Model_TestLarva*/
    PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
    lstrcpyW(m_szLoadingText, TEXT("Monster Model 원형 객체 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_TestLarva"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/LarvaRobo/Larva.fbx", g_hWnd, TEXT("../Bin/ModelFiles/Monster/Robo_Larva/Robo_Larva.bin"), CModel::MODEL_ANIM, PreTransformMatrix))))
        return E_FAIL;

    /* Prototype_Component_Model_TestSmall*/
    PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
    lstrcpyW(m_szLoadingText, TEXT("Monster Model 원형 객체 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_TestSmall"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/SmallRobo/SmallBot.fbx", g_hWnd, TEXT("../Bin/ModelFiles/Monster/Robo_Small/Robo_Small.bin"), CModel::MODEL_ANIM, PreTransformMatrix))))
        return E_FAIL;

    /* Prototype_Component_Model_TestDoor*/
    PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
    lstrcpyW(m_szLoadingText, TEXT("Monster Model 원형 객체 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_TestDoor"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Door/Door.fbx", g_hWnd, TEXT("../Bin/ModelFiles/Monster/Door/Door.bin"), CModel::MODEL_ANIM, PreTransformMatrix))))
        return E_FAIL;

    /* Prototype_Component_Model_TestGate*/
    PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
    lstrcpyW(m_szLoadingText, TEXT("Monster Model 원형 객체 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_TestGate"), CModel::Create_ForSave(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/Gate/Gate3.fbx", g_hWnd, TEXT("../Bin/ModelFiles/Monster/Gate/Gate.bin"), CModel::MODEL_ANIM, PreTransformMatrix))))
        return E_FAIL;

    /* Prototype_Component_Model_TestBoss*/
    PreTransformMatrix = XMMatrixScaling(0.0015f, 0.0015f, 0.0015f);
    lstrcpyW(m_szLoadingText, TEXT("Monster Model 원형 객체 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_TestBoss"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/BossRobo/BossTower.fbx", g_hWnd, TEXT("../Bin/ModelFiles/Monster/Robo_Boss/BossTower.bin"), CModel::MODEL_ANIM, PreTransformMatrix))))
        return E_FAIL;

    /* Prototype_Component_Model_TestBox*/
    PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
    lstrcpyW(m_szLoadingText, TEXT("Monster Model 원형 객체 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_TestBox"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/Monster/ItemBox/ItemBox.fbx", g_hWnd, TEXT("../Bin/ModelFiles/Monster/ItemBox/ItemBox.bin"), CModel::MODEL_ANIM, PreTransformMatrix))))
        return E_FAIL;


    /* Prototype_GameObject_TempCollider*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TempCollider"), CTempCollider::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_Component_VIBuffer_Cube*/
    lstrcpy(m_szLoadingText, TEXT("MapTool 용 Terrain_Buffer 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_VIBuffer_Cube"), CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_Component_Shader_VtxCube*/
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxCube"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE::tagElements, VTXCUBE::iNumElements))))
        return E_FAIL;

#pragma region TEST_BUILDINGS
    /* Prototype_Component_Shader_VtxMesh */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Shader_VtxMesh"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxMesh.hlsl"), VTXMESH::tagElements, VTXMESH::iNumElements))))
        return E_FAIL;

    /* Prototype_GameObject_TestBuilding */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestBigWall"), CTestWall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_GameObject_TestBigBigWall */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestBigBigWall"), CTestWall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_GameObject_TestPillar */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestPillar"), CTestWall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_GameObject_TestConcrete */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_GameObject_TestConcrete"), CTestWall::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    /* Prototype_Component_Model_TestBigWall */
    PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_TestBigWall"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/NonModel/Environment/Wall/Wall2.fbx", g_hWnd, TEXT("../Bin/DataFiles/BuildingFiles/BigWall.bin"), CModel::MODEL_NONANIM, PreTransformMatrix))))
        return E_FAIL;

    /* Prototype_Component_Model_TestBigBigWall */
    PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_TestBigBigWall"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/NonModel/Environment/BigWall/BigWall3.fbx", g_hWnd, TEXT("../Bin/DataFiles/BuildingFiles/BigBigWall.bin"), CModel::MODEL_NONANIM, PreTransformMatrix))))
        return E_FAIL;

    /* Prototype_Component_Model_TestPillar */
    PreTransformMatrix = XMMatrixScaling(0.00001f, 0.00001f, 0.00001f);
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_TestPillar"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/NonModel/Environment/Pillar/Pillar.fbx", g_hWnd, TEXT("../Bin/DataFiles/BuildingFiles/Pillar.bin"), CModel::MODEL_NONANIM, PreTransformMatrix))))
        return E_FAIL;

    /* Prototype_Component_Model_TestConcrete */
    PreTransformMatrix = XMMatrixScaling(0.0001f, 0.0001f, 0.0001f);
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_MAPTOOL, TEXT("Prototype_Component_Model_TestConcrete"), CModel::Create_ForLoad(m_pDevice, m_pContext, "../Bin/Resources/Models/NonModel/Environment/ConcreteWall/ConcreteWall2.fbx", g_hWnd, TEXT("../Bin/DataFiles/BuildingFiles/Concrete.bin"), CModel::MODEL_NONANIM, PreTransformMatrix))))
        return E_FAIL;

#pragma endregion



    lstrcpy(m_szLoadingText, TEXT("로딩 끝"));
    m_bIsFinished = true;

    return S_OK;
}

CLoader* CLoader::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVELID _eNextLevelID)
{
    CLoader* pInstance = new CLoader(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_eNextLevelID)))
    {
        MSG_BOX("Failed to Create : CLoader");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLoader::Free()
{
    __super::Free();

    // 로딩 중에 강제로 종료하면, 창파괴는 되지만, 프로세스는 구동 중이라서, memory leak으로 나올 때까지 기다리겠다는 의미. 
    WaitForSingleObject(m_hThread, INFINITE);

    DeleteObject(m_hThread);

    CloseHandle(m_hThread);

    DeleteCriticalSection(&m_CriticalSection);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
