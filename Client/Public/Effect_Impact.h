#pragma once
#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CEffect_Impact final : public CBlendObject
{
private:
    CEffect_Impact(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CEffect_Impact(const CEffect_Impact& _Prototype);
    virtual ~CEffect_Impact() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* _pArg) override;
    virtual void Priority_Update(_float _fTimeDelta) override;
    virtual void Update(_float _fTimeDelta) override;
    virtual void Late_Update(_float _fTimeDelta) override;
    virtual HRESULT Render() override;

    void Setting_BillBoard();

    void Activate_ImpactEffect(const _float3& _vHitPos);

    //void DeActivate_ImpactEffect();
    _bool   IsActive() { return m_bActive; }

private:
    CShader* m_pShaderCom = { nullptr };
    CTexture* m_pTextureCom = { nullptr };
    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
    _float m_fFrame = {}; 

    _bool   m_bActive = { false };

private:
    _float m_fTimeElapsed = 0.f; 
    _float m_fEffectDuration = 1.0f;

public:
    HRESULT Ready_Components();
    

public:
    static CEffect_Impact* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void Free() override;
};
END
