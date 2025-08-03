#include "pch.h"
#include "Player_Manager.h"
#include "GameInstance.h"
#include "Player.h"

IMPLEMENT_SINGLETON(CPlayer_Manager)

CPlayer_Manager::CPlayer_Manager()
{
    ZeroMemory(&m_PlayerData, sizeof(PLAYER_DATA));
}

void CPlayer_Manager::Save_PlayerData(CPlayer* pPlayer)
{
    if (nullptr == pPlayer)
        return;

    m_PlayerData.vPosition = pPlayer->Get_PlayerPosition();
    m_PlayerData.vLook = pPlayer->Get_PlayerLook();
    m_PlayerData.vUp = pPlayer->Get_PlayerUp();
    m_PlayerData.vRight = XMVector3Cross(m_PlayerData.vUp, m_PlayerData.vLook);

    m_PlayerData.iState = pPlayer->Get_PlayerState(); 

    m_PlayerData.bJumping = pPlayer->IsJumping(); 
    m_PlayerData.fJumpSpeed = pPlayer->Get_JumpSpeed(); 
    m_PlayerData.vJumpingHeight = pPlayer->Get_JumpHeight();

    m_PlayerData.bIsShootingState = pPlayer->IsShooting(); 
    m_PlayerData.bReLoading = pPlayer->IsReloading(); 
    m_PlayerData.fShootingAnimationTime = pPlayer->Get_ShootingAnimationTime(); 

    //m_PlayerData.iHp = pPlayer->Get_Health(); 

    m_PlayerData.iAmmoCount = pPlayer->Get_AmmoCount(); 

    m_PlayerData.bPlayerCameraActive = pPlayer->Get_PlayerCamera()->IsActive(); 
}

void CPlayer_Manager::Load_PlayerData(CPlayer* pPlayer)
{
    if (nullptr == pPlayer)
        return;

    pPlayer->Set_PlayerPosition(m_PlayerData.vPosition); 
    pPlayer->Set_PlayerLook(m_PlayerData.vLook);
    pPlayer->Set_PlayerUp(m_PlayerData.vUp); 
    pPlayer->Set_PlayerRight(m_PlayerData.vRight); 

    pPlayer->Set_PlayerState(m_PlayerData.iState);


    pPlayer->Set_Jumping(m_PlayerData.bJumping); 
    pPlayer->Set_JumpSpeed(m_PlayerData.fJumpSpeed);
    pPlayer->Set_JumpHeight(m_PlayerData.vJumpingHeight);

  
    pPlayer->Set_Shooting(m_PlayerData.bIsShootingState);
    pPlayer->Set_Reloading(m_PlayerData.bReLoading); 
    pPlayer->Set_ShootingAnimationTime(m_PlayerData.fShootingAnimationTime); 

    //pPlayer->Set_Health(m_PlayerData.iHp); 

    pPlayer->Set_AmmoCount(m_PlayerData.iAmmoCount);

    pPlayer->Get_PlayerCamera()->Set_Active(m_PlayerData.bPlayerCameraActive); 
}

void CPlayer_Manager::Free()
{

}