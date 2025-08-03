#include "EffectPartObject.h"
#include "GameInstance.h"

CEffectPartObject::CEffectPartObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CBlendObject{_pDevice, _pContext}
{
}

CEffectPartObject::CEffectPartObject(const CEffectPartObject& _Prototype)
    :CBlendObject(_Prototype)
{
}

HRESULT CEffectPartObject::Initialize_Prototype()
{
    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMMatrixIdentity());

    return S_OK;
}

HRESULT CEffectPartObject::Initialize(void* _pArg)
{
    EFFECT_PARTOBJECT_DESC* pDesc = static_cast<EFFECT_PARTOBJECT_DESC*>(_pArg);

    m_pParentWorldMatrix = pDesc->pParentWorldMatrix;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

void CEffectPartObject::Priority_Update(_float _fTimeDelta)
{
}

void CEffectPartObject::Update(_float _fTImeDelta)
{
}

void CEffectPartObject::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_BLEND, this);
}

HRESULT CEffectPartObject::Render()
{
    return S_OK;
}

void CEffectPartObject::Free()
{
    __super::Free();
}
