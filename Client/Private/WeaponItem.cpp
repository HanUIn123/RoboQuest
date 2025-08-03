#include "pch.h"
#include "WeaponItem.h"
#include "GameInstance.h"
#include "Player.h"


CWeaponItem::CWeaponItem(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CItem{ _pDevice, _pContext }
{
}

CWeaponItem::CWeaponItem(const CWeaponItem& _Prototype)
    :CItem(_Prototype)
{
}

HRESULT CWeaponItem::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CWeaponItem::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    _vector vItemPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    if (_pArg)
    {
        WEAPON_ITEM_DESC* pDesc = static_cast<WEAPON_ITEM_DESC*>(_pArg);

        m_pParentState = pDesc->pParentState;

        m_bCollected = pDesc->bCollected;

        m_iMaxAmmo = pDesc->iMaxAmmo;
        m_iCurrentAmmo = pDesc->iMaxAmmo;
        m_fDamage = pDesc->fDamage;
        m_fRange = pDesc->fRange;
        m_iWeaponTypeIndex = pDesc->iItemID;

        m_pPlayer = pDesc->pPlayer;
        vItemPosition = pDesc->vPosition;
    }



    if (FAILED(Ready_Components()))
        return E_FAIL;

    //m_pModelCom->SetUp_Animation(0, true, true);
    m_pTransformCom->Scaling(_float3(0.00005f, 0.00005f, 0.00005f));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vItemPosition, 1.0f));

    return S_OK;
}

void CWeaponItem::Priority_Update(_float _fTimeDelta)
{

}

void CWeaponItem::Update(_float _fTimeDelta)
{
    if (m_bCollected)
        return;

    auto WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    m_pColliderCom->Update(WorldMatrix);

    _vector vOriginalPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    static _float fRotationAngle = 0.0f;
    fRotationAngle += _fTimeDelta * XMConvertToRadians(10.0f) * 2.5f;
    m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), fRotationAngle);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vOriginalPos);
}

void CWeaponItem::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CWeaponItem::Render()
{
    if (m_bCollected)
        return S_OK;


    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture", 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture", 0)))
            return E_FAIL;

        m_pShaderCom->Begin(0);
        m_pModelCom->Render(i);
    }

    //m_pColliderCom->Render();
        
    return S_OK;;
}

void CWeaponItem::Use(CPlayer* _pPlayer)
{
    if (!m_bCollected && _pPlayer)
    {
        m_bCollected = true;
        _pPlayer->AddWeapon(this);
    }
}

void CWeaponItem::Set_Position()
{
}

void CWeaponItem::Set_Position(_vector _vPosition)
{
    //_vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPosition);
}

HRESULT CWeaponItem::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    switch (m_iWeaponTypeIndex)
    {
    case 1:
        if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Rifle"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
        break;
    //case 2:
    //    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Shotgun"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    //        return E_FAIL;
    //    break;
    case 2:
        if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Sniper"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
            return E_FAIL;
        break;
    }

    CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
    ColliderDesc.vExtents = _float3(10.40f, 5.55f, 0.40f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CWeaponItem::Bind_ShaderResources()
{
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    return S_OK;
}

CWeaponItem* CWeaponItem::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CWeaponItem* pInstance = new CWeaponItem(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CWeaponItem");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CWeaponItem::Clone(void* _pArg)
{
    CWeaponItem* pInstance = new CWeaponItem(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CWeaponItem");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWeaponItem::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
}
