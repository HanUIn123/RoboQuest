#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "ImGui_Manager.h"

BEGIN(Engine)
// 여기서 전방선언을 하지 않고, class CMainApp 저 안에다가 하면, 
// Client의 CGameInstance라고 인식되어버림. 필요한 것은 Engine의 CGameInstance이므로..
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
    CMainApp();
    virtual ~CMainApp() = default;

public:
    HRESULT                     Initialize();
    void                        Update(_float _fTimeDelta);
    HRESULT                     Render();

private:
    CGameInstance*              m_pGameInstance = { nullptr };
    ID3D11Device*               m_pDevice = { nullptr };
    ID3D11DeviceContext*        m_pContext = { nullptr };


#ifdef _DEBUG
private:
    _float                      m_fTimerAccumulate = {};
    _uint                       m_iDrawCount = {};
    _tchar                      m_szFPS[MAX_PATH] = {};
#endif

public:
    HRESULT                     Sample_Function();
    HRESULT                     Start_Level(LEVELID _eStartLevelID);
    HRESULT                     Create_Fonts();

    HRESULT                     Ready_Prototype_For_Static();

public:
    static CMainApp*            Create();
    virtual void                Free() override;
};

END
