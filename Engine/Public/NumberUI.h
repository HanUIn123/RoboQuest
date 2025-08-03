#pragma once
#include "UIObject.h"

BEGIN(Engine)
class ENGINE_DLL CNumberUI :public CBase
{
private:
    CNumberUI(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CNumberUI() = default;

public:
    HRESULT     Initialize();
private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };

public:
    static CNumberUI* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor);
    virtual void Free() override;
};

END