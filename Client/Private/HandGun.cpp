#include "pch.h"
#include "HandGun.h"
//#include "Bullet.h"
#include "Bullet_HandGun.h"
#include "Particle_MuzzleFire.h"
#include "GameInstance.h"

CHandGun::CHandGun(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGun(_pDevice, _pContext)
{
}

CHandGun::CHandGun(const CGun& _Prototype)
    :CGun(_Prototype)
{
}

HRESULT CHandGun::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CHandGun::Initialize(void* _pArg)
{
    HANDGUN_DESC* pDesc = static_cast<HANDGUN_DESC*>(_pArg);

    m_iPistolState = pDesc->iPistolState;
    
    m_pSocketMatrix = pDesc->pSocketMatrix;
    
    m_pMuzzleFireParticle = pDesc->pParticleMuzzleFire;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    _vector vInitialGunPos = XMVectorSet(-0.037f, 0.001f, 0.0f, 1.0f);
    m_pTransformCom->Scaling(_float3(0.00005f, 0.00005f, 0.00005f));
    //m_pTransformCom->Scaling(_float3(1.0f,1.0f,1.0f));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vInitialGunPos);

    m_pModelCom->SetUp_Animation(0, false, true);

    static float fRightOffset = 0.17f;
    static float fUpOffset = 5.73f;
    static float fLookOffset = -6.3f;

    m_pTransformCom->Rotation(fRightOffset, fUpOffset, fLookOffset);


    m_iMaxAmmoInMagazine = 50;       // 한 탄창에 최대 30발
    m_iCurrentAmmoInMagazine = 50;   // 초기 탄창 총알: 30발
    m_iTotalAmmo = 999;               // 전체 보유 총알: 100발


    m_fRayCoolDown = 0.005f;


    m_iGunDamage = 10;

    return S_OK;
}

void CHandGun::Priority_Update(_float _fTimeDelta)
{
    if (true == m_pModelCom->Play_Animation(_fTimeDelta))
    {
    }

    if (m_iPistolState != m_iPreAnimatioinState)
    {
        if (m_iPistolState == 1)
        {
            m_pModelCom->SetUp_Animation(0, false, true);
        }
        if (m_iPistolState == 2)
        {
            m_pModelCom->SetUp_Animation(1, true, true);
        }
        if (m_iPistolState == 3)
        {
            m_pModelCom->SetUp_Animation(2, true, true);
        }

        m_iPreAnimatioinState = m_iPistolState;

        //cout << "총의 애니메이션 상태 : " << m_iPistolState << endl;
    }

    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (size_t i = 0; i < 3; ++i)
    {
        // 행 3개를 길이 1로 만듬.
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
    }

    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));
}

void CHandGun::Update(_float _fTimeDelta)
{
    if (!Is_Visible())
        return;

    static _float fShootTimer = 0.0f;
    static _float fShootInterval = 0.2f;

    _bool bShooting = false;

    if (m_bReloading)
    {
        m_iPistolState = STATE_RELOAD;
        return;
    }

    if (!m_bShooting)
    {
        m_iPistolState = STATE_IDLE;
        m_pModelCom->Set_AnimationSpeed(1.0f, 1);
        return;
    }

    if (m_pGameInstance->Get_DIMouseState(DIM_LB) & 0x80)
    {
        if (fShootTimer == 0.0f)
        {
            Shoot_Bullet(_fTimeDelta);
            //Shoot_Ray();
            Update_RayCooldown(_fTimeDelta);

            m_iPistolState = STATE_FIRE;
            m_pModelCom->Set_AnimationSpeed(2.5f, 3);
            fShootTimer += _fTimeDelta;
        }
        else
        {
            fShootTimer += _fTimeDelta;

            if (fShootTimer >= fShootInterval)
            {
                fShootTimer = 0.0f;
                Shoot_Bullet(_fTimeDelta);
                //Shoot_Ray();
                Update_RayCooldown(_fTimeDelta);

                m_iPistolState = STATE_FIRE;
                m_pModelCom->Set_AnimationSpeed(2.5f, 3);
            }
        }
    }
    else
    {
        fShootTimer = 0.0f;
        m_iPistolState = STATE_IDLE;
    }

}

void CHandGun::Late_Update(_float _fTimeDelta)
{
    if (!Is_Visible())
        return;

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CHandGun::Render()
{
    if (!Is_Visible()) 
        return S_OK;

    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint			iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, (_uint)i, aiTextureType_DIFFUSE, "g_DiffuseTexture", 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, (_uint)i, aiTextureType_NORMALS, "g_NormalTexture", 0)))
            return E_FAIL;


        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        m_pShaderCom->Begin(0);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CHandGun::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPlayerAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Shotgun */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_HandGun"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CHandGun::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    return S_OK;
}

void CHandGun::Shoot_Bullet(_float _fTimeDelta)
{
    if (m_iCurrentAmmoInMagazine == 0) 
        return;

    list<CGameObject*>* pListBullet = m_pGameInstance->Get_GameObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Bullet"));

    for (CGameObject* pObj : *pListBullet)
    {
        CBullet_HandGun* pBullet_HandGun = dynamic_cast<CBullet_HandGun*>(pObj);
        if (pBullet_HandGun && !pBullet_HandGun->IsActiveBullet())
        {
            _vector vMuzzlePos = XMLoadFloat4x4(&m_CombinedWorldMatrix).r[3];
            _vector vMuzzleLook = -1.0f * XMVector3Normalize(XMLoadFloat4x4(&m_CombinedWorldMatrix).r[2]);

            m_pGameInstance->Play(L"ST_Handgun_PF_Shoot.wav", PLAYER_SHOOT, 0.1f);

            pBullet_HandGun->Initialize_Ray();
            pBullet_HandGun->Activate_Bullet(vMuzzlePos, vMuzzleLook);
            m_iCurrentAmmoInMagazine--;

            _matrix GunWorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
            _vector vMuzzleParticlePos = GunWorldMatrix.r[3];
            Shoot_MuzzleFireEffect(vMuzzleParticlePos + vOffset);

            return;
        }
    }
}

void CHandGun::Shoot_MuzzleFireEffect(_vector _vMuzzlePos)
{
    if (m_pMuzzleFireParticle)
        m_pMuzzleFireParticle->Set_FireParticle_To_Muzzle(_vMuzzlePos);
}

void CHandGun::Reload()
{
    if (m_bReloading) 
    {
        _uint iAmmoNeeded = m_iMaxAmmoInMagazine - m_iCurrentAmmoInMagazine;

        if (m_iTotalAmmo >= iAmmoNeeded) 
        {
            m_iCurrentAmmoInMagazine += iAmmoNeeded;
            m_iTotalAmmo -= iAmmoNeeded;
        }
        else {
            m_iCurrentAmmoInMagazine += m_iTotalAmmo;
            m_iTotalAmmo = 0;
        }

        m_bReloading = false; 
    }
}

CGun* CHandGun::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CHandGun* pInstance = new CHandGun(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CHandGun");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CHandGun::Clone(void* _pArg)
{
    CHandGun* pInstance = new CHandGun(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CHandGun");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHandGun::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
