#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)
class CLevel_Logo final : public CLevel
{
private:
    CLevel_Logo(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CLevel_Logo() = default;

public:
    virtual HRESULT Initialize() override;
    virtual void    Update(_float _fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    HRESULT Ready_Layer_MouseCursor(const _tchar* _pLayerTags);
    HRESULT Ready_Layer_BackGround(const _tchar* _pLayerTags);

public:
    static CLevel_Logo* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual void Free() override;
};
END

