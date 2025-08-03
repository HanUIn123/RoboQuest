#include "pch.h"
#include "../Public/Level_Loading.h"

#include "GameInstance.h"
#include "Loader.h"
#include "Level_Logo.h"
#include "Level_GamePlay.h"
#include "Level_Boss.h"
#include "Level_MapTool.h"
#include "StartBackGround.h"
#include "Bus.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CLevel(_pDevice, _pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVELID _eNextLevelID)
{

    m_pGameInstance->PlayBGM(L"LoadingSong.wav", 0.1f);
    m_eNextLevelID = _eNextLevelID;

    Loading_StartBackGround();

    //ShowCursor(false);

    if (FAILED(Ready_Layer_StartBackGround(TEXT("Layer_StartBackGround"))))
        return E_FAIL;

    m_pLoader = CLoader::Create(m_pDevice, m_pContext, _eNextLevelID);
    if (nullptr == m_pLoader)
        return E_FAIL;

    return S_OK;
}

void CLevel_Loading::Update(_float _fTimeDelta)
{
    // 이곳에서 이제 GameInstance를 추가하여, OpenLevel를 통해, 씬 전환도 하는 내용 추가.
    //if (GetKeyState(VK_SPACE) < 0)
    //m_pGameInstance->Press_Once(DIK_SPACE);
    //if ((m_pGameInstance->Press_Once(DIK_SPACE)))
    if (m_pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
    {
        if (true == m_pLoader->IsFinished())
        {
            switch (m_eNextLevelID)
            {
            case LEVEL_LOGO:
                m_pGameInstance->PlayBGM(L"ST_Music_Credits.wav", 0.1f);
                m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_Logo::Create(m_pDevice, m_pContext));
                break;
            case LEVEL_GAMEPLAY:
                m_pGameInstance->StopSound(SOUND_BGM);

                m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_GamePlay::Create(m_pDevice, m_pContext));
                break;
            case LEVEL_BOSS:
                m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_Boss::Create(m_pDevice, m_pContext));
                break;
            case LEVEL_MAPTOOL:
                m_pGameInstance->StopSound(SOUND_BGM);
                m_pGameInstance->Open_Level(m_eNextLevelID, CLevel_MapTool::Create(m_pDevice, m_pContext));
                break;
            }
        }
    }
}

HRESULT CLevel_Loading::Render()
{
    m_pLoader->SetUp_WindowText();

    if (m_pLoader->IsFinished())
    {
        m_pGameInstance->Render_Font(TEXT("Noto_San_KR"), TEXT("아무 키나 누르세요"), _float2(700.0f, 600.0f));

    }
    else
    {
        m_pGameInstance->Render_Font(TEXT("Noto_San_KR"), TEXT("로딩 중..."), _float2(1000.0f, 600.0f));
    }

    return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_StartBackGround(const _tchar* _pLayerTags)
{


    CStartBackGround::STBACKGROUND_DESC     Desc{};

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_LOADING, TEXT("Prototype_GameObject_StartBackGround"), LEVEL_LOADING, _pLayerTags, &Desc)))
        return E_FAIL;

    CBus::BUS_DESC     BusDesc{};

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_LOADING, TEXT("Prototype_GameObject_Bus"), LEVEL_LOADING, _pLayerTags, &BusDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CLevel_Loading::Loading_StartBackGround()
{
    //-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    lstrcpy(m_szLoadingText, TEXT("Texture원형을 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_BackGround"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Level_ImageBackGround/Loading_Level/Loading_BackGround.png"), 1))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_Bus"), CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/MyTextures/Level_ImageBackGround/Loading_Level/T_Bus_Carcass.png"), 1))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("Shader 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Shader_VtxPosTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::tagElements, VTXPOSTEX::iNumElements))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("BackGround Rect Buffer 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    lstrcpy(m_szLoadingText, TEXT("BackGround 원형 객체를 생성하는 중.."));
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_GameObject_StartBackGround"), CStartBackGround::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_GameObject_Bus"), CBus::Create(m_pDevice, m_pContext))))
        return E_FAIL;
    // 자원 로딩 종료
    lstrcpy(m_szLoadingText, TEXT("로딩 끝"));
    return S_OK;
}


CLevel_Loading* CLevel_Loading::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVELID _eNextLevelID)
{
    CLevel_Loading* pInstance = new CLevel_Loading(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_eNextLevelID)))
    {
        MSG_BOX("Failed to Created : CLevel_Loading");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Loading::Free()
{
    __super::Free();

    Safe_Release(m_pLoader);
}
