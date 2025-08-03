#include "pch.h"
#include "ProfileImage.h"
#include "GameInstance.h"

CProfileImage::CProfileImage(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CUIObject{_pDevice, _pContext}
{
}

CProfileImage::CProfileImage(const CProfileImage& _Prototype)
    :CUIObject(_Prototype)
{
}

HRESULT CProfileImage::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CProfileImage::Initialize(void* _pArg)
{
    return S_OK;
}

void CProfileImage::Priority_Update(_float _fTimeDelta)
{
}

void CProfileImage::Update(_float _fTimeDelta)
{
}

void CProfileImage::Late_Update(_float _fTimeDelta)
{
}

HRESULT CProfileImage::Render()
{
    return S_OK;
}

HRESULT CProfileImage::Ready_Component()
{
    return S_OK;
}

CProfileImage* CProfileImage::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    return nullptr;
}

CGameObject* CProfileImage::Clone(void* _pArg)
{
    return nullptr;
}

void CProfileImage::Free()
{
}
