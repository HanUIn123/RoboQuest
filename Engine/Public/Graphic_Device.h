#pragma once
#include "Base.h"

BEGIN(Engine)

class CGraphic_Device final : public CBase
{
private:
    CGraphic_Device();
    virtual ~CGraphic_Device() = default;

public:
    HRESULT Initialize(HWND _hWnd, _bool _isWindowed, _uint _iWinSizeX, _uint _iWinSizeY,
        _Inout_ ID3D11Device** _ppDevice, _Inout_ ID3D11DeviceContext** _ppContext);
    
    HRESULT Clear_BackBuffer_View(_float4 _vClearColor);

    HRESULT Clear_DepthStencil_View();

    HRESULT Present();

public:
    //ID3D11Device*               Get_GraphicDevice() { return m_pDevice; }
    //ID3D11DeviceContext*        Get_DeviceContext() {return m_pDeviceContext;)

    ID3D11Device* Get_GraphicDevice() { return m_pDevice; }
    ID3D11DeviceContext* Get_DeviceContext() { return m_pDeviceContext; }


private:
    ID3D11Device*               m_pDevice = { nullptr };
    ID3D11DeviceContext*        m_pDeviceContext = { nullptr };
    IDXGISwapChain*             m_pSwapChain = { nullptr };

    ID3D11RenderTargetView*     m_pBackBufferRTV = { nullptr };
    //ID3D11Texture2D*            m_pDepthTexture = { nullptr };
    ID3D11DepthStencilView*     m_pDepthStencilView = { nullptr };

private:
    HRESULT Ready_SwapChain(HWND _hWnd, _bool _isWindowed, _uint _iWinCX, _uint _iWinCY);
    HRESULT Ready_BackBufferRenderTargetView();
    HRESULT Ready_DepthStencilView(_uint _iWinCX, _uint _iWinCY);

public:
    static CGraphic_Device* Create(HWND _hWnd, _bool _isWindowed, _uint _iViewportWidth, _uint _iViewportHeight,
        _Inout_ ID3D11Device** _ppDevice,
        _Inout_ ID3D11DeviceContext** _ppDeviceContextOut);
    virtual void Free() override;
};

END
