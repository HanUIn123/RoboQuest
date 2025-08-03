#pragma once
#include "Client_Defines.h"
#include "UINumber.h"
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
END


BEGIN(Client)
class CUINumber_Manager : public CBase
{
    DECLARE_SINGLETON(CUINumber_Manager)

public:
    CUINumber_Manager();
    virtual ~CUINumber_Manager();

private:
    CGameInstance* m_pGameInstance = { nullptr };

public:
    HRESULT Initialize();
    HRESULT Add_NumberUI(_uint uiID, CUINumber::UINUMBER_DESC& _Desc);
 
    void Set_DigitAt(_uint uiID, _int _iRealNum);
    void Update(_float _fDeltaTime);
    void Late_Update(_float _fDeltaTime);

    void Render();                             

    void Set_UINumberVisible(_bool _bVisible) { m_bVisible = _bVisible; }

private:
    vector<CUINumber*> m_vecNumbers;               

    _bool m_bVisible = { true };
private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

private:
    map<_uint, vector<CUINumber*>> m_mapNumberUI;


    virtual void Free() override;
};

END
