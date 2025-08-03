#pragma once
#include "Base.h"

BEGIN(Engine)
class CLight_Manager final : public CBase
{
private:
    CLight_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CLight_Manager() = default;
    
public:
    const LIGHT_DESC* Get_LightDesc(_uint _iIndex) const;

public:
    HRESULT     Initialize();
    HRESULT     Add_Light(const LIGHT_DESC& _tagLightDesc);
    void        Render_Lights(class CShader* _pShader, class CVIBuffer_Rect* _pVIBuffer);

private:
    ID3D11Device*               m_pDevice;
    ID3D11DeviceContext*        m_pContext;
    list<class CLight*>         m_listLights;

public:
    static CLight_Manager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual void Free() override;
};

END