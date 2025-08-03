#include "pch.h"
#include "Sniper.h"
#include "GameInstance.h"

#include "Bullet_Sniper.h"

#include "ScopeGauge.h"


CSniper::CSniper(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGun(_pDevice, _pContext)
{
}

CSniper::CSniper(const CGun& _Prototype)
    :CGun(_Prototype)
{
}

HRESULT CSniper::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSniper::Initialize(void* _pArg)
{
    SNIPER_DESC* pDesc = static_cast<SNIPER_DESC*>(_pArg);

    m_iPistolState = pDesc->iPistolState;

    m_pSocketMatrix = pDesc->pSocketMatrix;

    //m_pMuzzleFireParticle = pDesc->pParticleMuzzleFire;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Scaling(_float3(0.00005f, 0.00005f, 0.00005f));

    //m_pModelCom->SetUp_Animation(0, false, true);

    m_iMaxAmmoInMagazine = 20;              // 한 탄창에 최대 30발
    m_iCurrentAmmoInMagazine = 20;          // 초기 탄창 총알: 30발
    m_iTotalAmmo = 500;                     // 전체 보유 총알: 100발


    m_fRayCoolDown = 0.005f;

    m_iGunDamage = 15;


    _float fRightOffset = 0.130f;
    _float fUpOffset = -0.38f;
    _float fBackOffset = 0.009f;

    _float fPosX = -0.104f;
    _float fPosY = -0.030f;
    _float fPosZ = -0.104f;

    _vector vInitialPosition = XMVectorSet(fPosX, fPosY, fPosZ, 1.0f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vInitialPosition);

    m_pTransformCom->Rotation(fRightOffset, fUpOffset, fBackOffset);


    m_pGaugeBar = static_cast<CScopeGauge*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_ScopeGauge"), LEVEL_GAMEPLAY, TEXT("Layer_PlayerEffect")));


    return S_OK;
}

void CSniper::Priority_Update(_float _fTimeDelta)
{
    //        
    //    _vector vGunPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    //    _float fPosX = XMVectorGetX(vGunPos);
    //    _float fPosY = XMVectorGetY(vGunPos);
    //    _float fPosZ = XMVectorGetZ(vGunPos);
    //
    //  
    //    static _float fRightOffset = 0.13f;
    //    static _float fUpOffset = 0.11f;
    //    static _float fBackOffset = 0.009f;
    //
    //#ifdef _DEBUG
    //    ImGui::Begin("Rifle Adjustment");
    //
    //    
    //    ImGui::SliderFloat("Right Offset", &fRightOffset, -1.0f, 1.0f);
    //    ImGui::SliderFloat("Up Offset", &fUpOffset, -1.0f, 20.0f);
    //    ImGui::SliderFloat("Back Offset", &fBackOffset, -0.300f, 20.0f);
    //
    //    
    //    ImGui::SliderFloat("Position X", &fPosX, -10.0f, 10.0f);
    //    ImGui::SliderFloat("Position Y", &fPosY, -10.0f, 10.0f);
    //    ImGui::SliderFloat("Position Z", &fPosZ, -10.0f, 10.0f);
    //
    //    ImGui::End();
    //#endif
    //
    //    
    //    vGunPos = XMVectorSet(fPosX, fPosY, fPosZ, 1.0f);
    //    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vGunPos);
    //
    //    
    //    m_pTransformCom->Rotation(fRightOffset, fUpOffset, fBackOffset);
    //

    if (true == m_pModelCom->Play_Animation(_fTimeDelta))
    {
    }
    if (m_iPistolState != m_iPreAnimatioinState)
    {
        if (m_iPistolState == 1)
        {
            m_pModelCom->SetUp_Animation(0, false, true);
        }
        m_iPreAnimatioinState = m_iPistolState;

        //cout << "총의 애니메이션 상태 : " << m_iPistolState << endl;
    }

    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (size_t i = 0; i < 3; ++i)
    {
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
    }

    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(m_pParentWorldMatrix));
}

