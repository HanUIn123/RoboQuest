#include "pch.h"
#include "../Public/Level_Logo.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "BackGround.h"
#include "MouseCursor.h"

// 자식에서, 부모생성자가 불리는 클래스의 규칙에 의해서, 필수적으로 부모생성자의 디바이스와 context를 써줄 필요가 있다.
CLevel_Logo::CLevel_Logo(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CLevel(_pDevice, _pContext)
{
    // 부모클래스 level에서 addref하고 있고, 해제 / discount 또한 하는 중. 
}

HRESULT CLevel_Logo::Initialize()
{
    if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_Logo::Update(_float _fTimeDelta)
{
    if (GetAsyncKeyState(VK_RETURN) & 0x8000)
    {
        // 일단 LEVEL_LOADING 을 하고나서, 그 이후에, 실질적인, LEVEL_GAMEPLAY 를 할당 할 것 이다.
        //levelchanging = true;
        m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_GAMEPLAY));
        return;
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_M) & 0x80)
    {
        //levelchanging = true;
         m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_MAPTOOL));
         return;
    }
}

HRESULT CLevel_Logo::Render()
{
#ifdef _DEBUG
    SetWindowText(g_hWnd, TEXT("로고레벨입니다."));
#endif

    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_MouseCursor(const _tchar* _pLayerTags)
{
    CMouseCursor::CURSOR_DESC           Desc{};

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_STATIC, TEXT("Prototype_GameObject_MouseCursor"), LEVEL_LOGO, _pLayerTags, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Logo::Ready_Layer_BackGround(const _tchar* _pLayerTags)
{
    CBackGround::BACKGROUND_DESC            Desc{};

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_LOGO, TEXT("Prototype_GameObject_BackGround"), LEVEL_LOGO, _pLayerTags, &Desc)))
    {
        return E_FAIL;
    }

    return S_OK;
}

CLevel_Logo* CLevel_Logo::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CLevel_Logo* pInstance = new CLevel_Logo(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_Logo");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CLevel_Logo::Free()
{
    __super::Free();
}
