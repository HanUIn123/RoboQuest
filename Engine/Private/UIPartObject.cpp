#include "UIPartObject.h"
#include "GameInstance.h"

CUIPartObject::CUIPartObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CUIObject{_pDevice , _pContext}
{
}

CUIPartObject::CUIPartObject(const CUIPartObject& _Prototype)
    :CUIObject(_Prototype)
    , m_CombinedWorldMatrix{ _Prototype.m_CombinedWorldMatrix }
{
}

HRESULT CUIPartObject::Initialize_Prototype()
{
    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMMatrixIdentity());

    return S_OK;
}

HRESULT CUIPartObject::Initialize(void* _pArg)
{
    UIPARTOBJECT_DESC* pDesc = static_cast<UIPARTOBJECT_DESC*>(_pArg);

    m_pParentWorldMatrix = pDesc->pParentWorldMatrix;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

void CUIPartObject::Priority_Update(_float _fTimeDelta)
{
}

void CUIPartObject::Update(_float _fTimeDelta)
{
}

void CUIPartObject::Late_Update(_float _fTimeDelta)
{
}

HRESULT CUIPartObject::Render()
{
    return S_OK;
}

void CUIPartObject::Free()
{
    __super::Free();
}
