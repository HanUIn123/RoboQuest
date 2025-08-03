#include "NumberUI_Manager.h"
#include "NumberUI.h"

CNumberUI_Manager::CNumberUI_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :m_pDevice{_pDevice}
    ,m_pContext{_pContext}
{
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pDevice);
}

HRESULT CNumberUI_Manager::Initialize()
{


    return S_OK;
}

CNumberUI_Manager* CNumberUI_Manager::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CNumberUI_Manager* pInstance = new CNumberUI_Manager(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed to Created : CNumberUI_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CNumberUI_Manager::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
