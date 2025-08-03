#pragma once
#include "GameObject.h"

BEGIN(Engine)
class ENGINE_DLL CUIObject abstract : public CGameObject
{
public:
    struct UIOBJECT_DESC : public CGameObject::GAMEOBJECT_DESC
    {
        _float          fX, fY, fZ;
        _float          fSizeX, fSizeY;
        _float          fNear, fFar;
    };

protected:
    CUIObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CUIObject(const CUIObject& _Prototype);
    virtual ~CUIObject() = default;

public:
    virtual HRESULT  Initialize_Prototype() override;
    virtual HRESULT Initialize(void* _pArg) override;
    virtual void Priority_Update(_float _fTimeDelta) override;
    virtual void Update(_float _fTimeDelta) override;
    virtual void Late_Update(_float _fTimeDelta) override;
    virtual HRESULT Render() override;

protected:
    _float4x4           m_ViewMatrix{}, m_ProjMatrix{};

public:
    virtual CGameObject* Clone(void* _pArg) = 0;
    virtual void Free() override;
};

END

