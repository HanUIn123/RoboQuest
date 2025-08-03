#include "Bounding_OBB.h"

CBounding_OBB::CBounding_OBB(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CBounding{ _pDevice ,_pContext }
{
}

void CBounding_OBB::Set_Desc(const void* _pDesc)
{
}

HRESULT CBounding_OBB::Initialize(const CBounding::BOUNDING_DESC* _pArg)
{
    const BOUNDING_OBB_DESC* pDesc = static_cast<const BOUNDING_OBB_DESC*>(_pArg);

    _float4 vOrientation = {};

    XMStoreFloat4(&vOrientation, XMQuaternionRotationRollPitchYaw(pDesc->vRotation.x, pDesc->vRotation.y, pDesc->vRotation.z));

    m_pPrevDesc = new BoundingOrientedBox(pDesc->vCenter, pDesc->vExtents, vOrientation);
    m_pDesc = new BoundingOrientedBox(*m_pPrevDesc);

    return S_OK;
}

void CBounding_OBB::Update(_fmatrix _WorldMatrix)
{
    m_pPrevDesc->Transform(*m_pDesc, _WorldMatrix);
}

HRESULT CBounding_OBB::Render(PrimitiveBatch<VertexPositionColor>* _pBatch, _fvector _vColor)
{
    DX::Draw(_pBatch, *m_pDesc, _vColor);

    return S_OK;
}

_bool CBounding_OBB::Intersect(CCollider::COLLIDER_TYPE _eColliderType, CBounding* _pTargetBounding)
{
    void* pTargetDesc = _pTargetBounding->Get_Desc();

    _bool       bIsColl = { false };

    switch (_eColliderType)
    {
    case CCollider::CT_AABB:
        bIsColl = m_pDesc->Intersects(*static_cast<BoundingBox*>(pTargetDesc));
        break;
    case CCollider::CT_OBB:
        bIsColl = m_pDesc->Intersects(*static_cast<BoundingOrientedBox*>(pTargetDesc));
        break;
    case CCollider::CT_SPHERE:
        bIsColl = m_pDesc->Intersects(*static_cast<BoundingSphere*>(pTargetDesc));
        break;
    }

    return bIsColl;
}

_bool CBounding_OBB::Intersect_Ray(const _vector& _vRayStartPosition, const _vector& _vRayDirection, _float& _fDistance)
{
    return _bool();
}

CBounding_OBB* CBounding_OBB::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const CBounding::BOUNDING_DESC* _pArg)
{
    CBounding_OBB* pInstance = new CBounding_OBB(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Created : CBounding_OBB");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBounding_OBB::Free()
{
    __super::Free();

    Safe_Delete(m_pPrevDesc);
    Safe_Delete(m_pDesc);
}
