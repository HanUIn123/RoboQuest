#pragma once
#include "Base.h"

BEGIN(Engine)
class CShadow final : public CBase
{
private:
	CShadow(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CShadow() = default;

public:
	HRESULT Initialize();
	HRESULT SetUp_ShadowLight(_fvector _vEye, _fvector _vAt, _float _fLightAngle, _float _fAspect, _float _fNear, _float _fFar);
	HRESULT Bind_Matrices(class CShader* _pShader, const _char* _pConstViewMatrixName, const _char* _pConstProjMatrixName);

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	_float4x4					m_LightViewMatrix = {};
	_float4x4					m_LightProjMatrix = {};

public:
	static CShadow* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END