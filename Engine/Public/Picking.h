#pragma once
#include "Base.h"

BEGIN(Engine)
class CPicking final : public CBase
{
private:
	CPicking(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CPicking() = default;

public:
	HRESULT							Initialize(HWND _hWnd);
	void							Update();
	_bool							Compute_PickPos(_float3* _pOut);

private:
	HWND							m_hWnd = {};
	_float							m_fViewportWidth, m_fViewportHeight;

	ID3D11Device*					m_pDevice = { nullptr };
	ID3D11DeviceContext*			m_pContext = { nullptr };

	/* ��ŷ�� ���� �ȼ��� ���̸� �����س��Ҵ� ����Ÿ���ؽ���(map, unmap�� �ȵǴϱ�)�� �����Ͽ� �����ϴ� �ؽ��� ����. */
	ID3D11Texture2D*				m_pTexture2D = { nullptr };

	class CGameInstance*			m_pGameInstance = { nullptr };

public:
	static CPicking*				Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, HWND _hWnd);
	virtual void					Free() override;
};

END