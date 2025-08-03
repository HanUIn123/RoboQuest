#include "../Public/GameInstance.h"
#include "Timer_Manager.h"
#include "Graphic_Device.h"
#include "Level_Manager.h"
#include "Prototype_Manager.h"
#include "Object_Manager.h"
#include "Input_Device.h"
#include "Light_Manager.h"
#include "Font_Manager.h"
#include "RenderTarget_Manager.h"
#include "Picking.h"
#include "Shadow.h"
#include "Frustum.h"
#include "SoundMgr.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
{
}

void CGameInstance::Hide_MouseCursor(_bool _bHideOn)
{
    if (_bHideOn)
        ShowCursor(false);
    else
        ShowCursor(true);
}

void CGameInstance::Click_Once(function<void()> _function)
{
    if (this->Get_DIMouseState(DIM_LB) & 0x80)
    {
        m_bClicked = true;
    }
    if (!(this->Get_DIMouseState(DIM_LB) & 0x80) && m_bClicked)
    {
        m_bClicked = false;
        if (_function)
            _function();
    }
}

_bool CGameInstance::Press_Once(_ubyte _byteKeyID, function<void()> _function)
{
    if (this->Get_DIKeyState(_byteKeyID) & 0x80)
    {
        m_bPressed = true;
        return false;
    }
    if (!(this->Get_DIKeyState(_byteKeyID) & 0x80) && m_bPressed)
    {
        m_bPressed = false;
        if (_function)
            _function();
        return true;
    }
    return false;
}

HRESULT CGameInstance::Initialize_Engine(const ENGINE_DESC& _EngineDesc, _Inout_ ID3D11Device** _ppDevice, _Inout_ ID3D11DeviceContext** _ppContext)
{
    m_iViewportWidth = _EngineDesc.iViewportWidth;
    m_iViewportHeight = _EngineDesc.iViewportHeight;

    m_pGraphic_Device = CGraphic_Device::Create(_EngineDesc.hWnd, _EngineDesc.isWindowed, _EngineDesc.iViewportWidth, _EngineDesc.iViewportHeight, _ppDevice, _ppContext);
    if (nullptr == m_pGraphic_Device)
        return E_FAIL;

    m_pTimer_Manager = CTimer_Manager::Create();
    if (nullptr == m_pTimer_Manager)
        return E_FAIL;

    m_pLight_Manager = CLight_Manager::Create(*_ppDevice, *_ppContext);
    if (nullptr == m_pLight_Manager)
        return E_FAIL;

    m_pInput_Device = CInput_Device::Create(_EngineDesc.hInstance, _EngineDesc.hWnd);
    if (nullptr == m_pInput_Device)
        return E_FAIL;

    m_pPipeLine = CPipeLine::Create();
    if (nullptr == m_pPipeLine)
        return E_FAIL;
    
    // Level_Manager의 Create는 별도의 Initialize함수 없이, 곧바로 생성자를 호출하고 있음.
    m_pLevel_Manager = CLevel_Manager::Create();
    if (nullptr == m_pLevel_Manager)
        return E_FAIL;

    m_pObject_Manager = CObject_Manager::Create(_EngineDesc.iNumLevels);
    if (nullptr == m_pObject_Manager)
        return E_FAIL;

    m_pPrototype_Manager = CPrototype_Manager::Create(_EngineDesc.iNumLevels);
    if (nullptr == m_pPrototype_Manager)
        return E_FAIL;

    m_pFont_Manager = CFont_Manager::Create(*_ppDevice, *_ppContext);
    if (nullptr == m_pFont_Manager)
        return E_FAIL;

    m_pRenderTarget_Manager = CRenderTarget_Manager::Create(*_ppDevice, *_ppContext);
    if (nullptr == m_pRenderTarget_Manager)
        return E_FAIL;

    m_pRenderer = CRenderer::Create(*_ppDevice, *_ppContext);
    if (nullptr == m_pRenderer)
        return E_FAIL;

    m_pPicking = CPicking::Create(*_ppDevice, *_ppContext, _EngineDesc.hWnd);
    if (nullptr == m_pPicking)
        return E_FAIL;

    m_pShadow = CShadow::Create(*_ppDevice, *_ppContext);
    if (nullptr == m_pShadow)
        return E_FAIL;

    m_pFrustum = CFrustum::Create();
    if (nullptr == m_pFrustum)
        return E_FAIL;

    m_pSound_Manager = CSoundMgr::Create();
    if (nullptr == m_pSound_Manager)
        return E_FAIL;

    return S_OK;
}

