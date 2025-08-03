#pragma once
#include "Client_Defines.h"
#include "Level.h"
#include "Load_Manager.h"
#include "Player_Manager.h"
#include "Player.h"

BEGIN(Client)
class CLevel_GamePlay final : public CLevel
{
private:
    CLevel_GamePlay(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CLevel_GamePlay() = default;

public:
    virtual HRESULT Initialize() override;
    virtual void    Update(_float _fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    HRESULT             Ready_Light();
    HRESULT             Ready_Layer_BasicTerrain(const _tchar* _pLayerTags);
    HRESULT             Ready_Layer_BackGround(const _tchar* _pLayerTags);
    HRESULT             Ready_Layer_Building(const _tchar* _pLayerTags);

    HRESULT             Ready_Layer_Camera_Free(const _tchar* _pLayerTags);

    HRESULT             Ready_Layer_PlayerEffect(const _tchar* _pLayerTags);
    HRESULT             Ready_Layer_Player(const _tchar* _pLayerTags);

    HRESULT             Ready_Layer_Fade(const _tchar* _pLayerTags);

    HRESULT             Ready_Layer_PlayerHpBar(const _tchar* _pLayerTags);
    
    HRESULT             Ready_Layer_BossHpBar(const _tchar* _pLayerTag);
    HRESULT             Ready_Layer_Monster(const _tchar* _pLayerTags);
    //HRESULT             Ready_Layer_AnimObject(const _tchar* _pLayerTags);

    HRESULT             Ready_Layer_MouseCursor(const _tchar* _pLayerTags);
    HRESULT             Ready_Layer_UI(const _tchar* _pLayerTags);
    HRESULT             Ready_Layer_Bullet(const _tchar* _pLayerTags);
    HRESULT             Ready_Layer_Missile(const _tchar* _pLayerTags);
    HRESULT             Ready_Layer_Laser(const _tchar* _pLayerTag);

    HRESULT             Ready_Layer_Item(const _tchar* _pLayerTag);
    HRESULT             Ready_Layer_FrameEffect(const _tchar* _pLayerTag);


    HRESULT             Ready_Layer_LCCollider(const _tchar* _pLayerTag);
    HRESULT             Ready_Layer_DashCube(const _tchar* _pLayerTag);
    //HRESULT             Ready_Layer_Effect(const _tchar* pLayerTag);

private:
    CPlayer*            m_pPlayer = { nullptr };
    _uint               m_iStageIndex = {};

public:
    static CLevel_GamePlay* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual void Free() override;
};

END

