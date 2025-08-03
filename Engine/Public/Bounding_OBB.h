#pragma once
#include "Bounding.h"

BEGIN(Engine)
class CBounding_OBB final : public CBounding
{
public:
    struct BOUNDING_OBB_DESC : public CBounding::BOUNDING_DESC
    {
        _float3 vExtents;
        _float3 vRotation;
    };

private:
    CBounding_OBB(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CBounding_OBB() = default;

public:
    virtual void* Get_Desc() const override
    {
        return m_pDesc;
    }

    virtual void Set_Desc(const void* _pDesc) override;

public:
    virtual HRESULT     Initialize(const CBounding::BOUNDING_DESC* _pArg) override;
    virtual void        Update(_fmatrix _WorldMatrix) override;
    virtual HRESULT     Render(PrimitiveBatch<VertexPositionColor>* _pBatch, _fvector _vColor) override;

public:
    virtual _bool       Intersect(CCollider::COLLIDER_TYPE _eColliderType, CBounding* _pTargetBounding) override;
    virtual _bool       Intersect_Ray(const _vector& _vRayStartPosition, const _vector& _vRayDirection, _float& _fDistance) override;
private:
    BoundingOrientedBox* m_pPrevDesc = { nullptr };
    BoundingOrientedBox* m_pDesc = { nullptr };

public:
    static CBounding_OBB* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const CBounding::BOUNDING_DESC* _pArg);
    virtual void Free() override;
};

END