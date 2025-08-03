#pragma once

#include "Client_Defines.h"
#include "Load_Manager.h"
#include "Base.h"


BEGIN(Engine)
class CGameInstance;
END


BEGIN(Client)

class CLoader final : public CBase
{
private:
    CLoader(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CLoader() = default;

public:
    HRESULT Initialize(LEVELID  _eNextLevelID);
    HRESULT Start_Loading();
    _bool   IsFinished() const { return m_bIsFinished; }

#ifdef _DEBUG
public: 
    void SetUp_WindowText();
#endif

private:
    // �� LoaderŬ������ ���������� ����̽��� Context�� �ʿ��ϱ⿡,
    ID3D11Device*                       m_pDevice = { nullptr };
    ID3D11DeviceContext*                m_pContext = { nullptr };
    CGameInstance*                      m_pGameInstance = { nullptr };

    LEVELID                             m_eNextLevelID = { LEVEL_END };

    HANDLE                              m_hThread = {};
    CRITICAL_SECTION                    m_CriticalSection = {};

    _bool					            m_bIsFinished = { false };
    _tchar					            m_szLoadingText[MAX_PATH] = {};

private:
    HRESULT                             Loading_For_Level_Logo();
    HRESULT                             Loading_For_Level_GamePlay();
    HRESULT                             Loading_For_Level_Boss();
    HRESULT                             Loading_For_Level_MapTool();


public:
    static CLoader*                     Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVELID _eNextLevelID);
    virtual void                        Free() override;
};

END