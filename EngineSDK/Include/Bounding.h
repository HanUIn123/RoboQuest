#pragma once
#include "Collider.h"
#include "DebugDraw.h"

BEGIN(Engine)
class CBounding abstract : public CBase
{
public:
    struct BOUNDING_DESC
    {
        _float3     vCenter;
    };

protected:
    CBounding(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CBounding() = default;

public:
    virtual void* Get_Desc() const = 0;
    virtual void Set_Desc(const void* _pDesc) = 0; // 인터페이스 추가

public:
    virtual HRESULT     Initialize(const CBounding::BOUNDING_DESC* _pArg) = 0;
    virtual void        Update(_fmatrix _WorldMatrix) = 0;

    // DebugDraw 를 통해서 박스 그릴꺼임!! 
    virtual HRESULT     Render(PrimitiveBatch<VertexPositionColor>* _pBatch, _fvector _vColor) = 0;

public:
    virtual _bool Intersect(CCollider::COLLIDER_TYPE eColliderType, CBounding* pTargetBounding) = 0;
    virtual _bool Intersect_Ray(const _vector& _vRayStartPosition, const _vector& _vRayDirection, _float& _fDistance) = 0;
protected:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

public:
    virtual void Free();

};

END