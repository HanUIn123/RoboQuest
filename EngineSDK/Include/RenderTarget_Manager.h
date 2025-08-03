#pragma once
#include "Base.h"

BEGIN(Engine)
class CRenderTarget_Manager final : public CBase
{
private:
    CRenderTarget_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CRenderTarget_Manager() = default;

public:
	HRESULT Initialize();

	HRESULT Add_RenderTarget(const _wstring& _strRenderTargetTag, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor);
	HRESULT Add_MRT(const _wstring& _strMRTTag, const _wstring& _strRenderTargetTag);

	HRESULT Begin_MRT(const _wstring& _strMRTTag, _bool isClear);
	HRESULT End_MRT();

	HRESULT	Bind_ShaderResource(const _wstring& _strRenderTargetTag, class CShader* _pShader, const _char* _pConstantName);
	HRESULT	Copy_TextureResource(const _wstring& _strRenderTargetTag, ID3D11Texture2D* _pTexture2D);


#ifdef _DEBUG
public:
	HRESULT	Ready_RenderTarget_DebugMode(const _wstring& _strRenderTargetTag, _float _fX, _float _fY, _float _fSizeX, _float _fSizeY);
	HRESULT	Render_RenderTarget_DebugMode(const _wstring& _strMRTTag, class CShader* _pShader, class CVIBuffer_Rect* _pVIBuffer);
#endif



private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };

	ID3D11RenderTargetView* m_pBackRTV = { nullptr };
	ID3D11DepthStencilView* m_pOriginalDSV = { nullptr };

private:
	map<const _wstring, class CRenderTarget*>			m_mapRenderTargets;
	class CRenderTarget* Find_RenderTarget(const _wstring& _strRenderTargetTag);

	map<const _wstring, vector<class CRenderTarget*>>	m_mapMRTs;
	vector<class CRenderTarget*>* Find_MRT(const _wstring& _strMRTTag);


	
public:
	static CRenderTarget_Manager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;

};

END