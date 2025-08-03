#pragma once
#include "Base.h"
#include "NumberUI.h"

BEGIN(Engine)
class CNumberUI_Manager final : public CBase
{
private:
    CNumberUI_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CNumberUI_Manager() = default;

public:
	HRESULT Initialize();

	//HRESULT	Add_NumberUI();

private:
	ID3D11Device*		m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };


private:
	vector<CNumberUI*>			m_vecNumberUI;

public:
	static CNumberUI_Manager* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;


};

END