#pragma once
#include "VIBuffer.h"


BEGIN(Engine)
class ENGINE_DLL CVIBuffer_VerRect final: public CVIBuffer
{
private:
    CVIBuffer_VerRect(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CVIBuffer_VerRect(const CVIBuffer_VerRect& _Prototype);
    virtual ~CVIBuffer_VerRect() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;

    void                        Update_Vertex(_vector vTopLeft, _vector vTopRight, _vector vBottomRight, _vector vBottomLeft);
public:
    static CVIBuffer_VerRect*   Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CComponent*         Clone(void* _pArg) override;
    virtual void                Free() override;

};
END

