#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instancing;
END

BEGIN(Client)
class CParticle_MuzzleFire final : public CPartObject
{
public:
    struct PARTICLE_MUZZLE_FIRE_DESC : public CPartObject::PARTOBJECT_DESC
    {

    };

public:
    _bool                               IsActive() { return m_bActivate; }
private:
    CParticle_MuzzleFire(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CParticle_MuzzleFire(const CParticle_MuzzleFire& _Prototype);
    virtual ~CParticle_MuzzleFire() = default;

public:
    virtual HRESULT                     Initialize_Prototype() override;
    virtual HRESULT                     Initialize(void* _pArg) override;

    virtual void                        Priority_Update(_float _fTimeDelta) override;
    virtual void                        Update(_float _fTimeDelta) override;
    virtual void                        Late_Update(_float _fTimeDelta) override;

    virtual HRESULT                     Render() override;
    void                                Set_FireParticle_To_Muzzle(const _vector& _vGunMuzzlePos);

    void Set_ParentWorldMatrix(const _float4x4* _pParentWorldMatrix)
    {
        m_pParentWorldMatrix = _pParentWorldMatrix;
    }

private:
    CShader*                            m_pShaderCom = { nullptr };
    CTexture*                           m_pTextureCom = { nullptr };
    CVIBuffer_Point_Instancing*         m_pVIBufferCom = { nullptr };

    _bool                               m_bActivate = { false };
    _float                              m_fEffectDuration = {};
public:
    HRESULT                             Ready_Components();

    float                               m_fOffsetRight = 0.0f; 
    float                               m_fOffsetUp = 0.05f; 
    float                               m_fOffsetLook = 0.1f;  

    _vector                             vOffset = XMVectorSet(0.49f, 0.01f, 0.4f, 0.0f);

    _float                              m_fAnimationTime = {};


public:
    static CParticle_MuzzleFire*        Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*                Clone(void* _pArg) override;
    virtual void                        Free() override;
};

END