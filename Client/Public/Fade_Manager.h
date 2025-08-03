#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "FadeObject.h"

BEGIN(Engine)
class CGameInstance;
END

BEGIN(Client)
class CFade_Manager : public CBase
{
    DECLARE_SINGLETON(CFade_Manager)
        
public:
    CFade_Manager();
    virtual ~CFade_Manager();

private:
    CGameInstance* m_pGameInstance = { nullptr };

public:
    void                    Start_FadeIn(_float _fDuration);
    void                    Start_FadeOut(_float _fDuration);
    void                    Update(_float _fDeltaTime);
    void                    Render();

    void                    Set_Collide(_bool _bCollided) { m_bCollided = _bCollided; }

private:
    _bool                    m_bIsFading = false;
    _bool                    m_bIsFadingIn = true;
    _float                   m_fAlpha = 1.0f;
    _float                   m_fFadeTime = 0.0f;
    _float                   m_fDuration = 1.0f;

    CFadeObject*            m_pFadeObject = { nullptr };

    ID3D11Device*           m_pDevice = { nullptr };
    ID3D11DeviceContext*    m_pContext = { nullptr };

    _bool                   m_bCollided = { false };

private:
    virtual void            Free() override;
};
END
