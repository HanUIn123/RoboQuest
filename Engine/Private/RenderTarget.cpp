#include "RenderTarget.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CRenderTarget::CRenderTarget(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :m_pDevice{_pDevice}
    ,m_pContext{_pContext}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CRenderTarget::Initialize(_uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor)
{
	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = _iWidth;
	TextureDesc.Height = _iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = _ePixelFormat;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	/* 동적? 정적?  */
	TextureDesc.Usage = D3D11_USAGE_DEFAULT /* 정적 */;
	/* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
	TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV)))
		return E_FAIL;

	m_vClearColor = _vClearColor;

    return S_OK;
}

void CRenderTarget::Clear()
{
	m_pContext->ClearRenderTargetView(m_pRTV, reinterpret_cast<_float*>(&m_vClearColor));
}

HRESULT CRenderTarget::Bind_ShaderResource(CShader* _pShader, const _char* _pContantName)
{
	return _pShader->Bind_SRV(_pContantName, m_pSRV);
}

HRESULT CRenderTarget::Copy_TextureResource(ID3D11Texture2D* _pTexture2D)
{
	// Context 에 CopyResource 라는 텍스쳐 복사 담당하는 함수가 있더라..
	// _pTexture2D 에다가, 내가 들고 있는 m_pTexture2D 를 저장해주자. 
	m_pContext->CopyResource(_pTexture2D, m_pTexture2D);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderTarget::Ready_DebugMode(_float _fX, _float _fY, _float _fSizeX, _float _fSizeY)
{
	// if (FAILED(m_pGameInstance->Ready_RenderTarget_DebugMode(TEXT("Target_Diffuse"), 150.0f, 150.0f, 300.0f, 300.0f))) 라고 Renderer 에서 호출해주면,
	// RenderTarget_Manager 에서 현재 이 RenderTarget 의 Ready_DebugMode 호출을 하는데, 
	// 저 TEXT("Target_Diffuse") 는 RenderTarget_Manager 에서, CRenderTarget* pRenderTarget = Find_RenderTarget(_strRenderTargetTag); 에서 찾아낸
	// 이 RenderTarget 클래스의 현재 이 함수를 호출하게 되는거임 ㅋ
	// 그래서 만약 Renderer 에서 TEXT("Target_Diffuse") 라고 해놓으면 지금 이 함수는 이제 Diffuse 용도의 RenderTarget 꺼로 처리되면서 저기서 정해준
	// 사이즈 크기대로 RenderTarget 이 나오는 거임. 

	_uint				iNumViewPorts = { 1 };
	D3D11_VIEWPORT		ViewPortDesc{};

	// 뷰와 투영행렬은 context 로 부터 가져와서 이제 얘네들은 같이 쓸 꺼임. 
	m_pContext->RSGetViewports(&iNumViewPorts, &ViewPortDesc);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(_fSizeX, _fSizeY, 1.0f));

	XMStoreFloat4((_float4*)&m_WorldMatrix.m[3], XMVectorSet(_fX - (ViewPortDesc.Width * 0.5f), -_fY + (ViewPortDesc.Height * 0.5f), 0.0f, 1.0f));

	return S_OK;
}

HRESULT CRenderTarget::Render_DebugMode(CShader* _pShader, CVIBuffer_Rect* _pVIBuffer)
{
	// 직교로 그릴 것이기 때문에, 화면에 사각형으로 띄워놀 것이라서, WorldMatrix만 이렇게 따로 설정하고, 나머지,
	// View / Proj 행렬들은, 다른 RenderTarget 들과 같이 쓰는 식으로 할 것이야. 직교니까, 0 - 0		Diffuse - Shade
	//																					  0			Normal

	// 그래서 View 랑 Proj 행렬은, Ready_DebugMode 함수에서 미리 준비해 둘 것이다. 
	// 내가 세팅한 월드행렬만 그대로 호출하자. 
	_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);

	_pShader->Bind_SRV("g_Texture", m_pSRV);

	// Deffered Shader 의 0 번 째 pass 
	_pShader->Begin(0);

	_pVIBuffer->Bind_InputAssembler();

	_pVIBuffer->Render();

	return S_OK;
}

#endif
CRenderTarget* CRenderTarget::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor)
{
	CRenderTarget* pInstance = new CRenderTarget(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_iWidth, _iHeight, _ePixelFormat, _vClearColor)))
	{
		MSG_BOX("Failed to Created : CRenderTarget");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRenderTarget::Free()
{
	__super::Free();

	Safe_Release(m_pSRV);
	Safe_Release(m_pRTV);
	Safe_Release(m_pTexture2D);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
