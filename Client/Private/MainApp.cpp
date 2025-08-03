#include "pch.h"
#include "../Public/MainApp.h"
#include "GameInstance.h"
#include "Level_Loading.h"

CMainApp::CMainApp() 
    : m_pGameInstance{CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
}


HRESULT CMainApp::Initialize()
{
//#ifdef _DEBUG
//
//    if (::AllocConsole() == TRUE)
//    {
//        FILE* nfp[3];
//        freopen_s(nfp + 0, "CONOUT$", "rb", stdin);
//        freopen_s(nfp + 1, "CONOUT$", "wb", stdout);
//        freopen_s(nfp + 2, "CONOUT$", "wb", stderr);
//        ios::sync_with_stdio();
//    }
//#endif // _DEBUG

    ENGINE_DESC EngineDesc{};

    EngineDesc.hInstance = g_hInst;
    EngineDesc.hWnd = g_hWnd; 
    EngineDesc.iNumLevels = LEVEL_END;
    EngineDesc.isWindowed = true;
    EngineDesc.iViewportWidth = g_iWinSizeX;
    EngineDesc.iViewportHeight = g_iWinSizeY;

    if (FAILED(m_pGameInstance->Initialize_Engine(EngineDesc, &m_pDevice, &m_pContext)))
        return E_FAIL;

    if (FAILED(Ready_Prototype_For_Static()))
        return E_FAIL;

    if (FAILED(Sample_Function()))
        return E_FAIL;

    if (FAILED(Create_Fonts()))
        return E_FAIL;

    if(FAILED(Start_Level(LEVEL_LOGO)))
        return E_FAIL;



    return S_OK;
}

void CMainApp::Update(_float _fTimeDelta)
{
    CImGui_Manager::GetInstance()->ImGui_Tick();

    // CGameInstance클래스에서 만들어두었던, Update_Engine함수를 통해서, 내 로딩의 render 와 update가 무한하게 돌아가고 있음.
    m_pGameInstance->Update_Engine(_fTimeDelta);



#ifdef _DEBUG
    m_fTimerAccumulate += _fTimeDelta;
#endif

}

HRESULT CMainApp::Render()
{
    if (FAILED(m_pGameInstance->Render_Begin(_float4(0.f, 0.f, 1.f, 1.f))))
        return E_FAIL;  

    // GameInstance 클래스에 만들어놨던 Draw함수를 호출해서, 모아둔 객체들의 Update 및 Draw기능을 실행시켜준다.
    m_pGameInstance->Draw();

#ifdef _DEBUG
    ++m_iDrawCount;

    if (1.0f <= m_fTimerAccumulate)
    {
        wsprintf(m_szFPS, TEXT("FPS : %d"), m_iDrawCount);
        m_fTimerAccumulate = 0.0f;
        m_iDrawCount = 0;
    }
    //SetWindowText(g_hWnd, m_szFPS);

    //m_pGameInstance->Render_Font(TEXT("Noto_San_KR"), m_szFPS, _float2(0.0f, 0.0f));


#endif

    CImGui_Manager::GetInstance()->ImGui_Render();

    m_pGameInstance->Render_End();

    return S_OK;
}

