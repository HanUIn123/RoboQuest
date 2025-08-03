#include "Bounding_AABB.h"

CBounding_AABB::CBounding_AABB(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CBounding{ _pDevice, _pContext }
{
}



void CBounding_AABB::Set_Desc(const void* _pDesc)
{
    // nullptr üũ
    if (nullptr == _pDesc)
        return;

    // ���޹��� �����͸� BOUNDING_AABB_DESC�� ��ȯ
    const BOUNDING_AABB_DESC* pAABBDesc = static_cast<const BOUNDING_AABB_DESC*>(_pDesc);

    // m_pDesc�� nullptr�̸� ���� ����
    if (nullptr == m_pDesc)
    {
        m_pDesc = new BoundingBox;
    }

    // Center�� Extents �� ������Ʈ (XMFLOAT3�� ���� ����)
    m_pDesc->Center = pAABBDesc->vCenter;   // XMFLOAT3 ����
    m_pDesc->Extents = pAABBDesc->vExtents; // XMFLOAT3 ����
}

HRESULT CBounding_AABB::Initialize(const CBounding::BOUNDING_DESC* _pArg)
{
    const BOUNDING_AABB_DESC* pDesc = static_cast<const BOUNDING_AABB_DESC*>(_pArg);

    // ���� �ݶ��̴��� ���� ��ġ �������� �� ���°Ŷ�, ����� �ؾ��ϴϱ�, �ƿ� BoundingBox �ϳ� �� ����, 
    // ���� ���� �ٿ�� �ڽ���, -> �̰� ���� ���� �ٿ�� �ڽ���
    // �긦 �ƽθ� ���� �����ؼ�, ����� ���� �ٿ�� �ڽ���, ó������ 
    m_pPrevDesc = new BoundingBox(pDesc->vCenter, pDesc->vExtents);
    m_pDesc = new BoundingBox(*m_pPrevDesc);

    //m_pBoxDesc = new BoundingBox(pDesc->vCenter, pDesc->vExtents);

    return S_OK;
}

void CBounding_AABB::Update(_fmatrix _WorldMatrix)
{
    _matrix         TransformMatrix = _WorldMatrix;

    // ���� �÷��̾����� �ٿ��� �ݶ��̴� �ڽ� (AABB ��) ����ϰ���, �÷��̾� ȸ������ ����, �ڽ��� ������� �Ÿ�. 
    // �װ� �Ž����� �̰� ó���ؾ߰ٴ� ��� �ϸ�, 

    // ����� 1 ~ 3����, ��İ�����, �� ����� �� ���̸� ���� ������ ��������. �׷� ���� ��, �� ���� ���̰� 2 ���� �׷���,
    // 1���� ���Ұ� 2,0,0,0 �� �Ǵ� ����. 
    // ���� 1 0 0 0  �̶� �س��� �� 1 ���ٰ� �׳� �� 1 0 0 0 �߿��� �̾ƿ� ����(1) �� �׳� �״�� ���ϰ� ������, �ڽ� ������ 1�� �ɰ��� 
    // ���� �ƹ� ���̳� ���� 1 ���� 2 ��� �ϸ� �׸� ŭ �ɲ��� 
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

    // BoundingBox::Intersects �����ϳ� ����
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
