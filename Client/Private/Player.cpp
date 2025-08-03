#include "pch.h"
#include "../Public/Player.h"
#include "GameInstance.h"

#include "Body_Player.h"
//#include "Gun.h"
//#include "HandGun.h"


//#include "Bullet.h"
#include "Bullet_HandGun.h"
#include "Bullet_Rifle.h"
#include "Bullet_Sniper.h"

#include "Monster.h"
#include "Boss_Tower.h"

#include "LevelChangeCollider.h"


CPlayer::CPlayer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CContainerObject{ _pDevice, _pContext }
{

}

CPlayer::CPlayer(const CPlayer& _Prototype)
    :CContainerObject(_Prototype)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;


    return S_OK;
}

HRESULT CPlayer::Initialize(void* _pArg)
{
    CGameObject::GAMEOBJECT_DESC		Desc{};

    Desc.fSpeedPerSec = 3.f;
    Desc.fRotationPerSec = XMConvertToRadians(90.f);

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    _vector vInitialPosition = XMVectorSet(5.0f, 0.0f, 5.0f, 1.0f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vInitialPosition);

    m_pBasicTerrain = static_cast<CBasicTerrain*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_BasicTerrain"), LEVEL_GAMEPLAY, TEXT("Layer_BasicTerrain")));

    //m_iAmmoCount = 100.0f;

    //m_iHandGun_BulletCount = Get_HandGunCount();

    m_iCurrentHp = 250;
    m_iMaxHp = 250;

    m_pDashEffect = static_cast<CDashEffect*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_DashEffect"), LEVEL_GAMEPLAY, TEXT("Layer_PlayerEffect")));
    if (!m_pDashEffect)
        return E_FAIL;

    m_pHurtEffect = static_cast<CHurtEffect*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_HurtEffect"), LEVEL_GAMEPLAY, TEXT("Layer_PlayerEffect")));
    if (!m_pHurtEffect)
        return E_FAIL;

    m_pBurnEffect = static_cast<CBurnEffect*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_BurnEffect"), LEVEL_GAMEPLAY, TEXT("Layer_PlayerEffect")));
    if (!m_pBurnEffect)
        return E_FAIL;

    m_pHpBar = static_cast<CPlayerHpbar*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_PlayerHpBar"), LEVEL_GAMEPLAY, TEXT("Layer_PlayerHpBar")));

    m_pScopeScreen = static_cast<CScopeScreen*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_ScopeScreen"), LEVEL_GAMEPLAY, TEXT("Layer_PlayerEffect")));
    if (!m_pScopeScreen)
        return E_FAIL;

    m_pHealEffect = static_cast<CHealEffect*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_HealEffect"), LEVEL_GAMEPLAY, TEXT("Layer_PlayerEffect")));
    if (!m_pHealEffect)
        return E_FAIL;


    //m_pBoss_Barrier = static_cast<CBoss_Barrier*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Barrier"), LEVEL_GAMEPLAY, TEXT("Layer_PlayerHpBar")));

    //m_pBoss_Tower = dynamic_cast<CBoss_Tower*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Boss"), LEVEL_GAMEPLAY, TEXT("Layer_Monster")));


    Initialize_WeaponUI();
    Initialize_InteractionUI();
    Initialize_LetterUI();

    return S_OK;
}

void CPlayer::Priority_Update(_float _fTimeDelta)
{
    // 픽셀피킹

   /* if (GetKeyState(VK_RBUTTON) & 0x8000)
    {
        _float3		vWorldPos;
        if (true == m_pGameInstance->Compute_PickPos(&vWorldPos))
        {
            m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vWorldPos), 1.f));
        }
    }*/

    Setting_PlayerMove(_fTimeDelta);

    CheckForNearbyItems();

    if (nullptr != m_pBasicTerrain)
    {
        _vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        _float fPosX = XMVectorGetX(vPosition);
        _float fPosY = XMVectorGetY(vPosition);
        _float fPosZ = XMVectorGetZ(vPosition);

        _float fTerrainHeight = m_pBasicTerrain->Compute_Height({ fPosX, fPosY, fPosZ });
        _float fNavigationHeight = m_pNavigationCom->Compute_Height(vPosition);

        _float fResultY = fPosY;

        fResultY = Player_Jump(fResultY, fTerrainHeight, fNavigationHeight, _fTimeDelta);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosX, fResultY, fPosZ, 1.0f));
    }

    __super::Priority_Update(_fTimeDelta);
}

_float CPlayer::Player_Jump(_float _fResultY, _float _fTerrainHeight, _float _fNaviHeight, _float _fTimeDelta)
{
    if (m_bJumping)
    {
        m_fJumpSpeed += m_fGravity * _fTimeDelta;
        _fResultY += m_fJumpSpeed * _fTimeDelta;

        if (_fTerrainHeight > 0.0f && _fResultY <= _fTerrainHeight + 1.15f)
        {
            _fResultY = _fTerrainHeight + 1.15f;
            m_bJumping = false;
        }
        else if (_fResultY <= _fNaviHeight + 1.0f)
        {
            _fResultY = _fNaviHeight + 1.0f;
            m_bJumping = false;
        }
    }
    else
    {
        _fResultY = (_fNaviHeight > 0.0f) ? (_fTerrainHeight + 1.15f) : (_fNaviHeight + 1.0f);
    }

    return _fResultY;
}

