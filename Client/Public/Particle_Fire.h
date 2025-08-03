#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instancing;
END

BEGIN(Client)
class CParticle_Fire final: public CPartObject
{
public:
    struct PARTICLE_FIRE_DESC : public CPartObject::PARTOBJECT_DESC
    {
        const _float4x4* pSocketMatrix = { nullptr };
    };

private:
    CParticle_Fire(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CParticle_Fire(const CParticle_Fire& _Prototype);
    virtual ~CParticle_Fire() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;

    virtual void                Priority_Update(_float _fTimeDelta) override;
    virtual void                Update(_float _fTimeDelta) override;
    virtual void                Late_Update(_float _fTimeDelta) override;
    virtual HRESULT             Render() override;
    virtual HRESULT             Render_Glow()override;

    void                        Activate(const _vector& _vPosition, const _vector& _vDirection);  
    void                        Deactivate();

private:
    HRESULT                     Ready_Components();

    CShader*                    m_pShaderCom = nullptr;
    CTexture*                   m_pTextureCom = nullptr;
    CVIBuffer_Point_Instancing* m_pVIBufferCom = nullptr;

    _bool                       m_bActive = false;
    _float                      m_fDuration = 3.0f; // 불꽃 지속 시간
    _vector                     m_vFirePosition;   // 불꽃 위치
    _vector                     m_vFireDirection;  // 불꽃 방향

    _float                      m_fAnimationTime = 0.0f;  // 애니메이션 시간 (초 단위)
    _float                      m_fAnimationSpeed = 8.0f; // 초당 프레임 변경 속도


public:
    static CParticle_Fire*      Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*        Clone(void* _pArg) override;
    virtual void                Free() override;
};
END
