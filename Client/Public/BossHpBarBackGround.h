#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CBossHpBarBackGround final : public CUIObject
{
public:
    struct BOSS_HPBARCGROUND_DESC : public CUIObject::UIOBJECT_DESC
    {
    };

private:
    CBossHpBarBackGround(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CBossHpBarBackGround(const CBossHpBarBackGround& _Prototype);
    virtual ~CBossHpBarBackGround() = default;

public:
    virtual HRESULT Initialize_Prototype() override;           
    virtual HRESULT Initialize(void* _pArg)override;           

    virtual void Priority_Update(_float _fTimeDelta)override;
    virtual void Update(_float _fTimeDelta)override;
    virtual void Late_Update(_float _fTimeDelta)override;

    virtual HRESULT Render()override;

    void    Set_Visible(_bool _bIsVisible) { m_bIsVisible = _bIsVisible; }

private:
    CShader* m_pShaderCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    CTexture* m_pTextureCom = { nullptr };

private:
    _bool           m_bIsVisible= { false };

public:
    HRESULT     Ready_Component();
    void        Activate_HpBarBackGround();


public:
    static CBossHpBarBackGround* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void Free() override;
};
END
