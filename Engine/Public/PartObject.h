#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CPartObject abstract : public CGameObject
{
public:
	struct PARTOBJECT_DESC : public CGameObject::GAMEOBJECT_DESC
	{
		const _float4x4* pParentWorldMatrix;
	};

protected:
	CPartObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CPartObject(const CPartObject& _Prototype);
	virtual ~CPartObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* _pArg);
	virtual void	Priority_Update(_float _fTimeDelta);
	virtual void	Update(_float _fTimeDelta);
	virtual void	Late_Update(_float _fTimeDelta);
	virtual HRESULT Render();

protected:
	const _float4x4*							m_pParentWorldMatrix = { nullptr };
	_float4x4									m_CombinedWorldMatrix;

	_bool										m_bIsEffectiveObject = { false };

public:
	virtual CGameObject* Clone(void* _pArg) = 0;
	virtual void Free() override;
};

END