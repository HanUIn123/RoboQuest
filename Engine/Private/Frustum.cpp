#include "Frustum.h"
#include "GameInstance.h"

CFrustum::CFrustum()
    :m_pGameInstance{CGameInstance::GetInstance()}
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CFrustum::Initialize()
{
	// 큐브 모양으로 생각해서 앞면 0 - 1 - 2 - 3 뒷면 4 - 5 - 6 - 7 이라고 보면 됨.
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
	// 뷰 역행렬 / 투영 역행렬을 곱해서 절두체 점을 월드로 내려.
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

	ax + by + cz + d = ? ;*/		// -> 이렇게 각성분을 곱해서 더하는 행위가 내적이므로, Dot 할 껀지 TransformCoord 로 할 건지.. 
	// TransformCoord 로 해서 안전하게 w 1 로 채우고 합시다. 

	// 면 6개에 대해서, 점이 객체 (_vWorldPoint 가 게임오브젝트의 월드좌표가 되겠다)
	// 평면방정식의 해가 0 이면 평면상에 존재
	// 평면방정식의 해가 양의 수라면, 평면의 앞면상에 존재 -> 객체의 위치와 평면 상의 거리라고도 볼 수 있겠음.  
	// 평면방정식의 해가 음의 수라면, 평면의 뒷면의 존재 
	for (size_t i = 0; i < 6; i++)
	{	
		// 양수라면 평면의 앞면상에 존재하니, 결국 절두체 안에 객체가 없다는 소리. 그래서 IsIn 은 False 
		// _fRange 는 offset 느낌이라고 보면 되겠다. 객체가 어느 정도 절두체와 닿는 것 인지 봐주는 용도의 변수라고 보자. 작을 수록 좀 빡세게 체크해서
		// 바로 뿅 사라질꺼임 적당히 1~2 해서 하자.
		if (_fRange < XMVectorGetX(XMPlaneDotCoord(XMLoadFloat4(&m_vWorld_Planes[i]), _vWorldPoint)))
			return false;
	}

	// 6개의 면에 하나라도 false 걸리지 않았다면? 절두체 안에 객체가 있다는 소리라서, True
	return true;
}

void CFrustum::Make_Planes(const _float4* _pPoints, _float4* _pPlanes)
{
	// m_vWorldPlanes 를 절두체 점 8개를, 뷰역 투역 해서 구한 월드 스페이스로 바꾼 m_vWorldPoints 로 평면을 만들어주세요.
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
