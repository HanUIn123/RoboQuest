#include "../Public/PartObject.h"
#include "GameInstance.h"

CPartObject::CPartObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGameObject{ _pDevice , _pContext }
{
}

CPartObject::CPartObject(const CPartObject& _Prototype)
    :CGameObject(_Prototype)
    , m_CombinedWorldMatrix{ _Prototype.m_CombinedWorldMatrix }
{
}

HRESULT CPartObject::Initialize_Prototype()
{
    // 최종행렬을 일단 항등행렬로 저장해둠.
    // 여기서의 최종행렬은, 부모 상태 행렬을 포함 중인 상태이다. 
    // 이 콤바인드 최종행렬을, Body_Player 에서 쉐이더로 던져줄 것임.
    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMMatrixIdentity());

    return S_OK;
}

HRESULT CPartObject::Initialize(void* _pArg)
{
    PARTOBJECT_DESC* pDesc = static_cast<PARTOBJECT_DESC*>(_pArg);

    //이 PartObject 클래스 속 구조체의 월드행렬 정보를 멤버변수에 저장해주자.
    m_pParentWorldMatrix = pDesc->pParentWorldMatrix;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

void CPartObject::Priority_Update(_float _fTimeDelta)
{
}

void CPartObject::Update(_float _fTimeDelta)
{
}

void CPartObject::Late_Update(_float _fTimeDelta)
{
}

HRESULT CPartObject::Render()
{
    return S_OK;
}


void CPartObject::Free()
{
    __super::Free();


}
