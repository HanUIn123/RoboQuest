#pragma once
//#include "Base.h"
#include "Renderer.h"       // Renderer 안에 Base 있으니, ㄱㅊ
#include "Prototype_Manager.h"
#include "PipeLine.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
    DECLARE_SINGLETON(CGameInstance)

private:
    CGameInstance();
    virtual ~CGameInstance() = default;


public:
    void Hide_MouseCursor(_bool _bHideOn);
public:
    POINT       Get_MousePosition(HWND _hWnd)
    {
        POINT       ptMouseCursor;
        GetCursorPos(&ptMouseCursor);
        ScreenToClient(_hWnd, &ptMouseCursor);
        return ptMouseCursor;
    }

public:
    _uint2 Get_ViewportSize() const {
        return _uint2(m_iViewportWidth, m_iViewportHeight);
    }

public:
    void Click_Once(function<void()> _function);
    _bool Press_Once(_ubyte _byteKeyID, function<void()> _function = 0);


public:
    // Client에서 호출할 만한 Engine의 기능을 위해 필요한 초기화를 수행한다.
    HRESULT Initialize_Engine(const ENGINE_DESC& _EngineDesc, _Inout_ ID3D11Device** _ppDevice, _Inout_ ID3D11DeviceContext** _ppContext);
    void    Update_Engine(_float _fTimeDelta);
    HRESULT Render_Begin(const _float4& _vClearColor);
    HRESULT Draw();
    HRESULT Render_End();

    // 특정 레벨의 자원들만 지우는 함수이다.
    void Clear(_uint _iLevelIndex);

    _float Compute_Random_Normal() { return static_cast<_float>(rand()) / RAND_MAX; }

    _float Compute_Random(_float _fMin, _float _fMax) {
        return _fMin + Compute_Random_Normal() * (_fMax - _fMin);
    }

    //#pragma region IMGUI
public:
    ID3D11Device* Get_Device();
    ID3D11DeviceContext* Get_Context();
    //#pragma endregion



    HRESULT Clear_DepthStencil_View();



#pragma region TIMER_MANAGER
public:
    _float  Get_TimeDelta(const _wstring& _strTimerTag);
    void Set_TimeDelta(const _wstring& _strTimerTag, _float _fTimeScale);
    HRESULT Add_Timer(const _wstring& _strTimerTag);
    void    Compute_TimeDelta(const _wstring& _strTimerTag);
#pragma endregion

#pragma region LIGHT_MANAGER
public:
    const LIGHT_DESC* Get_LightDesc(_uint _iIndex) const;
    HRESULT             Add_Light(const LIGHT_DESC& _tagLightDesc);
    void                Render_Lights(class CShader* _pShader, class CVIBuffer_Rect* _pVIBuffer);
#pragma endregion

#pragma region LEVEL_MANAGER
public:
    HRESULT Open_Level(_uint _iNewLevelIndex, class CLevel* _pNewLevel);
#pragma endregion 

#pragma region INPUT_DEVICE
public:
    _byte	Get_DIKeyState(_ubyte byKeyID);
    _byte	Get_DIMouseState(MOUSEKEYSTATE eMouse);
    _long	Get_DIMouseMove(MOUSEMOVESTATE eMouseState);
#pragma endregion 

#pragma region PIPE_LINE
public:
    _float4x4       Get_Transform_Float4x4(CPipeLine::D3DTRANSFORMSTATE _eState) const;
    _matrix         Get_Transform_Matrix(CPipeLine::D3DTRANSFORMSTATE _eState) const;
    _float4x4       Get_Transform_Float4x4_Inverse(CPipeLine::D3DTRANSFORMSTATE _eState) const;
    _matrix         Get_Transform_Matrix_Inverse(CPipeLine::D3DTRANSFORMSTATE _eState) const;
    _float4         Get_CameraPos() const;
    void            Set_Transform(CPipeLine::D3DTRANSFORMSTATE _eState, _fmatrix _TransformMatrix);
#pragma endregion 

#pragma region PROTOTYPE_MANAGER
public:
    HRESULT Add_Prototype(_uint _iLevelIndex, const _wstring& _strPrototypeTag, CBase* _pPrototype);
    CBase* Clone_Prototype(PROTOTYPE _eType, _uint _iLevelIndex, const _wstring& _strPrototypeTag, void* _pArg = nullptr);



#pragma endregion 

