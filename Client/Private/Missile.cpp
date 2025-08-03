#include "pch.h"
#include "Missile.h"
#include "GameInstance.h"


CMissile::CMissile(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGameObject{ _pDevice, _pContext }
{
}

CMissile::CMissile(const CMissile& _Prototype)
    :CGameObject(_Prototype)
{
}

HRESULT CMissile::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CMissile::Initialize(void* _pArg)
{
    MISSILE_DESC* pDesc = static_cast<MISSILE_DESC*>(_pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, TEXT("Layer_Player")));


    return S_OK;
}

void CMissile::Priority_Update(_float _fTimeDelta)
{
}

void CMissile::Update(_float _fTimeDelta)
{
}

void CMissile::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CMissile::Render()
{

    return S_OK;
}

void CMissile::Missile_Fly(_float _fTimeDelta)
{
    m_pTransformCom->Go_Straight(_fTimeDelta);
}

void CMissile::DeActivate_Missile()
{
    m_bActive = false;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(9999.0f, 9999.0f, 9999.0f, 1.0f));
}

void CMissile::Activate_Missile()
{
    if (m_bActive)
        return;

    m_bActive = true;
}

void CMissile::Hit_Target()
{
    DeActivate_Missile();

    if (m_pPlayer)
    {
        m_pPlayer->Be_Hited(10);
    }
}

void CMissile::Free()
{
    __super::Free();

}
