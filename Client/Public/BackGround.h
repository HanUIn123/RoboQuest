#pragma once

#include "Client_Defines.h"
//#include "GameObject.h"
#include "UIObject.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

//class CBackGround final : public CGameObject
class CBackGround final : public CUIObject
{
public:
    //struct BACKGROUND_DESC : public CGameObject::GAMEOBJECT_DESC
    struct BACKGROUND_DESC : public CUIObject::UIOBJECT_DESC
    {

    };

private:
    CBackGround(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CBackGround(const CBackGround& _Prototype);
    virtual ~CBackGround() = default;

public:
    virtual HRESULT Initialize_Prototype() override;            // prototype 호출.
    virtual HRESULT Initialize(void* _pArg)override;            // 원형객체를 파라미터로 받아와서 호출한다...

    virtual void Priority_Update(_float _fTimeDelta)override;
    virtual void Update(_float _fTimeDelta)override;
    virtual void Late_Update(_float _fTimeDelta)override;

    virtual HRESULT Render()override;

private:
    CShader*                        m_pShaderCom = { nullptr };
    CVIBuffer_Rect*                 m_pVIBufferCom = { nullptr };
    CTexture*                       m_pTextureCom = { nullptr };

public:
    HRESULT     Ready_Component();

public:
    static CBackGround* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void Free() override;

};

END