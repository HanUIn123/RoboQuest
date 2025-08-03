#pragma once
#include "BlendObject.h"

BEGIN(Engine)
class ENGINE_DLL CEffectPartObject abstract : public CBlendObject
{
public:
    struct EFFECT_PARTOBJECT_DESC : public CBlendObject::GAMEOBJECT_DESC
    {
        const _float4x4* pParentWorldMatrix;
        const _float4x4* pSocketMatrix = { nullptr };
    };

protected:
    CEffectPartObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CEffectPartObject(const CEffectPartObject& _Prototype);
    virtual ~CEffectPartObject() = default;

public:
    virtual HRESULT     Initialize_Prototype();
    virtual HRESULT     Initialize(void* _pArg);
    virtual void        Priority_Update(_float _fTimeDelta);
    virtual void        Update(_float _fTImeDelta);
    virtual void        Late_Update(_float _fTimeDelta);
    virtual HRESULT     Render();

protected:
    const _float4x4* m_pParentWorldMatrix = { nullptr };
    _float4x4           m_CombinedWorldMatrix;
    const _float4x4* m_pSocketMatrix = { nullptr };

public:
    virtual CGameObject* Clone(void* _pArg) = 0;
    virtual void Free() override;
};


END