_bool CPlayer::Collide_With_Collider()
{
    list<CGameObject*>* pCheckCollider = m_pGameInstance->Get_GameObjectList(LEVEL_GAMEPLAY, TEXT("Layer_LCCollider"));

    for (CGameObject* pObj : *pCheckCollider)
    {
        CLevelChangeCollider* pCollider = dynamic_cast<CLevelChangeCollider*>(pObj);
        if (pCollider)
        {
            _float fDistance = 0.0f;
            if (pCollider->Check_Collision_With_Player())
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    return false;
}

void CPlayer::Switch_Weapon(_uint _iWeaponIndex)
{
    if (_iWeaponIndex >= m_vecWeapons.size())
        return;

    if (m_pCurrentGun)
        m_pCurrentGun->Set_Visible(false);

    m_pGameInstance->PlayOnce(L"ST_Pickup_Weapon.wav", PLAYER_MOVE, 0.2f);
    m_pCurrentGun = m_vecWeapons[_iWeaponIndex];
    m_pCurrentGun->Set_Visible(true);

    Change_WeaponImage();
}

void CPlayer::Change_WeaponImage()
{
    auto iter = find(m_vecWeapons.begin(), m_vecWeapons.end(), m_pCurrentGun);
    _uint currentIndex = _uint(distance(m_vecWeapons.begin(), iter));

    for (_uint i = 0; i < m_vecUIWeapons.size(); ++i)
    {
        if (i == currentIndex)
        {
            m_vecUIWeapons[i]->Set_HighLight(true);
            m_vecUIWeapons[i]->Set_Visible(true);
        }
        else
        {
            m_vecUIWeapons[i]->Set_HighLight(false);
            m_vecUIWeapons[i]->Set_Visible(true);
        }
    }
}

void CPlayer::Be_Hited(_uint _iDamage)
{
    m_iCurrentHp -= _iDamage;

    if (m_iCurrentHp <= 0)
    {
        m_iCurrentHp = 0;
        cout << "플레이어 사망!" << endl;
    }

    if (m_pHpBar)
    {
        m_pHpBar->Decrease_Hp(_iDamage, m_iMaxHp);
    }

    cout << "현재 플레이어 체력: " << m_iCurrentHp << endl;

    m_pHurtEffect->Set_Alpha(1.0f);
}

void CPlayer::Be_Burned(_uint _iDamage)
{
    m_iCurrentHp -= _iDamage;

    if (m_iCurrentHp <= 0)
    {
        m_iCurrentHp = 0;
        cout << "플레이어 사망!" << endl;
    }

    if (m_pHpBar)
    {
        m_pHpBar->Decrease_Hp(_iDamage, m_iMaxHp);
    }

    m_pBurnEffect->Activate_BurnEffect(true);
    m_pBurnEffect->Set_Alpha(1.0f);
}

void CPlayer::Initialize_WeaponUI()
{
    CUIWeapon::UIWEAPON_DESC HandgunDesc{};
    HandgunDesc.iTextureIndex = 0;
    HandgunDesc.fSizeX = 150.0f;
    HandgunDesc.fSizeY = 50.0f;
    HandgunDesc.fX = 1180.0f;
    HandgunDesc.fY = 622.0f;
    HandgunDesc.fZ = 0.1f;
    HandgunDesc.fNear = 0.0f;
    HandgunDesc.fFar = 1.0f;

    CUIWeapon* pHandgunIcon = static_cast<CUIWeapon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_WeaponIcon"), &HandgunDesc));

    if (pHandgunIcon)
    {
        pHandgunIcon->Set_Visible(true);
        m_vecUIWeapons.push_back(pHandgunIcon);
    }

    m_pCurrentWeaponUI = pHandgunIcon;
}

void CPlayer::Initialize_InteractionUI()
{
    CUIInteraction::UIINTERACTION_DESC InteractionDesc{};
    InteractionDesc.fSizeX = 80.0f;
    InteractionDesc.fSizeY = 80.0f;
    InteractionDesc.fX = _float(m_pGameInstance->Get_ViewportSize().x / 2);
    InteractionDesc.fY = _float(m_pGameInstance->Get_ViewportSize().y / 2 + 100.0f);
    InteractionDesc.fZ = 0.05f;
    InteractionDesc.fNear = 0.0f;
    InteractionDesc.fFar = 1.0f;

    CUIInteraction* pUIInteraction = static_cast<CUIInteraction*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Interaction"), &InteractionDesc));

    if (pUIInteraction)
    {
        m_vecUIInteractions.push_back(pUIInteraction);
    }
}

void CPlayer::Check_Interaction(_float _fTimeDelta, _uint _iTextureIndex , _bool _bVisible, _bool _bHighLighted)
{
    for (auto& pUIInteraction : m_vecUIInteractions)
    {
        pUIInteraction->Set_Visible(_bVisible);
        pUIInteraction->Set_TextureIndex(_iTextureIndex);

        pUIInteraction->Update(_fTimeDelta);

        pUIInteraction->Late_Update(_fTimeDelta);
        pUIInteraction->Set_HighLight(_bHighLighted);
    }
}

void CPlayer::Initialize_LetterUI()
{
    for (int i = 0; i < 5; ++i) 
    {
        CUILetters::LETTER_DESC Desc{};
        Desc.fSizeX = 200.0f;
        Desc.fSizeY = 80.0f;
        Desc.fX = _float(m_pGameInstance->Get_ViewportSize().x / 2);
        Desc.fY = _float(m_pGameInstance->Get_ViewportSize().y / 2 + 100.0f);
        Desc.fZ = 0.1f;
        Desc.fNear = 0.0f;
        Desc.fFar = 1.0f;

        CUILetters* pUILetters = static_cast<CUILetters*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Letter"), &Desc));

        if (pUILetters)
        {
            pUILetters->Set_Visible(false);  
            pUILetters->Set_TextureIndex(rand() % 5);  

            _vector vPos = XMVectorSet(Desc.fX, Desc.fY, 0.1f, 1.0f);
            pUILetters->Set_Position(vPos);

            m_vecUILetters.push_back(pUILetters); 
        }
    }
}

void CPlayer::Heal()
{
    m_pHealEffect->Set_Alpha(2.0f);

    m_pHpBar->Increase_Hp(30, m_iMaxHp);

    m_iCurrentHp += 30;
}


void CPlayer::Update(_float _fTimeDelta)
{
    Activate_PlayerCamera();

    if (m_pGameInstance->Get_DIKeyState(DIK_1) & 0x80)
    {
        if (!m_vecWeapons.empty())
            Switch_Weapon(0);
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_2) & 0x80)
    {
        if (m_vecWeapons.size() > 1)
            Switch_Weapon(1);
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_3) & 0x80)
    {
        if (m_vecWeapons.size() > 2)
            Switch_Weapon(2);
    }




    if (m_pCurrentGun)
    {
        if (m_bReLoading)
        {
            m_pCurrentGun->Ready_Shooting(false);
            m_pCurrentGun->Ready_Reload(true);
        }
        else
        {
            // 현재 무기의 타입을 체크하하고 상태 설정 ㄱ
            if (typeid(*m_pCurrentGun) == typeid(CHandGun))
            {
                m_iState == STATE_HANDGUN_DASH ? m_pCurrentGun->Ready_Shooting(false) : m_pCurrentGun->Ready_Shooting(true);
            }
            else if (typeid(*m_pCurrentGun) == typeid(CRifle))
            {
                m_iState == STATE_RIFLE_DASH ? m_pCurrentGun->Ready_Shooting(false) : m_pCurrentGun->Ready_Shooting(true);
            }
            else if (typeid(*m_pCurrentGun) == typeid(CSniper))
            {
                m_iState == STATE_RIFLE_DASH ? m_pCurrentGun->Ready_Shooting(false) : m_pCurrentGun->Ready_Shooting(true);
            }

            m_pCurrentGun->Ready_Reload(false);
        }
    }

    if (m_pCurrentGun && m_pCurrentGun->Get_CurrentAmmoInMagazine() <= 0 ) 
    {
        Check_Interaction(_fTimeDelta, 4, true, true);
    }

    for (auto& pUILetters : m_vecUILetters)
    {
        if (pUILetters->Is_Visible())
        {
            pUILetters->Update(_fTimeDelta);
        }
    }


    m_pColliderCom->Update(XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));

    __super::Update(_fTimeDelta);
}

