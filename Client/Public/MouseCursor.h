#pragma once
#include "Client_Defines.h"
#include "UIObject.h"
BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
class CTransform;
END

BEGIN(Client)
class CMouseCursor final : public CUIObject
{
public:
    struct CURSOR_DESC : public CUIObject::UIOBJECT_DESC
    {

    };
private:
    CMouseCursor(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CMouseCursor(const CMouseCursor& _Prototype);
    virtual ~CMouseCursor() = default;

public:
    virtual HRESULT Initialize_Prototype() override;            // prototype 호출.
    virtual HRESULT Initialize(void* _pArg)override;            // 원형객체를 파라미터로 받아와서 호출한다...

    virtual void Priority_Update(_float _fTimeDelta)override;
    virtual void Update(_float _fTimeDelta)override;
    virtual void Late_Update(_float _fTimeDelta)override;

    virtual HRESULT Render()override;

private:
    CShader* m_pShaderCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CTexture* m_pTextureCom = { nullptr };

public:
    HRESULT     Ready_Component();

public:
    static CMouseCursor* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void Free() override;

};

END