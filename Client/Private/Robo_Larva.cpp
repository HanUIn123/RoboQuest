#include "pch.h"
#include "Robo_Larva.h"

#include "GameInstance.h"

#include "Player.h"
#include "Player_Camera.h"

CRobo_Larva::CRobo_Larva(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CMonster{ _pDevice, _pContext }
{
}

CRobo_Larva::CRobo_Larva(const CMonster& _Prototype)
    :CMonster(_Prototype)
{
}

HRESULT CRobo_Larva::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRobo_Larva::Initialize(void* _pArg)
{
    ROBO_LARVA_DESC* pDesc = static_cast<ROBO_LARVA_DESC*>(_pArg);

    pDesc->fSpeedPerSec = 1.0f;

    m_iHp = 300;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_iState = STATE_LAR_IDLE;

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

    m_fLaserCoolTime = 0.0f;

    m_pNavigationCom->Set_CurrentNaviIndex(vMonsterPos);

    return S_OK;
}

void CRobo_Larva::Priority_Update(_float _fTimeDelta)
{
    m_fTargetDistance = Compute_Distance_To_Target();

    Robo_Larva_MoveState(_fTimeDelta);

    _vector vMonsterPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _float fPosX = XMVectorGetX(vMonsterPosition);
    _float fPosZ = XMVectorGetZ(vMonsterPosition);

    _float fNavigationHeight = m_pNavigationCom->Compute_Height(vMonsterPosition);
    _float fResultY = (fNavigationHeight > 0.0f) ? (Compute_Monster_Position_Terrain(vMonsterPosition)) : (fNavigationHeight);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosX, fResultY, fPosZ, 1.0f));

    __super::Priority_Update(_fTimeDelta);
}

void CRobo_Larva::Update(_float _fTimeDelta)
{
    _vector vLarvaPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    switch (m_iState)
    {
    case STATE_LAR_IDLE:
        Setting_IdleState(_fTimeDelta);
        break;
    case STATE_LAR_MOVE:
        Setting_RunState(_fTimeDelta);
        break;
    case STATE_LAR_LASER_READY:
        break;
    case STATE_LAR_LASER_LOOP:
        Setting_Fire_ShootState(_fTimeDelta);
        break;
    case STATE_LAR_BACKSHOT_READY:
        break;
    case STATE_LAR_BACKSHOT_LOOP:
        break;
    }

    Fire_Laser(_fTimeDelta);

    if (m_pBody_Robo_Larva->IsDissolving())
    {
        if (m_pBody_Robo_Larva->Get_DissolveTime() >= 0.8f)
        {
            m_pGameInstance->Delete_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), this);
            return;
        }
    }


    // 충돌체 업데이트
    auto WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    m_pColliderCom[COLL_AABB]->Update(WorldMatrix);
    m_pColliderCom[COLL_SPHERE]->Update(WorldMatrix);

    __super::Update(_fTimeDelta);
}

void CRobo_Larva::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);

    CCollider* pTargetCollider = static_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), 0));
    m_bDetect_Target = m_pColliderCom[COLL_SPHERE]->Intersect(pTargetCollider);

    Late_Update_Laser(_fTimeDelta);

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CRobo_Larva::Render()
{
#ifdef _DEBUG
    //m_pNavigationCom->Render();
    //m_pColliderCom[COLL_AABB]->Render();
    //m_pColliderCom[COLL_SPHERE]->Render();
#endif
    return S_OK;
}

void CRobo_Larva::Robo_Larva_MoveState(_float _fTimeDelta)
{
    if (m_bDetect_Target)
    {
        if (m_fTargetDistance > 10.0f)
        {
            m_iState = STATE_LAR_MOVE;
        }
        else
            m_iState = STATE_LAR_LASER_LOOP;
    }
    else if (!(m_iState & STATE_LAR_MOVE))
    {
        m_iState = STATE_LAR_IDLE;
    }
}

void CRobo_Larva::Setting_IdleState(_float _fTimeDelta)
{
    m_fIdleTime -= _fTimeDelta;

    if (m_fIdleTime <= 0.0f)
    {
        m_fIdleTime = 3.0f;

        m_fSelfMoveCoolTime = 2.0f;

        SelfMove_Randomly();

        m_iState = STATE_LAR_MOVE;
    }
}

