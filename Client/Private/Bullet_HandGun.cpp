#include "pch.h"
#include "Bullet_HandGun.h"
#include "Player_Camera.h"
#include "Player.h"
#include "GameInstance.h"



CBullet_HandGun::CBullet_HandGun(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CBullet(_pDevice, _pContext)
{
}

CBullet_HandGun::CBullet_HandGun(const CBullet& _Prototype)
    :CBullet(_Prototype)
{
}

HRESULT CBullet_HandGun::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBullet_HandGun::Initialize(void* _pArg)
{
    BULLET_HANDGUN_DESC* pDesc = static_cast<BULLET_HANDGUN_DESC*>(_pArg);

    pDesc->fSpeedPerSec = 90.0f;
    pDesc->fRotationPerSec = XMConvertToRadians(90.f);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    //_vector         vInitialBulletPos = XMVectorSet(999.0f, 999.0f, 999.0f, 1.0f);
    //m_pTransformCom->Scaling(_float3(1.0f,1.0f,1.0f));
    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, vInitialBulletPos);

    m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, TEXT("Layer_Player")));

    return S_OK;
}

void CBullet_HandGun::Priority_Update(_float _fTimeDelta)
{
    if (!m_bActive)
    {
        if(m_dequeBulletPosition.size() > 2)
            m_dequeBulletPosition.pop_front();

        return;
    }

    Bullet_Fly(_fTimeDelta);

    _vector vBulletPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _float fBulletDistance = XMVectorGetX(XMVector3Length(vBulletPosition));
    _float fResultRange = m_bRayHit ? m_fRayDistance : m_fDefaultRange;

    _float3 vHitPos;
    if (m_pGameInstance->Compute_PickPos(&vHitPos))
    {
        _vector vHitPosition = XMLoadFloat3(&vHitPos);
        _float fHitDistance = XMVectorGetX(XMVector3Length(vHitPosition - vBulletPosition));

        if (fHitDistance < 5.0f)
        {
            CEffect_Impact* pImpactEffect = dynamic_cast<CEffect_Impact*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Effect_Impact"), &vHitPosition));

            m_listImpact_Effect.push_back(pImpactEffect);

            if (pImpactEffect)
            {
                pImpactEffect->Activate_ImpactEffect(vHitPos);
            }
            //m_pGameInstance->PlayOnce(L"ST_Weapon_Impact_Bullet_Dirt_02.wav", PLAYER_SHOOT, 0.2f);
            DeActivate_Bullet();
            return;
        }
    }

    if (fBulletDistance - 1.5f > fResultRange)
        DeActivate_Bullet();
}

void CBullet_HandGun::Update(_float _fTimeDelta)
{
    for (auto& pEffect : m_listImpact_Effect)
    {
        if (pEffect->IsActive())
        {
            pEffect->Update(_fTimeDelta);
            pEffect->Late_Update(_fTimeDelta);
        }
    }
}

void CBullet_HandGun::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CBullet_HandGun::Render()
{
    if (!m_bActive)
    {
        return S_OK;
    }

    if (FAILED(Bind_ShaderResource()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture", 0)))
            return E_FAIL;

        m_pShaderCom->Begin(0);

        m_pModelCom->Render(i);
    }

    if (m_dequeBulletPosition.size() > 4)
    {
        if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pTrailShaderCom, "g_TrailTexture", 0)))
            return E_FAIL;

        m_pTrailShaderCom->Begin(8);
        m_pTrailBufferCom->Bind_InputAssembler();
        m_pTrailBufferCom->Render();
    }

    return S_OK;
}

HRESULT CBullet_HandGun::Ready_Component()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Bullet"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_TrailBuffer"), TEXT("Com_TrailBuffer"), reinterpret_cast<CComponent**>(&m_pTrailBufferCom))))
        return E_FAIL;




    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxPosTex"), TEXT("Com_TrailShader"), reinterpret_cast<CComponent**>(&m_pTrailShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_TrailTexture"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

        return S_OK;
}