HRESULT CMainApp::Sample_Function()
{
    // Cell 임시로 만들어보기. 
    //_ulong          dwByte = {};
    //HANDLE          hFile = CreateFile(TEXT("../Bin/DataFiles/Navigation.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    // Cell 들을 만든다.
    //_float3         vPoints[3] = {};

    //vPoints[0] = _float3(0.0f, 2.0f, 20.0f);
    //vPoints[1] = _float3(20.0f, 2.0f, 0.0f);
    //vPoints[2] = _float3(0.0f, 2.0f, 0.0f);
    //WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
    //
    //vPoints[0] = _float3(0.0f, 2.0f, 20.0f);
    //vPoints[1] = _float3(20.0f, 10.0f, 20.0f);
    //vPoints[2] = _float3(20.0f, 2.0f, 0.0f);
    //WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
    //
    //vPoints[0] = _float3(0.0f, 2.0f, 40.0f);
    //vPoints[1] = _float3(20.0f, 10.0f, 20.0f);
    //vPoints[2] = _float3(0.0f, 2.0f, 20.0f);
    //WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
    //
    //vPoints[0] = _float3(20.0f, 10.0f, 20.0f);
    //vPoints[1] = _float3(40.0f, 2.0f, 0.0f);
    //vPoints[2] = _float3(20.0f, 2.0f, 0.0f);
    //WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

    //vPoints[0] = _float3(0.f, 2.f, 20.f);
    //vPoints[1] = _float3(20.f, 2.f, 0.f);
    //vPoints[2] = _float3(0.f, 2.f, 0.f);
    //WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
    //
    //vPoints[0] = _float3(0.f, 2.f, 20.f);
    //vPoints[1] = _float3(20.f, 7.f, 20.f);
    //vPoints[2] = _float3(20.f, 2.f, 0.f);
    //WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
    //
    //vPoints[0] = _float3(0.f, 2.f, 40.f);
    //vPoints[1] = _float3(20.f, 7.f, 20.f);
    //vPoints[2] = _float3(0.f, 2.f, 20.f);
    //WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);
    //
    //vPoints[0] = _float3(20.f, 7.f, 20.f);
    //vPoints[1] = _float3(40.f, 2.f, 0.f);
    //vPoints[2] = _float3(20.f, 2.f, 0.f);
    //WriteFile(hFile, vPoints, sizeof(_float3) * 3, &dwByte, nullptr);

    //CloseHandle(hFile);

#pragma region 텍스쳐2D 생성
    ///* 텍스쳐를 메모리공간상에 할당한다. */
    //ID3D11Texture2D* pTexture2D = { nullptr };

    //D3D11_TEXTURE2D_DESC			TextureDesc{};

    //TextureDesc.Width = 256;
    //TextureDesc.Height = 256;
    //TextureDesc.MipLevels = 1;
    //TextureDesc.ArraySize = 1;
    //TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    //TextureDesc.SampleDesc.Quality = 0;
    //TextureDesc.SampleDesc.Count = 1;
    //TextureDesc.Usage = D3D11_USAGE_STAGING;
    //TextureDesc.BindFlags = 0;
    //TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
    //TextureDesc.MiscFlags = 0;

    //_uint			iNumPixels = TextureDesc.Width * TextureDesc.Height;

    //_uint* pPixels = new _uint[iNumPixels];
    //ZeroMemory(pPixels, sizeof(_uint) * iNumPixels);

    //D3D11_SUBRESOURCE_DATA		InitialDesc{};
    //InitialDesc.pSysMem = pPixels;
    //InitialDesc.SysMemPitch = 4 * TextureDesc.Width;

    //if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &InitialDesc, &pTexture2D)))
    //    return E_FAIL;

    //D3D11_MAPPED_SUBRESOURCE	SubResource{};

    ///* 텍스체에 값을 채운다. 수정 변경한다. */
    //m_pContext->Map(pTexture2D, 0, D3D11_MAP_READ_WRITE, 0, &SubResource);

    //_uint* pMappedPixel = static_cast<_uint*>(SubResource.pData);

    //for (_uint i = 0; i < TextureDesc.Height; i++)
    //{
    //    for (_uint j = 0; j < TextureDesc.Width; j++)
    //    {
    //        _uint		iIndex = i * TextureDesc.Width + j;

    //        if (j < 128)
    //            pMappedPixel[iIndex] = 0xffffffff;
    //        else
    //            pMappedPixel[iIndex] = 0xff0000ff;
    //    }
    //}



    //m_pContext->Unmap(pTexture2D, 0);

    ///* 텍스쳐를 파일로 저장한다. */
    //if (FAILED(SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../Bin/Resources/MyTextures/Terrain/WOW.dds"))))
    //    return E_FAIL;

    //Safe_Release(pTexture2D);
    //
    //Safe_Delete_Array(pPixels);
#pragma endregion

    return S_OK;

}


HRESULT CMainApp::Start_Level(LEVELID _eStartLevelID)
{
    if (LEVEL_LOADING == _eStartLevelID)
    {
        return E_FAIL;
    }

    if (FAILED(m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, _eStartLevelID))))
        return E_FAIL;

    return S_OK;
}

HRESULT CMainApp::Create_Fonts()
{
    if (FAILED(m_pGameInstance->Add_Font(TEXT("Noto_San_KR"), TEXT("../Bin/Resources/Fonts/148ex.spritefont2"))))
        return E_FAIL;
    
    return S_OK;
}

HRESULT CMainApp::Ready_Prototype_For_Static()
{
    /* For. Prototype_Component_Shader_VtxPosTex */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"), CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::tagElements, VTXPOSTEX::iNumElements))))
        return E_FAIL;

    /* For. Prototype_Component_VIBuffer_Rect */
    if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
        return E_FAIL;

    return S_OK;
}

CMainApp* CMainApp::Create()
{
    CMainApp* pInstance = new CMainApp();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed To Created : CMainApp");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMainApp::Free()
{
    /* 부모의 멤버를 정리하라고 이야기한다. */
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);

    m_pGameInstance->Release_Engine();

    Safe_Release(m_pGameInstance);
}
