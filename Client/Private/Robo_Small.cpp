#include "pch.h"
#include "Robo_Small.h"

#include "GameInstance.h"
#include "Player.h"

CRobo_Small::CRobo_Small(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CMonster{ _pDevice, _pContext }
{
}

CRobo_Small::CRobo_Small(const CMonster& _Prototype)
    :CMonster(_Prototype)
{
}

HRESULT CRobo_Small::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRobo_Small::Initialize(void* _pArg)
{
    ROBO_SMALL_DESC* pDesc = static_cast<ROBO_SMALL_DESC*>(_pArg);

    pDesc->fSpeedPerSec = 2.0f;

    m_iHp = 40;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_iState = STATE_SMALL_IDLE;

    // Monster Position 값 Loading 해온 값으로 세팅
    _vector vMonsterPos = XMLoadFloat3(&pDesc->vMonsterpos);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vMonsterPos, 1.0f));

    // Monster Rotation 값 Loading 해온 값으로 세팅
    _matrix matRotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&pDesc->vMonsterRotation));
    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, matRotation.r[0]);
    m_pTransformCom->Set_State(CTransform::STATE_UP, matRotation.r[1]);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, matRotation.r[2]);

    // Monster Scale 값 Loading 해온 값으로 세팅
    m_pTransformCom->Scaling(pDesc->vMonsterScale);

    m_fShakeTimer = 1.0f;
    m_fDetectTimer = 2.0f;

    m_pNavigationCom->Set_CurrentNaviIndex(vMonsterPos);

    return S_OK;
}

void CRobo_Small::Priority_Update(_float _fTimeDelta)
{
    m_fTargetDistance = Compute_Distance_To_Target();

    Robo_Small_MoveState(_fTimeDelta);

    _vector vMonsterPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _float fPosX = XMVectorGetX(vMonsterPosition);
    _float fPosZ = XMVectorGetZ(vMonsterPosition);

    _float fNavigationHeight = m_pNavigationCom->Compute_Height(vMonsterPosition);
    _float fResultY = (fNavigationHeight > 0.0f) ? (Compute_Monster_Position_Terrain(vMonsterPosition)) : (fNavigationHeight);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosX, fResultY, fPosZ, 1.0f));

    __super::Priority_Update(_fTimeDelta);
}

void CRobo_Small::Update(_float _fTimeDelta)
{
    switch (m_iState)
    {
    case STATE_SMALL_IDLE:
        Setting_IdleState(_fTimeDelta);
        break;
    case STATE_SMALL_SHOOT:
        Setting_ShootState(_fTimeDelta);
        break;
    case STATE_SMALL_SHAKE:
        //Setting_ShakeState(_fTimeDelta);
        break;
    case STATE_SMALL_RUN:
        Setting_RunState(_fTimeDelta);
        break;
    case STATE_SMALL_DETECT:
        Setting_DetectState(_fTimeDelta);
        break;
    }

    if (m_pBody_Robo_Small->IsDissolving())
    {
        if (m_pBody_Robo_Small->Get_DissolveTime() >= 0.8f)
        {
            m_pGameInstance->Delete_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), this);
            return;
        }
    }


    if (m_pSmallExplosion->IsExplosion_Finished())
    {
        //m_pGameInstance->Delete_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), this);
        return;
    }

    auto WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    m_pColliderCom[COLL_AABB]->Update(WorldMatrix);
    m_pColliderCom[COLL_SPHERE]->Update(WorldMatrix);

    __super::Update(_fTimeDelta);
}

void CRobo_Small::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);

    CCollider* pTargetCollider = static_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), 0));
    m_bDetect_Target = m_pColliderCom[COLL_SPHERE]->Intersect(pTargetCollider);

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CRobo_Small::Render()
{
#ifdef _DEBUG
    //m_pNavigationCom->Render();
    //m_pColliderCom[COLL_AABB]->Render();
    //m_pColliderCom[COLL_SPHERE]->Render();
#endif
    return S_OK;
}

void CRobo_Small::Robo_Small_MoveState(_float _fTimeDelta)
{
    if (m_iState == STATE_SMALL_SHAKE)
        return;

    if (m_bDetect_Target)
    {
        if (m_fTargetDistance > 2.5f)
            m_iState = STATE_SMALL_RUN;
        else
            m_iState = STATE_SMALL_DETECT;
    }
    else if (!(m_iState & STATE_SMALL_RUN))
    {
        m_iState = STATE_SMALL_IDLE;
    }
}

void CRobo_Small::Setting_IdleState(_float _fTimeDelta)
{
    m_fIdleTime -= _fTimeDelta;

    if (m_fIdleTime <= 0.0f)
    {
        m_fIdleTime = 3.0f;

        m_fSelfMoveCoolTime = 2.0f;

        SelfMove_Randomly();

        m_iState = STATE_SMALL_RUN;
    }
}

