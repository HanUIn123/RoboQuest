#include "Component.h"
#include "GameInstance.h"

CComponent::CComponent(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice{_pDevice}
    , m_pContext{_pContext}
    , m_pGameInstance{CGameInstance::GetInstance()}
    , m_bClone(false)
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

CComponent::CComponent(const CComponent& _Prototype)
    : m_pDevice{_Prototype.m_pDevice}
    , m_pContext{_Prototype.m_pContext}
    , m_pGameInstance{ CGameInstance::GetInstance() }
    , m_bClone(true)
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CComponent::Initialize_Prototype()
{


    return S_OK;
}

HRESULT CComponent::Initialize(void* _pArg)
{
    return S_OK;
}


void CComponent::Free()
{
    __super::Free();


    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
