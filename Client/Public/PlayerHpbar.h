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
class CPlayerHpbar final : public CUIObject
{
public:
    struct HP_BAR : public CUIObject::UIOBJECT_DESC
    {

    };

private:
    CPlayerHpbar(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CPlayerHpbar(const CPlayerHpbar& _Prototype);
    virtual ~CPlayerHpbar() = default;

public:
    virtual HRESULT Initialize_Prototype() override;           
    virtual HRESULT Initialize(void* _pArg)override;           

    virtual void Priority_Update(_float _fTimeDelta)override;
    virtual void Update(_float _fTimeDelta)override;
    virtual void Late_Update(_float _fTimeDelta)override;

    virtual HRESULT Render()override;

    void        Decrease_Hp(_uint _iDamage, _uint _iMaxHp);
    void        Increase_Hp(_uint _iHeal, _uint _iMaxHp);
    void        Set_Visible(_bool _bIsVisible) { m_bIsVisible = _bIsVisible; }

private:
    CShader* m_pShaderCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CTexture* m_pTextureCom = {nullptr};

    _bool m_bIsVisible = true;
public:
    HRESULT     Ready_Component();

public:
    static CPlayerHpbar* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void Free() override;
};

END