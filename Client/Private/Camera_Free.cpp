#include "pch.h"
#include "Camera_Free.h"
#include "GameInstance.h"
#include "Player.h"

CCamera_Free::CCamera_Free(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CCamera{ _pDevice, _pContext }
    , m_fMouseSensity{ 0 }
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& _Prototype)
    :CCamera(_Prototype)
    , m_fMouseSensity{ 0 }
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CCamera_Free::Initialize(void* _pArg)
{
    CAMERA_FREE_DESC* pDesc = static_cast<CAMERA_FREE_DESC*>(_pArg);

    m_fMouseSensity = pDesc->fMouseSensity;

    // 카메라 초기 위치를 플레이어 위치로 설정
    //CGameObject* pPlayerObject = m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, TEXT("Layer_Player"));
    //if (pPlayerObject)
    //{
    //    CTransform* pPlayerTransform = static_cast<CTransform*>(pPlayerObject->Find_Component(TEXT("Com_Transform")));
    //    if (pPlayerTransform)
    //    {
    //        _vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
    //        m_fCameraPosX = XMVectorGetX(vPlayerPos);
    //        m_fCameraPosY = XMVectorGetY(vPlayerPos);
    //        m_fCameraPosZ = XMVectorGetZ(vPlayerPos);
    //    }
    //}

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    return S_OK;
}

void CCamera_Free::Priority_Update(_float _fTimeDelta)
{
    if (m_bFPV)
        Move_Mouse(_fTimeDelta);
    else
    {
        if (m_pGameInstance->Get_DIKeyState(DIK_UP) & 0x80)
        {
            m_pTransformCom->Go_Straight(_fTimeDelta);
        }
        if (m_pGameInstance->Get_DIKeyState(DIK_DOWN) & 0x80)
        {
            m_pTransformCom->Go_Backward(_fTimeDelta);
        }
        if (m_pGameInstance->Get_DIKeyState(DIK_LEFT) & 0x80)
        {
            m_pTransformCom->Go_Left(_fTimeDelta);
        }
        if (m_pGameInstance->Get_DIKeyState(DIK_RIGHT) & 0x80)
        {
            m_pTransformCom->Go_Right(_fTimeDelta);
        }
    }

    Toggle_Tab(_fTimeDelta);

    __super::Priority_Update(_fTimeDelta);
}

void CCamera_Free::Late_Update(_float _fTimeDelta)
{
#ifdef _DEBUG   
    _float CameraSpeed = m_pTransformCom->Get_Speed();
    ImGui::SliderFloat("##1", &(CameraSpeed), 80.0f, 85.0f);
    m_pTransformCom->Set_Speed(CameraSpeed);
#endif
}

void CCamera_Free::Update(_float _fTimeDelta)
{
    static float fRightOffset = 0.0f; 
    static float fUpOffset = 7.0f;
    static float fLookOffset = -20.0f; 

    if (m_bFPV)
    {
        CGameObject* pPlayerObject = m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, TEXT("Layer_Player"));
        if (!pPlayerObject) return;

        CTransform* pPlayerTransform = static_cast<CTransform*>(pPlayerObject->Find_Component(TEXT("Com_Transform")));
        if (!pPlayerTransform) return;

        _vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
        _vector vPlayerLook = pPlayerTransform->Get_State(CTransform::STATE_LOOK);    
        _vector vPlayerUp = pPlayerTransform->Get_State(CTransform::STATE_UP);        
        _vector vPlayerRight = pPlayerTransform->Get_State(CTransform::STATE_RIGHT);  

        _vector vCameraPosition = vPlayerPos + (vPlayerRight * fRightOffset) + (vPlayerLook * fLookOffset) + XMVectorSet(0.f, fUpOffset, 0.f, 0.f);
        //_vector vCameraPosition = vPlayerPos;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCameraPosition);
        m_pTransformCom->Set_State(CTransform::STATE_LOOK, vPlayerLook);
        m_pTransformCom->Set_State(CTransform::STATE_UP, vPlayerUp);
        m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vPlayerRight);
    }

//#ifdef _DEBUG
//    ImGui::Begin("Camera Adjustment"); 
//    ImGui::SliderFloat("Right Offset", &fRightOffset, 0.0f, 1.0f); 
//    ImGui::SliderFloat("Up Offset", &fUpOffset, 0.0f, 20.0f);
//    ImGui::SliderFloat("Look Offset", &fLookOffset, -20.f, 20.0f);  
//    ImGui::End(); 
//#endif
    Switch_To_FPV(_fTimeDelta);

    __super::Update(_fTimeDelta);
}

HRESULT CCamera_Free::Render()
{
    return S_OK;
}

void CCamera_Free::Fix_MouseCursor()
{
    _uint2      vViewPortSize = m_pGameInstance->Get_ViewportSize();
    POINT       ptMouse{ LONG(vViewPortSize.x >> 1), LONG(vViewPortSize.y >> 1) };

    ClientToScreen(g_hWnd, &ptMouse);
    SetCursorPos(ptMouse.x, ptMouse.y);
}

void CCamera_Free::Move_Mouse(_float _fTimeDelta)
{
    // 이제 보스레벨로 오면 이게 안되는구나.. ㅇㅅㅇ ..
    CGameObject* pPlayerObject = m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, TEXT("Layer_Player"));
    CTransform*  pPlayerTransform = static_cast<CTransform*>(pPlayerObject->Find_Component(TEXT("Com_Transform")));

    _long	MouseMove = {};

    if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
    {
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), _fTimeDelta * MouseMove * m_fMouseSensity * 0.3f);
        pPlayerTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), _fTimeDelta * MouseMove * m_fMouseSensity * 0.3f);
    }

    if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
    {
        m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), _fTimeDelta * MouseMove * m_fMouseSensity * 0.3f);
        pPlayerTransform->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), _fTimeDelta * MouseMove * m_fMouseSensity * 0.3f);
    }
}

void CCamera_Free::Toggle_Tab(_float _fTimeDelta)
{
    if (m_pGameInstance->Get_DIKeyState(DIK_Q) & 0x80)
    {
        if (!m_bTabbed)
        {
            m_bTabbed = true;
            m_bPressed = !m_bPressed;
        }
    }
    else
        m_bTabbed = false;

    if (m_bPressed)
    {
        Move_Mouse(_fTimeDelta);
        Fix_MouseCursor();
    }
}

void CCamera_Free::Switch_To_FPV(_float _fTimeDelta)
{
    if (m_pGameInstance->Get_DIKeyState(DIK_V) & 0x80)
        m_bFPV = true;
    //else
    //    m_bFPV = false;
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCamera_Free* pInstance = new CCamera_Free(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CCamera_Free");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CCamera_Free::Clone(void* _pArg)
{
    CCamera_Free* pInstance = new CCamera_Free(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CCamera_Free");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Free::Free()
{
    __super::Free();
}