#pragma region OBJECT_MANAGER
public:
    HRESULT Add_GameObject_To_Layer(_uint _iProtoLevelIndex, const _wstring& _strPrototypeTag, _uint _iLevelIndex, const _wstring& _strLayerTag, void* _pArg = nullptr);
    //HRESULT Get_GameObject(const _tchar* _pObjectTag, _uint _iLevelIndex, const _wstring& _strLayerTag);
    class CGameObject* Get_GameObject(const _wstring& _strPrototypeTag, _uint _iLevelIndex, const _wstring& _strLayerTag);
    class CComponent* Get_Component(const _wstring& _strPrototypeTag, _uint _iLevelIndex, const _wstring& _strLayerTag);
    list<class CGameObject*>* Get_GameObjectList(_uint _iLevelIndex, const _wstring& _strLayerTag);

    //HRESULT Delete_GameObject(_uint _iLevelIndex, const _wstring& _strLayerTag, const _wstring& _strProtoTypeTag);
    HRESULT Delete_GameObject(_uint _iLevelIndex, const _wstring& _strLayerTag, CGameObject* _pGameObject);

    class CComponent* Find_Component(_uint _iLevelIndex, const _wstring& _strLayerTag, const _wstring& _strComponetTag, _uint _iIndex = 0);
    class CComponent* Find_Component(_uint _iLevelIndex, const _wstring& _strLayerTag, const _wstring& _strPartTag, const _wstring& _strComponetTag, _uint _iIndex = 0);

#pragma endregion

#pragma region FONT_MANAGER
    HRESULT         Add_Font(const _wstring& _strFontTag, const _tchar* _pFontFilePath);
    HRESULT         Render_Font(const _wstring& _strFontTag, const _tchar* _pText, const _float2& _vPosition, _fvector _vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f), _float _fRotation = 0.f, const _float2& _vOrigin = _float2(0.f, 0.f), const _float _fScale = 1.f);
    _float2         Convert_WorldToScreen(const _float3& worldPosition);


#pragma endregion


#pragma region RENDERTARGET_MANAGER
    HRESULT         Add_RenderTarget(const _wstring& _strRenderTargetTag, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor);
    HRESULT         Add_MRT(const _wstring& _strMRTTag, const _wstring& _strRenderTargetTag);
    HRESULT         Begin_MRT(const _wstring& _strMRTTag, _bool _bIsClear = true);
    HRESULT         End_MRT();
    HRESULT         Bind_RenderTarget_ShaderResource(const _wstring& _strRenderTargetTag, class CShader* _pShader, const _char* _pConstantName);
    HRESULT         Copy_RenderTarget_TextureResource(const _wstring& _strRenderTargetTag, ID3D11Texture2D* _pTexture2D);

#ifdef _DEBUG
public:
    HRESULT	Ready_RenderTarget_DebugMode(const _wstring& _strRenderTargetTag, _float _fX, _float _fY, _float _fSizeX, _float _fSizeY);
    HRESULT	Render_RenderTarget_DebugMode(const _wstring& _strMRTTag, class CShader* _pShader, class CVIBuffer_Rect* _pVIBuffer);
#endif 

#pragma endregion


#pragma region RENDERER
public:
    HRESULT Add_RenderGroup(CRenderer::RENDERGROUP _eRenderGroupID, class CGameObject* _pGameObject);
#pragma endregion

#pragma region SHADOW
    HRESULT SetUp_ShadowLight(_fvector _vEye, _fvector _vAt, _float _fLightAngle, _float _fAspect, _float _fNear, _float _fFar);
    HRESULT Bind_Shadow_Matrices(class CShader* _pShader, const _char* _pConstViewMatrixName, const _char* _pConstProjMatrixName);
#pragma endregion

#pragma region PICKING
    _bool							Compute_PickPos(_float3* _pOut);
#pragma endregion

#pragma region FRUSTUM
    _bool isIn_Frustum_WorldSpace(_fvector vWorldPoint, _float fRange = 0.f);
#pragma endregion

#pragma region SOUND_MANAGER
    void Play(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);
    void LoopPlay(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);
    void PlayOnce(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);
    void PlayBGM(const TCHAR* pSoundKey, float fVolume);
    void PlaySelfish(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);
    void PlayTimid(const TCHAR* pSoundKey, CHANNELID eID, float fVolume);

    void StopSound(CHANNELID eID);
    void StopAll();
    void SetChannelVolume(CHANNELID eID, float fVolume);
    void LoadSoundFile();
#pragma endregion

private:
    _uint                           m_iViewportWidth{}, m_iViewportHeight{};

private:
    class CTimer_Manager* m_pTimer_Manager = { nullptr };
    class CGraphic_Device* m_pGraphic_Device = { nullptr };
    class CLevel_Manager* m_pLevel_Manager = { nullptr };
    class CInput_Device* m_pInput_Device = { nullptr };
    class CObject_Manager* m_pObject_Manager = { nullptr };
    class CPrototype_Manager* m_pPrototype_Manager = { nullptr };
    class CRenderer* m_pRenderer = { nullptr };
    class CPipeLine* m_pPipeLine = { nullptr };
    class CLight_Manager* m_pLight_Manager = { nullptr };
    class CFont_Manager* m_pFont_Manager = { nullptr };
    class CRenderTarget_Manager* m_pRenderTarget_Manager = { nullptr };
    class CPicking* m_pPicking = { nullptr };
    class CShadow* m_pShadow = { nullptr };
    class CFrustum* m_pFrustum = { nullptr };
    class CSoundMgr* m_pSound_Manager = { nullptr };

private:
    // Click Mouse Once
    _bool                           m_bClicked = { false };
    // Press Key Once
    _bool                           m_bPressed = { false };

public:
    void Release_Engine();
    virtual void Free() override;
};

END

