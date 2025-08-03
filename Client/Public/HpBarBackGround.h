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
class CHpBarBackGround  final : public CUIObject
{
public:
    struct HPBACK_DESC : public CUIObject::UIOBJECT_DESC
    {

    };

private:
    CHpBarBackGround(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CHpBarBackGround(const CHpBarBackGround& _Prototype);
    virtual ~CHpBarBackGround() = default;

public:
    virtual HRESULT Initialize_Prototype() override;            // prototype 호출.
    virtual HRESULT Initialize(void* _pArg)override;            // 원형객체를 파라미터로 받아와서 호출한다...

    virtual void Priority_Update(_float _fTimeDelta)override;
    virtual void Update(_float _fTimeDelta)override;
    virtual void Late_Update(_float _fTimeDelta)override;

    virtual HRESULT Render()override;
    void        Set_Visible(_bool _bIsVisible) { m_bIsVisible = _bIsVisible; }
private:
    CShader*                    m_pShaderCom = { nullptr };
    CVIBuffer_Rect*             m_pVIBufferCom = { nullptr };
    CTexture*                   m_pTextureCom = { nullptr };

    _bool m_bIsVisible = true;

public:
    HRESULT     Ready_Component();

public:
    static CHpBarBackGround* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void Free() override;
};
END

