#include "pch.h"
#include "Building.h"
#include "GameInstance.h"

CBuilding::CBuilding(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGameObject{_pDevice, _pContext}
{
}

CBuilding::CBuilding(const CBuilding& _Prototype)
    :CGameObject(_Prototype)
{
}

HRESULT CBuilding::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBuilding::Initialize(void* _pArg)
{
    BUILDING_DESC* pDesc = static_cast<BUILDING_DESC*>(_pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CBuilding::Priority_Update(_float _fTimeDelta)
{
}

void CBuilding::Update(_float _fTimeDelta)
{
}

void CBuilding::Late_Update(_float _fTimeDelta)
{
}

HRESULT CBuilding::Render()
{
    return S_OK;
}

void CBuilding::Free()
{
    __super::Free();
}
