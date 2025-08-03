#include "pch.h"
#include "Robo_Goliath.h"
#include "GameInstance.h"

//#include "Missile_Goliath.h"

CRobo_Goliath::CRobo_Goliath(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CMonster{ _pDevice, _pContext }
{
}

CRobo_Goliath::CRobo_Goliath(const CMonster& _Prototype)
    :CMonster(_Prototype)
{
}

HRESULT CRobo_Goliath::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRobo_Goliath::Initialize(void* _pArg)
{
    ROBO_GOLIATH_DESC* pDesc = static_cast<ROBO_GOLIATH_DESC*>(_pArg);

    pDesc->fSpeedPerSec = 1.5f;
    //pDesc->fSpeedPerSec = 0.0f;

    m_fMissile_Height_Offset = 0.8f;
    m_fMissile_Width_Offset = -0.05f;
    m_iHp = 400;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_iState = STATE_GOL_IDLE;

    // Monster Position 값 Loading 해온 값으로 세팅ㄱㄱㄱ
    _vector vMonsterPos = XMLoadFloat3(&pDesc->vMonsterpos);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vMonsterPos, 1.0f));

    // Monster Rotation 값 Loading 해온 값으로 세팅ㄱㄱㄱ
    _matrix matRotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&pDesc->vMonsterRotation));
    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, matRotation.r[0]);
    m_pTransformCom->Set_State(CTransform::STATE_UP, matRotation.r[1]);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, matRotation.r[2]);

    // Monster Scale 값 Loading 해온 값으로 세팅ㄱㄱㄱ
    m_pTransformCom->Scaling(pDesc->vMonsterScale);

    //m_pNavigationCom->Setting_ShrinkCell(1.0f);

    m_pNavigationCom->Set_CurrentNaviIndex(vMonsterPos);

    m_fStunTime = 1.5f;

    //m_pScopeScreen = static_cast<CScopeScreen*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_ScopeScreen"), LEVEL_GAMEPLAY, TEXT("Layer_PlayerEffect")));
    //if (!m_pScopeScreen)
    //    return E_FAIL;

    m_pBreakEffect = static_cast<CBreakEffect*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Break"), LEVEL_GAMEPLAY, TEXT("Layer_PlayerEffect")));
    if (!m_pBreakEffect)
        return E_FAIL;

    return S_OK;
}

void CRobo_Goliath::Priority_Update(_float _fTimeDelta)
{
    m_fTargetDistance = Compute_Distance_To_Target();

    if (m_bMortal_Mode && m_iState == STATE_GOL_MORTAL_SHOOT)
    {
        m_fMortalShootTime -= _fTimeDelta;
        if (m_fMortalShootTime <= 0.0f)
        {
            m_bMortal_Mode = false;
            if (m_bDetect_Target)
            {
                m_iState = STATE_GOL_DASH;
                m_bDash_Mode = true;
                m_fDashTime = 0.0f;
            }
            else
            {
                m_iState = STATE_GOL_IDLE;
            }
        }
    }

    Robo_Goliath_MoveState(_fTimeDelta);

    _vector vMonsterPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _float fPosX = XMVectorGetX(vMonsterPosition);
    _float fPosZ = XMVectorGetZ(vMonsterPosition);

    _float fNavigationHeight = m_pNavigationCom->Compute_Height(vMonsterPosition);
    _float fResultY = (fNavigationHeight > 0.0f) ? (Compute_Monster_Position_Terrain(vMonsterPosition)) : (fNavigationHeight);


    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosX, fResultY, fPosZ, 1.0f));

    __super::Priority_Update(_fTimeDelta);
}

