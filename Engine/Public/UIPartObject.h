#pragma once
//#include "GameObject.h"
#include "UIObject.h"
BEGIN(Engine)

class ENGINE_DLL CUIPartObject abstract :public CUIObject
{
public:
    struct UIPARTOBJECT_DESC : public CUIObject::UIOBJECT_DESC
    {
        const _float4x4* pParentWorldMatrix;
    };

protected:
    CUIPartObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CUIPartObject(const CUIPartObject& _Prototype);
    virtual ~CUIPartObject() = default;

public:
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* _pArg);
    virtual void	Priority_Update(_float _fTimeDelta);
    virtual void	Update(_float _fTimeDelta);
    virtual void	Late_Update(_float _fTimeDelta);
    virtual HRESULT Render();

protected:
    const _float4x4*        m_pParentWorldMatrix = { nullptr };
    _float4x4               m_CombinedWorldMatrix;

public:
    virtual CGameObject* Clone(void* _pArg) = 0;
    virtual void Free() override;
};

END