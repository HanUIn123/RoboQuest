#pragma once
#include "Base.h"

BEGIN(Engine)
class CRenderTarget final : public CBase
{
private:
    CRenderTarget(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CRenderTarget() = default;

public:
    ID3D11RenderTargetView* Get_RTV() const { return m_pRTV; }

public:
    HRESULT Initialize(_uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor);

    // 한 번 그리고 나서 그 그렷던거 지워주는 함수. 이거 없으면 한 번 그려졌던 것들이 안지워지고, 계속 남아서 마치 잔상처럼 되어버림;;
    void Clear();

    HRESULT Bind_ShaderResource(class CShader* _pShader, const _char* _pContantName);
    HRESULT Copy_TextureResource(ID3D11Texture2D* _pTexture2D);

#ifdef _DEBUG
    HRESULT Ready_DebugMode(_float _fX, _float _fY, _float _fSizeX, _float _fSizeY);
    HRESULT Render_DebugMode(class CShader* _pShader, class CVIBuffer_Rect* _pVIBuffer);
#endif 

private:
    ID3D11Device*                   m_pDevice = { nullptr };
    ID3D11DeviceContext*            m_pContext = { nullptr };

    ID3D11Texture2D*                m_pTexture2D = { nullptr };
    ID3D11RenderTargetView*         m_pRTV = { nullptr };
    ID3D11ShaderResourceView*       m_pSRV = { nullptr };

    _float4                         m_vClearColor = {};



#ifdef _DEBUG
private:
    _float4x4                       m_WorldMatrix = {};
#endif


public:
    static CRenderTarget* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor);
    virtual void Free() override;
};

END