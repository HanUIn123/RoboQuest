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
    // ��������� �ϴ� �׵���ķ� �����ص�.
    // ���⼭�� ���������, �θ� ���� ����� ���� ���� �����̴�. 
    // �� �޹��ε� ���������, Body_Player ���� ���̴��� ������ ����.
    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMMatrixIdentity());

    return S_OK;
}

HRESULT CPartObject::Initialize(void* _pArg)
{
    PARTOBJECT_DESC* pDesc = static_cast<PARTOBJECT_DESC*>(_pArg);

    //�� PartObject Ŭ���� �� ����ü�� ������� ������ ��������� ����������.
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
