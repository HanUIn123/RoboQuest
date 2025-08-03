#include "../Public/Collider.h"

#include "GameInstance.h"
#include "Bounding_AABB.h"


CCollider::CCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CComponent{ _pDevice, _pContext }
{
}

CCollider::CCollider(const CCollider& _Prototype)
    :CComponent(_Prototype)
    , m_eColliderType{ _Prototype.m_eColliderType }
#ifdef _DEBUG
    , m_pBatch{ _Prototype.m_pBatch }
    , m_pEffect{ _Prototype.m_pEffect }
    , m_pInputLayout{ _Prototype.m_pInputLayout }
#endif 
{
#ifdef _DEBUG
    Safe_AddRef(m_pInputLayout);
#endif


}

HRESULT CCollider::Initialize_Prototype(COLLIDER_TYPE _eColliderType)
{
    m_eColliderType = _eColliderType;

#ifdef _DEBUG 
    m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
    m_pEffect = new BasicEffect(m_pDevice);

    // �� ���̴��� �ʿ��� ����, ���ǵ��� ������ �����ϴ� �Լ��̴�. 
    m_pEffect->SetVertexColorEnabled(true);

    const void* pShaderByteCode = { nullptr };
    size_t                      iShaderByteCodeLength = {};

    // ���� ���̴��� �� ���ַ���, �ش� ���̴���, �����ؼ� GetPass~~ �ؼ� �������� ������, 
    // ���� effect ��, GetVertexShaderBytecode �� ó���Ѵ�. 
    m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

    if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode,
        iShaderByteCodeLength, &m_pInputLayout)))
        return E_FAIL;
#endif
    return S_OK;
}

HRESULT CCollider::Initialize(void* _pArg)
{
    CBounding::BOUNDING_DESC* pBoundingDesc = static_cast<CBounding::BOUNDING_DESC*>(_pArg);

    switch (m_eColliderType)
    {
    case Engine::CCollider::CT_AABB:
        m_pBounding = CBounding_AABB::Create(m_pDevice, m_pContext, pBoundingDesc);
        break;
    case Engine::CCollider::CT_OBB:
        m_pBounding = CBounding_OBB::Create(m_pDevice, m_pContext, pBoundingDesc);
        break;
    case Engine::CCollider::CT_SPHERE:
        m_pBounding = CBounding_Sphere::Create(m_pDevice, m_pContext, pBoundingDesc);
        break;
    }

    if (nullptr == m_pBounding)
        return E_FAIL;

    return S_OK;
}

void CCollider::Update(_fmatrix _WorldMatrix)
{
    m_pBounding->Update(_WorldMatrix);
}

_bool CCollider::Intersect(CCollider* _pTargetCollider)
{
    m_isColl = m_pBounding->Intersect(_pTargetCollider->m_eColliderType, _pTargetCollider->m_pBounding);

    return m_isColl;
}

_bool CCollider::Intersect_Ray(const _vector& _vRayStartPosition, const _vector& _vRayDirection, _float& _fDistance)
{
    if (nullptr == m_pBounding)
        return false;

    return m_pBounding->Intersect_Ray(_vRayStartPosition, _vRayDirection, _fDistance);
}

HRESULT CCollider::Render()
{
    if (nullptr == m_pBounding)
        return E_FAIL;

    // ���� �� �ϳ� �׸����� ���̴��� ���ε� �ϴ� �۾��� �־��µ�, �� �����̴�. 
    // ���̴� �� �� �� �� �ؾ��ϴµ� �츮�� �������� �����ϳ� �Ἥ �� ���� �̸����� ���̴��� �Ѱ�����µ�, 
    // �̸��� ���ݾ�? �ٵ� ��! Effect �� �ش� ��ĵ��� �����ؼ� �˾Ƽ� �Ѱ��ִ� ����� �Լ��� ���� 
    // SetWorld SetView SetProjection 

    // �ϴ� ��������� ������ �ָ��ϴ�, �׵����� ó���صα�! 
    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

    // Effect ���� ���̴����� �� ���ϰ��� ���������� Apply �Լ����� ������� ȣ���ϴ� ����̾��µ�, �̹�
    // Effect �̰Ŵ� �� ��� ������ �Ǿ��ְ�, �׳� �ٷ� Apply �����, ���ڷ� Context �߰��ϸ� ��!! 
    m_pEffect->Apply(m_pContext);

    // �׸��� inputlayout �� ���� ������ �ϹǷ�, inputlayout �� ������ �ϳ� ���������. 
    m_pContext->IASetInputLayout(m_pInputLayout);

    m_pBatch->Begin();

    m_pBounding->Render(m_pBatch, true == m_isColl ? XMVectorSet(1.f, 0.f, 0.f, 1.f) : XMVectorSet(0.f, 1.f, 0.f, 1.f));


    m_pBatch->End();

    return S_OK;
}

CCollider* CCollider::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, COLLIDER_TYPE _eColliderType)
{
    CCollider* pInstance = new CCollider(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype(_eColliderType)))
    {
        MSG_BOX("Failed To Created : CCollider");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CCollider::Clone(void* _pArg)
{
    CCollider* pInstance = new CCollider(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Created : CCollider");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCollider::Free()
{
    __super::Free();

    Safe_Release(m_pBounding);

#ifdef _DEBUG
    Safe_Release(m_pInputLayout);

    // �纻�� �ƴ� ����!! (������ ����, ��ġ�� Effect �� ������.)
    if (false == m_bClone)
    {
        Safe_Delete(m_pBatch);
        Safe_Delete(m_pEffect);
    }

#endif

}
