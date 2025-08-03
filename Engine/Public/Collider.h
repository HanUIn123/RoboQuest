#pragma once
#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CCollider final: public CComponent
{
public:
    enum COLLIDER_TYPE { CT_AABB, CT_OBB, CT_SPHERE, CT_END };
private:
    CCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CCollider(const CCollider& _Prototype);
    virtual ~CCollider() = default;

public:
    virtual HRESULT     Initialize_Prototype(COLLIDER_TYPE _eColliderType);
    virtual HRESULT     Initialize(void* _pArg) override;

    virtual void        Update(_fmatrix _WorldMatrix);

public:
    _bool               Intersect(CCollider* _pTargetCollider);
    _bool               Intersect_Ray(const _vector& _vRayStartPosition, const _vector& _vRayDirection, _float& _fDistance);

    class CBounding* Get_Bounding() const { return m_pBounding; } // m_pBounding은 CBounding* 타입

#ifdef _DEBUG
public:
    virtual HRESULT     Render();
#endif


public:
    COLLIDER_TYPE               m_eColliderType = { CT_END };
    class CBounding*            m_pBounding = { nullptr };
    _bool                       m_isColl = { false };

#ifdef _DEBUG
private:
    // PrimitiveBatch 얘는 Engine_Define 헤더에, DirectXTK 에서의 primitivebatch 헤더를 추가해줬기 때문에 사용가능, Effect 또한 마찬가지.
    // 이 PrimitiveBatch 는, 필요한 최대 갯수의 정점 버퍼와 인덱스 버퍼를 알아서 할당해주는 역할을 한다. 
    // Effect 는 그냥 거의 쉐이더를 의미하고, 장점은, 내가 별도로 쉐이더 파일 코드를 작성안해도 된다라는 것과 그만큼, 제공되있는 걸 사용하기 때문에,
    // 추가적으로 특정기능을 추가하는 것은 불가능하다. 
    // BasicEffect 의 역할은 그저 ,PrimitiveBatch 를 그리기 위한 용도일 뿐!! 
    PrimitiveBatch<DirectX::VertexPositionColor>* m_pBatch = { nullptr };
    BasicEffect* m_pEffect = { nullptr };
    ID3D11InputLayout* m_pInputLayout = { nullptr };

#endif


public:
    static CCollider* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, COLLIDER_TYPE _eColliderType);
    virtual CComponent* Clone(void* _pArg) override;
    virtual void Free() override;
};

END