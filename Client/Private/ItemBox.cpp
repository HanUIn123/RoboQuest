#include "pch.h"
#include "ItemBox.h"
#include "GameInstance.h"
#include "Player.h"

CItemBox::CItemBox(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CMonster{ _pDevice, _pContext }
{
}

CItemBox::CItemBox(const CMonster& _Prototype)
    :CMonster(_Prototype)
{
}

HRESULT CItemBox::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CItemBox::Initialize(void* _pArg)
{
    ITEM_BOX_DESC* pDesc = static_cast<ITEM_BOX_DESC*>(_pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_iState = STATE_BOX_IDLE;

    _vector vMonsterPos = XMLoadFloat3(&pDesc->vMonsterpos);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vMonsterPos, 1.0f));

    _matrix matRotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&pDesc->vMonsterRotation));
    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, matRotation.r[0]);
    m_pTransformCom->Set_State(CTransform::STATE_UP, matRotation.r[1]);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, matRotation.r[2]);

    m_pTransformCom->Scaling(pDesc->vMonsterScale);

    m_iCurrentWeaponIndex = pDesc->iWeaponTypeIndex;

    return S_OK;
}

void CItemBox::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CItemBox::Update(_float _fTimeDelta)
{
    auto WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    m_pColliderCom->Update(WorldMatrix);

    __super::Update(_fTimeDelta);
}

void CItemBox::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);

    if (m_iState == STATE_BOX_OPEN)
    {
        if (m_pBodyBox->IsAnimationFinished() && !m_bItemDropped)
        {
            Drop_Item();
            m_bItemDropped = true;
        }
        return;
    }

    CCollider* pTargetCollider = static_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), 0));
    CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, TEXT("Layer_Player")));

    if (pTargetCollider && m_pColliderCom->Intersect(pTargetCollider))
    {
        Set_State(STATE_BOX_HOVER);

        pPlayer->Check_Interaction(_fTimeDelta, 3 ,true);

        if (m_pGameInstance->Get_DIKeyState(DIK_E) && m_iState != STATE_BOX_OPEN)
        {
            m_pGameInstance->Play(L"ST_Chest_Open.wav", NPC_GLAD, 0.1f);
            Set_State(STATE_BOX_OPEN);
            pPlayer->Check_Interaction(_fTimeDelta, 3 ,false);
        }
    }

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CItemBox::Render()
{
#ifdef _DEBUG
    //m_pColliderCom->Render();
#endif

    return S_OK;
}

_bool CItemBox::Check_Collision_With_Ray(const _vector& _vRayStart, const _vector& _vRayDirection, _float& _fDistance)
{
    return _bool();
    //return true;
}

void CItemBox::Be_Hited(_uint _iCurrentGunDamage)
{
}

HRESULT CItemBox::Ready_Component()
{
    CBounding_Sphere::BOUNDING_SPHERE_DESC  ColliderSphereDesc{};
    ColliderSphereDesc.fRadius = 2.5f;
    ColliderSphereDesc.vCenter = _float3(0.f, ColliderSphereDesc.fRadius - 2.5f, 0.f);
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider_SPHERE"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderSphereDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CItemBox::Ready_PartObjects()
{
#pragma region BODY_DOOR
    // BODY_DOOR 을 만들자. 
    CBody_Box::BODY_BOX_DESC       BoxDesc{};
   BoxDesc.pParentState = &m_iState;
   BoxDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
   BoxDesc.fSpeedPerSec = 0.0f;
   BoxDesc.fRotationPerSec = 0.0f;

    if (FAILED(__super::Add_PartObject(TEXT("Part_Body"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Body_ItemBox"), &BoxDesc)))
        return E_FAIL;

    m_pBodyBox = static_cast<CBody_Box*>(Find_PartObject(TEXT("Part_Body")));
    if (nullptr == m_pBodyBox)
        return E_FAIL;
#pragma endregion

    return S_OK;
}

void CItemBox::Drop_Item(/*_uint _iWeaponTypeIndex*/)
{
    if (m_bItemDropped)
        return;

    CWeaponItem* pWeaponItem = nullptr;

    if (m_iCurrentWeaponIndex == 1)
    {
        pWeaponItem = static_cast<CWeaponItem*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_WeaponItemRifle"), LEVEL_GAMEPLAY, TEXT("Layer_Item")));
    }
    else if (m_iCurrentWeaponIndex == 2)
    {
        pWeaponItem = static_cast<CWeaponItem*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_WeaponItemSniper"), LEVEL_GAMEPLAY, TEXT("Layer_Item")));
    }

    if (pWeaponItem)
    {
        _vector vBoxPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        pWeaponItem->Set_Position(vBoxPosition + XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
    }

    m_bItemDropped = true;

}

void CItemBox::Set_State(ITEM_BOX_STATE _eState)
{
    if (m_iState == _eState)
        return;

    m_iState = _eState;
}

CMonster* CItemBox::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CItemBox* pInstance = new CItemBox(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CItemBox");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CItemBox::Clone(void* _pArg)
{
    CItemBox* pInstance = new CItemBox(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CItemBox");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CItemBox::Free()
{
    __super::Free();


    Safe_Release(m_pColliderCom);
}
