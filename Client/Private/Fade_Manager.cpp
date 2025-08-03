#include "pch.h"
#include "Fade_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CFade_Manager)

CFade_Manager::CFade_Manager()
    : m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
}

CFade_Manager::~CFade_Manager()
{
}

void CFade_Manager::Start_FadeIn(_float _fDuration)
{
    m_bIsFading = true;
    m_bIsFadingIn = true;
    m_fFadeTime = 0.0f;
    m_fDuration = _fDuration;
    m_fAlpha = 1.0f;
}

void CFade_Manager::Start_FadeOut(_float _fDuration)
{
    if (m_bIsFading) 
        return;

    m_bIsFading = true;
    m_bIsFadingIn = false;
    m_fFadeTime = 0.0f;
    m_fDuration = _fDuration;
    m_fAlpha = 0.0f;
}

void CFade_Manager::Update(_float _fDeltaTime)
{
    if (nullptr == m_pFadeObject)
    {
        m_pFadeObject = static_cast<CFadeObject*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Black"), LEVEL_GAMEPLAY, TEXT("Layer_Fade")));
    }

    if (!m_pFadeObject)
        return;

    if (m_bCollided)
    {
        m_fAlpha += _fDeltaTime * 0.5f; 
        if (m_fAlpha > 1.0f)
            m_fAlpha = 1.0f;
    }
    else
    {
        m_fAlpha -= _fDeltaTime * 0.5f;  
        if (m_fAlpha < 0.0f)
            m_fAlpha = 0.0f;
    }

    m_pFadeObject->Set_Alpha(m_fAlpha);
}

void CFade_Manager::Render()
{
    if (!m_pFadeObject)
        return;

    m_pFadeObject->Render();
}

void CFade_Manager::Free()
{
    Safe_Release(m_pGameInstance);

    __super::Free();
}
