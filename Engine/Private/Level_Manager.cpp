#include "Level_Manager.h"
#include "Level.h"
#include "GameInstance.h"

CLevel_Manager::CLevel_Manager()
    : m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CLevel_Manager::Open_Level(_uint _iNewLevelIndex, CLevel* _pNewLevel)
{
    // ���� ������ �ı��� ��, ���� ������ �ڿ��� �����, 
    // �Ҵ��ϰ��� �ϴ� ���ο� �ε���, ���� ������ ���� ������ �ε����� ��� �ִٰ�, �װ� clear��. 
    // level�� ����°� �ƴ϶�, �� level�� �ε��س���, ���� ��ü���� �����ϴ� ��.


    // ������ �� level���� �з��Ǿ� ���� GameInstance��ü�� ���� Clear ��ų ���̴�.
    if (nullptr != m_pCurrentLevel)
        m_pGameInstance->Clear(m_iLevelIndex);

    // _iNewLevelIndex �� _uint �� �� ������, Client������Ʈ���� �� �ǵ�, enum ���� ��. 
    Safe_Release(m_pCurrentLevel);

    m_pCurrentLevel = _pNewLevel;

    m_iLevelIndex = _iNewLevelIndex;

    return S_OK;
}

void CLevel_Manager::Update(_float _fTimeDelta)
{
    if (nullptr != m_pCurrentLevel)
        m_pCurrentLevel->Update(_fTimeDelta);
}

HRESULT CLevel_Manager::Render()
{
    if (nullptr == m_pCurrentLevel)
        return E_FAIL;

    return m_pCurrentLevel->Render();
}

CLevel_Manager* CLevel_Manager::Create()
{
    return new CLevel_Manager();
}

void CLevel_Manager::Free()
{
    __super::Free();

    Safe_Release(m_pCurrentLevel);

    Safe_Release(m_pGameInstance);
}
