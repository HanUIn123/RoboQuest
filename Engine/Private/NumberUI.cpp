#include "NumberUI.h"

CNumberUI::CNumberUI(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :m_pDevice{_pDevice}
    ,m_pContext{_pContext}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CNumberUI::Initialize()
{

	return S_OK;
}

CNumberUI* CNumberUI::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iWidth, _uint _iHeight, DXGI_FORMAT _ePixelFormat, const _float4& _vClearColor)
{
	CNumberUI* pInstance = new CNumberUI(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed to Created : CNumberUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNumberUI::Free()
{
	__super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
