#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CSkyBoxCube final : public CVIBuffer
{
private:
    CSkyBoxCube(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CSkyBoxCube(const CSkyBoxCube& _Prototype);
    virtual ~CSkyBoxCube() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* _pArg) override;

public:
    static CSkyBoxCube* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CComponent* Clone(void* _pArg) override;
    virtual void Free() override;
};
END