void CRobo_Larva::Setting_RunState(_float _fTimeDelta)
{
    if (m_bDetect_Target)
    {
        if (m_fTargetDistance < 2.0f)
        {
            m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSet(0.f, 0.f, 1.f, 0.f));
            return;
        }

        //LerpMove_Before_Face_To_Target(_fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

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

        _vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
        //_vector vNewLook = XMVector3Normalize(XMVectorLerp(vLook, vDirection, 0.5f * _fTimeDelta));

        _vector vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
        //_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vNewLook));
        _vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));
        //vUp = XMVector3Normalize(XMVector3Cross(vNewLook, vRight));
        vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight));

        m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
        m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
        //m_pTransformCom->Set_State(CTransform::STATE_LOOK, vNewLook);
        m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);
    }
    m_fSelfMoveCoolTime -= _fTimeDelta;

    _vector vCurrentPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    // 랜덤 이동
    _vector vDirection = XMVector3Normalize(m_vRandomPosition - vCurrentPosition);
    _vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
    _vector vNewLook = XMVector3Normalize(XMVectorLerp(vLook, vDirection, 0.5f * _fTimeDelta));

    _vector vUp = m_pTransformCom->Get_State(CTransform::STATE_UP);
    _vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vNewLook));

    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
    m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, vNewLook);
    m_pTransformCom->Go_Straight(_fTimeDelta, m_pNavigationCom);

    if (XMVectorGetX(XMVector3Length(m_vRandomPosition - vCurrentPosition)) < 0.5f)
    {
        m_iState = STATE_LAR_IDLE;
        m_fIdleTime = 3.0f;
    }
    else if (m_pNavigationCom->bIsOn_Line(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
    {
        m_pTransformCom->Set_State(CTransform::STATE_LOOK, (-1.0f) * vNewLook);
    }
}

void CRobo_Larva::Setting_Fire_ShootState(_float _fTimeDelta)
{
    //if (SUCCEEDED(Setting_Direction_To_Target(_fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
    if (SUCCEEDED(Setting_Direction_To_Target_Lerp(_fTimeDelta, m_pTransformCom->Get_State(CTransform::STATE_POSITION))))
    {
        if (m_fLaserCoolTime <= 0.0f)
        {
            Activate_Laser();
            m_fLaserCoolTime = 0.1f;
        }
        else
        {
            m_fLaserCoolTime -= _fTimeDelta;
        }
    }
}

void CRobo_Larva::SelfMove_Randomly()
{
    _vector vCurrentMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    _float fRandomX = ((rand() % 2001) - 1000) / 100.0f;
    _float fRandomZ = ((rand() % 2001) - 1000) / 100.0f;

    m_vRandomPosition = XMVectorSet(XMVectorGetX(vCurrentMonsterPos) + fRandomX, XMVectorGetY(vCurrentMonsterPos), XMVectorGetZ(vCurrentMonsterPos) + fRandomZ, 1.0f);

    _float fTerrainHeight = Compute_Monster_Position_Terrain(m_vRandomPosition);
    m_vRandomPosition = XMVectorSetY(m_vRandomPosition, fTerrainHeight);
}

void CRobo_Larva::Fire_Laser(_float _fTimeDelta)
{
    for (auto& pLaser : m_listLaser)
    {
        pLaser->Update(_fTimeDelta);
    }
}

void CRobo_Larva::Late_Update_Laser(_float _fTimeDelta)
{
    for (auto& pLaser : m_listLaser)
    {
        if (pLaser->IsActiveBullet())
        {
            pLaser->Late_Update(_fTimeDelta);
        }
    }
}

_vector CRobo_Larva::Compute_LaserHeight_Offset()
{
    _float3  fHeightOffset = { 0.0f, 1.0f , 0.0f };

    _matrix worldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());

    _vector worldLaserPos = XMVector2TransformCoord(XMLoadFloat3(&fHeightOffset), worldMatrix);


    return worldLaserPos;
}

