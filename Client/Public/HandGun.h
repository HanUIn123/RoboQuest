#pragma once
#include "Client_Defines.h"
#include "Gun.h"

BEGIN(Engine)
class CShader;
class CModel;
END


BEGIN(Client)
class CPlayer;
class CHandGun final: public CGun
{
public:
	enum PISTOL_STATE {
		STATE_IDLE = 0x00000001,
		STATE_RELOAD = 0x00000002,
		STATE_FIRE = 0x00000003
	};

public:
	struct HANDGUN_DESC : public CPartObject::PARTOBJECT_DESC
	{
		_uint				iPistolState = {};
		const _float4x4* pSocketMatrix = { nullptr };
		CParticle_MuzzleFire* pParticleMuzzleFire = { nullptr };
	};

public:
	//void												Ready_Shooting(_bool _bShoot) { m_bShooting = _bShoot; }
	//void												Ready_Reload(_bool _bReload) { m_bReloading = _bReload; }
private:
	//_bool												m_bShooting = { true };
	//_bool												m_bReloading = { false };

public:
	_vector CHandGun::Get_MuzzlePosition() const {
		return m_vMuzzleParticlePos;
	}

	_uint												Get_HandGunBulletCount()  const { return m_iHandGunTotalAmmoCount; }
	void Set_HandGunBulletCount(_uint _iBulletCount) { m_iHandGunTotalAmmoCount = _iBulletCount; }



	//// ÅºÃ¢¿¡ ³²Àº ÃÑ¾Ë (Current Ammo in Magazine)
	//_uint Get_CurrentAmmoInMagazine() const { return m_iCurrentAmmoInMagazine; }

	//// ÀüÃ¼ ÃÑ¾Ë (Total Ammo)
	//_uint Get_TotalAmmo() const { return m_iTotalAmmo; }


private:
	CHandGun(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CHandGun(const CGun& _Prototype);
	virtual ~CHandGun() = default;

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
	void												Shoot_MuzzleFireEffect(_vector _vMuzzlePos);

	virtual void										Reload() ;

private:
	CShader*											m_pShaderCom = { nullptr };
	CModel*												m_pModelCom = { nullptr };
	const _float4x4*									m_pSocketMatrix = { nullptr };

private:
	CParticle_MuzzleFire*								m_pMuzzleFireParticle = { nullptr };
	_vector												vOffset = XMVectorSet(0.49f, 0.01f, 0.4f, 0.0f);
	_uint												m_iHandGunTotalAmmoCount = { 100 };

public:
	static CGun*										Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*								Clone(void* _pArg) override;
	virtual void										Free() override;
};

END