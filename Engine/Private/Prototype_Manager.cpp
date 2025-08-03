#include "../Public/Prototype_Manager.h"
#include "GameObject.h"
#include "Component.h"

CPrototype_Manager::CPrototype_Manager()
{
}

HRESULT CPrototype_Manager::Initialize(_uint _iNumLevels)
{
    m_iNumLevels = _iNumLevels;

    // 동적으로 할당. 
    m_mapPrototype = new PROTOTYPES[_iNumLevels];

    return S_OK;
}

// 어느 level에 추가하겠다 의 _iLevelIndex
// 그 level에 추가한 원형 객체의 키값(이름) 이 무엇인가, 
// 그 원형 객체의 정체!!! 
HRESULT CPrototype_Manager::Add_Prototype(_uint _iLevelIndex, const _wstring& _strPrototypeTag, CBase* _pPrototype)
{
    if ((_iLevelIndex >= m_iNumLevels) || nullptr != Find_Prototype(_iLevelIndex, _strPrototypeTag))
        return E_FAIL;

    m_mapPrototype[_iLevelIndex].emplace(_strPrototypeTag, _pPrototype);

    return S_OK;
}

CBase* CPrototype_Manager::Clone_Prototype(PROTOTYPE _eType, _uint _iLevelIndex, const _wstring& _strProtoTag, void* _pArg)
{
    CBase* pPrototype = Find_Prototype(_iLevelIndex, _strProtoTag);
    if (nullptr == pPrototype)
        return nullptr;

    CBase* pResult = { nullptr };
        
    if (_eType == PROTOTYPE::TYPE_GAMEOBJECT)
        pResult = dynamic_cast<CGameObject*>(pPrototype)->Clone(_pArg);
    else
        pResult = dynamic_cast<CComponent*>(pPrototype)->Clone(_pArg);

    return pResult;


    // return _eType == PROTOTYPE::TYPE_GAMEOBJECT ? dynamic_cast<CGameObject*>(pPrototype)->Clone(_pArg) : nullptr;
}

void CPrototype_Manager::Clear(_uint _iLevelIndex)
{
    for (auto& Pair : m_mapPrototype[_iLevelIndex])
    {
        Safe_Release(Pair.second);
    }

    m_mapPrototype[_iLevelIndex].clear();
}

// 중복체크. map 담아둔, m_mapPrototype[_iLevelIndex] : 특정 level의 그 객체의 키값이 만약에 null이야. 없어! 
// 그러면, null 반환한다.그게 아니면, 그 원형 객체를 반환해줘요.
CBase* CPrototype_Manager::Find_Prototype(_uint _iLevelIndex, const _wstring& _strPrototypeTag)
{
    auto iter = m_mapPrototype[_iLevelIndex].find(_strPrototypeTag);

    if (iter == m_mapPrototype[_iLevelIndex].end())
        return nullptr;

    return iter->second;
}

CPrototype_Manager* CPrototype_Manager::Create(_uint _iNumLevels)
{
    CPrototype_Manager* pInstance = new CPrototype_Manager();

    if (FAILED(pInstance->Initialize(_iNumLevels)))
    {
        MSG_BOX("Failed To Created : CPrototype_Manager");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CPrototype_Manager::Free()
{
    __super::Free();

    for (size_t i = 0; i < m_iNumLevels; ++i)
    {
        for (auto& Pair : m_mapPrototype[i])
            Safe_Release(Pair.second);

        m_mapPrototype[i].clear();
    }

    Safe_Delete_Array(m_mapPrototype);
}
