#include "Frustum.h"
#include "GameInstance.h"

CFrustum::CFrustum()
    :m_pGameInstance{CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CFrustum::Initialize()
{
	// ť�� ������� �����ؼ� �ո� 0 - 1 - 2 - 3 �޸� 4 - 5 - 6 - 7 �̶�� ���� ��.
	m_vOriginal_Points[0] = _float4(-1.f, 1.f, 0.f, 1.f);
	m_vOriginal_Points[1] = _float4(1.f, 1.f, 0.f, 1.f);
	m_vOriginal_Points[2] = _float4(1.f, -1.f, 0.f, 1.f);
	m_vOriginal_Points[3] = _float4(-1.f, -1.f, 0.f, 1.f);

	m_vOriginal_Points[4] = _float4(-1.f, 1.f, 1.f, 1.f);
	m_vOriginal_Points[5] = _float4(1.f, 1.f, 1.f, 1.f);
	m_vOriginal_Points[6] = _float4(1.f, -1.f, 1.f, 1.f);
	m_vOriginal_Points[7] = _float4(-1.f, -1.f, 1.f, 1.f);

    return S_OK;
}

void CFrustum::Update()
{
	// �� ����� / ���� ������� ���ؼ� ����ü ���� ����� ����.
	_matrix			ViewMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);
	_matrix			ProjMatrixInverse = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_PROJ);

	for (_uint i = 0; i < 8; i++)
	{
		XMStoreFloat4(&m_vWorld_Points[i], XMVector3TransformCoord(XMLoadFloat4(&m_vOriginal_Points[i]), ProjMatrixInverse));
		XMStoreFloat4(&m_vWorld_Points[i], XMVector3TransformCoord(XMLoadFloat4(&m_vWorld_Points[i]), ViewMatrixInverse));
	}

	Make_Planes(m_vWorld_Points, m_vWorld_Planes);
}

_bool CFrustum::isIn_WorldSpace(_fvector _vWorldPoint, _float _fRange)
{
	/*a b c d
	 x y z 1

	ax + by + cz + d = ? ;*/		// -> �̷��� �������� ���ؼ� ���ϴ� ������ �����̹Ƿ�, Dot �� ���� TransformCoord �� �� ����.. 
	// TransformCoord �� �ؼ� �����ϰ� w 1 �� ä��� �սô�. 

	// �� 6���� ���ؼ�, ���� ��ü (_vWorldPoint �� ���ӿ�����Ʈ�� ������ǥ�� �ǰڴ�)
	// ���������� �ذ� 0 �̸� ���� ����
	// ���������� �ذ� ���� �����, ����� �ո�� ���� -> ��ü�� ��ġ�� ��� ���� �Ÿ���� �� �� �ְ���.  
	// ���������� �ذ� ���� �����, ����� �޸��� ���� 
	for (size_t i = 0; i < 6; i++)
	{	
		// ������ ����� �ո�� �����ϴ�, �ᱹ ����ü �ȿ� ��ü�� ���ٴ� �Ҹ�. �׷��� IsIn �� False 
		// _fRange �� offset �����̶�� ���� �ǰڴ�. ��ü�� ��� ���� ����ü�� ��� �� ���� ���ִ� �뵵�� ������� ����. ���� ���� �� ������ üũ�ؼ�
		// �ٷ� �� ��������� ������ 1~2 �ؼ� ����.
		if (_fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vWorld_Planes[i]), _vWorldPoint)))
			return false;
	}

	// 6���� �鿡 �ϳ��� false �ɸ��� �ʾҴٸ�? ����ü �ȿ� ��ü�� �ִٴ� �Ҹ���, True
	return true;
}

void CFrustum::Make_Planes(const _float4* _pPoints, _float4* _pPlanes)
{
	// m_vWorldPlanes �� ����ü �� 8����, �俪 ���� �ؼ� ���� ���� �����̽��� �ٲ� m_vWorldPoints �� ����� ������ּ���.
	/* +x */
	XMStoreFloat4(&_pPlanes[0], XMPlaneFromPoints(
		XMLoadFloat4(&_pPoints[1]), 
		XMLoadFloat4(&_pPoints[5]), 
		XMLoadFloat4(&_pPoints[6])));
	/* -x */
	XMStoreFloat4(&_pPlanes[1], XMPlaneFromPoints(
		XMLoadFloat4(&_pPoints[4]),
		XMLoadFloat4(&_pPoints[0]),
		XMLoadFloat4(&_pPoints[3])
	));

	/* +y */
	XMStoreFloat4(&_pPlanes[2], XMPlaneFromPoints(
		XMLoadFloat4(&_pPoints[4]),
		XMLoadFloat4(&_pPoints[5]),
		XMLoadFloat4(&_pPoints[1])
	));

	/* -y */
	XMStoreFloat4(&_pPlanes[3], XMPlaneFromPoints(
		XMLoadFloat4(&_pPoints[3]),
		XMLoadFloat4(&_pPoints[2]),
		XMLoadFloat4(&_pPoints[6])
	));

	/* +z */
	XMStoreFloat4(&_pPlanes[4], XMPlaneFromPoints(
		XMLoadFloat4(&_pPoints[5]),
		XMLoadFloat4(&_pPoints[4]),
		XMLoadFloat4(&_pPoints[7])
	));

	/* -z */
	XMStoreFloat4(&_pPlanes[5], XMPlaneFromPoints(
		XMLoadFloat4(&_pPoints[0]),
		XMLoadFloat4(&_pPoints[1]),
		XMLoadFloat4(&_pPoints[2])
	));
}

CFrustum* CFrustum::Create()
{
	CFrustum* pInstance = new CFrustum();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed To Created : CFrustum");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFrustum::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
