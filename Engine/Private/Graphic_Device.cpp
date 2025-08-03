#include "../Public/Graphic_Device.h"

CGraphic_Device::CGraphic_Device()
    : m_pDevice{ nullptr }
    , m_pDeviceContext{ nullptr }
{
}

HRESULT CGraphic_Device::Initialize(HWND _hWnd, _bool _isWindowed, _uint _iWinSizeX, _uint _iWinSizeY, _Inout_ ID3D11Device** _ppDevice, _Inout_ ID3D11DeviceContext** _ppContext)
{
	_uint		iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif
	D3D_FEATURE_LEVEL			FeatureLV;

	/* dx9 : ��ġ�ʱ�ȭ�� �ϱ����� ������ �޿� �ϰ��� ���������� ��ġ��ü�� �����Ѵ�. */
	/* dx11 : �켱������ ��ġ ��ü�� �����ϰ� ��ġ��ü�� ���ؼ� ��Ÿ �ʱ�ȭ�۾� �� ������ �س�����. */

	/* �׷��� ��ġ�� �ʱ�ȭ�Ѵ�. */
	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0, D3D11_SDK_VERSION, &m_pDevice, &FeatureLV, &m_pDeviceContext)))
		return E_FAIL;

	/* SwapChain : ������۸�. ����� �ĸ���۸� �����ư��� ȭ�鿡 �����ش�.(Present) */


	/* ����ü�ΰ�ü�� �����Ͽ��� ������ ����ü�� ��ü�� ����۸� �����Ѵ�. ����۸� �����ϱ� ���� ID3D11Texture2D ����ž�. */
	/* ����ü�� ��ü�� ����鼭 ����ۿ� �ش��ϴ� ID3D11Texture2D��ü�� ����� ����ü�� ��ü�� �����Ѵ�. */
	if (FAILED(Ready_SwapChain(_hWnd, _isWindowed, _iWinSizeX, _iWinSizeY)))
		return E_FAIL;

	/* ����ü���� ��� �ִ� �ؽ��� 2D�� �����ͼ� �̸� �������� ����� ����Ÿ�� �並 �����.*/
	if (FAILED(Ready_BackBufferRenderTargetView()))
		return E_FAIL;

	if (FAILED(Ready_DepthStencilView(_iWinSizeX, _iWinSizeY)))
		return E_FAIL;

	/* ��ġ�� ���ε��س��� ����Ÿ�ٵ�� �������ٽǺ並 �����Ѵ�. */
	/* ��ġ�� ���ÿ� �ִ� 8���� ����Ÿ���� ��� ���� �� �ִ�. */
	// ������ 1�� �ƴ�, 8 ��. 
	ID3D11RenderTargetView* pRTVs[1] = {
		m_pBackBufferRTV,
	};

	/* ����Ÿ���� �ȼ� ���� ���̽��ٽǹ����� �ȼ����� ���� �ٸ��ٸ� ���� �������� �Ұ���������. */
	m_pDeviceContext->OMSetRenderTargets(1, pRTVs,
		m_pDepthStencilView);

	// �Ʒ��� ������ ����۸� �����ش�. 
	D3D11_VIEWPORT			ViewPortDesc;
	ZeroMemory(&ViewPortDesc, sizeof(D3D11_VIEWPORT));
	ViewPortDesc.TopLeftX = 0;
	ViewPortDesc.TopLeftY = 0;
	ViewPortDesc.Width = (_float)_iWinSizeX;
	ViewPortDesc.Height = (_float)_iWinSizeY;
	ViewPortDesc.MinDepth = 0.f;
	ViewPortDesc.MaxDepth = 1.f;

	m_pDeviceContext->RSSetViewports(1, &ViewPortDesc);



	*_ppDevice = m_pDevice;
	*_ppContext = m_pDeviceContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_BackBuffer_View(_float4 _vClearColor)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	/* DX9���� : Clear�Լ��� �����, ���̽��ٽǹ��۸� �Ѳ����� �����.  */
	// m_pGraphic_Device->Clear(� ������ŭ �����, � �͵��� �����? , ���� ���ﰡ. );	

	/* ����۸� �ʱ�ȭ�Ѵ�.  */
	m_pDeviceContext->ClearRenderTargetView(m_pBackBufferRTV, (_float*)&_vClearColor);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_DepthStencil_View()
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT CGraphic_Device::Present()
{
	if (nullptr == m_pSwapChain)
		return E_FAIL;

	/* ���� ���ۿ� �ĸ� ���۸� ��ü�Ͽ� �ĸ� ���۸� �������� �����ִ� ������ �Ѵ�. */
	/* �ĸ� ���۸� ���� ȭ�鿡 �����ٰ�. */
	return m_pSwapChain->Present(0, 0);
}

