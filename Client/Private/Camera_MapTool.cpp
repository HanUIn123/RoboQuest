#include "pch.h"
#include "Camera_MapTool.h"
#include "GameInstance.h"

CCamera_MapTool::CCamera_MapTool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CCamera{ _pDevice, _pContext }
    , m_fMouseSensity{ 0 }
{
}

CCamera_MapTool::CCamera_MapTool(const CCamera_MapTool& _Prototype)
    :CCamera(_Prototype)
    , m_fMouseSensity{ 0 }
{
}

HRESULT CCamera_MapTool::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera_MapTool::Initialize(void* _pArg)
{
    CAMERA_MAPTOOL_DESC* pDesc = static_cast<CAMERA_MAPTOOL_DESC*>(_pArg);

    m_fMouseSensity = pDesc->fMouseSensity;

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_MAPTOOL, TEXT("Prototype_Component_VIBuffer_MapToolTerrain"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pMTTerrain_BufferCom))))
        return E_FAIL;

    return S_OK;
}

void CCamera_MapTool::Priority_Update(_float _fTimeDelta)
{
    Move_Camera(_fTimeDelta);

    Toggle_Button(_fTimeDelta);

    __super::Priority_Update(_fTimeDelta);
}

void CCamera_MapTool::Late_Update(_float _fTimeDelta)
{
#ifdef _DEBUG
    ImGui::Begin("Device Settings", NULL, ImGuiWindowFlags_MenuBar);
    _float CameraSpeed = m_pTransformCom->Get_Speed();
    ImGui::SliderFloat("Camera Speed", &(CameraSpeed), 10.0f, 80.0f);
    m_pTransformCom->Set_Speed(CameraSpeed);
    ImGui::End();
#endif
}

void CCamera_MapTool::Update(_float _fTimeDelta)
{
}

HRESULT CCamera_MapTool::Render()
{
    return S_OK;
}

void CCamera_MapTool::Fix_MouseCursor()
{
    _uint2      vViewPortSize = m_pGameInstance->Get_ViewportSize();
    POINT       ptMouse{ LONG(vViewPortSize.x >> 1), LONG(vViewPortSize.y >> 1) };

    ClientToScreen(g_hWnd, &ptMouse);
    SetCursorPos(ptMouse.x, ptMouse.y);
}

void CCamera_MapTool::Move_Mouse(_float _fTimeDelta)
{
    if (m_bCameraFix)
        return;

    _long	MouseMove = {};

    if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_X))
    {
        m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), _fTimeDelta * MouseMove * m_fMouseSensity);
    }

    if (MouseMove = m_pGameInstance->Get_DIMouseMove(DIMS_Y))
    {
        m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), _fTimeDelta * MouseMove * m_fMouseSensity);
    }
}

void CCamera_MapTool::Move_Camera(_float _fTimeDelta)
{
    if (m_pGameInstance->Get_DIKeyState(DIK_W) & 0x80)
    {
        m_pTransformCom->Go_Straight(_fTimeDelta);
    }
    if (m_pGameInstance->Get_DIKeyState(DIK_S) & 0x80)
    {
        m_pTransformCom->Go_Backward(_fTimeDelta);
    }
    if (m_pGameInstance->Get_DIKeyState(DIK_A) & 0x80)
    {
        m_pTransformCom->Go_Left(_fTimeDelta);
    }
    if (m_pGameInstance->Get_DIKeyState(DIK_D) & 0x80)
    {
        m_pTransformCom->Go_Right(_fTimeDelta);
    }
}

void CCamera_MapTool::Toggle_Button(_float _fTimeDelta)
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

void CCamera_MapTool::Switch_To_FPV(_float _fTimeDelta)
{
   
}

void CCamera_MapTool::Click_Terrain(_float _fTimDelta)
{
    
}

void CCamera_MapTool::Switch_SightView()
{
    const float MapSize = 129.0f;
    const float HalfMapSize = MapSize / 2.0f;

    _vector vCameraPosition = XMVectorSet(HalfMapSize, 150.0f, HalfMapSize, 1.0f);
    _vector vCameraLook = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCameraPosition);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, vCameraLook);

    m_bCameraFix = true;
}

CCamera_MapTool* CCamera_MapTool::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCamera_MapTool* pInstance = new CCamera_MapTool(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Create : CCamera_MapTool");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CCamera_MapTool::Clone(void* _pArg)
{
    CCamera_MapTool* pInstance = new CCamera_MapTool(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CCamera_MapTool");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_MapTool::Free()
{
    __super::Free();

    Safe_Release(m_pMTTerrain_BufferCom);
}