void CSniper::Update(_float _fTimeDelta)
{
    if (!Is_Visible())
        return;

    static _float fShootTimer = 0.0f;
    static _float fShootInterval = 0.2f;

    _bool bShooting = false;


    if (m_pGaugeBar->Is_FullCharge())
    {
        m_iGunDamage = 120;

    }
    else if (m_pGaugeBar->Compute_GaugePercent() < 1.0f)
    {
        m_iGunDamage = 15;
        m_iGunDamage += (_uint)(m_iGunDamage * m_pGaugeBar->Compute_GaugePercent());
    }

    if (m_bReloading)
    {
        m_iPistolState = STATE_RELOAD;
        return;
    }

    if (!m_bShooting)
    {
        m_iPistolState = STATE_IDLE;
        //m_pModelCom->Set_AnimationSpeed(1.0f, 1);
        return;
    }

    if (m_pGaugeBar->Is_FullCharge())
    {
        if (m_pGameInstance->Get_DIMouseState(DIM_LB) & 0x80)
        {
           
            if (fShootTimer == 0.0f)
            {
                Shoot_Bullet(_fTimeDelta);
                Update_RayCooldown(_fTimeDelta);

                m_iPistolState = STATE_FIRE;
                //m_pModelCom->Set_AnimationSpeed(2.5f, 1);
                fShootTimer += _fTimeDelta;
                m_pGaugeBar->Initialize_GaugeBar(0.0f);
            }
            else
            {
                fShootTimer += _fTimeDelta;

                if (fShootTimer >= fShootInterval)
                {
                    fShootTimer = 0.0f;
                    Shoot_Bullet(_fTimeDelta);
                    Update_RayCooldown(_fTimeDelta);

                    m_iPistolState = STATE_FIRE;
                    //m_pModelCom->Set_AnimationSpeed(2.5f, 1);
                    m_pGaugeBar->Initialize_GaugeBar(0.0f);
                }
            }
        }
        else
        {
            fShootTimer = 0.0f;
            //m_pGaugeBar->Initialilze_FullCharge(false);
            m_iPistolState = STATE_IDLE;
        }
    }


}

void CSniper::Late_Update(_float _fTimeDelta)
{
    if (!Is_Visible())
        return;

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CSniper::Render()
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

HRESULT CSniper::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPlayerAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_Shotgun */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_SniperModel"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CSniper::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    return S_OK;
}

void CSniper::Shoot_Bullet(_float _fTimeDelta)
{
    if (m_iCurrentAmmoInMagazine == 0)
        return;

    list<CGameObject*>* pListBullet = m_pGameInstance->Get_GameObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Bullet"));

    for (CGameObject* pObj : *pListBullet)
    {
        CBullet_Sniper* pBullet_Sniper = dynamic_cast<CBullet_Sniper*>(pObj);
        if (pBullet_Sniper && !pBullet_Sniper->IsActiveBullet())
        {
            _vector vMuzzlePos = XMLoadFloat4x4(&m_CombinedWorldMatrix).r[3];
            _vector vMuzzleLook = -1.0f * XMVector3Normalize(XMLoadFloat4x4(&m_CombinedWorldMatrix).r[0]);

            m_pGameInstance->Play(L"ST_BlastSniper_PF_Shoot_02.wav", PLAYER_SHOOT, 0.1f);

            pBullet_Sniper->Initialize_Ray();
            pBullet_Sniper->Activate_Bullet(vMuzzlePos, vMuzzleLook);
            m_iCurrentAmmoInMagazine--;


            _matrix GunWorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
            _vector vMuzzleParticlePos = GunWorldMatrix.r[3];

            return;
        }
    }
}

void CSniper::Reload()
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

CGun* CSniper::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CSniper* pInstance = new CSniper(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CSniper");
        Safe_Release(pInstance);
    }

    return pInstance;;
}

CGameObject* CSniper::Clone(void* _pArg)
{
    CSniper* pInstance = new CSniper(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CSniper");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSniper::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
