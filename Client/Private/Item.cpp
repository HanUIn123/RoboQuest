#include "pch.h"
#include "Item.h"
#include "GameInstance.h"
#include    "Player.h"

CItem::CItem(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGameObject{_pDevice, _pContext}
{
}
    
CItem::CItem(const CItem& _Prototype)
    :CGameObject(_Prototype)
{
}

HRESULT CItem::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CItem::Initialize(void* _pArg)
{
 
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (_pArg)
    {
        ITEM_DESC* pDesc = static_cast<ITEM_DESC*>(_pArg);
        m_eItemType = pDesc->eItemType;
        m_pPlayer = pDesc->pPlayer;
        //pDesc->vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    }

    return S_OK;
}

void CItem::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CItem::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);
}

void CItem::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT CItem::Render()
{
    return S_OK;
}

void CItem::Free()
{
    __super::Free();
}
