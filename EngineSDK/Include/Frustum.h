#pragma once
#include "Base.h"

BEGIN(Engine)
class CFrustum final : public CBase
{
private:
	CFrustum();
	virtual ~CFrustum() = default;

public:
	HRESULT Initialize();
	void Update();

public:
	_bool isIn_WorldSpace(_fvector _vWorldPoint, _float _fRange = 0.f);

private:
	class CGameInstance*	m_pGameInstance = { nullptr };
	_float4					m_vOriginal_Points[8];
	_float4					m_vWorld_Points[8];
	_float4					m_vWorld_Planes[6];

private:
	void Make_Planes(const _float4* _pPoints, _float4* _pPlanes);

public:
	static CFrustum* Create();
	virtual void Free() override;
};
END