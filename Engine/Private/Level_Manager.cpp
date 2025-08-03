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
    // 기존 레벨이 파괴될 때, 기존 레벨용 자원을 지우고, 
    // 할당하고자 하는 새로운 인덱스, 대입 전까지 기존 레벨의 인덱스를 들고 있다가, 그걸 clear함. 
    // level을 지우는게 아니라, 그 level를 로드해놨던, 원형 객체들을 정리하는 것.


    // 앞으로 각 level마다 분류되어 사용된 GameInstance객체들 또한 Clear 시킬 것이다.
    if (nullptr != m_pCurrentLevel)
        m_pGameInstance->Clear(m_iLevelIndex);

    // _iNewLevelIndex 를 _uint 로 한 이유는, Client프로젝트에서 쓸 건데, enum 값이 라서. 
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
