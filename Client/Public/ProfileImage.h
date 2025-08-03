#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CProfileImage final : public CUIObject
{
    enum PROFILE_TYPE {PR_PLAYER, PR_BOOS, PR_END };

public:
    struct BOSS_HPBAR_DESC : public CUIObject::UIOBJECT_DESC
    {

    };

private:
    CProfileImage(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CProfileImage(const CProfileImage& _Prototype);
    virtual ~CProfileImage() = default;

public:
    virtual HRESULT                 Initialize_Prototype() override;
    virtual HRESULT                 Initialize(void* _pArg)override;

    virtual void                    Priority_Update(_float _fTimeDelta)override;
    virtual void                    Update(_float _fTimeDelta)override;
    virtual void                    Late_Update(_float _fTimeDelta)override;

    virtual HRESULT                 Render()override;

private:
    CShader*                        m_pShaderCom = { nullptr };
    CVIBuffer_Rect*                 m_pVIBufferCom = { nullptr };
    CTexture*                       m_pTextureCom = { nullptr };

    PROFILE_TYPE                    m_eProfileType = PR_END;

public:
    HRESULT                         Ready_Component();

public:
    static CProfileImage*           Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*            Clone(void* _pArg) override;
    virtual void                    Free() override;

};
END