void CPlayer::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);

    for (auto& pUIWeapon : m_vecUIWeapons)
    {
        if (pUIWeapon)
            pUIWeapon->Late_Update(_fTimeDelta);
    }

    for (auto& pUILetters : m_vecUILetters)
    {
        if (pUILetters->Is_Visible())
        {
            pUILetters->Late_Update(_fTimeDelta);
        }
    }

#ifdef _DEBUG
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
#endif
}

HRESULT CPlayer::Render()
{
#ifdef _DEBUG
    //m_pNavigationCom->Render();
    //m_pColliderCom->Render();
#endif

    for (auto& pUIWeapon : m_vecUIWeapons)
    {
        if (pUIWeapon)
            pUIWeapon->Render();
    }

    return S_OK;
}

HRESULT CPlayer::Ready_Components()
{
    /* Com_Navigation */
    CNavigation::NAVIGATION_DESC		NaviDesc{};

    NaviDesc.iCurrentCellIndex = 0;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;

    /* Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.3f, -0.3f, 0.3f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer::Ready_PartObjects()
{
#pragma region BODY_PLAYER
    // Body_Player 를 만들자. 
    CBody_Player::BODY_PLAYER_DESC      BodyDesc{};
    BodyDesc.pParentState = &m_iState;
    BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    BodyDesc.fSpeedPerSec = 0.0f;
    BodyDesc.fRotationPerSec = 0.0f;

    if (FAILED(__super::Add_PartObject(TEXT("Part_Body"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Body_Player"), &BodyDesc)))
        return E_FAIL;
    m_pBody_Player = static_cast<CBody_Player*>(Find_PartObject(TEXT("Part_Body")));
    if (nullptr == m_pBody_Player)
        return E_FAIL;
#pragma endregion

#pragma region PATICLE_MUZZLE_FIRE
    // 이펙트를 만들자!! 
    CParticle_MuzzleFire::PARTICLE_MUZZLE_FIRE_DESC EffectDesc = {};
    EffectDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();

    if (FAILED(Add_PartObject(TEXT("Part_Muzzle_Fire_Effect"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Particle_MuzzleFire"), &EffectDesc)))
        return E_FAIL;
#pragma endregion

#pragma region HAND_GUN
    CHandGun::HANDGUN_DESC      GunDesc{};

    CModel* pBodyModelCom = dynamic_cast<CModel*>(__super::Find_PartObject_Component(TEXT("Part_Body"), TEXT("Com_Model")));
    if (nullptr == pBodyModelCom)
        return E_FAIL;

    GunDesc.pSocketMatrix = pBodyModelCom->Get_BoneMatrix("Weapon_Attachement");
    GunDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    GunDesc.fSpeedPerSec = 0.0f;
    GunDesc.fRotationPerSec = 0.0f;
    GunDesc.pParticleMuzzleFire = Get_FireParticle();

    if (FAILED(__super::Add_PartObject(TEXT("Part_HandGun"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_HandGun"), &GunDesc)))
        return E_FAIL;

    m_pHandGun = static_cast<CHandGun*>(Find_PartObject(TEXT("Part_HandGun")));
    if (m_pHandGun == nullptr)
        return E_FAIL;

    m_vecWeapons.push_back(m_pHandGun);

    if (!m_vecWeapons.empty())
    {
        m_pCurrentGun = m_vecWeapons[0];
        m_pCurrentGun->Set_Visible(true);
    }


#pragma endregion

#pragma region PLAYER_CAMERA
    // 플레이어용 카메라를 만들자!! 
    CPlayer_Camera::PLAYER_CAMERA_DESC CameraDesc{};
    CameraDesc.fFov = XMConvertToRadians(60.0f);
    CameraDesc.fNear = 0.1f;
    CameraDesc.fFar = 1000.0f;
    CameraDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    CameraDesc.fSpeedPerSec = 0.0f;
    CameraDesc.fRotationPerSec = 0.0f;

    if (FAILED(__super::Add_PartObject(TEXT("Part_Camera"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_PlayerCamera"), &CameraDesc)))
        return E_FAIL;

    m_pPlayerCamera = static_cast<CPlayer_Camera*>(Find_PartObject(TEXT("Part_Camera")));
    if (nullptr == m_pPlayerCamera)
        return E_FAIL;
#pragma endregion
    return S_OK;
}

void CPlayer::Activate_PlayerCamera()
{
    static bool bUsingPlayerCamera = false;
    static bool bKeyPressed = false;

    //if (m_pGameInstance->Get_DIKeyState(DIK_U) & 0x80)
    {
        if (!bKeyPressed)
        {
            bKeyPressed = true;
            bUsingPlayerCamera = !bUsingPlayerCamera;

            CPlayer_Camera* pPlayerCamera = dynamic_cast<CPlayer_Camera*>(Find_PartObject(TEXT("Part_Camera")));
            if (pPlayerCamera)
            {
                pPlayerCamera->Set_Active(bUsingPlayerCamera);
            }
            m_pGameInstance->Hide_MouseCursor(bUsingPlayerCamera);
        }
    }
    //else
    //{
    //    bKeyPressed = false;
    //}
}

void CPlayer::Setting_PlayerMove(_float _fTimeDelta)
{
    static CCrossHair* pCrossHair = static_cast<CCrossHair*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_CrossHair"), LEVEL_GAMEPLAY, TEXT("Layer_UI")));
    CBody_Player* pBodyPlayer = static_cast<CBody_Player*>(Find_PartObject(TEXT("Part_Body")));
    _bool               bIsWalking = false;
    _bool               bIsDash = false;
    _bool               bIsShooting = false;
    _bool               bIsZooming = false;



    if (m_pGameInstance->Get_DIKeyState(DIK_LSHIFT) & 0x80)
    {
        
        bIsDash = true;

        m_fDashEffectTime = 0.0f;

        if (m_pDashEffect)
            m_pDashEffect->Set_Alpha(1.0f); // 이펙트 활성화
    }

    if (typeid(*m_pCurrentGun) == typeid(CSniper))
    {
        if (m_pGameInstance->Get_DIMouseState(DIM_RB) & 0x80)
        {
            bIsZooming = true;
            m_pScopeScreen->Activate_ScopeScreen();

            if (m_pPlayerCamera)
            {
                m_pPlayerCamera->Set_Fov(XMConvertToRadians(15.0f));
            }
        }
        else
        {
            bIsZooming = false;
            m_pScopeScreen->DeActivate_ScopeScreen();

            if (m_pPlayerCamera)
            {
                m_pPlayerCamera->Set_Fov(XMConvertToRadians(60.0f));
            }
        }
    }

    if (!m_bReLoading && (m_pGameInstance->Get_DIMouseState(DIM_LB) & 0x80))
    {
        bIsShooting = true;
    }

    if (!m_bReLoading && (m_pGameInstance->Get_DIMouseState(DIM_LB) & 0x80))
    {
        if (typeid(*m_pCurrentGun) == typeid(CSniper) && bIsZooming) // 스나이퍼는 줌 상태에서만 발사 가능
        {
            bIsShooting = true;
        }
        else if (typeid(*m_pCurrentGun) != typeid(CSniper)) // 다른 무기는 그냥 발사 가능
        {
            bIsShooting = true;
        }
    }


    if (m_pGameInstance->Get_DIKeyState(DIK_R) & 0x80)
    {
        if (!m_bReLoading)
        {
            m_bReLoading = true;
            //m_iState = STATE_HANDGUN_RELOAD;

            // 현재 무기에 따른 상태 설정
            if (typeid(*m_pCurrentGun) == typeid(CHandGun))
            {
                m_pGameInstance->PlayOnce(L"ST_Handgun_Reload.wav", PLAYER_MOVE, 0.2f);
                m_iState = STATE_HANDGUN_RELOAD;  // 핸드건 재장전 상태
            }
            else if (typeid(*m_pCurrentGun) == typeid(CRifle))
            {
                m_pGameInstance->PlayOnce(L"ST_AssaultRifle_Reload.wav", PLAYER_MOVE, 0.2f);
                m_iState = STATE_RIFLE_RELOAD;  // 라이플 재장전 상태
            }
            else if (typeid(*m_pCurrentGun) == typeid(CSniper))
            {
                m_pGameInstance->PlayOnce(L"ST_AssaultRifle_Reload.wav", PLAYER_MOVE, 0.2f);
                m_iState = STATE_RIFLE_RELOAD;  // 스나이퍼 재장전 상태 ( 라이플이랑 스나이퍼의 플레이어 애니메이션은 같아서)
            }

            if (typeid(*m_pCurrentGun) == typeid(CHandGun) || typeid(*m_pCurrentGun) == typeid(CRifle) || typeid(*m_pCurrentGun) == typeid(CSniper))
            {
                m_pCurrentGun->Ready_Reload(true);
            }
        }
    }

    if (m_bReLoading)
    {
        if ((m_iState == STATE_HANDGUN_RELOAD || m_iState == STATE_RIFLE_RELOAD) && pBodyPlayer->IsAnimationFinished())
        {
            m_bReLoading = false;

            if (m_pCurrentGun)
            {
                if (typeid(*m_pCurrentGun) == typeid(CHandGun))
                {
                    static_cast<CHandGun*>(m_pCurrentGun)->Reload();
                }
                else if (typeid(*m_pCurrentGun) == typeid(CRifle))
                {
                    static_cast<CRifle*>(m_pCurrentGun)->Reload();
                }
                else if (typeid(*m_pCurrentGun) == typeid(CSniper))
                {
                    static_cast<CSniper*>(m_pCurrentGun)->Reload();
                }

            }
        }
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_SPACE) & 0x80)
    {
        //m_pGameInstance->PlayOnce(L"ST_Player_Jump.wav", PLAYER_MOVE, 0.2f);
        if (!m_bJumping)
        {
            m_bJumping = true;
            m_fJumpSpeed = m_fInitialJumpSpeed;
        }
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
    {
        //m_pGameInstance->PlayOnce(L"ST_Player_Footstep_Scuff_Concrete.wav", PLAYER_MOVE, 0.5f);
        bIsWalking = true;

        bIsDash == true ? m_pTransformCom->Go_Straight(_fTimeDelta * 4.0f, m_pNavigationCom) : m_pTransformCom->Go_Straight(_fTimeDelta, m_pNavigationCom);
        bIsDash == true ? m_pGameInstance->PlayOnce(L"ST_Player_Footstep_Scuff_Concrete.wav", PLAYER_MOVE, 0.1f) : m_pGameInstance->PlayOnce(L"ST_Player_Switch_T01.wav", PLAYER_MOVE, 0.4f);
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
    {
        //m_pGameInstance->PlayOnce(L"ST_Player_Footstep_Scuff_Concrete.wav", PLAYER_MOVE, 0.5f);
        bIsWalking = true;

        bIsDash == true ? m_pTransformCom->Go_Backward(_fTimeDelta * 4.0f, m_pNavigationCom) : m_pTransformCom->Go_Backward(_fTimeDelta, m_pNavigationCom);
        bIsDash == true ? m_pGameInstance->PlayOnce(L"ST_Player_Footstep_Scuff_Concrete.wav", PLAYER_MOVE, 0.1f) : m_pGameInstance->PlayOnce(L"ST_Player_Switch_T01.wav", PLAYER_MOVE, 0.4f);
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
    {
       // m_pGameInstance->PlayOnce(L"ST_Player_Footstep_Scuff_Concrete.wav", PLAYER_MOVE, 0.5f);
        bIsWalking = true;

        bIsDash == true ? m_pTransformCom->Go_Left(_fTimeDelta * 4.0f, m_pNavigationCom) : m_pTransformCom->Go_Left(_fTimeDelta, m_pNavigationCom);
        bIsDash == true ? m_pGameInstance->PlayOnce(L"ST_Player_Footstep_Scuff_Concrete.wav", PLAYER_MOVE, 0.1f) : m_pGameInstance->PlayOnce(L"ST_Player_Switch_T01.wav", PLAYER_MOVE, 0.4f);
    }

    if (m_pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
    {
        //m_pGameInstance->PlayOnce(L"ST_Player_Footstep_Scuff_Concrete.wav", PLAYER_MOVE, 0.5f);
        bIsWalking = true;

        bIsDash == true ? m_pTransformCom->Go_Right(_fTimeDelta * 4.0f, m_pNavigationCom) : m_pTransformCom->Go_Right(_fTimeDelta, m_pNavigationCom);
        bIsDash == true ? m_pGameInstance->PlayOnce(L"ST_Player_Footstep_Scuff_Concrete.wav", PLAYER_MOVE, 0.1f) : m_pGameInstance->PlayOnce(L"ST_Player_Switch_T01.wav", PLAYER_MOVE, 0.4f);
    }

    if (!bIsDash && m_pDashEffect)
    {
        m_fDashEffectTime += _fTimeDelta;
        _float fAlpha = max(0.0f, 1.0f - (m_fDashEffectTime * 5.0f));
        m_pDashEffect->Set_Alpha(fAlpha);
    }

    if (m_bReLoading)
    {
        // 현재 무기에 따른 상태 설정
        if (typeid(*m_pCurrentGun) == typeid(CHandGun))
        {
            m_iState = STATE_HANDGUN_RELOAD;  // 핸드건 재장전 상태
        }
        else if (typeid(*m_pCurrentGun) == typeid(CRifle))
        {
            m_iState = STATE_RIFLE_RELOAD;  // 라이플 재장전 상태
        }
        else if (typeid(*m_pCurrentGun) == typeid(CSniper))
        {
            m_iState = STATE_RIFLE_RELOAD;  // 스나이퍼 재장전 상태 (라이플이랑 애니메이션 같음) 
        }
    }
    else if (bIsDash)
    {
        if (typeid(*m_pCurrentGun) == typeid(CHandGun))
        {
            m_iState = STATE_HANDGUN_DASH;  // 핸드건 대쉬 상태
        }
        else if (typeid(*m_pCurrentGun) == typeid(CRifle))
        {
            m_iState = STATE_RIFLE_DASH;  // 라이플 대쉬 상태
        }
        else if (typeid(*m_pCurrentGun) == typeid(CSniper))
        {
            m_iState = STATE_RIFLE_DASH;  // 스나이퍼 대쉬 상태 (라이플이랑 애니메이션 같음) 
        }

        pCrossHair->Set_CrossHair_TextureIndex(1);
    }
    else if (bIsWalking && bIsShooting)
    {
        if (typeid(*m_pCurrentGun) == typeid(CHandGun))
        {
            m_iState = STATE_HANDGUN_SHOOT;  // 핸드건 슈팅 상태
        }
        else if (typeid(*m_pCurrentGun) == typeid(CRifle))
        {
            m_iState = STATE_RIFLE_SHOOT;  // 라이플 슈팅 상태
        }
        else if (typeid(*m_pCurrentGun) == typeid(CSniper))
        {
            m_iState = STATE_RIFLE_SHOOT;  // 스나이퍼 슈팅 상태 (라이플이랑 애니메이션 같음) 
        }

        pCrossHair->Set_CrossHair_TextureIndex(0);
    }
    else if (bIsWalking)
    {
        if (typeid(*m_pCurrentGun) == typeid(CHandGun))
        {
            m_iState = STATE_HANDGUN_WALK;  // 핸드건 걷기 상태
        }
        else if (typeid(*m_pCurrentGun) == typeid(CRifle))
        {
            m_iState = STATE_RIFLE_WALK;  // 라이플 걷기 상태
        }
        else if (typeid(*m_pCurrentGun) == typeid(CSniper))
        {
            m_iState = STATE_RIFLE_WALK;  // 스나이퍼 걷기 상태 (라이플이랑 애니메이션 같음) 
        }

        pCrossHair->Set_CrossHair_TextureIndex(0);
    }
    else if (bIsShooting)
    {
        if (typeid(*m_pCurrentGun) == typeid(CHandGun))
        {
            m_iState = STATE_HANDGUN_SHOOT;  // 핸드건 슈팅 상태
        }
        else if (typeid(*m_pCurrentGun) == typeid(CRifle))
        {
            m_iState = STATE_RIFLE_SHOOT;  // 라이플 슈팅 상태
        }
        else if (typeid(*m_pCurrentGun) == typeid(CSniper))
        {
            m_iState = STATE_RIFLE_SHOOT;  // 스나이퍼 슈팅 상태 (라이플이랑 애니메이션 같음) 
        }
    }
    else
    {
        if (typeid(*m_pCurrentGun) == typeid(CHandGun))
        {
            m_iState = STATE_HANDGUN_IDLE;  // 핸드건 가만있는 상태
        }
        else if (typeid(*m_pCurrentGun) == typeid(CRifle) || typeid(*m_pCurrentGun) == typeid(CSniper))
        {
            m_iState = STATE_RIFLE_IDLE;  // 라이플 이랑 스나이퍼 가만있는 상태
        }
    }
}

void CPlayer::Shoot_Ray()
{
    CPlayer_Camera* pPlayerCamera = dynamic_cast<CPlayer_Camera*>(Find_PartObject(TEXT("Part_Camera")));

    if (nullptr == pPlayerCamera)
        return;

    //if (typeid(*m_pCurrentGun) == typeid(CRifle))
    //{
    //    //pPlayerCamera->Start_CameraShake(0.5f, 0.02f, 10.0f);  
    //    //pPlayerCamera->Start_CameraShake(0.5f, 0.5f, 10.0f);  
    //}

    CPlayer_Camera::Ray ray = pPlayerCamera->Create_Ray();

    list<CGameObject*>* pMonsterList = m_pGameInstance->Get_GameObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
    list<CGameObject*>* pBulletList = m_pGameInstance->Get_GameObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Bullet"));

    m_fRayDistance = 100.0f;

    CBoss_Tower* pBoss = static_cast<CBoss_Tower*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Boss"), LEVEL_GAMEPLAY, TEXT("Layer_Monster")));

    if (pBoss)
    {
        _float fDistance = 0.0f;
        if (pBoss->Check_Collision_Barrier(ray.vStartPosition, ray.vRayDirection, fDistance))
        {
            //pBoss->Barrier_Hited();
            return;
        }
    }

    for (CGameObject* pObj : *pMonsterList)
    {
        //CRobo_Boom* pMonster = dynamic_cast<CRobo_Boom*>(pObj);
        CMonster* pMonster = dynamic_cast<CMonster*>(pObj);

        if (pMonster)
        {
            _float fDistance = 0.0f;
            if (pMonster->Check_Collision_With_Ray(ray.vStartPosition, ray.vRayDirection, fDistance))
            {
                pMonster->Be_Hited(m_pCurrentGun->Get_GunDamage());
                cout << "몬스터 Hit다!! Hit!!" << fDistance << endl;

                if (fDistance < m_fRayDistance)
                    m_fRayDistance = fDistance;
                m_bHited = true;
                break;
            }

            else
                m_bHited = false;
        }
    }

    for (auto& pUILetters : m_vecUILetters)
    {
        if (!pUILetters->Is_Visible())  
        {
            pUILetters->Set_Visible(true);
            pUILetters->Set_TextureIndex(rand() % 5);

            _float randX = _float(m_pGameInstance->Get_ViewportSize().x / 2) - 140.0f + (rand() % 60 - 30);
            _float randY = _float(m_pGameInstance->Get_ViewportSize().y / 2) - 500.0f+ (rand() % 40 - 20);
            _vector vPos = XMVectorSet(randX, randY, 0.1f, 1.0f);
            pUILetters->Set_Position(vPos); 
            break;  
        }
    }

    if (m_bHited)
    {
        for (CGameObject* pObj : *pBulletList)
        {
            CBullet_HandGun* pBullet_HandGun = static_cast<CBullet_HandGun*>(pObj);
            if (pBullet_HandGun && pBullet_HandGun->IsActiveBullet() && !pBullet_HandGun->IsRayHit())
            {
                pBullet_HandGun->Set_RayDistance(m_fRayDistance);
                break;
            }
        }

        for (CGameObject* pObj : *pBulletList)
        {
            CBullet_Rifle* pBullet_Rifle = static_cast<CBullet_Rifle*>(pObj);
            if (pBullet_Rifle && pBullet_Rifle->IsActiveBullet() && !pBullet_Rifle->IsRayHit())
            {
                pBullet_Rifle->Set_RayDistance(m_fRayDistance);
                break;
            }
        }

        for (CGameObject* pObj : *pBulletList)
        {
            CBullet_Sniper* pBullet_Sniper = static_cast<CBullet_Sniper*>(pObj);
            if (pBullet_Sniper && pBullet_Sniper->IsActiveBullet() && !pBullet_Sniper->IsRayHit())
            {
                pBullet_Sniper->Set_RayDistance(m_fRayDistance);
                break;
            }
        }
    }
}

void CPlayer::PickUpItem(CItem* _pItem)
{
    if (_pItem)
    {
        _pItem->Use(this);
    }
}

void CPlayer::AddWeapon(CWeaponItem* _pWeapon)
{
    if (_pWeapon)
    {
        CModel* pBodyModelCom = dynamic_cast<CModel*>(Find_PartObject_Component(TEXT("Part_Body"), TEXT("Com_Model")));
        if (nullptr == pBodyModelCom)
            return;

        switch (_pWeapon->Get_WeaponTypeIndex())
        {
        case 1: // Rifle
        {
            if (m_pCurrentGun)
            {
                m_pCurrentGun->Set_Visible(false);
            }

            CRifle::RIFLE_DESC RifleDesc{};
            RifleDesc.pSocketMatrix = pBodyModelCom->Get_BoneMatrix("Weapon_Attachement");
            RifleDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
            RifleDesc.fSpeedPerSec = 0.0f;
            RifleDesc.fRotationPerSec = 0.0f;
            if (FAILED(__super::Add_PartObject(TEXT("Part_Rifle"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Rifle"), &RifleDesc)))
                return;

            m_pRifle = static_cast<CRifle*>(Find_PartObject(TEXT("Part_Rifle")));
            if (m_pRifle == nullptr)
                return;

            m_vecWeapons.push_back(m_pRifle);

            m_pCurrentGun = m_pRifle;
            m_pCurrentGun->Set_Visible(true);

            CUIWeapon::UIWEAPON_DESC RifleUIDesc{};
            RifleUIDesc.iTextureIndex = 1;
            RifleUIDesc.fSizeX = 150.0f;
            RifleUIDesc.fSizeY = 50.0f;
            RifleUIDesc.fX = 1180.0f;
            RifleUIDesc.fY = 562.0f;
            RifleUIDesc.fZ = 0.1f;
            RifleUIDesc.fNear = 0.0f;
            RifleUIDesc.fFar = 1.0f;

            CUIWeapon* pRifleIcon = static_cast<CUIWeapon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_WeaponIcon"), &RifleUIDesc));
            if (pRifleIcon)
            {
                pRifleIcon->Set_Visible(true);
                m_vecUIWeapons.push_back(pRifleIcon);
            }
            Change_WeaponImage();
        }
        break;

        case 2:
        {
            if (m_pCurrentGun)
            {
                m_pCurrentGun->Set_Visible(false);
            }

            CRifle::RIFLE_DESC RifleDesc{};
            RifleDesc.pSocketMatrix = pBodyModelCom->Get_BoneMatrix("Weapon_Attachement");
            RifleDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
            RifleDesc.fSpeedPerSec = 0.0f;
            RifleDesc.fRotationPerSec = 0.0f;
            if (FAILED(__super::Add_PartObject(TEXT("Part_Sniper"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Sniper"), &RifleDesc)))
                return;

            m_pSniper = static_cast<CSniper*>(Find_PartObject(TEXT("Part_Sniper")));

            //if (m_pRifle == nullptr)
            if (m_pSniper == nullptr)
                return;

            m_vecWeapons.push_back(m_pSniper);

            m_pCurrentGun = m_pSniper;
            m_pCurrentGun->Set_Visible(true);

            CUIWeapon::UIWEAPON_DESC SniperDesc{};
            SniperDesc.iTextureIndex = 2;
            SniperDesc.fSizeX = 150.0f;
            SniperDesc.fSizeY = 50.0f;
            SniperDesc.fX = 1180.0f;
            SniperDesc.fY = 502.0f;
            SniperDesc.fZ = 0.1f;
            SniperDesc.fNear = 0.0f;
            SniperDesc.fFar = 1.0f;

            CUIWeapon* pSniperIcon = static_cast<CUIWeapon*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_WeaponIcon"), &SniperDesc));
            if (pSniperIcon)
            {
                pSniperIcon->Set_Visible(true);
                m_vecUIWeapons.push_back(pSniperIcon);
            }

            Change_WeaponImage();
        }
        break;
        }
    }
}

void CPlayer::CheckForNearbyItems()
{
    list<CGameObject*>* pItemList = m_pGameInstance->Get_GameObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Item"));

    for (CGameObject* pObj : *pItemList)
    {
        CItem* pItem = dynamic_cast<CItem*>(pObj);
        if (pItem && !pItem->IsCollected())
        {
            _float fDistance = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - pItem->Get_ItemPos()));

            if (fDistance < 2.0f)
            {

                m_pGameInstance->PlayOnce(L"ST_Pickup_Weapon.wav", PLAYER_MOVE, 0.2f);
                PickUpItem(pItem);
            }
        }
    }
}


CPlayer* CPlayer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPlayer* pInstance = new CPlayer(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CPlayer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer::Clone(void* _pArg)
{
    CPlayer* pInstance = new CPlayer(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CPlayer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer::Free()
{
    __super::Free();

    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pColliderCom);

    for (auto& pUIWeapon : m_vecUIWeapons)
    {
        Safe_Release(pUIWeapon);
    }
    m_vecUIWeapons.clear();

    for (auto& pUIInteraction : m_vecUIInteractions)
    {
        Safe_Release(pUIInteraction);
    }
    m_vecUIInteractions.clear();

    for (auto& pUILetters : m_vecUILetters)
    {
        Safe_Release(pUILetters);
    }
    m_vecUILetters.clear();
}