HRESULT CBullet_HandGun::Bind_ShaderResource()
{
    // 월드 행렬 바인딩. 
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    // 뷰 행렬 바인딩. 
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    // 투영 행렬 바인딩. 
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;



    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pTrailShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    // 뷰 행렬 바인딩. 
    if (FAILED(m_pTrailShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    // 투영 행렬 바인딩. 
    if (FAILED(m_pTrailShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;



    return S_OK;
}

void CBullet_HandGun::Activate_Bullet(const _vector& _vPosition, const _vector& _vGunMuzzleLook)
{
    // if (m_bActive)
    //     return;
    //
    // m_bActive = true;

    __super::Activate_Bullet(_vPosition, _vGunMuzzleLook);

    Set_Bullet_To_Muzzle(_vPosition, _vGunMuzzleLook);
}

void CBullet_HandGun::Bullet_Fly(_float _fTimeDelta)
{
    __super::Bullet_Fly(_fTimeDelta);

    if (m_dequeBulletPosition.size() < 32)
    {
        m_dequeBulletPosition.push_back(_float3(m_pTransformCom->Get_WorldMatrix_Ptr()->_41, m_pTransformCom->Get_WorldMatrix_Ptr()->_42, m_pTransformCom->Get_WorldMatrix_Ptr()->_43));
    }
    else
    {
        m_dequeBulletPosition.pop_front();
        m_dequeBulletPosition.push_back(_float3(m_pTransformCom->Get_WorldMatrix_Ptr()->_41, m_pTransformCom->Get_WorldMatrix_Ptr()->_42, m_pTransformCom->Get_WorldMatrix_Ptr()->_43));
    }

    
    if (m_dequeBulletPosition.size() > 2)
    {
       m_pTrailBufferCom->Update_Trail_Vertex(m_dequeBulletPosition, _float3(0.f, 0.05f, 0.0f), XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));
    }
}

void CBullet_HandGun::Set_Bullet_To_Muzzle(const _vector& _vGunMuzzlePos, const _vector& _vGunMuzzleLook)
{
    _vector vBulletPosOffset = _vGunMuzzlePos;

    CPlayer_Camera* pCamera = m_pPlayer->Get_PlayerCamera();
    if (!pCamera)
        return;

    _vector vPlayerCameraLook = XMVector3Normalize(pCamera->Get_LookVector());

    _vector vPlayerUp = XMVector3Normalize(m_pPlayer->Get_PlayerUp());

    // 플레이어카메라새끼의 look 이랑 플레이어(팔)의 up 을 외적해서 right를 구해벌힘
    _vector vRight = XMVector3Normalize(XMVector3Cross(vPlayerUp, vPlayerCameraLook));
    // 글케 구한 right 랑 플레이어 카메라 look이랑 외적해서, up 다시구해가지고 그거를 set_State ㄱㄱ 
    _vector vUp = XMVector3Normalize(XMVector3Cross(vPlayerCameraLook, vRight));

    // 이제 여기서 right 랑 up 만 조정해서 총알이 총구 끝에 나가게끔 조절만 그냥 해주면됨 ㅇㅅㅇ 
    vBulletPosOffset += (vRight * 0.15f) + (vUp * -0.01f) + vPlayerCameraLook;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vBulletPosOffset);
    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
    m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, vPlayerCameraLook);

    //m_pTransformCom->Scaling(_float3(0.00001f, 0.00001f, 0.00001f));
    //m_pTransformCom->Scaling(_float3(1.0f, 1.0f, 1.0f));
}

void CBullet_HandGun::DeActivate_Bullet()
{
    __super::DeActivate_Bullet();

    _uint iSizeDeuqeBullet = (_uint)m_dequeBulletPosition.size();

    for (_uint i = 0; i < iSizeDeuqeBullet; ++i)
    {
        m_dequeBulletPosition.pop_front();
    }
}

void CBullet_HandGun::Set_RayDistance(_float _fRayDistance)
{
    __super::Set_RayDistance(_fRayDistance);
}

void CBullet_HandGun::Initialize_Ray()
{
    __super::Initialize_Ray();
}

CEffect_Impact* CBullet_HandGun::Find_Inactive_ImpactEffect()
{
    list<CGameObject*>* pEffectLayer = m_pGameInstance->Get_GameObjectList(LEVEL_GAMEPLAY, TEXT("Layer_Effect"));

    if (!pEffectLayer)
        return nullptr;

    for (CGameObject* pGameObject : *pEffectLayer)
    {
        CEffect_Impact* pImpactEffect = dynamic_cast<CEffect_Impact*>(pGameObject);
        if (pImpactEffect && !pImpactEffect->IsActive())
        {
            return pImpactEffect;
        }
    }

    return nullptr;
}

CBullet_HandGun* CBullet_HandGun::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBullet_HandGun* pInstance = new CBullet_HandGun(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created: CBullet_HandGun");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CBullet_HandGun::Clone(void* _pArg)
{
    CBullet_HandGun* pInstance = new CBullet_HandGun(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Created: CBullet_HandGun");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBullet_HandGun::Free()
{
    __super::Free();

    for (auto& pEffect : m_listImpact_Effect)
        Safe_Release(pEffect);

    m_listImpact_Effect.clear();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);

    Safe_Release(m_pTrailShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pTrailBufferCom);
    


}
