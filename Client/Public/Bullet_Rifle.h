#pragma once
#include "Client_Defines.h"
#include "Bullet.h"
#include "Effect_Impact.h"

BEGIN(Engine)
class CShader;
class CModel;
class CTrailBuffer;
class CTexture;
END

BEGIN(Client)
class CPlayer;
class CBullet_Rifle final: public CBullet
{
public:
    struct BULLET_RIFLE_DESC : public CGameObject::GAMEOBJECT_DESC
    {

    };

private:
    CBullet_Rifle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CBullet_Rifle(const CBullet& _Prototype);
    virtual ~CBullet_Rifle() = default;

public:
    virtual HRESULT                     Initialize_Prototype() override;
    virtual HRESULT                     Initialize(void* _pArg) override;

    virtual void                        Priority_Update(_float _fTimeDelta)override;
    virtual void                        Update(_float _fTimeDelta)override;
    virtual void                        Late_Update(_float _fTimeDelta)override;

    virtual HRESULT                     Render()override;

public:
    HRESULT                             Ready_Component();
    HRESULT                             Bind_ShaderResource();

    void                                Activate_Bullet(const _vector& _vPosition, const _vector& _vGunMuzzleLook);
    void                                Set_Bullet_To_Muzzle(const _vector& _vGunMuzzlePos, const _vector& _vGunMuzzleLook);

public:
    void                                Bullet_Fly(_float _fTimeDelta);
    void                                DeActivate_Bullet();
    void                                Set_RayDistance(_float _fRayDistance);
    void                                Initialize_Ray();

    CEffect_Impact*                     Find_Inactive_ImpactEffect();


private:
    CShader*                            m_pShaderCom = { nullptr };
    CModel*                             m_pModelCom = { nullptr };
    CPlayer*                            m_pPlayer = { nullptr };

    list<CEffect_Impact*>               m_listImpact_Effect;


    deque<_float3>                      m_dequeBulletPosition;
    CShader*                            m_pTrailShaderCom = { nullptr };
    CTexture*                           m_pTextureCom = { nullptr };
    CTrailBuffer*                       m_pTrailBufferCom = { nullptr };

public:
    static CBullet_Rifle*               Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*                Clone(void* _pArg) override;
    virtual void                        Free() override;
};
END
