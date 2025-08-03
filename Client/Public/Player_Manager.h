#pragma once
#include "Client_Defines.h"
#include "Player.h"
#include "Base.h"

BEGIN(Client)

class CPlayer_Manager : public CBase
{
    DECLARE_SINGLETON(CPlayer_Manager)

private:
    CPlayer_Manager();
    virtual ~CPlayer_Manager() = default;

public:
 
    struct PLAYER_DATA
    {
        _vector vPosition;
        _vector vLook;
        _vector vUp;
        _vector vRight;

        _uint iState;
        _bool bJumping;
        _float fJumpSpeed;
        _float3 vJumpingHeight;

        _bool bIsShootingState;
        _bool bReLoading;
        _float fShootingAnimationTime;

        _int iHp;

        _uint iAmmoCount; 

        _bool bPlayerCameraActive;
    };


    void Save_PlayerData(CPlayer* pPlayer);


    void Load_PlayerData(CPlayer* pPlayer);


    const PLAYER_DATA& Get_PlayerData() const { return m_PlayerData; }

private:
    PLAYER_DATA m_PlayerData; 

public:
    virtual void Free() override;
};
END