void CRobo_Goliath::Update(_float _fTimeDelta)
{
    _vector vMonsterPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    switch (m_iState)
    {
    case STATE_GOL_DASH:
        Setting_DashState(_fTimeDelta);
        break;

    case STATE_GOL_MISSILE_SHOT:
        Setting_ShootState(_fTimeDelta);
        break;

    case STATE_GOL_IDLE:
        Setting_IdleState(_fTimeDelta);
        break;

    case STATE_GOL_MORTAL_IN:
        Setting_Ready_MortalShoot(_fTimeDelta);
        break;

    case STATE_GOL_MORTAL_SHOOT:
        Setting_Fire_MortalShoot(_fTimeDelta);

        break;

    case STATE_GOL_RUN_FRONT:
        Setting_RunState(_fTimeDelta);
        break;

    case STATE_GOL_STUN:
        Setting_StunState(_fTimeDelta);
        break;
    }

    Shoot_Missiles(_fTimeDelta);
    Shoot_MortalMissiles(_fTimeDelta, Compute_Target_Position());

    if (m_pBody_Robo_Goliath->IsDissolving())
    {
        if (m_pBody_Robo_Goliath->Get_DissolveTime() >= 0.8f)
        {
            m_pGameInstance->Delete_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), this);
            return;
        }
    }

    auto WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    m_pColliderCom[COLL_AABB]->Update(WorldMatrix);
    m_pColliderCom[COLL_SPHERE]->Update(WorldMatrix);

    __super::Update(_fTimeDelta);
}

void CRobo_Goliath::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);

    //CCollider* pTargetCollider = static_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), 0));
    m_pTargetCollider = static_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), 0));
    m_bDetect_Target = m_pColliderCom[COLL_SPHERE]->Intersect(m_pTargetCollider);

    Late_Update_Missiles(_fTimeDelta);
    Late_Update_MortalMissiles(_fTimeDelta);

    if (m_bDetect_Target)
    {
        m_fDashTime += _fTimeDelta;
        if (!m_bMortal_Mode)
        {
            m_fMortalShootTime += _fTimeDelta;
        }

        if (m_fDashTime >= 10.0f)
            m_bDash_Mode = true;

        if (m_fMortalShootTime >= 5.0f && !m_bMortal_Mode)
        {
            m_bMortal_Mode = true;
            m_fMortalShootTime = 0.0f;
        }
    }

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CRobo_Goliath::Render()
{
#ifdef _DEBUG
    //m_pNavigationCom->Render();
    //m_pColliderCom[COLL_AABB]->Render();
    //m_pColliderCom[COLL_SPHERE]->Render();
#endif
    return S_OK;
}

void CRobo_Goliath::Setting_IdleState(_float _fTimeDelta)
{
    m_pMortalExplosion->DeActivate_Effect();

    m_fIdleTime -= _fTimeDelta;

    if (m_fIdleTime <= 0.0f)
    {
        m_bMortal_Mode = false;
        m_fIdleTime = 2.0f;
        m_fSelfMoveCoolTime = 2.0f;
        SelfMove_Randomly();
        m_iState = STATE_GOL_RUN_FRONT;
    }
}

void CRobo_Goliath::Setting_RunState(_float _fTimeDelta)
{
    m_pMortalExplosion->DeActivate_Effect();

    if (m_bDetect_Target)
    {
        //LerpMove_Before_Face_To_Target(_fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
        //m_pTransformCom->Go_Straight(_fTimeDelta, m_pNavigationCom);


        if (SUCCEEDED(Setting_Direction_To_Target(_fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
        {
            m_pTransformCom->Go_Straight(_fTimeDelta, m_pNavigationCom);
        }
        return;
    }

    m_fSelfMoveCoolTime -= _fTimeDelta;

    _vector vCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    _vector vDirection = XMVector3Normalize(m_vRandomPosition - vCurrentPosition);
    _vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
    _vector vNewLook = XMVector3Normalize(XMVectorLerp(vLook, vDirection, 0.5f * _fTimeDelta));

    _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
    _vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vNewLook));
    vUp = XMVector3Normalize(XMVector3Cross(vNewLook, vRight));

    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
    m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, vNewLook);
    m_pTransformCom->Go_Straight(_fTimeDelta, m_pNavigationCom);

    if (XMVectorGetX(XMVector3Length(m_vRandomPosition - vCurrentPosition)) < 2.0f)
    {
        m_iState = STATE_GOL_IDLE;
        m_fIdleTime = 2.0f;
    }
    else if (m_pNavigationCom->bIsOn_Line(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
    {
        _vector vNewLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
        vNewLook = (-1.0f) * vNewLook;

        _vector vUp = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP));

        _vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vNewLook));

        vUp = XMVector3Normalize(XMVector3Cross(vNewLook, vRight));

        m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
        m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
        m_pTransformCom->Set_State(CTransform::STATE_LOOK, vNewLook);

        //m_iState = STATE_GOL_IDLE;
        //m_fIdleTime = 2.0f;
    }
}

