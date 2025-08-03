#include "Bounding_AABB.h"

CBounding_AABB::CBounding_AABB(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CBounding{ _pDevice, _pContext }
{
}



void CBounding_AABB::Set_Desc(const void* _pDesc)
{
    // nullptr 체크
    if (nullptr == _pDesc)
        return;

    // 전달받은 데이터를 BOUNDING_AABB_DESC로 변환
    const BOUNDING_AABB_DESC* pAABBDesc = static_cast<const BOUNDING_AABB_DESC*>(_pDesc);

    // m_pDesc가 nullptr이면 새로 생성
    if (nullptr == m_pDesc)
    {
        m_pDesc = new BoundingBox;
    }

    // Center와 Extents 값 업데이트 (XMFLOAT3로 직접 대입)
    m_pDesc->Center = pAABBDesc->vCenter;   // XMFLOAT3 대입
    m_pDesc->Extents = pAABBDesc->vExtents; // XMFLOAT3 대입
}

HRESULT CBounding_AABB::Initialize(const CBounding::BOUNDING_DESC* _pArg)
{
    const BOUNDING_AABB_DESC* pDesc = static_cast<const BOUNDING_AABB_DESC*>(_pArg);

    // 현재 콜라이더는 로컬 위치 기준으로 딱 나온거라서, 월드로 해야하니까, 아예 BoundingBox 하나 더 만들어서, 
    // 원래 원본 바운딩 박스를, -> 이게 로컬 때의 바운딩 박스임
    // 얘를 아싸리 새로 복사해서, 월드용 때의 바운딩 박스로, 처리하자 
    m_pPrevDesc = new BoundingBox(pDesc->vCenter, pDesc->vExtents);
    m_pDesc = new BoundingBox(*m_pPrevDesc);

    //m_pBoxDesc = new BoundingBox(pDesc->vCenter, pDesc->vExtents);

    return S_OK;
}

void CBounding_AABB::Update(_fmatrix _WorldMatrix)
{
    _matrix         TransformMatrix = _WorldMatrix;

    // 현재 플레이어한테 붙여온 콜라이더 박스 (AABB 용) 출력하고나서, 플레이어 회전시켜 보면, 박스가 움찔움찔 거림. 
    // 그게 거슬려서 이거 처리해야겟다 라고 하면, 

    // 행렬의 1 ~ 3행의, 행렬값을에, 그 행렬의 행 길이를 꺼낸 값으로 곱해주자. 그럼 예를 들어서, 각 행의 길이가 2 였다 그러면,
    // 1행의 원소가 2,0,0,0 이 되는 식임. 
    // 현재 1 0 0 0  이라 해놓고 이 1 에다가 그냥 이 1 0 0 0 중에서 뽑아온 길이(1) 로 그냥 그대로 곱하고 있으니, 박스 간격이 1이 될것임 
    // 내가 아무 행이나 가서 1 말고 2 라고 하면 그만 큼 될꺼임 
    TransformMatrix.r[0] = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f) * XMVector3Length(_WorldMatrix.r[0]);
    TransformMatrix.r[1] = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) * XMVector3Length(_WorldMatrix.r[1]);
    TransformMatrix.r[2] = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f) * XMVector3Length(_WorldMatrix.r[2]);

    m_pPrevDesc->Transform(*m_pDesc, TransformMatrix);
}

HRESULT CBounding_AABB::Render(PrimitiveBatch<VertexPositionColor>* _pBatch, _fvector _vColor)
{
    DX::Draw(_pBatch, *m_pDesc, _vColor);

    return S_OK;
}

_bool CBounding_AABB::Intersect(CCollider::COLLIDER_TYPE _eColliderType, CBounding* _pTargetBounding)
{
    void* pTargetDesc = _pTargetBounding->Get_Desc();

    _bool	isColl = { false };

    switch (_eColliderType)
    {
    case CCollider::CT_AABB:
        isColl = m_pDesc->Intersects(*static_cast<BoundingBox*>(pTargetDesc));
        // isColl = Intersect_AABB(static_cast<BoundingBox*>(pTargetDesc));
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

_bool CBounding_AABB::Intersect_Ray(const _vector& _vRayStartPosition, const _vector& _vRayDirection, _float& _fDistance)
{
    if (nullptr == m_pDesc)
        return false;

    // BoundingBox::Intersects 제공하네 ㅋㅋ
    return m_pDesc->Intersects(_vRayStartPosition, _vRayDirection, _fDistance);
}

CBounding_AABB* CBounding_AABB::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const CBounding::BOUNDING_DESC* _pArg)
{
    CBounding_AABB* pInstance = new CBounding_AABB(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Created : CBounding_AABB");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBounding_AABB::Free()
{
    __super::Free();

    Safe_Delete(m_pPrevDesc);
    Safe_Delete(m_pDesc);
}
