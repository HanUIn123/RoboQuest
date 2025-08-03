#pragma once
#include "Client_Defines.h"
#include "PartObject.h"
#include "Particle_MuzzleFire.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)

class CPlayer;
class CGun : public CPartObject
{
public:
	enum PISTOL_STATE {
		STATE_IDLE = 0x00000001,
		STATE_RELOAD = 0x00000002,
		STATE_FIRE = 0x00000003
	};

public:
	struct GUN_DESC : public CPartObject::PARTOBJECT_DESC
	{
		_uint				iPistolState = {};
		const _float4x4*	pSocketMatrix = { nullptr };
		CParticle_MuzzleFire* pParticleMuzzleFire = { nullptr };
	};

	void												Set_Visible(_bool bVisible) { m_bVisible = bVisible; }
	_bool												Is_Visible() const { return m_bVisible; }

public:
	void												Ready_Shooting(_bool _bShoot) { m_bShooting = _bShoot; }
	void												Ready_Reload(_bool _bReload) { m_bReloading = _bReload; }
protected:
	_bool												m_bShooting = { true };
protected:
	_bool												m_bReloading = { false };

public:
	_vector CGun::Get_MuzzlePosition() const {
		return m_vMuzzleParticlePos;
	}

protected:
	CGun(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CGun(const CGun& _Prototype);
	virtual ~CGun() = default;

public:
	virtual HRESULT										Initialize_Prototype() override;
	virtual HRESULT										Initialize(void* _pArg) override;

	virtual void										Priority_Update(_float _fTimeDelta) override;
	virtual void										Update(_float _fTimeDelta) override;
	virtual void										Late_Update(_float _fTimeDelta) override;

	virtual HRESULT										Render() override;

	// 탄창에 남은 총알 (Current Ammo in Magazine)
	_uint Get_CurrentAmmoInMagazine() const { return m_iCurrentAmmoInMagazine; }

	// 전체 총알 (Total Ammo)
	_uint Get_TotalAmmo() const { return m_iTotalAmmo; }

	_uint Get_GunDamage() const { return m_iGunDamage; }


	void												Activate_ReloadUI(_float _fTimeDelta);

private:
	//CPlayer*											m_pPlayer = { nullptr };

protected:
	_uint												m_iPistolState = {};
	_uint												m_iPreAnimatioinState = {};
	//const _float4x4*									m_pSocketMatrix = { nullptr };

protected:
	virtual void										Shoot_Ray();
	virtual void										Reload() = 0;


	_uint m_iMaxAmmoInMagazine = 0;   // 한 탄창에 들어가는 최대 총알 수
	_uint m_iTotalAmmo = 0;           // 전체 보유 총알 수
	_uint m_iCurrentAmmoInMagazine = 0; // 현재 탄창에 남은 총알 수

protected:
	_vector												m_vGunMuzzlePos = {};
	_vector												m_vGunMuzzleLook = {};
	_vector												m_vMuzzleParticlePos = {};

protected:
	_bool												m_bHited = { false };

	_float												m_fRayCoolDown = {};
	_float												m_fCurrentRayTime = {};
	virtual void										Update_RayCooldown(_float _fTimeDelta); // 쿨다운 로직

	_bool												m_bVisible = { true };  // 가시성 상태

	_uint												m_iGunDamage = {};


public:
	virtual CGameObject*								Clone(void* _pArg) = 0;
	virtual void										Free() override;
};

END