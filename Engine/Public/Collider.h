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

    class CBounding* Get_Bounding() const { return m_pBounding; } // m_pBounding�� CBounding* Ÿ��

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
    // PrimitiveBatch ��� Engine_Define �����, DirectXTK ������ primitivebatch ����� �߰������ ������ ��밡��, Effect ���� ��������.
    // �� PrimitiveBatch ��, �ʿ��� �ִ� ������ ���� ���ۿ� �ε��� ���۸� �˾Ƽ� �Ҵ����ִ� ������ �Ѵ�. 
    // Effect �� �׳� ���� ���̴��� �ǹ��ϰ�, ������, ���� ������ ���̴� ���� �ڵ带 �ۼ����ص� �ȴٶ�� �Ͱ� �׸�ŭ, �������ִ� �� ����ϱ� ������,
    // �߰������� Ư������� �߰��ϴ� ���� �Ұ����ϴ�. 
    // BasicEffect �� ������ ���� ,PrimitiveBatch �� �׸��� ���� �뵵�� ��!! 
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