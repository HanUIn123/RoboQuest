#include "pch.h"
#include "Bullet.h"
//#include "Player_Camera.h"
//#include "Player.h"
#include "GameInstance.h"



CBullet::CBullet(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGameObject{ _pDevice, _pContext }
{
}

CBullet::CBullet(const CBullet& _Prototype)
    :CGameObject(_Prototype)
{
}

HRESULT CBullet::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBullet::Initialize(void* _pArg)
{
    BULLET_DESC* pDesc = static_cast<BULLET_DESC*>(_pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CBullet::Priority_Update(_float _fTimeDelta)
{
}

void CBullet::Update(_float _fTimeDelta)
{
}

void CBullet::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CBullet::Render()
{
    return S_OK;
}

void CBullet::Activate_Bullet(const _vector& _vPosition, const _vector& _vGunMuzzleLook)
{
    if (m_bActive)
        return;

    m_bActive = true;
}

void CBullet::DeActivate_Bullet()
{
    m_bActive = false;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(9999.0f, 9999.0f, 9999.0f, 1.0f));
}

void CBullet::Set_RayDistance(_float _fRayDistance)
{
    m_fRayDistance = _fRayDistance;
    m_bRayHit = true; 
}

void CBullet::Initialize_Ray()
{
    m_fRayDistance = 100.0f;
    m_bRayHit = false;
}

void CBullet::Bullet_Fly(_float _fTimeDelta)
{
    m_pTransformCom->Go_Straight(_fTimeDelta);
}

void CBullet::Free()
{
    __super::Free();
}
