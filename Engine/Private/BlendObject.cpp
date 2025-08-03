#include "BlendObject.h"
#include "GameInstance.h"

CBlendObject::CBlendObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGameObject{ _pDevice, _pContext}
{
}

CBlendObject::CBlendObject(const CBlendObject& _Prototype)
    :CGameObject(_Prototype)
{
}

HRESULT CBlendObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CBlendObject::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

void CBlendObject::Priority_Update(_float _fTimeDelta)
{
}

void CBlendObject::Update(_float _fTimeDelta)
{
}

void CBlendObject::Late_Update(_float _fTimeDelta)
{
}

HRESULT CBlendObject::Render()
{
    return S_OK;
}

void CBlendObject::Compute_CamDistance()
{
    _float4			vCamPosition = m_pGameInstance->Get_CameraPos();
    _vector			vWorldPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    m_fCamDistance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vCamPosition) - vWorldPos));
}

void CBlendObject::Free()
{
    __super::Free();
}
