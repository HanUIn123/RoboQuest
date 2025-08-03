#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CRectangle_Cube final : public CVIBuffer
{
private:
    CRectangle_Cube(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CRectangle_Cube(const CRectangle_Cube& _Prototype);
    virtual ~CRectangle_Cube() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* _pArg) override;

public:
    static CRectangle_Cube* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CComponent* Clone(void* _pArg) override;
    virtual void Free() override;
};
END