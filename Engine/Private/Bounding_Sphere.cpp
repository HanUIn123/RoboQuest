#include "Bounding_Sphere.h"

CBounding_Sphere::CBounding_Sphere(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CBounding{ _pDevice, _pContext }
{
}

void CBounding_Sphere::Set_Desc(const void* _pDesc)
{
}

HRESULT CBounding_Sphere::Initialize(const CBounding::BOUNDING_DESC* _pArg)
{
	const BOUNDING_SPHERE_DESC* pDesc = static_cast<const BOUNDING_SPHERE_DESC*>(_pArg);

	m_pPrevDesc = new BoundingSphere(pDesc->vCenter, pDesc->fRadius);
	m_pDesc = new BoundingSphere(*m_pPrevDesc);

	return S_OK;
}

void CBounding_Sphere::Update(_fmatrix _WorldMatrix)
{
	m_pPrevDesc->Transform(*m_pDesc, _WorldMatrix);
}

HRESULT CBounding_Sphere::Render(PrimitiveBatch<VertexPositionColor>* _pBatch, _fvector _vColor)
{
	DX::Draw(_pBatch, *m_pDesc, _vColor);

	return S_OK;
}

_bool CBounding_Sphere::Intersect(CCollider::COLLIDER_TYPE _eColliderType, CBounding* _pTargetBounding)
{
	void* pTargetDesc = _pTargetBounding->Get_Desc();

	_bool	isColl = { false };

	switch (_eColliderType)
	{
	case CCollider::CT_AABB:
		isColl = m_pDesc->Intersects(*static_cast<BoundingBox*>(pTargetDesc));
		break;
	case CCollider::CT_OBB:
		isColl = m_pDesc->Intersects(*static_cast<BoundingOrientedBox*>(pTargetDesc));
		break;
	case CCollider::CT_SPHERE:
		isColl = m_pDesc->Intersects(*static_cast<BoundingSphere*>(pTargetDesc));
		break;
	}

	return isColl;
}

_bool CBounding_Sphere::Intersect_Ray(const _vector& _vRayStartPosition, const _vector& _vRayDirection, _float& _fDistance)
{
	return _bool();
}

CBounding_Sphere* CBounding_Sphere::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const CBounding::BOUNDING_DESC* _pArg)
{
	CBounding_Sphere* pInstance = new CBounding_Sphere(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed To Created : CBounding_Sphere");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBounding_Sphere::Free()
{
	__super::Free();

	Safe_Delete(m_pPrevDesc);
	Safe_Delete(m_pDesc);
}
