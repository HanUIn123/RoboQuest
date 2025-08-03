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

    // 내 쉐이더에 필요한 정보, 정의들을 가져와 세팅하는 함수이다. 
    m_pEffect->SetVertexColorEnabled(true);

    const void* pShaderByteCode = { nullptr };
    size_t                      iShaderByteCodeLength = {};

    // 원래 쉐이더에 뭐 해주려면, 해당 쉐이더에, 접근해서 GetPass~~ 해서 가져오는 과정이, 
    // 지금 effect 는, GetVertexShaderBytecode 로 처리한다. 
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

    // 원래 뭐 하나 그릴려면 쉐이더에 바인딩 하는 작업이 있었는데, 그 과정이다. 
    // 쉐이더 월 뷰 투 를 해야하는데 우리는 그전까지 변수하나 써서 그 변수 이름으로 쉐이더에 넘겨줬었는데, 
    // 이름이 없잖아? 근데 또! Effect 에 해당 행렬들을 세팅해서 알아서 넘겨주는 기능의 함수가 있음 
    // SetWorld SetView SetProjection 

    // 일단 월드행렬은 아직은 애매하니, 항등으로 처리해두기! 
    m_pEffect->SetWorld(XMMatrixIdentity());
    m_pEffect->SetView(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW));
    m_pEffect->SetProjection(m_pGameInstance->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ));

    // Effect 원래 쉐이더에서 막 머하고나서 최종적으로 Apply 함수들을 끄집어내서 호출하는 방식이었는데, 이미
    // Effect 이거는 다 기능 구현이 되어있고, 그냥 바로 Apply 쓰면됨, 인자로 Context 추가하면 끝!! 
    m_pEffect->Apply(m_pContext);

    // 그리고 inputlayout 을 원래 해줬어야 하므로, inputlayout 을 변수로 하나 만들어주자. 
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

    // 사본이 아닐 때만!! (원형일 때만, 배치랑 Effect 를 지우자.)
    if (false == m_bClone)
    {
        Safe_Delete(m_pBatch);
        Safe_Delete(m_pEffect);
    }

#endif

}
