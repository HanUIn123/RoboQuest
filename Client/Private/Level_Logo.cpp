#include "pch.h"
#include "../Public/Level_Logo.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "BackGround.h"
#include "MouseCursor.h"

// �ڽĿ���, �θ�����ڰ� �Ҹ��� Ŭ������ ��Ģ�� ���ؼ�, �ʼ������� �θ�������� ����̽��� context�� ���� �ʿ䰡 �ִ�.
CLevel_Logo::CLevel_Logo(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CLevel(_pDevice, _pContext)
{
    // �θ�Ŭ���� level���� addref�ϰ� �ְ�, ���� / discount ���� �ϴ� ��. 
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
        // �ϴ� LEVEL_LOADING �� �ϰ���, �� ���Ŀ�, ��������, LEVEL_GAMEPLAY �� �Ҵ� �� �� �̴�.
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
    SetWindowText(g_hWnd, TEXT("�ΰ����Դϴ�."));
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
