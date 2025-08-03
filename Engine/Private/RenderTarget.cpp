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

	/* ����? ����?  */
	TextureDesc.Usage = D3D11_USAGE_DEFAULT /* ���� */;
	/* ���Ŀ� � �뵵�� ���ε� �� �� �ִ� ViewŸ���� �ؽ��ĸ� ��������� Texture2D�Դϱ�? */
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
	// Context �� CopyResource ��� �ؽ��� ���� ����ϴ� �Լ��� �ִ���..
	// _pTexture2D ���ٰ�, ���� ��� �ִ� m_pTexture2D �� ����������. 
	m_pContext->CopyResource(_pTexture2D, m_pTexture2D);

	return S_OK;
}

#ifdef _DEBUG
HRESULT CRenderTarget::Ready_DebugMode(_float _fX, _float _fY, _float _fSizeX, _float _fSizeY)
{
	// if (FAILED(m_pGameInstance->Ready_RenderTarget_DebugMode(TEXT("Target_Diffuse"), 150.0f, 150.0f, 300.0f, 300.0f))) ��� Renderer ���� ȣ�����ָ�,
	// RenderTarget_Manager ���� ���� �� RenderTarget �� Ready_DebugMode ȣ���� �ϴµ�, 
	// �� TEXT("Target_Diffuse") �� RenderTarget_Manager ����, CRenderTarget* pRenderTarget = Find_RenderTarget(_strRenderTargetTag); ���� ã�Ƴ�
	// �� RenderTarget Ŭ������ ���� �� �Լ��� ȣ���ϰ� �Ǵ°��� ��
	// �׷��� ���� Renderer ���� TEXT("Target_Diffuse") ��� �س����� ���� �� �Լ��� ���� Diffuse �뵵�� RenderTarget ���� ó���Ǹ鼭 ���⼭ ������
	// ������ ũ���� RenderTarget �� ������ ����. 

	_uint				iNumViewPorts = { 1 };
	D3D11_VIEWPORT		ViewPortDesc{};

	// ��� ��������� context �� ���� �����ͼ� ���� ��׵��� ���� �� ����. 
	m_pContext->RSGetViewports(&iNumViewPorts, &ViewPortDesc);

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(_fSizeX, _fSizeY, 1.0f));

	XMStoreFloat4((_float4*)&m_WorldMatrix.m[3], XMVectorSet(_fX - (ViewPortDesc.Width * 0.5f), -_fY + (ViewPortDesc.Height * 0.5f), 0.0f, 1.0f));

	return S_OK;
}

HRESULT CRenderTarget::Render_DebugMode(CShader* _pShader, CVIBuffer_Rect* _pVIBuffer)
{
	// ������ �׸� ���̱� ������, ȭ�鿡 �簢������ ����� ���̶�, WorldMatrix�� �̷��� ���� �����ϰ�, ������,
	// View / Proj ��ĵ���, �ٸ� RenderTarget ��� ���� ���� ������ �� ���̾�. �����ϱ�, 0 - 0		Diffuse - Shade
	//																					  0			Normal

	// �׷��� View �� Proj �����, Ready_DebugMode �Լ����� �̸� �غ��� �� ���̴�. 
	// ���� ������ ������ĸ� �״�� ȣ������. 
	_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);

	_pShader->Bind_SRV("g_Texture", m_pSRV);

	// Deffered Shader �� 0 �� ° pass 
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
