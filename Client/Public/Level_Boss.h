#pragma once
#include "Client_Defines.h"
#include "Level.h"
#include "Load_Manager.h"
#include "Player_Manager.h"

BEGIN(Client)
class CLevel_Boss final : public CLevel
{
private:
    CLevel_Boss(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CLevel_Boss() = default;

public:
    virtual HRESULT     Initialize() override;
    virtual void        Update(_float _fTimeDelta) override;
    virtual HRESULT     Render() override;

private:
    HRESULT             Ready_Light();
    HRESULT             Ready_Layer_BossTerrain(const _tchar* _pLayerTags);
    HRESULT             Ready_Layer_BackGround(const _tchar* _pLayerTags);
    HRESULT             Ready_Layer_Building(const _tchar* _pLayerTags);

    HRESULT             Ready_Layer_Camera_Free(const _tchar* _pLayerTags);
    HRESULT             Ready_Layer_Player(const _tchar* _pLayerTags);
    HRESULT             Ready_Layer_PlayerHpBar(const _tchar* _pLayerTags);

    HRESULT             Ready_Layer_Boss(const _tchar* _pLayerTags);

    HRESULT             Ready_Layer_MouseCursor(const _tchar* _pLayerTags);
    HRESULT             Ready_Layer_UI(const _tchar* _pLayerTags);
    HRESULT             Ready_Layer_Bullet(const _tchar* _pLayerTags);
    HRESULT             Ready_Layer_Missile(const _tchar* _pLayerTags);
    HRESULT             Ready_Layer_Effect(const _tchar* _pLayerTag);

public:
    static CLevel_Boss* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual void        Free() override;
};
END