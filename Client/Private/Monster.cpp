#include "pch.h"
#include "../Public/Monster.h"
#include "GameInstance.h"

#include "Player.h"
#include "Player_Camera.h"
#include "BasicTerrain.h"
CMonster::CMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CContainerObject{ _pDevice, _pContext }
{

}


CMonster::CMonster(const CMonster& _Prototype)
    :CContainerObject(_Prototype)
{
}

HRESULT CMonster::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster::Initialize(void* _pArg)
{
    MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(_pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, TEXT("Layer_Player")));

    m_pBasicTerrain = static_cast<CBasicTerrain*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_BasicTerrain"), LEVEL_GAMEPLAY, TEXT("Layer_BasicTerrain")));

    return S_OK;
}

void CMonster::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CMonster::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}

void CMonster::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT CMonster::Render()
{
    return S_OK;
}

HRESULT  CMonster::Setting_Direction_To_Target(_float _fTimeDelta, _vector _vMonsterPos)
{
    _vector vMonsterLook = {};
    _vector vMonsterRight = {};
    _vector vMonsterUp = {};
    
    vMonsterUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
    vMonsterLook = (m_pPlayer->Get_PlayerPosition() - _vMonsterPos);
    vMonsterRight = XMVector3Cross(vMonsterUp, vMonsterLook);
    vMonsterLook = XMVector3Cross(vMonsterRight, vMonsterUp);
    
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vMonsterLook));
    m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vMonsterUp));
    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vMonsterRight));

    return S_OK;
}

HRESULT CMonster::Setting_Direction_To_Target_Lerp(_float _fTimeDelta, _vector _vMonsterPos)
{
    // 현재 보스의 Look 벡터
    _vector vCurrentLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

    // 목표 Look 벡터 (플레이어를 향하는 방향)
    _vector vTargetLook = m_pPlayer->Get_PlayerPosition() - _vMonsterPos;

    // Y축 방향으로 기울어지는 것 방지 (XZ 평면에서만 회전)
    vTargetLook = XMVectorSetY(vTargetLook, 0.0f);
    vTargetLook = XMVector3Normalize(vTargetLook);

    // Lerp 적용하여 부드럽게 회전
    _float fRotationSpeed = 3.0f; // 회전 속도 조절
    _vector vNewLook = XMVectorLerp(vCurrentLook, vTargetLook, _fTimeDelta * fRotationSpeed);
    vNewLook = XMVector3Normalize(vNewLook);

    // 항상 Up 벡터는 (0,1,0) 유지 (보스가 눕는 걸 방지)
    _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);

    // Right 벡터 계산 (Look과 Up의 크로스)
    _vector vRight = XMVector3Cross(vUp, vNewLook);
    vRight = XMVector3Normalize(vRight);

    // Up 벡터 다시 보정
    _vector vAdjustedUp = XMVector3Cross(vNewLook, vRight);
    vAdjustedUp = XMVector3Normalize(vAdjustedUp);

    // Transform 적용
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, vNewLook);
    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
    m_pTransformCom->Set_State(CTransform::STATE_UP, vAdjustedUp);

    return S_OK;
}

_vector CMonster::Setting_MissileDirection_To_Target(_vector _vMonsterPos, _float _fOffset, _float _fWidthOffset)
{
    m_fMissile_Direction_Height_Offset = _fOffset;

    if (m_pPlayer)
    {
        _vector vPlayerPosition = m_pPlayer->Get_PlayerPosition();
        vPlayerPosition = XMVectorSetY(vPlayerPosition, XMVectorGetY(vPlayerPosition) - m_fMissile_Direction_Height_Offset);

        _vector vDirection = XMVector3Normalize(vPlayerPosition - _vMonsterPos);

        _vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), vDirection));
        vDirection = XMVector3Normalize(vDirection + vRight * _fWidthOffset);

        return vDirection;
    }
    return XMVectorZero();
}

_vector CMonster::Setting_MissilePosition_To_Target(_vector _vMonsterPos, _float _fHeightOffset, _float _fWidthOffset)
{
    m_fMissile_Position_Offset.x = _fWidthOffset;
    m_fMissile_Position_Offset.y = _fHeightOffset;

    _vector vMonsterPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    //XMStoreFloat4(&vMonsterPosition, _vMonsterPos);
    //XMStoreFloat4(&vMonsterPosition , )

    return _vector();
}

_float CMonster::Compute_Monster_Position_Terrain(_vector _vMonsterPosition)
{
    _float fTerrainHeight = {};

    if (nullptr != m_pBasicTerrain)
    {
        _float fPosX = XMVectorGetX(_vMonsterPosition);
        _float fPosY = XMVectorGetY(_vMonsterPosition);
        _float fPosZ = XMVectorGetZ(_vMonsterPosition);
        fTerrainHeight = m_pBasicTerrain->Compute_Height({ fPosX, fPosY, fPosZ });
    }
    return fTerrainHeight;
}

_float CMonster::Compute_Distance_To_Target()
{
    if (!m_pPlayer)
        return FLT_MAX;

    _vector vPlayerPos = m_pPlayer->Get_PlayerPosition();
    _vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    return XMVectorGetX(XMVector3Length(vPlayerPos - vMonsterPos));
}

_vector CMonster::Compute_Target_Position()
{
    if (m_pPlayer)
    {
        return m_pPlayer->Get_PlayerPosition();
    }

    return XMVectorZero();
}

void CMonster::LerpMove_Before_Face_To_Target(_float _fTimeDelta, _vector _vMonsterPos)
{


    _vector vTargetPosition = Compute_Target_Position();
    _vector vDirectionToTarget = XMVector3Normalize(vTargetPosition - _vMonsterPos);
    _vector vCurrentLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
    _vector vNewLook = XMVector3Normalize(XMVectorLerp(vCurrentLook, vDirectionToTarget, _fTimeDelta * 5.0f));

    _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    _vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vNewLook));
    vUp = XMVector3Normalize(XMVector3Cross(vNewLook, vRight));

    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
    m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, vNewLook);

}

void CMonster::Memorize_Position_Before_DectectMode(_vector _vMonsterPosition)
{

}

_vector CMonster::Get_PlayerCameraLook()
{
    if (m_pPlayer)
    {
        CPlayer_Camera* pPlayerCamera = m_pPlayer->Get_PlayerCamera();
        if (pPlayerCamera)
        {
            return pPlayerCamera->Get_LookVector();
        }
    }
    return XMVectorZero();
}

_vector CMonster::Get_PlayerTargetLook(const _vector& vMonsterPosition)
{
    if (m_pPlayer)
    {
        _vector vPlayerPosition = m_pPlayer->Get_PlayerPosition();

        vPlayerPosition = XMVectorSetY(vPlayerPosition, XMVectorGetY(vPlayerPosition) - 0.7f);

        return XMVector3Normalize(vPlayerPosition - vMonsterPosition);
    }
    return XMVectorZero();
}

//void CMonster::Be_Hited()
//{
//
//}



void CMonster::Free()
{
    __super::Free();
}