void CGameInstance::Update_Engine(_float _fTimeDelta)
{
    // 내 게임 내에 필요한 다수의 객체들의 갱신 작업을 이곳에서 모두 모아, 수행할 예정

    if (nullptr == m_pLevel_Manager)
        return;

    m_pInput_Device->Update_InputDev();

    m_pPicking->Update();

    m_pLevel_Manager->Update(_fTimeDelta);
     
    m_pObject_Manager->Priority_Update(_fTimeDelta);

    m_pPipeLine->Update();

    m_pFrustum->Update();

    m_pObject_Manager->Update(_fTimeDelta);
    m_pObject_Manager->Late_Update(_fTimeDelta);
}

HRESULT CGameInstance::Render_Begin(const _float4& _vClearColor)
{
    m_pGraphic_Device->Clear_BackBuffer_View(_vClearColor);
    m_pGraphic_Device->Clear_DepthStencil_View();

    return S_OK;
}

HRESULT CGameInstance::Draw()
{
    m_pRenderer->Render();

    m_pLevel_Manager->Render();

    return S_OK;
}

HRESULT CGameInstance::Render_End()
{
    m_pGraphic_Device->Present();

    return E_NOTIMPL;
}

void CGameInstance::Clear(_uint _iLevelIndex)
{
    // 삭제된 레벨용 원형 객체를 클리어 한다.
    m_pPrototype_Manager->Clear(_iLevelIndex);

    // 삭제된 레벨용 사본객체를 들고있는 레이어를 클리어한다.
    m_pObject_Manager->Clear(_iLevelIndex);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
    return m_pGraphic_Device->Clear_DepthStencil_View();
}

#pragma region IMGUI
ID3D11Device* CGameInstance::Get_Device()
{
    return m_pGraphic_Device->Get_GraphicDevice();
}

ID3D11DeviceContext* CGameInstance::Get_Context()
{
    return m_pGraphic_Device->Get_DeviceContext();
}

#pragma endregion

#pragma region TIMER_MANAGER
_float CGameInstance::Get_TimeDelta(const _wstring& _strTimerTag)
{
    return m_pTimer_Manager->Get_TimeDelta(_strTimerTag);
}

void CGameInstance::Set_TimeDelta(const _wstring& _strTimerTag, _float _fTimeScale)
{
    return m_pTimer_Manager->Set_TimeDelta(_strTimerTag, _fTimeScale);
}

HRESULT CGameInstance::Add_Timer(const _wstring& _strTimerTag)
{
    return m_pTimer_Manager->Add_Timer(_strTimerTag);
}

void CGameInstance::Compute_TimeDelta(const _wstring& _strTimerTag)
{
    m_pTimer_Manager->Compute_TimeDelta(_strTimerTag);
}
#pragma endregion

#pragma region LIGHT_MANAGER
const LIGHT_DESC* CGameInstance::Get_LightDesc(_uint _iIndex) const
{
    return m_pLight_Manager->Get_LightDesc(_iIndex);
}
HRESULT CGameInstance::Add_Light(const LIGHT_DESC& _tagLightDesc)
{
    return m_pLight_Manager->Add_Light(_tagLightDesc);
}
void CGameInstance::Render_Lights(CShader* _pShader, CVIBuffer_Rect* _pVIBuffer)
{
    return m_pLight_Manager->Render_Lights(_pShader, _pVIBuffer);
}
#pragma endregion

