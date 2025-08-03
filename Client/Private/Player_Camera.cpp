#include "pch.h"
#include "Player_Camera.h"
#include "GameInstance.h"


CPlayer_Camera::CPlayer_Camera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CPartObject{_pDevice, _pContext}
{
}

CPlayer_Camera::CPlayer_Camera(const CPlayer_Camera& _Prototype)
    :CPartObject(_Prototype)
{
}

HRESULT CPlayer_Camera::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CPlayer_Camera::Initialize(void* _pArg)
{
    PLAYER_CAMERA_DESC* pDesc = static_cast<PLAYER_CAMERA_DESC*>(_pArg);

    m_pParentWorldMatrix = pDesc->pParentWorldMatrix;
    m_fFov = pDesc->fFov;
    m_fNear = pDesc->fNear;
    m_fFar = pDesc->fFar;

    _uint2 vViewPortSize = m_pGameInstance->Get_ViewportSize();
    m_fAspect = static_cast<_float>(vViewPortSize.x) / vViewPortSize.y;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

void CPlayer_Camera::Priority_Update(_float _fTimeDelta)
{  
    if (!m_bActive)
        return;

    _matrix ParentMatrix = XMLoadFloat4x4(m_pParentWorldMatrix);

    //static _float fRightOffset = 0.157f;
    //static _float fUpOffset = 0.020f;
    //static _float fBackOffset = 0.009f;

    //static _float fYaw = 0.223f;
    //static _float fPitch = 0.0f;
    //static _float fRoll = 0.0f;

    static _float fRightOffset = 0.13f;
    static _float fUpOffset = 0.11f;
    static _float fBackOffset = 0.009f;

    static _float fYaw = 0.344f;
    static _float fPitch = 0.0f;
    static _float fRoll = 0.0f;


    _vector vOffset = XMVectorSet(fRightOffset, fUpOffset, fBackOffset, 0.0f);
    _vector vCameraPosition = XMVector3TransformCoord(vOffset, ParentMatrix);

    if (m_bShakeActive)
    {
        m_fShakeTimer -= _fTimeDelta;
        m_fShakeTimeElapsed += _fTimeDelta * m_fShakeFrequency; // 시간 누적 (진동 속도 조절)

        if (m_fShakeTimer <= 0.0f)
        {
            m_bShakeActive = false;
        }
        else
        {
            float shakeX = sin(m_fShakeTimeElapsed) * m_fShakeIntensity;
            float shakeY = cos(m_fShakeTimeElapsed * 1.5f) * m_fShakeIntensity * 0.5f; // Y축은 조금 더 약하게
            float shakeZ = sin(m_fShakeTimeElapsed * 0.8f) * m_fShakeIntensity * 0.3f; // Z축도 랜덤 패턴 추가

            vCameraPosition += XMVectorSet(shakeX, shakeY, shakeZ, 0.0f);
        }
    }


    _matrix RotationMatrix = XMMatrixRotationRollPitchYaw(fPitch, fYaw, fRoll);
    _vector vTransformedLook = XMVector3TransformNormal(ParentMatrix.r[2], RotationMatrix);
    _vector vTransformedUp = XMVector3TransformNormal(ParentMatrix.r[1], RotationMatrix);
    _vector vTransformedRight = XMVector3TransformNormal(ParentMatrix.r[0], RotationMatrix);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCameraPosition);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vTransformedLook));
    m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vTransformedUp));
    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vTransformedRight));

    m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
    m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFov, m_fAspect, m_fNear, m_fFar));

//#ifdef _DEBUG
//    ImGui::Begin("Camera Adjustment");
//    ImGui::SliderFloat("Right Offset", &fRightOffset, -1.0f, 1.0f);
//    ImGui::SliderFloat("Up Offset", &fUpOffset, -1.0f, 20.0f);
//    ImGui::SliderFloat("Look Offset", &fBackOffset, -0.300f, 20.0f);
//
//    ImGui::SliderFloat("Yaw", &fYaw, -XM_PI, XM_PI);
//    ImGui::SliderFloat("Pitch", &fPitch, -XM_PI / 2, XM_PI / 2);
//    ImGui::SliderFloat("Roll", &fRoll, -XM_PI, XM_PI);
//    ImGui::End();
//#endif
}

void CPlayer_Camera::Update(_float _fTimeDelta)
{ 
  
}

void CPlayer_Camera::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CPlayer_Camera::Render()
{
    return S_OK;
}

CPlayer_Camera::Ray CPlayer_Camera::Create_Ray()
{
    Ray ray;
    ray.vStartPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    ray.vRayDirection = XMVector3Normalize(Get_LookVector());

    return ray;
}

void CPlayer_Camera::Set_CameraPosition(_vector _vPosition, _vector _vDirection)
{
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vPosition);
}

void CPlayer_Camera::Update_Camera(_float _fCurrentFov, _float _fLastFov, _float _fTimeDelta, _float _fSpeed)
{
    //_fCurrentFov += (_fLastFov - _fCurrentFov) * _fSpeed * _fTimeDelta;

    //if (fabs(_fLastFov - _fCurrentFov) < 0.01f)
    //{
    //    _fCurrentFov = _fLastFov;
    //}
    //XMConvertToRadians()


}

void CPlayer_Camera::Start_CameraShake(_float _fDuration, _float _fIntensity, _float _fFrequency)
{
    m_bShakeActive = true;
    m_fShakeDuration = _fDuration;
    m_fShakeTimer = _fDuration;
    m_fShakeIntensity = _fIntensity;
    m_fShakeFrequency = _fFrequency;
    m_fShakeTimeElapsed = 0.0f; // 초기화
}

const XMMATRIX CPlayer_Camera::Get_ViewMatrix() const
{
    return  XMLoadFloat4x4(&m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW));
}

const XMMATRIX CPlayer_Camera::Get_ProjMatrix() const
{
    return  XMLoadFloat4x4(&m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ));
}


CPlayer_Camera* CPlayer_Camera::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPlayer_Camera* pInstance = new CPlayer_Camera(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CPlayer_Camera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayer_Camera::Clone(void* _pArg)
{
    CPlayer_Camera* pInstance = new CPlayer_Camera(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CPlayer_Camera");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayer_Camera::Free()
{
    __super::Free();
}
