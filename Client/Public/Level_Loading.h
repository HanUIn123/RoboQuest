#pragma once

#include "Client_Defines.h"
#include "Level.h"

// 1. 다음 씬이 필요로 하는 자원을 준비해야 한다.
// 2. 로딩화면 자체를 구성한다는 것을 보여줘야 한다. ( CPU 에서 처리를 하는데, 이는 쓰레드를 이용해서 처리 ) 

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
    CLevel_Loading(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CLevel_Loading() = default;

public:
    virtual HRESULT Initialize(LEVELID _eNextLevelID);
    virtual void    Update(_float _fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    LEVELID                 m_eNextLevelID = { LEVEL_END };
    class CLoader*          m_pLoader = { nullptr };

private:
    HRESULT Ready_Layer_StartBackGround(const _tchar* _pLayerTags);
    //HRESULT Ready_Layer_MousePosition(const _tchar* _pLayerTags);
    HRESULT Loading_StartBackGround();


    _tchar					            m_szLoadingText[MAX_PATH] = {};
public:
    // 다른 level클래스들과 다르게 Create 자체에서, 앞으로 로딩 다음, 어떠한 레벨에서는 어떤 객체가 필요하다 라는 개념이 필요하므로,
    // 추가적으로 인자에 levelID를 받아준다. (어떤 씬의 자원인지에 대한 인덱스를 의미)
    // 따라서 Create가 조금 변형되어서, 이 클래스의 Initialize는 Level 부모클래스의 Initialize 함수의 오버라이딩을 해제한다. 
    static CLevel_Loading* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LEVELID _eNextLevelID);
    virtual void Free() override;

};

END