#pragma region LEVEL_MANAGER
HRESULT CGameInstance::Open_Level(_uint _iNewLevelIndex, class CLevel* _pNewLevel)
{
    return m_pLevel_Manager->Open_Level(_iNewLevelIndex, _pNewLevel);
}
#pragma endregion

#pragma region INPUT_DEVICE
_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
    return m_pInput_Device->Get_DIKeyState(byKeyID);
}
_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE eMouse)
{
    return m_pInput_Device->Get_DIMouseState(eMouse);
}
_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMouseState)
{
    return m_pInput_Device->Get_DIMouseMove(eMouseState);
}
#pragma endregion

#pragma region PIPE_LINE
_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE _eState) const
{
    return m_pPipeLine->Get_Transform_Float4x4(_eState);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE _eState) const
{
    return m_pPipeLine->Get_Transform_Matrix(_eState);
}

_float4x4 CGameInstance::Get_Transform_Float4x4_Inverse(CPipeLine::D3DTRANSFORMSTATE _eState) const
{
    return m_pPipeLine->Get_Transform_Float4x4_Inverse(_eState);
}

_matrix CGameInstance::Get_Transform_Matrix_Inverse(CPipeLine::D3DTRANSFORMSTATE _eState) const
{
    return m_pPipeLine->Get_Transform_Matrix_Inverse(_eState);
}

_float4 CGameInstance::Get_CameraPos() const
{
    return m_pPipeLine->Get_CameraPos();
}

void CGameInstance::Set_Transform(CPipeLine::D3DTRANSFORMSTATE _eState, _fmatrix _TransformMatrix)
{
    return m_pPipeLine->Set_Transform(_eState, _TransformMatrix);
}

#pragma endregion

#pragma region PROTOTYPE_MANAGER
HRESULT CGameInstance::Add_Prototype(_uint _iLevelIndex, const _wstring& _strPrototypeTag, CBase* _pPrototype)
{
    return m_pPrototype_Manager->Add_Prototype(_iLevelIndex, _strPrototypeTag, _pPrototype);
}

CBase* CGameInstance::Clone_Prototype(PROTOTYPE _eType, _uint _iLevelIndex, const _wstring& _strPrototypeTag, void* _pArg)
{
    return m_pPrototype_Manager->Clone_Prototype(_eType, _iLevelIndex, _strPrototypeTag, _pArg);
}
#pragma endregion

#pragma region OBJECT_MANAGER
HRESULT CGameInstance::Add_GameObject_To_Layer(_uint _iProtoLevelIndex, const _wstring& _strPrototypeTag, _uint _iLevelIndex, const _wstring& _strLayerTag, void* _pArg)
{
    return m_pObject_Manager->Add_GameObject_To_Layer(_iProtoLevelIndex, _strPrototypeTag, _iLevelIndex, _strLayerTag, _pArg);
}
CGameObject* CGameInstance::Get_GameObject(const _wstring& _strPrototypeTag, _uint _iLevelIndex, const _wstring& _strLayerTag)
{
    return m_pObject_Manager->Get_GameObject(_strPrototypeTag, _iLevelIndex, _strLayerTag);
}
CComponent* CGameInstance::Get_Component(const _wstring& _strPrototypeTag, _uint _iLevelIndex, const _wstring& _strLayerTag)
{
    return m_pObject_Manager->Get_Component(_strPrototypeTag, _iLevelIndex, _strLayerTag);
}

list<class CGameObject*>* CGameInstance::Get_GameObjectList(_uint _iLevelIndex, const _wstring& _strLayerTag)
{
    return m_pObject_Manager->Get_GameObjectList(_iLevelIndex, _strLayerTag);
}

