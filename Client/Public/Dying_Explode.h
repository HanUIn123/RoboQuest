#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
//class CVIBuffer_Rect;
class CVIBuffer_Point_Instancing;
END

BEGIN(Client)
class CDying_Explode final : public CGameObject
{
public:
	struct DYING_EXPLO_DESC : public CGameObject::GAMEOBJECT_DESC
	{

	};

private:
	CDying_Explode(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CDying_Explode(const CDying_Explode& _Prototype);
	virtual ~CDying_Explode() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize(void* _pArg) override;
	virtual void				Priority_Update(_float _fTimeDelta) override;
	virtual void				Update(_float _fTimeDelta) override;
	virtual void				Late_Update(_float _fTimeDelta) override;
	virtual HRESULT				Render() override;


	void						Activate_Explode();
	void						DeActivate_Explode();
	_bool						IsAnim() { return m_bAnimStart; }
	_float						Get_AnimTime() { return m_fAnimationTime; }
	_bool						IsActive() { return m_bActivate; }
	 void						Set_Position(const _vector& _vMissilePos, const _vector& _vMissileDir);

private:
	CShader*					m_pShaderCom = { nullptr };
	CTexture*					m_pTextureCom = { nullptr };
	//CVIBuffer_Rect*				m_pVIBufferCom = { nullptr };

	CVIBuffer_Point_Instancing* m_pVIBufferCom = { nullptr };

	_float4x4*					m_pWorldMatrix = {};

	_float						m_fFrame = {};

	_float						m_fGlobalTime = {};
	_float						m_fAnimationTime = {};

	_bool						m_bActivate = { false };
	_bool						m_bFinish = { false };
	_bool						m_bAnimStart = { false };

	_float                      m_fTimer = {};

public:
	HRESULT						Ready_Components();
	HRESULT                     Bind_ShaderResource();


public:
	static CDying_Explode*		Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject*		Clone(void* _pArg) override;
	virtual void				Free() override;
};
END