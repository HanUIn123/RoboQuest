#include "VIBuffer_Point_Instancing.h"
#include "GameInstance.h"

CVIBuffer_Point_Instancing::CVIBuffer_Point_Instancing(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CVIBuffer_Particle_Instancing{ _pDevice, _pContext }
{
}

CVIBuffer_Point_Instancing::CVIBuffer_Point_Instancing(const CVIBuffer_Point_Instancing& _Prototype)
	:CVIBuffer_Particle_Instancing(_Prototype)
{
}

HRESULT CVIBuffer_Point_Instancing::Initialize_Prototype(const PARTICLE_INSTANCING_DESC* _pArg)
{
	const PARTICLE_POINT_INSTANCING_DESC* pDesc = static_cast<const PARTICLE_POINT_INSTANCING_DESC*>(_pArg);

	m_iNumInstance = pDesc->iNumInstance;
	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTXPOINT);
	m_iInstanceVertexStride = sizeof(VTX_PARTICLE_INSTANCE);
	m_iNumIndexPerInstance = 1;		// 도형 하나를 그리기 위해서 필요한 인덱스는 1개이다. 
	m_iNumIndices = m_iNumIndexPerInstance * m_iNumInstance;
	m_iIndexStride = 2;
	m_iNumVertexBuffers = 2;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

#pragma region INSTANCE_BUFFER
	/* 사봔을 만들때 실제 활용하기위한 인스턴스 정점 버퍼를 할당하기위해 필요한 초기 데이터를 미리 다 셋팅해놓는다.  */
	ZeroMemory(&m_tagInstanceBufferDesc, sizeof m_tagInstanceBufferDesc);
	m_tagInstanceBufferDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;
	m_tagInstanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_tagInstanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tagInstanceBufferDesc.StructureByteStride = m_iInstanceVertexStride;
	m_tagInstanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_tagInstanceBufferDesc.MiscFlags = 0;

	m_pInstanceVertices = new VTX_PARTICLE_INSTANCE[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTX_PARTICLE_INSTANCE) * m_iNumInstance);

	m_pSpeeds = new _float[m_iNumInstance];
	ZeroMemory(m_pSpeeds, sizeof(_float) * m_iNumInstance);

	m_bLoop = pDesc->isLoop;

	m_vPivot = pDesc->vPivot;

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_float4			vTranslation = {};

		vTranslation.x = m_pGameInstance->Compute_Random(pDesc->vCenter.x - pDesc->vRange.x * 0.5f, pDesc->vCenter.x + pDesc->vRange.x * 0.5f);
		vTranslation.y = m_pGameInstance->Compute_Random(pDesc->vCenter.y - pDesc->vRange.y * 0.5f, pDesc->vCenter.y + pDesc->vRange.y * 0.5f);
		vTranslation.z = m_pGameInstance->Compute_Random(pDesc->vCenter.z - pDesc->vRange.z * 0.5f, pDesc->vCenter.z + pDesc->vRange.z * 0.5f);
		vTranslation.w = 1;

		m_pSpeeds[i] = m_pGameInstance->Compute_Random(pDesc->vSpeed.x, pDesc->vSpeed.y);

		// x축
		m_pInstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		// y축
		m_pInstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		// z축
		m_pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		m_pInstanceVertices[i].vTranslation = vTranslation;
		m_pInstanceVertices[i].vLifeTime.x = m_pGameInstance->Compute_Random(pDesc->vLifeTime.x, pDesc->vLifeTime.y);

		_float			fScale = m_pGameInstance->Compute_Random(pDesc->vSize.x, pDesc->vSize.y);

		XMStoreFloat4(&m_pInstanceVertices[i].vRight, XMLoadFloat4(&m_pInstanceVertices[i].vRight) * fScale);
		XMStoreFloat4(&m_pInstanceVertices[i].vUp, XMLoadFloat4(&m_pInstanceVertices[i].vUp) * fScale);
		XMStoreFloat4(&m_pInstanceVertices[i].vLook, XMLoadFloat4(&m_pInstanceVertices[i].vLook) * fScale);
	}

	m_InstanceInitialData.pSysMem = m_pInstanceVertices;

#pragma endregion

#pragma region VERTEX_BUFFER
	ZeroMemory(&m_tagBufferDesc, sizeof m_tagBufferDesc);
	m_tagBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	m_tagBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tagBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_tagBufferDesc.StructureByteStride = m_iVertexStride;
	m_tagBufferDesc.CPUAccessFlags = 0;
	m_tagBufferDesc.MiscFlags = 0;

	VTXPOINT* pVertices = new VTXPOINT[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXPOINT) * m_iNumVertices);

	pVertices[0].vPosition = _float3(0.f, 0.0f, 0.f);
	pVertices[0].fPointSize = 1.f;

	ZeroMemory(&m_tagInitialData, sizeof m_tagInitialData);
	m_tagInitialData.pSysMem = pVertices;

	if (FAILED(__super::Create_Buffer(&m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region INDEX_BUFFER
	ZeroMemory(&m_tagBufferDesc, sizeof m_tagBufferDesc);
	m_tagBufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	m_tagBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tagBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_tagBufferDesc.StructureByteStride = m_iIndexStride;
	m_tagBufferDesc.CPUAccessFlags = 0;
	m_tagBufferDesc.MiscFlags = 0;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	ZeroMemory(&m_tagInitialData, sizeof m_tagInitialData);
	m_tagInitialData.pSysMem = pIndices;

	if (FAILED(__super::Create_Buffer(&m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);
#pragma endregion
	return S_OK;
}

HRESULT CVIBuffer_Point_Instancing::Initialize(void* _pArg)
{
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Point_Instancing* CVIBuffer_Point_Instancing::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const PARTICLE_INSTANCING_DESC* _pArg)
{
	CVIBuffer_Point_Instancing* pInstance = new CVIBuffer_Point_Instancing(_pDevice, _pContext);
	if (FAILED(pInstance->Initialize_Prototype(_pArg)))
	{
		MSG_BOX("Failed To Created : CVIBuffer_Point_Instancing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Point_Instancing::Clone(void* _pArg)
{
	CVIBuffer_Point_Instancing* pInstance = new CVIBuffer_Point_Instancing(*this);
	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed To Cloned : CVIBuffer_Point_Instancing");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point_Instancing::Free()
{
	__super::Free();
}