void CRobo_Goliath::Setting_DashState(_float _fTimeDelta)
{
    m_pMortalExplosion->DeActivate_Effect();

    m_pTransformCom->Go_Straight(_fTimeDelta * 8.0f, m_pNavigationCom);

    if (m_pColliderCom[COLL_AABB]->Intersect(m_pTargetCollider))
    {
        //Hit_Target();

        m_pBreakEffect->Activate_BreakEffect(true);
        m_pBreakEffect->Set_Alpha(1.0f);
        m_pPlayer->Get_PlayerCamera()->Start_CameraShake(0.5f, 0.5f, 10.0f);

        m_pPlayer->Be_Hited(10);
        m_iState = STATE_GOL_STUN;
        m_fStunTime = 1.0f;
        m_bDash_Mode = false;
        m_fDashTime = 0.0f;
    }


    if (m_pNavigationCom->bIsOn_Line(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
    {
        m_iState = STATE_GOL_STUN;
        m_fStunTime = 1.0f;       // Stun 상태 지속 시간
        m_bDash_Mode = false;     // Dash 모드 종료

        m_bDetect_Target = false;

        m_fDashTime = 0.0f;
    }
}

void CRobo_Goliath::Setting_ShootState(_float _fTimeDelta)
{
    if (SUCCEEDED(Setting_Direction_To_Target(_fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
    {
        if (m_fShootCoolTime <= 0.0f)
        {
            Activate_Missile();
            m_fShootCoolTime = 0.5f;
        }
        else
        {
            m_fShootCoolTime -= _fTimeDelta;
        }
    }
}

void CRobo_Goliath::Setting_Ready_MortalShoot(_float _fTimeDelta)
{
    if (SUCCEEDED(Setting_Direction_To_Target(_fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
    {
        // 여기서 Delay Time 을 좀 줘야 될듯.
        // 일단 m_fReadyMortalTime 가 0.3 보다 작같 해야, 실행되는데
        if (m_fReadyMortalTime <= 0.3f)
        {
            if (m_pBody_Robo_Goliath->IsAnimationFinished())
            {
                m_iState = STATE_GOL_MORTAL_SHOOT;

                // 대포 발사 애니상태로 전환하고나서 m_fReadyMortalTime  를 0.5로 올려버림.
                m_fReadyMortalTime = 0.5f;
            }
        }
        else
        {
            // 0.3 보다 크면 줄이기 시작.
            // 그럼 대포발사 애니할 나오고 있을 때 얘는 줄어들고 잇겟지?
            m_fReadyMortalTime -= _fTimeDelta;
        }

    }
}

void CRobo_Goliath::Setting_Fire_MortalShoot(_float _fTimeDelta)
{
    if (m_fMortalShootTime > 0.0f)
    {
        Activate_MortalMissile();
    }
    else
    {
        m_bMortal_Mode = false;

        if (m_bDetect_Target)
        {
            m_iState = STATE_GOL_DASH;
            m_bDash_Mode = true;
            m_fDashTime = 0.0f;

            // 대포 발사 모드 끝나고 DASH 로 갈 때 한 0.2정도로 줘버려.
            m_fReadyMortalTime = 0.2f;
        }
        else
        {
            m_iState = STATE_GOL_IDLE;
            // IDLE 모드 끝나고 DASH 로 갈 때 한 0.2정도로 줘버려.
            m_fReadyMortalTime = 0.2f;
        }
    }
}

void CRobo_Goliath::Setting_StunState(_float _fTimeDelta)
{
    m_fStunTime -= _fTimeDelta;
    if (m_fStunTime <= 0.0f)
    {
        if(m_iHp > 0)
            m_iState = STATE_GOL_IDLE;
    }
}

void CRobo_Goliath::Shoot_Missiles(_float _fTimeDelta)
{
    for (auto& pMissile : m_listMissile)
    {
        if (pMissile->IsActiveBullet())
        {
            pMissile->Update(_fTimeDelta);
        }
    }
}

void CRobo_Goliath::Shoot_MortalMissiles(_float _fTimeDelta, _vector _fTargetPosition)
{
    for (auto& pMissile : m_listMortal_Missile)
    {
        if (pMissile->IsActiveBullet())
        {
            pMissile->Update(_fTimeDelta, _fTargetPosition);
        }
    }
}

void CRobo_Goliath::Late_Update_Missiles(_float _fTimeDelta)
{
    for (auto& pMissile : m_listMissile)
    {
        if (pMissile->IsActiveBullet())
        {
            pMissile->Late_Update(_fTimeDelta);
        }
    }
}

void CRobo_Goliath::Late_Update_MortalMissiles(_float _fTimeDelta)
{
    for (auto& pMissile : m_listMortal_Missile)
    {
        if (pMissile->IsActiveBullet())
        {
            pMissile->Late_Update(_fTimeDelta);
        }
    }
}

void CRobo_Goliath::Robo_Goliath_MoveState(_float _fTimeDelta)
{
    if (m_iState == STATE_GOL_STUN)
        return;

    if (m_bDash_Mode)
    {
        m_iState = STATE_GOL_DASH;
        return;
    }

    if (m_bMortal_Mode)
    {
        m_fMortalShootTime -= _fTimeDelta;
        if (m_fMortalShootTime <= 0.0f)
        {
            m_bMortal_Mode = false;
            if (m_bDetect_Target)
            {
                m_iState = STATE_GOL_DASH;
                m_bDash_Mode = true;
                m_fDashTime = 0.0f;
            }
            else
            {
                m_iState = STATE_GOL_IDLE;
            }
        }
        return;
    }

    if (m_bDetect_Target)
    {
        if (m_fTargetDistance > 12.0f)
        {
            m_iState = STATE_GOL_RUN_FRONT;
        }
        else if (m_fTargetDistance > 9.0f)
        {
            m_iState = STATE_GOL_MORTAL_IN;
            m_bMortal_Mode = true;
            m_fMortalShootTime = 7.0f; // 대포 지속시간.
        }
        else
        {
            m_iState = STATE_GOL_MISSILE_SHOT;
        }
    }
    else
    {
        if (m_iState != STATE_GOL_RUN_FRONT)
        {
            m_iState = STATE_GOL_IDLE;
        }
    }
}

void CRobo_Goliath::SelfMove_Randomly()
{
    _vector vCurrentMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    _float fRandomX = ((rand() % 2001) - 1000) / 100.0f;
    _float fRandomZ = ((rand() % 2001) - 1000) / 100.0f;

    m_vRandomPosition = XMVectorSet(XMVectorGetX(vCurrentMonsterPos) + fRandomX, XMVectorGetY(vCurrentMonsterPos), XMVectorGetZ(vCurrentMonsterPos) + fRandomZ, 1.0f);

    _float fTerrainHeight = Compute_Monster_Position_Terrain(m_vRandomPosition);
    m_vRandomPosition = XMVectorSetY(m_vRandomPosition, fTerrainHeight);
}

HRESULT CRobo_Goliath::Ready_Component()
{
    /* Com_Navigation */
    CNavigation::NAVIGATION_DESC		NaviDesc{};

    NaviDesc.iCurrentCellIndex = 0;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;

    /* Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
    ColliderDesc.vExtents = _float3(1.5f, 1.5f, 1.5f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom[COLL_AABB]), &ColliderDesc)))
        return E_FAIL;


    CBounding_Sphere::BOUNDING_SPHERE_DESC  ColliderSphereDesc{};
    ColliderSphereDesc.fRadius = 13.0f;
    ColliderSphereDesc.vCenter = _float3(0.f, Compute_Monster_Position_Terrain(m_pTransformCom->Get_State(CTransform::STATE_POSITION)), 0.f);
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider_SPHERE"), reinterpret_cast<CComponent**>(&m_pColliderCom[COLL_SPHERE]), &ColliderSphereDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRobo_Goliath::Ready_PartObjects()
{
#pragma region BODY_GOLIATH
    // BODY_ROBO_BOOM을 만들자. 
    CBody_Robo_Goliath::BODY_GOLIATH_DESC       BodyDesc{};
    BodyDesc.pParentState = &m_iState;
    BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    BodyDesc.fSpeedPerSec = 0.0f;
    BodyDesc.fRotationPerSec = 0.0f;

    if (FAILED(__super::Add_PartObject(TEXT("Part_GoliathBody"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Body_RoboGoliath"), &BodyDesc)))
        return E_FAIL;

    m_pBody_Robo_Goliath = static_cast<CBody_Robo_Goliath*>(Find_PartObject(TEXT("Part_GoliathBody")));
    if (nullptr == m_pBody_Robo_Goliath)
        return E_FAIL;
#pragma endregion

#pragma region MORTAL
    // MORTAL 을 만들자.
    CMortal::MORTAL_DESC MortalDesc{};

    CModel* pBodyModelCom = dynamic_cast<CModel*>(__super::Find_PartObject_Component(TEXT("Part_GoliathBody"), TEXT("Com_Model")));
    if (nullptr == pBodyModelCom)
        return E_FAIL;

    MortalDesc.pSocketMatrix = pBodyModelCom->Get_BoneMatrix("L_Mortar_Front");
    MortalDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();


    if (FAILED(__super::Add_PartObject(TEXT("Part_Mortal"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Mortal"), &MortalDesc)))
        return E_FAIL;

    m_pMortal = static_cast<CMortal*>(Find_PartObject(TEXT("Part_Mortal")));
    if (nullptr == m_pMortal)
        return E_FAIL;
#pragma endregion

#pragma region MORTAL_EFFECT
    // MORTAL EFFECT 를 만들자. 
    CMortal_Explosion::MORTAL_EFFECT_DESC MortalEffect{};
    MortalEffect.pSocketMatrix = pBodyModelCom->Get_BoneMatrix("L_Mortar_Front");
    MortalEffect.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    if (FAILED(__super::Add_Effect_PartObject(TEXT("Part_MortalExplosion"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Mortal_Explosion"), &MortalEffect)))
        return E_FAIL;

    m_pMortalExplosion = static_cast<CMortal_Explosion*>(Find_Effect_PartObject(TEXT("Part_MortalExplosion")));
    if (nullptr == m_pMortalExplosion)
        return E_FAIL;
#pragma endregion


#pragma region MISSILE_GOLIATH
    // MISSILE_GOLIATH 을 만들자. 
    for (_uint i = 0; i < 50; ++i)
    {
        CMissile_Goliath::MISSILE_GOLIATH_DESC MissileDesc{};

        CMissile_Goliath* pMissile = dynamic_cast<CMissile_Goliath*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Missile_Goliath"), &MissileDesc));

        if (!pMissile)
            return E_FAIL;

        m_listMissile.push_back(pMissile);
    }
#pragma endregion

#pragma region MISSILE_MORTAL
    // MISSILE_MORTAL 을 만들자. 
    for (_uint i = 0; i < 1; ++i)
    {
        CMissile_Mortal::MISSILE_MORTAL_DESC MissileDesc{};

        CMissile_Mortal* pMissile = dynamic_cast<CMissile_Mortal*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Missile_Mortal"), &MissileDesc));

        if (!pMissile)
            return E_FAIL;

        m_listMortal_Missile.push_back(pMissile);
    }
#pragma endregion

#pragma region HPBAR
    // 몬스터의 체력바를 만들자. 
    CHpBar_Robo_Goliath::GOLIATH_HPBAR_DESC       HpBarDesc{};
    HpBarDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    HpBarDesc.pMonsterHp = &m_iHp;

    if (FAILED(__super::Add_UIPartObject(TEXT("Part_HpBar_RoboGoliath"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_HpBar_RoboGoliath"), &HpBarDesc)))
        return E_FAIL;

    m_pHpBar_Robo_Goliath = static_cast<CHpBar_Robo_Goliath*>(Find_UIPartObject(TEXT("Part_HpBar_RoboGoliath")));

    if (nullptr == m_pHpBar_Robo_Goliath)
        return E_FAIL;
#pragma endregion



#pragma region DAMAGE_NUMBER
    for (_uint i = 0; i < 3; ++i)
    {
        CDamageNumber::DAMAGE_NUM_DESC      DamageNumDesc{};
        DamageNumDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
        DamageNumDesc.fLifeTime = 1.0f;
        DamageNumDesc.iIndex = i;

        _float fOffset = (g_iWinSizeY * 0.5f);
        DamageNumDesc.fY = fOffset - (fOffset * 0.012f);

        wstring strPartName = L"Part_DamageNumber_" + to_wstring(i);

        if (FAILED(__super::Add_UIPartObject(strPartName.c_str(), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_DamageNumber"), &DamageNumDesc)))
            return E_FAIL;

        m_pDamageNumber[i] = static_cast<CDamageNumber*>(Find_UIPartObject(strPartName.c_str()));
        if (nullptr == m_pDamageNumber[i])
            return E_FAIL;
    }

#pragma endregion


    return S_OK;
}

_bool CRobo_Goliath::Check_Collision_With_Ray(const _vector& _vRayStart, const _vector& _vRayDirection, _float& _fDistance)
{
    if (nullptr == m_pColliderCom[COLL_AABB])
        return false;

    return m_pColliderCom[COLL_AABB]->Intersect_Ray(_vRayStart, _vRayDirection, _fDistance);
}

void CRobo_Goliath::Be_Hited(_uint _iCurrentGunDamage)
{
    if (m_iHp > 0)
    {
        if ((m_iState & STATE_GOL_IDLE) || (m_iState & STATE_GOL_RUN_FRONT) || (m_iState & STATE_GOL_MORTAL_SHOOT) || (m_iState & STATE_GOL_MISSILE_SHOT))
        {
            m_iPrevState = m_iState;
            m_fStunTime = 0.5f;
        }

        m_pHpBar_Robo_Goliath->Show_HealthBar(true);

        m_iHp = max(0, m_iHp - 10);  // 예제: 데미지 10
        m_pHpBar_Robo_Goliath->Decrease_Hp(m_iHp);

        m_pHpBar_Robo_Goliath->Update(0.016f);

        //_uint iDamage = 10;
        _uint iDamage = _iCurrentGunDamage;
        _uint iDigits[3] = { (iDamage / 100) % 10, (iDamage / 10) % 10, iDamage % 10 };

        _uint iStartIndex = (iDigits[0] == 0) ? 1 : 0;

        for (int i = iStartIndex; i < 3; ++i)
        {
            if (m_pDamageNumber[i])
            {
                m_pDamageNumber[i]->Set_DamageNumberDigit(iDigits[i], i - iStartIndex);
                m_pDamageNumber[i]->Set_LifeTime(0.3f);
            }
        }


        if (m_pBody_Robo_Goliath)
            m_pBody_Robo_Goliath->Activate_HittedEffect();


        if (m_iHp <= 0 && !m_pBody_Robo_Goliath->IsDissolving())
        {
            m_iState = STATE_GOL_STUN;
            m_pBody_Robo_Goliath->Activate_DissolveEffect();
            m_pHpBar_Robo_Goliath->Show_HealthBar(false);
            return;
        }

       /* if (m_iHp <= 0)
        {
            m_pGameInstance->Delete_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), this);
        }*/
    }

  
}

void CRobo_Goliath::Activate_Missile()
{
    for (auto& pMissile : m_listMissile)
    {
        if (!pMissile->IsActiveBullet())
        {
            _vector vMuzzlePos = Compute_RightArm_Muzzle();

            _vector vTargetDirection = XMVector3Normalize(Setting_MissileDirection_To_Target(vMuzzlePos, 0.0f));

            m_pGameInstance->PlayOnce(L"ST_Goliath_Mortar_Cast.wav", MONSTER_SHOOT, 0.2f);
            pMissile->Activate_Missile(vMuzzlePos, vTargetDirection);

            return;
        }
    }
}

void CRobo_Goliath::Activate_MortalMissile()
{
    for (auto& pMissile : m_listMortal_Missile)
    {
        if (!pMissile->IsActiveBullet())
        {
            _vector vMortalPos = XMLoadFloat4x4(&m_pMortal->Get_CombinedMatrix()).r[3];
            _vector vMortalUp = XMLoadFloat4x4(&m_pMortal->Get_CombinedMatrix()).r[0];

            _vector vTargetPos = Compute_Target_Position();


            m_pMortalExplosion->Set_Activate_Effect(true);

            m_pGameInstance->PlayOnce(L"ST_Goliath_Mortar_Cast.wav", MONSTER_SHOOT, 0.2f);
            pMissile->Activate_MortalMissile(vMortalPos, vMortalUp, vTargetPos);

            return;
        }
    }
}

_vector CRobo_Goliath::Compute_RightArm_Muzzle()
{
    _float3 fRightArmOffset = { 1.8f, 0.8f, 0.0f };

    _matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());

    _vector worldMuzzlePos = XMVector3TransformCoord(XMLoadFloat3(&fRightArmOffset), worldMatrix);

    return worldMuzzlePos;
}

_vector CRobo_Goliath::Compute_Shoulder_Muzzle()
{
    _float3 fShoulderOffset = { 0.8f, 0.8f, 0.0f };

    _matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());

    _vector worldMuzzlePos = XMVector3TransformCoord(XMLoadFloat3(&fShoulderOffset), worldMatrix);

    return worldMuzzlePos;
}

CMonster* CRobo_Goliath::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CRobo_Goliath* pInstance = new CRobo_Goliath(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CRobo_Goliath");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CRobo_Goliath::Clone(void* _pArg)
{
    CRobo_Goliath* pInstance = new CRobo_Goliath(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CRobo_Goliath");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRobo_Goliath::Free()
{
    __super::Free();

    for (auto& pMissile : m_listMissile)
        Safe_Release(pMissile);
    m_listMissile.clear();

    for (auto& pMissile : m_listMortal_Missile)
        Safe_Release(pMissile);
    m_listMortal_Missile.clear();

    //for (auto& pMortalImpact : m_vecMortalImpact)
    //    Safe_Release(pMortalImpact);
    //m_vecMortalImpact.clear();


    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pColliderCom[COLL_AABB]);
    Safe_Release(m_pColliderCom[COLL_SPHERE]);
}
