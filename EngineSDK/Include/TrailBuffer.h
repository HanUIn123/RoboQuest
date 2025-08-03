#pragma once
#include "VIBuffer.h"

BEGIN(Engine)
class ENGINE_DLL CTrailBuffer final : public CVIBuffer
{
private:
    CTrailBuffer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CTrailBuffer(const CTrailBuffer& _Prototype);
    virtual ~CTrailBuffer() = default;

public:
    virtual HRESULT         Initialize_Prototype() override;
    virtual HRESULT         Initialize(void* _pArg) override;

    //void                    Update_Trail_Vertex(_vector _vFirstWorldPos, _vector _vSecondWorldPos);
    //void                    Update_Trail_Vertex(const vector<_float3>& _vBulletPos, const _float3& _vUp, _matrix _mBulletMatrix);
    void                    Update_Trail_Vertex(const deque<_float3>& _vBulletPos, const _float3& _vUp, _matrix _mBulletMatrix);


private:
    ID3D11Buffer*           m_pVBInstance = { nullptr };


public:
    static CTrailBuffer* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CComponent* Clone(void* _pArg) override;
    virtual void Free() override;
};

END