HRESULT CRobo_Larva::Ready_Component()
{
    /* Com_Navigation */
    CNavigation::NAVIGATION_DESC		NaviDesc{};

    NaviDesc.iCurrentCellIndex = 0;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;

    /* Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
    ColliderDesc.vExtents = _float3(2.0f, 2.0f, 2.0f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom[COLL_AABB]), &ColliderDesc)))
        return E_FAIL;


    CBounding_Sphere::BOUNDING_SPHERE_DESC  ColliderSphereDesc{};
    ColliderSphereDesc.fRadius = 11.0f;
    ColliderSphereDesc.vCenter = _float3(0.f, Compute_Monster_Position_Terrain(m_pTransformCom->Get_State(CTransform::STATE_POSITION)), 0.f);
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider_SPHERE"), reinterpret_cast<CComponent**>(&m_pColliderCom[COLL_SPHERE]), &ColliderSphereDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRobo_Larva::Ready_PartObjects()
{
#pragma region BODY_LARVA
    // BODY_ROBO_BOOM을 만들자. 
    CBody_Robo_Larva::BODY_LARVA_DESC       BodyDesc{};
    BodyDesc.pParentState = &m_iState;
    BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    BodyDesc.fSpeedPerSec = 0.0f;
    BodyDesc.fRotationPerSec = 0.0f;

    if (FAILED(__super::Add_PartObject(TEXT("Part_LarvaBody"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Body_RoboLarva"), &BodyDesc)))
        return E_FAIL;

    m_pBody_Robo_Larva = static_cast<CBody_Robo_Larva*>(Find_PartObject(TEXT("Part_LarvaBody")));
    if (nullptr == m_pBody_Robo_Larva)
        return E_FAIL;
#pragma endregion

#pragma region LASER_LARVA
    // LASER_LARVA 를 만들자. 
    for (_uint i = 0; i < 10; ++i)
    {
        CLaser_Larva::LASER_DESC                LaserDesc{};
        CLaser_Larva* pLaser = dynamic_cast<CLaser_Larva*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Laser_Larva"), &LaserDesc));

        if (!pLaser)
            return E_FAIL;

        m_listLaser.push_back(pLaser);
    }
#pragma endregion

#pragma region LARVAR HP BAR
    // 몬스터의 체력바를 만들자. 
    CHpBar_Robo_Larva::LARVA_HPBAR_DESC HpBarDesc{};
    HpBarDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    HpBarDesc.pMonsterHp = &m_iHp;

    /*
    _float fOffset = (g_iWinSizeY * 0.5f);
    pDesc->fY = fOffset - (fOffset * 0.006f);
    */
    _float fOffset = (g_iWinSizeY * 0.5f);
    HpBarDesc.fY = fOffset - (fOffset * 0.0075f);

    if (FAILED(__super::Add_UIPartObject(TEXT("Part_HpBar_RoboLarva"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_HpBar_RoboLarva"), &HpBarDesc)))
        return E_FAIL;

    m_pHpBar_Robo_Larva = static_cast<CHpBar_Robo_Larva*>(Find_UIPartObject(TEXT("Part_HpBar_RoboLarva")));

    if (nullptr == m_pHpBar_Robo_Larva)
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
        DamageNumDesc.fY = fOffset - (fOffset * 0.0085f);

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

void CRobo_Larva::Activate_Laser()
{
    if (!m_listLaser.empty())
    {
        for (auto& pLaser : m_listLaser)
        {
            if (!pLaser->IsActiveBullet())
            {
                _vector vLaserHeight = Compute_LaserHeight_Offset();

                //_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
                _vector vTargetDirection = XMVector3Normalize(Setting_MissileDirection_To_Target(vLaserHeight, 0.0f));

                m_pGameInstance->PlayOnce(L"ST_Enemy_Larva_Shockwave_Shoot.wav", MONSTER_SHOOT, 0.1f);
                pLaser->Activate_Laser(vLaserHeight, vTargetDirection);
                return;
            }
        }
    }
}

_bool CRobo_Larva::Check_Collision_With_Ray(const _vector& _vRayStart, const _vector& _vRayDirection, _float& _fDistance)
{
    if (nullptr == m_pColliderCom[COLL_AABB])
        return false;

    return m_pColliderCom[COLL_AABB]->Intersect_Ray(_vRayStart, _vRayDirection, _fDistance);
}

void CRobo_Larva::Be_Hited(_uint _iCurrentGunDamage)
{
    if (m_iHp > 0)
    {
        m_pHpBar_Robo_Larva->Show_HealthBar(true);

        m_iHp = max(0, m_iHp - 10);
        m_pHpBar_Robo_Larva->Decrease_Hp(m_iHp);

        m_pHpBar_Robo_Larva->Update(0.016f);

        //_uint iDamage = 10;
        _uint iDamage = _iCurrentGunDamage;
        _uint iDigits[3] = { (iDamage / 100) % 10, (iDamage / 10) % 10, iDamage % 10 };

        _uint iStartIndex = (iDigits[0] == 0) ? 1 : 0;

        //for (int i = 0; i < 3; ++i)
        for (int i = iStartIndex; i < 3; ++i)
        {
            if (m_pDamageNumber[i])
            {
                m_pDamageNumber[i]->Set_DamageNumberDigit(iDigits[i], i - iStartIndex);
                m_pDamageNumber[i]->Set_LifeTime(0.3f);
            }
        }

        if (m_pBody_Robo_Larva)
            m_pBody_Robo_Larva->Activate_HittedEffect();

        if (m_iHp <= 0 && !m_pBody_Robo_Larva->IsDissolving())
        {
            m_iState = STATE_LAR_BACKSHOT_LOOP;
            m_pBody_Robo_Larva->Activate_DissolveEffect();
            m_pHpBar_Robo_Larva->Show_HealthBar(false);
            return;
        }
    }

   
}

CMonster* CRobo_Larva::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CRobo_Larva* pInstance = new CRobo_Larva(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CRobo_Larva");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CRobo_Larva::Clone(void* _pArg)
{
    CRobo_Larva* pInstance = new CRobo_Larva(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CRobo_Larva");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRobo_Larva::Free()
{
    __super::Free();

    for (auto& pLaser : m_listLaser)
        Safe_Release(pLaser);
    m_listLaser.clear();

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pColliderCom[COLL_AABB]);
    Safe_Release(m_pColliderCom[COLL_SPHERE]);
}
