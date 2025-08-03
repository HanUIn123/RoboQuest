#pragma once
#include "Base.h"

BEGIN(Engine)
class CLight final : public CBase
{
private:
    CLight(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CLight() = default;

public:
    const LIGHT_DESC* Get_LightDesc() const 
    {
        return &m_tagLightDesc;
    }

public:
    HRESULT Initialize(const LIGHT_DESC& _tagLightDesc);
    void Render(class CShader* _pShader, class CVIBuffer_Rect* _pVIBuffer);

private:
    ID3D11Device*           m_pDevice = { nullptr };
    ID3D11DeviceContext*    m_pContext = { nullptr };
    LIGHT_DESC              m_tagLightDesc{};

public:
    static CLight* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LIGHT_DESC& _tagLightDesc);
    virtual void Free() override;
};

END