#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CBlendObject abstract : public CGameObject
{
protected:
    CBlendObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CBlendObject(const CBlendObject& _Prototype);
    virtual ~CBlendObject() = default;

public:
    _float Get_CamDistance() const {return m_fCamDistance;}
public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* _pArg)override;
    virtual void    Priority_Update(_float _fTimeDelta)override;
    virtual void    Update(_float _fTimeDelta)override;
    virtual void    Late_Update(_float _fTimeDelta)override;
    virtual HRESULT Render()override;
protected:
    _float              m_fCamDistance = {};

protected:
    void Compute_CamDistance();

public:
    virtual CGameObject* Clone(void* _pArg) = 0;
    virtual void Free() override;
};

END