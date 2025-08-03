#pragma once
#include "Client_Defines.h"
//#include "UIPartObject.h"

#include "Mon_HpBar.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CHpBar_Robo_Boom final : public CMon_HpBar
{
public:
    struct BOOM_HPBAR_DESC : public CMon_HpBar::MONSTER_HPBAR_DESC
    {
    };

public:
    void Set_WorldMatrix(_float4x4* _pWorldMatrix) { m_CombinedWorldMatrix = *_pWorldMatrix; }

private:
    CHpBar_Robo_Boom(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CHpBar_Robo_Boom(const CHpBar_Robo_Boom& _Prototype);
    virtual ~CHpBar_Robo_Boom() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;
    virtual void                Priority_Update(_float _fTimeDelta) override;
    //virtual void                Update(_float _fTimeDelta) override;
    virtual void                Late_Update(_float _fTimeDelta) override;
    virtual HRESULT             Render() override;


private:
    CShader*                    m_pShaderCom = { nullptr };
    CTexture*                   m_pTextureCom = { nullptr };
    CTexture*                   m_pBackgroundTextureCom = { nullptr };
    CVIBuffer_Rect*             m_pVIBufferCom = { nullptr };

public:
    HRESULT             Ready_Components();

public:
    static CHpBar_Robo_Boom* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void Free() override;
};

END