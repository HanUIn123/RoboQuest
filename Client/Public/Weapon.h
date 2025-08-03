#pragma once
#include "Client_Defines.h"
#include "PartObject.h"
#include "ImGui_Manager.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CWeapon final : public CPartObject
{
public:
	struct WEAPON_DESC : public CPartObject::PARTOBJECT_DESC
	{
		const _float4x4* pSocketMatrix = { nullptr };
	};
private:
	CWeapon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CWeapon(const CWeapon& _Prototype);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;

	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;

	virtual HRESULT Render() override;

private:
	CShader* m_pShaderCom = { nullptr };
	CModel* m_pModelCom = { nullptr };
	const _float4x4* m_pSocketMatrix = { nullptr };

public:
	HRESULT Ready_Components();
	HRESULT Bind_ShaderResources();

private:
	_float fX = {};
	_float fY = {};
	_float fZ = {};

public:
	static CWeapon* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CGameObject* Clone(void* _pArg) override;
	virtual void Free() override;


};

END