HRESULT CGameInstance::Delete_GameObject(_uint _iLevelIndex, const _wstring& _strLayerTag, CGameObject* _pGameObject)
{
    return m_pObject_Manager->Delete_GameObject(_iLevelIndex, _strLayerTag, _pGameObject);
}
CComponent* CGameInstance::Find_Component(_uint _iLevelIndex, const _wstring& _strLayerTag, const _wstring& _strComponetTag, _uint _iIndex)
{
    return m_pObject_Manager->Find_Component(_iLevelIndex, _strLayerTag, _strComponetTag, _iIndex);
}
CComponent* CGameInstance::Find_Component(_uint _iLevelIndex, const _wstring& _strLayerTag, const _wstring& _strPartTag, const _wstring& _strComponetTag, _uint _iIndex)
{
    return m_pObject_Manager->Find_Component(_iLevelIndex, _strLayerTag, _strPartTag, _strComponetTag, _iIndex);
}
#pragma endregion

#pragma region FONT_MANAGER
HRESULT CGameInstance::Add_Font(const _wstring& _strFontTag, const _tchar* _pFontFilePath)
{
    return m_pFont_Manager->Add_Font(_strFontTag, _pFontFilePath);
}
HRESULT CGameInstance::Render_Font(const _wstring& _strFontTag, const _tchar* _pText, const _float2& _vPosition, _fvector _vColor, _float _fRotation, const _float2& _vOrigin, const _float _fScale)
{
    return m_pFont_Manager->Render(_strFontTag, _pText, _vPosition, _vColor, _fRotation, _vOrigin, _fScale);
}

_float2 CGameInstance::Convert_WorldToScreen(const _float3& worldPosition)
{
    return m_pFont_Manager->Convert_WorldToScreen(worldPosition);
}
#pragma endregion


#pragma region RENDERTARGET_MANAGER
HRESULT CGameInstance::Add_RenderTarget(const _wstring& _strRenderTargetTag, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor)
{
    return m_pRenderTarget_Manager->Add_RenderTarget(_strRenderTargetTag, _iWidth, _iHeight, _ePixelFormat, _vClearColor);
}
HRESULT CGameInstance::Add_MRT(const _wstring& _strMRTTag, const _wstring& _strRenderTargetTag)
{
    return m_pRenderTarget_Manager->Add_MRT(_strMRTTag, _strRenderTargetTag);
}
HRESULT CGameInstance::Begin_MRT(const _wstring& _strMRTTag, _bool _bIsClear)
{
    return m_pRenderTarget_Manager->Begin_MRT(_strMRTTag, _bIsClear);
}
HRESULT CGameInstance::End_MRT()
{
    return m_pRenderTarget_Manager->End_MRT();
}

HRESULT CGameInstance::Bind_RenderTarget_ShaderResource(const _wstring& _strRenderTargetTag, CShader* _pShader, const _char* _pConstantName)
{
    return m_pRenderTarget_Manager->Bind_ShaderResource(_strRenderTargetTag, _pShader,_pConstantName);
}

HRESULT CGameInstance::Copy_RenderTarget_TextureResource(const _wstring& _strRenderTargetTag, ID3D11Texture2D* _pTexture2D)
{
    return m_pRenderTarget_Manager->Copy_TextureResource(_strRenderTargetTag, _pTexture2D);
}

#ifdef _DEBUG
HRESULT CGameInstance::Ready_RenderTarget_DebugMode(const _wstring& _strRenderTargetTag, _float _fX, _float _fY, _float _fSizeX, _float _fSizeY)
{
    return m_pRenderTarget_Manager->Ready_RenderTarget_DebugMode(_strRenderTargetTag, _fX, _fY, _fSizeX, _fSizeY);
}
HRESULT CGameInstance::Render_RenderTarget_DebugMode(const _wstring& _strMRTTag, CShader* _pShader, CVIBuffer_Rect* _pVIBuffer)
{
    return m_pRenderTarget_Manager->Render_RenderTarget_DebugMode(_strMRTTag, _pShader, _pVIBuffer);
}
#endif

