#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Point_Instancing;
END

BEGIN(Client)

class CMissileSmokeParticle final: public CGameObject
{
public:
    struct SMOKE_PARTICLE_DESC : public CGameObject::GAMEOBJECT_DESC
    {
		//const _float4x4* pSocketMatrix = { nullptr };
    };
private:
	CMissileSmokeParticle(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CMissileSmokeParticle(const CMissileSmokeParticle& _Prototype);
	virtual ~CMissileSmokeParticle() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* _pArg) override;
	virtual void				Priority_Update(_float _fTimeDelta) override;
	virtual void				Update(_float _fTimeDelta) override;
	virtual void				Late_Update(_float _fTimeDelta) override;
	virtual HRESULT				Render() override;
	_bool						IsActive() { return m_bActive; }
	void						Set_Position(const _vector& _vMissilePos, const _vector& _vMissileDir);


	void						Activate_Smoke();
	void						DeActivate_Smoke();

private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	CVIBuffer_Point_Instancing* m_pVIBufferCom = { nullptr };

	_float                      m_fAnimationTime = 0.0f;
	_bool						m_bActive = { false };

	_vector						m_vSmokeDirection = {};

public:
	HRESULT						Ready_Components();

public:
	static CMissileSmokeParticle*	Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*			Clone(void* pArg) override;
	virtual void					Free() override;
};
END