void CRobo_Small::Setting_RunState(_float _fTimeDelta)
{
    if (m_bDetect_Target)
    {
        if (m_fTargetDistance < 2.0f)
        {
            m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));
            return;
        }

        //LerpMove_Before_Face_To_Target(_fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
        //
        //m_pTransformCom->Go_Straight(_fTimeDelta, m_pNavigationCom);

        if (SUCCEEDED(Setting_Direction_To_Target_Lerp(_fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
        {
            m_pTransformCom->Go_Straight(_fTimeDelta, m_pNavigationCom);
        }

        //return;
    }
    else
    {
        _vector vCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        _vector vDirection = XMVector3Normalize(m_vRandomPosition - vCurrentPosition);

        _vector vLook = vDirection;
        _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
        _vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
        vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

        m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
        m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
        m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
    }
    m_fSelfMoveCoolTime -= _fTimeDelta;

    _vector vCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    // 랜덤 이동
    _vector vDirection = XMVector3Normalize(m_vRandomPosition - vCurrentPosition);
    _vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
    //_vector vNewLook = XMVector3Normalize(XMVectorLerp(vLook, vDirection, 0.5f * _fTimeDelta));

    _vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
    //_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vNewLook));
    _vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
    m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
    //m_pTransformCom->Set_State(CTransform::STATE_LOOK, vNewLook);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
    m_pTransformCom->Go_Straight(_fTimeDelta, m_pNavigationCom);

    if (XMVectorGetX(XMVector3Length(m_vRandomPosition - vCurrentPosition)) < 0.5f)
    {
        m_iState = STATE_SMALL_IDLE;
        m_fIdleTime = 3.0f;
    }
    else if (m_pNavigationCom->bIsOn_Line(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
    {
        //m_pTransformCom->Set_State(CTransform::STATE_LOOK, (-1.0f) * vNewLook);
        //m_pTransformCom->Set_State(CTransform::STATE_LOOK, (-1.0f) * vLook);

        m_iState = STATE_SMALL_IDLE;
    }
}

void CRobo_Small::Setting_ShootState(_float _fTimeDelta)
{
}

void CRobo_Small::Setting_ShakeState(_float _fTimeDelta)
{
    m_fShakeTimer -= _fTimeDelta;

    if (m_fShakeTimer <= 0.0f)
    {
        m_fShakeTimer = 1.0f;

        m_pSmallExplosion->Set_Activate_Effect(true);

        return;
    }
}

void CRobo_Small::Setting_DetectState(_float _fTimeDelta)
{
    m_fDetectTimer -= _fTimeDelta;

    if (m_fDetectTimer <= 0.0f)
    {
        if (SUCCEEDED(Setting_Direction_To_Target_Lerp(_fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
        {
            _vector vTargetPosition = Compute_Target_Position();
            _vector vCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
            _vector vDirection = XMVector3Normalize(vTargetPosition - vCurrentPosition);

            _vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
            //_vector vNewLook = XMVector3Normalize(XMVectorLerp(vLook, vDirection, 0.5f * _fTimeDelta));

            _vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
            //_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vNewLook));
            _vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

            m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
            m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
            //m_pTransformCom->Set_State(CTransform::STATE_LOOK, vNewLook);
            m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
            m_pTransformCom->Go_Straight(_fTimeDelta, m_pNavigationCom);

            if (XMVectorGetX(XMVector3Length(vTargetPosition - vCurrentPosition)) < 4.5f)
            {
                m_fDetectTimer = 20.0f;

                CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, TEXT("Layer_Player")));
                m_pGameInstance->Play(L"ST_Healbot_Use_09_15_2023.wav", NPC_GLAD, 0.1f);
                pPlayer->Heal();
                m_iState = STATE_SMALL_IDLE;
                m_bDetect_Target = false;
                return;
            }
        }
    }
}

void CRobo_Small::SelfMove_Randomly()
{
    _vector vCurrentMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    _float fRandomX = ((rand() % 2001) - 1000) / 100.0f;
    _float fRandomZ = ((rand() % 2001) - 1000) / 100.0f;

    m_vRandomPosition = XMVectorSet(XMVectorGetX(vCurrentMonsterPos) + fRandomX, XMVectorGetY(vCurrentMonsterPos), XMVectorGetZ(vCurrentMonsterPos) + fRandomZ, 1.0f);

    _float fTerrainHeight = Compute_Monster_Position_Terrain(m_vRandomPosition);
    m_vRandomPosition = XMVectorSetY(m_vRandomPosition, fTerrainHeight);
}


_bool CRobo_Small::Check_Collision_With_Ray(const _vector& _vRayStart, const _vector& _vRayDirection, _float& _fDistance)
{
    if (nullptr == m_pColliderCom[COLL_AABB])
        return false;

    return m_pColliderCom[COLL_AABB]->Intersect_Ray(_vRayStart, _vRayDirection, _fDistance);
}

void CRobo_Small::Be_Hited(_uint _iCurrentGunDamage)
{
    if (m_iHp > 0)
    {

        m_pHpBar_Small->Show_HealthBar(true);

        m_iHp = max(0, m_iHp - 10);  // 예제: 데미지 10
        m_pHpBar_Small->Decrease_Hp(m_iHp);

        // 체력바 쭈욱쭉떨어지는 연출
        m_pHpBar_Small->Update(0.016f);

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

        if (m_pBody_Robo_Small)
            m_pBody_Robo_Small->Activate_HittedEffect();

        if (m_iHp <= 0 && !m_pBody_Robo_Small->IsDissolving())
        {
            m_iState = STATE_SMALL_SHAKE;
            m_pBody_Robo_Small->Activate_DissolveEffect();
            m_pHpBar_Small->Show_HealthBar(false);
            return;
        }
    }


}

HRESULT CRobo_Small::Ready_Component()
{
    /* Com_Navigation */
    CNavigation::NAVIGATION_DESC		NaviDesc{};

    NaviDesc.iCurrentCellIndex = 0;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;




    /* Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.40f, 0.55f, 0.40f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom[COLL_AABB]), &ColliderDesc)))
        return E_FAIL;


    CBounding_Sphere::BOUNDING_SPHERE_DESC  ColliderSphereDesc{};
    ColliderSphereDesc.fRadius = 5.0f;
    ColliderSphereDesc.vCenter = _float3(0.f, Compute_Monster_Position_Terrain(m_pTransformCom->Get_State(CTransform::STATE_POSITION)), 0.f);
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider_SPHERE"), reinterpret_cast<CComponent**>(&m_pColliderCom[COLL_SPHERE]), &ColliderSphereDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRobo_Small::Ready_PartObjects()
{
#pragma region BODY_SMALL
    // BODY_ROBO_BOOM을 만들자. 
    CBody_Robo_Small::BODY_SMALL_DESC       BodyDesc{};
    BodyDesc.pParentState = &m_iState;
    BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    BodyDesc.fSpeedPerSec = 0.0f;
    BodyDesc.fRotationPerSec = 0.0f;

    if (FAILED(__super::Add_PartObject(TEXT("Part_SmallBody"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Body_RoboSmall"), &BodyDesc)))
        return E_FAIL;

    m_pBody_Robo_Small = static_cast<CBody_Robo_Small*>(Find_PartObject(TEXT("Part_SmallBody")));
    if (nullptr == m_pBody_Robo_Small)
        return E_FAIL;
#pragma endregion


#pragma region SMALL_EXPLOSION
    // SMALL Explosion 을 만들자 
    CSmall_Explosion::SMALL_EFFECT_DESC     ExploDesc{};

    CModel* pBodyModelCom = dynamic_cast<CModel*>(__super::Find_PartObject_Component(TEXT("Part_SmallBody"), TEXT("Com_Model")));
    if (pBodyModelCom == nullptr)
        return E_FAIL;

    ExploDesc.pSocketMatrix = pBodyModelCom->Get_BoneMatrix("Head");
    ExploDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    if (FAILED(__super::Add_Effect_PartObject(TEXT("Part_SmallExplosion"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Small_Explosion"), &ExploDesc)))
        return E_FAIL;

    m_pSmallExplosion = static_cast<CSmall_Explosion*>(Find_Effect_PartObject(TEXT("Part_SmallExplosion")));

    if (nullptr == m_pSmallExplosion)
        return E_FAIL;
#pragma endregion

#pragma region HPBAR
    // 몬스터의 체력바를 만들자. 
    CHpBar_Robo_Small::SMALL_HPBAR_DESC HpBarDesc{};
    HpBarDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    HpBarDesc.pMonsterHp = &m_iHp;
    _float fOffset = (g_iWinSizeY * 0.5f);
    HpBarDesc.fY = fOffset - (fOffset * 0.004f);


    if (FAILED(__super::Add_UIPartObject(TEXT("Part_HpBar_RoboSmall"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_HpBar_RoboSmall"), &HpBarDesc)))
        return E_FAIL;

    m_pHpBar_Small = static_cast<CHpBar_Robo_Small*>(Find_UIPartObject(TEXT("Part_HpBar_RoboSmall")));

    if (nullptr == m_pHpBar_Small)
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
        DamageNumDesc.fY = fOffset - (fOffset * 0.006f);

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

CMonster* CRobo_Small::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CRobo_Small* pInstance = new CRobo_Small(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CRobo_Small");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CRobo_Small::Clone(void* _pArg)
{
    CRobo_Small* pInstance = new CRobo_Small(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CRobo_Small");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRobo_Small::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pColliderCom[COLL_AABB]);
    Safe_Release(m_pColliderCom[COLL_SPHERE]);
}
