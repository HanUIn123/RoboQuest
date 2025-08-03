#include "../Public/UIObject.h"
#include "GameInstance.h"

CUIObject::CUIObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGameObject{_pDevice, _pContext}
{
}

CUIObject::CUIObject(const CUIObject& _Prototype)
    : CGameObject(_Prototype)
    , m_ViewMatrix{_Prototype.m_ViewMatrix}
    , m_ProjMatrix{_Prototype.m_ProjMatrix}
{
}

HRESULT CUIObject::Initialize_Prototype()
{
    // 항등행렬로 뷰와 투영 행렬 만들어주기.
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());

    return S_OK;
}

HRESULT CUIObject::Initialize(void* _pArg)
{
    // 현재 뷰포트 정보를 얻어옴. 
    UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(_pArg);

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    _uint2 vViewportSize = m_pGameInstance->Get_ViewportSize();

    m_pTransformCom->Scaling(_float3(pDesc->fSizeX, pDesc->fSizeY, 1.0f));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(pDesc->fX - (vViewportSize.x * 0.5f), -pDesc->fY + (vViewportSize.y * 0.5f), pDesc->fZ, 1.0f));
    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(700.0f, 700.0f, 0.0f, 1.0f));

    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH((float)vViewportSize.x, (float)vViewportSize.y, pDesc->fNear, pDesc->fFar));
    //XMStoreFloat4x4(&m_ProjMatrix, XMMatrixPerspectiveFovLH(0.25f * 3.14, 1280.0f / 720.0f, pDesc->fNear, pDesc->fFar));
    return S_OK;
}

void CUIObject::Priority_Update(_float _fTimeDelta) 
{
}

void CUIObject::Update(_float _fTimeDelta)
{
}

void CUIObject::Late_Update(_float _fTimeDelta)
{
}

HRESULT CUIObject::Render()
{
    return S_OK;
}

void CUIObject::Free()
{
    __super::Free();
}