HRESULT CGraphic_Device::Ready_SwapChain(HWND _hWnd, _bool _isWindowed, _uint _iWinCX, _uint _iWinCY)
{
	IDXGIDevice* pDevice = nullptr;
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDevice);

	IDXGIAdapter* pAdapter = nullptr;
	pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);

	IDXGIFactory* pFactory = nullptr;
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	/* ����ü���� �����Ѵ�. = �ؽ��ĸ� �����ϴ� ���� + �����ϴ� ����  */
	DXGI_SWAP_CHAIN_DESC		SwapChain;
	ZeroMemory(&SwapChain, sizeof(DXGI_SWAP_CHAIN_DESC));

	/* ����� == �ؽ��� */
	/*�ؽ�ó(����� == ID3D11Texture2D)�� �����ϴ� ����*/
	SwapChain.BufferDesc.Width = _iWinCX;	/* ���� �ȼ� �� */
	SwapChain.BufferDesc.Height = _iWinCY;	/* ���� �ȼ� �� */

	/* float4(1.f, 1.f, 1.f, 1.f) */
	/* float4(1.f, 0.f, 0.f, 1.f) */
	/* ���� �ȼ��ϳ��� ������ ���� : 32BIT�ȼ������ϵ� ��ȣ�� ���� ����ȭ�� ���� �����Ҳ� */
	SwapChain.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; 	
	// ���� ����� ���� �ʰ�, scaling�۾��� �Բ� �ϰڴ�,(����ۿ� ���缭 �����Ѵ�) 
	SwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	/* ����ġ�Ͽ� ����� �׸�����. */
	/* RENDER_TARGET : �׸��� ���ϴ� ���. ����ġ�� */
	SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	// ���� �ȼ��� �ϳ��θ� �����Ѵ�. 
	SwapChain.BufferCount = 1;

	/*�����ϴ� ���� : ����� �ֻ����� ���� �����ص� ��. */
	// ���� ���� 144�ֻ����̸� 144�� �ص� �ȴܴ�.
	SwapChain.BufferDesc.RefreshRate.Numerator = 60;
	SwapChain.BufferDesc.RefreshRate.Denominator = 1;

	/* ��Ƽ���ø� : ��Ƽ�����¡ (����������) */ // �ȼ��� ���� ��â���� ��, ������ ���� ������ �Ѵٴ� �ǹ�.
	/* �����߿� ��ó�� ������ : ��Ƽ���ø� ����(x) */ 
	// ���߿� ������ ��ó�� ���̵��� �����߻��� ���ɼ��� �־, �� ��Ƽ�����¡ �۾��� �����ʴ´�.
	SwapChain.SampleDesc.Quality = 0;
	SwapChain.SampleDesc.Count = 1;

	SwapChain.OutputWindow = _hWnd;
	SwapChain.Windowed = _isWindowed;
	SwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	/* ����۶�� �ؽ�ó�� �����ߴ�. */
	if (FAILED(pFactory->CreateSwapChain(m_pDevice, &SwapChain, &m_pSwapChain)))
		return E_FAIL;



	Safe_Release(pFactory);
	Safe_Release(pAdapter);
	Safe_Release(pDevice);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_BackBufferRenderTargetView()
{
	if (nullptr == m_pDevice)
		return E_FAIL;



	/* ���� ������ ����ϱ����� �뵵�� �ؽ��ĸ� �����ϱ����� ���̽� �����͸� ������ �ִ� ��ü�̴�. */
	/* ���� ������ ����ϱ����� �뵵�� �ؽ��� : ID3D11RenderTargetView, ID3D11ShaderResoureView, ID3D11DepthStencilView */
	ID3D11Texture2D* pBackBufferTexture = nullptr;

	/* ����ü���� ����ִ� �ؽ�ó�� �����ͺ�. */
	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture)))
		return E_FAIL;

	/* ���� ����Ÿ�ٿ뵵�� ����� �� �ִ� �ؽ��� Ÿ��(ID3D11RenderTargetView)�� ��ü�� ����������. */
	if (FAILED(m_pDevice->CreateRenderTargetView(pBackBufferTexture, nullptr, &m_pBackBufferRTV)))
		return E_FAIL;

	Safe_Release(pBackBufferTexture);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_DepthStencilView(_uint _iWinCX, _uint _iWinCY)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	
	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	/* ���� ������ �ȼ��� ��������� �ȼ��� ������ �����ؾ߸� ���� �ؽ�Ʈ�� ����������. */
	/* �ȼ��� ���� �ٸ��� �ƿ� �������� ����. */
	TextureDesc.Width = _iWinCX;
	TextureDesc.Height = _iWinCY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	/* ����? ����?  */
	TextureDesc.Usage = D3D11_USAGE_DEFAULT /* ���� */;
	/* ���Ŀ� � �뵵�� ���ε� �� �� �ִ� ViewŸ���� �ؽ��ĸ� ��������� Texture2D�Դϱ�? */
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL
		/*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	// �ؽ��� �ȼ��� �ʱⰪ : nullptr ������ black�ε� clear�� ��� ȣ�� �ϱ⿡ blue�� ����.
	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	/* RenderTargetView */
	/* ShaderResourceView */
	/* DepthStencilView */

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDepthStencilView)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

CGraphic_Device* CGraphic_Device::Create(HWND _hWnd, _bool _isWindowed, _uint _iViewportWidth, _uint _iViewportHeight, _Inout_ ID3D11Device** _ppDevice, _Inout_ ID3D11DeviceContext** _ppDeviceContextOut)
{
	CGraphic_Device* pInstance = new CGraphic_Device();

	if (FAILED(pInstance->Initialize(_hWnd, _isWindowed, _iViewportWidth, _iViewportHeight, _ppDevice, _ppDeviceContextOut)))
	{
		MSG_BOX("Failed to Created : CGraphic_Device");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGraphic_Device::Free()
{
	Safe_Release(m_pSwapChain);
	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pBackBufferRTV);
	Safe_Release(m_pDeviceContext);
	//Safe_Release(m_pDepthTexture);

	

//#if defined(DEBUG) || defined(_DEBUG)
//	ID3D11Debug* d3dDebug;
//	HRESULT hr = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
//	if (SUCCEEDED(hr))
//	{
//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
//		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker \r ");
//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
//
//		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
//
//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
//		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker END \r ");
//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
//	}
//	if (d3dDebug != nullptr)            d3dDebug->Release();
//#endif


	Safe_Release(m_pDevice);
}
