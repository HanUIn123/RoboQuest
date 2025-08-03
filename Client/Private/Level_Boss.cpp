#include "pch.h"
#include "Level_Boss.h"
#include "GameInstance.h"
#include "Camera_Free.h"


CLevel_Boss::CLevel_Boss(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CLevel{_pDevice, _pContext}
{
}

HRESULT CLevel_Boss::Initialize()
{
    if (FAILED(Ready_Light()))
        return E_FAIL;

    if (FAILED(Ready_Layer_BossTerrain(TEXT("Layer_BossTerrain"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
        return E_FAIL;

    if (FAILED(Ready_Layer_Camera_Free(TEXT("Layer_Camera_Free"))))
        return E_FAIL;

    return S_OK;
}

void CLevel_Boss::Update(_float _fTimeDelta)
{
}

HRESULT CLevel_Boss::Render()
{
    return S_OK;
}

HRESULT CLevel_Boss::Ready_Light()
{
    LIGHT_DESC          tagDesc{};

#pragma region ¹æÇâ¼º±¤¿ø_Directional
    ZeroMemory(&tagDesc, sizeof tagDesc);

    tagDesc.eState = LIGHT_DESC::LT_DIRECTIONAL;

    tagDesc.vDirection = _float4(1.0f, -1.0f, 1.0f, 0.0f);

    tagDesc.vDiffuse = _float4(1.0f, 1.0f, 1.0f, 1.0f);

    tagDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.0f);

    tagDesc.vSpecular = _float4(1.0f, 1.0f, 1.0f, 1.0f);

    if (FAILED(m_pGameInstance->Add_Light(tagDesc)))
        return E_FAIL;
#pragma endregion

    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_BossTerrain(const _tchar* _pLayerTags)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_BOSS, TEXT("Prototype_GameObject_BossTerrain"), LEVEL_BOSS, _pLayerTags, nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_BackGround(const _tchar* _pLayerTags)
{
    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Building(const _tchar* _pLayerTags)
{
    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Camera_Free(const _tchar* _pLayerTags)
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

    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_BOSS, TEXT("Prototype_GameObject_Camera_Free"), LEVEL_BOSS, _pLayerTags, &Desc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Player(const _tchar* _pLayerTags)
{
    if (FAILED(m_pGameInstance->Add_GameObject_To_Layer(LEVEL_BOSS, TEXT("Prototype_GameObject_Player"), LEVEL_BOSS, _pLayerTags, nullptr)))
        return E_FAIL;

    
    static CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_BOSS, TEXT("Layer_Player")));
    //if (!pPlayer)
    //    return;
    
    //m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_BOSS, TEXT("Layer_Player")));


    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_PlayerHpBar(const _tchar* _pLayerTags)
{
    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Boss(const _tchar* _pLayerTags)
{
    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_MouseCursor(const _tchar* _pLayerTags)
{
    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_UI(const _tchar* _pLayerTags)
{
    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Bullet(const _tchar* _pLayerTags)
{
    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Missile(const _tchar* _pLayerTags)
{
    return S_OK;
}

HRESULT CLevel_Boss::Ready_Layer_Effect(const _tchar* _pLayerTag)
{
    return S_OK;
}

CLevel_Boss* CLevel_Boss::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CLevel_Boss* pInstance = new CLevel_Boss(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CLevel_Boss");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLevel_Boss::Free()
{
    __super::Free();
}