#pragma endregion


#pragma region RENDERER
HRESULT CGameInstance::Add_RenderGroup(CRenderer::RENDERGROUP _eRenderGroupID, CGameObject* _pGameObject)
{
    return m_pRenderer->Add_RenderGroup(_eRenderGroupID, _pGameObject);
}
#pragma endregion

#pragma region SHADOW
HRESULT CGameInstance::SetUp_ShadowLight(_fvector _vEye, _fvector _vAt, _float _fLightAngle, _float _fAspect, _float _fNear, _float _fFar)
{
    return m_pShadow->SetUp_ShadowLight(_vEye, _vAt, _fLightAngle, _fAspect, _fNear, _fFar);
}

HRESULT CGameInstance::Bind_Shadow_Matrices(CShader* _pShader, const _char* _pConstViewMatrixName, const _char* _pConstProjMatrixName)
{
    return m_pShadow->Bind_Matrices(_pShader, _pConstViewMatrixName, _pConstProjMatrixName);
}
_bool CGameInstance::Compute_PickPos(_float3* _pOut)
{
    return m_pPicking->Compute_PickPos(_pOut);
}
#pragma endregion

#pragma region FRUSTUM
_bool CGameInstance::isIn_Frustum_WorldSpace(_fvector _vWorldPoint, _float _fRange)
{
    return m_pFrustum->isIn_WorldSpace(_vWorldPoint, _fRange);
}
#pragma endregion


#pragma region SOUND_MANAGER
void CGameInstance::Play(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
    return m_pSound_Manager->Play(pSoundKey, eID, fVolume);
}
void CGameInstance::LoopPlay(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
    return m_pSound_Manager->LoopPlay(pSoundKey, eID, fVolume);
}
void CGameInstance::PlayOnce(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
    return m_pSound_Manager->PlayOnce(pSoundKey, eID, fVolume);
}
void CGameInstance::PlayBGM(const TCHAR* pSoundKey, float fVolume)
{
    return m_pSound_Manager->PlayBGM(pSoundKey, fVolume);
}
void CGameInstance::PlaySelfish(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
    return m_pSound_Manager->PlaySelfish(pSoundKey, eID, fVolume);
}
void CGameInstance::PlayTimid(const TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
    return m_pSound_Manager->PlayTimid(pSoundKey, eID, fVolume);
}
void CGameInstance::StopSound(CHANNELID eID)
{
    return m_pSound_Manager->StopSound(eID);
}
void CGameInstance::StopAll()
{
    return m_pSound_Manager->StopAll();
}
void CGameInstance::SetChannelVolume(CHANNELID eID, float fVolume)
{
    return m_pSound_Manager->SetChannelVolume(eID, fVolume);
}
void CGameInstance::LoadSoundFile()
{
    //return m_pSound_Manager->loadsound
}
#pragma endregion
void CGameInstance::Release_Engine()
{
    Safe_Release(m_pGraphic_Device);
    Safe_Release(m_pInput_Device);
    Safe_Release(m_pTimer_Manager);
    Safe_Release(m_pLevel_Manager);
    Safe_Release(m_pPipeLine);
    Safe_Release(m_pRenderer);
    Safe_Release(m_pObject_Manager);
    Safe_Release(m_pPrototype_Manager);
    Safe_Release(m_pLight_Manager);
    Safe_Release(m_pFont_Manager);
    Safe_Release(m_pRenderTarget_Manager);

    Safe_Release(m_pPicking);
    Safe_Release(m_pShadow);
    Safe_Release(m_pFrustum);
    Safe_Release(m_pSound_Manager);

    CGameInstance::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
    __super::Free();

    //Safe_Release(m_pGraphic_Device);
    //Safe_Release(m_pTimer_Manager);
    //Safe_Release(m_pLevel_Manager);
}
