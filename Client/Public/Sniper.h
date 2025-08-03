#pragma once
#include "Client_Defines.h"
#include "Gun.h"
#include "ScopeGauge.h"


BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CPlayer;
class CSniper final:public CGun
{
public:
    enum SNIPER_STATE
    {
        STATE_SNIPER_SHOT = 0x00000001
    };

public:
    struct SNIPER_DESC : public CPartObject::PARTOBJECT_DESC
    {
        _uint iPistolState = {};
        const _float4x4* pSocketMatrix = { nullptr };
        //cpar
    };

public:
    _vector CSniper::Get_MuzzlePosition() const {
        return m_vMuzzleParticlePos;
    }

    _uint Get_HandGunBulletCount()  const { return m_iHandGunTotalAmmoCount; }
    void Set_HandGunBulletCount(_uint _iBulletCount) { m_iHandGunTotalAmmoCount = _iBulletCount; }



    // ÅºÃ¢¿¡ ³²Àº ÃÑ¾Ë (Current Ammo in Magazine)
    _uint Get_CurrentAmmoInMagazine() const { return m_iCurrentAmmoInMagazine; }

    // ÀüÃ¼ ÃÑ¾Ë (Total Ammo)
    _uint Get_TotalAmmo() const { return m_iTotalAmmo; }


private:
    CSniper(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CSniper(const CGun& _Prototype);
    virtual ~CSniper() = default;

public:
    virtual HRESULT										Initialize_Prototype() override;
    virtual HRESULT										Initialize(void* _pArg) override;

    virtual void										Priority_Update(_float _fTimeDelta) override;
    virtual void										Update(_float _fTimeDelta) override;
    virtual void										Late_Update(_float _fTimeDelta) override;

    virtual HRESULT										Render() override;

public:
    HRESULT												Ready_Components();
    HRESULT												Bind_ShaderResources();

public:
    void												Shoot_Bullet(_float _fTimeDelta);

    virtual void										Reload();

private:
    CShader*                                            m_pShaderCom = { nullptr };
    CModel*                                             m_pModelCom = { nullptr };
    const _float4x4*                                    m_pSocketMatrix = { nullptr };

    _uint												m_iHandGunTotalAmmoCount = { 100 };

    CScopeGauge*                                        m_pGaugeBar = { nullptr };


public:
    static CGun*                                        Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*                                Clone(void* _pArg) override;
    virtual void										Free() override;


};
END