#include "../Public/Object_Manager.h"
#include "Layer.h"

#include "GameInstance.h"
#include "GameObject.h"

CObject_Manager::CObject_Manager()
    : m_pGameInstance(CGameInstance::GetInstance())     // GameInstance에 있는 Prototype Manager를 사용하기 위해서, 
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CObject_Manager::Initialize(_uint _iNumLevels)
{
    m_iNumLevels = _iNumLevels;

    m_mapLayers = new map<const _wstring, CLayer*>[_iNumLevels];

    return S_OK;
}

// Priority_Update 들을 싸악 호출하고 끝나면, Update 함수들 호출 하고 다 끝나고 나면, Late_Update 함수들 쫘악 호출하는 식으로 할 것.
// 현재 내 프로젝트는, LEVEL 이 2개, ( LEVEL_STATIC + LEVEL_LOGO ) / ( LEVEL_STATIC + LEVEL_GAMEPLAY )


void CObject_Manager::Priority_Update(_float _fTimeDelta)
{
    for (size_t i = 0; i < m_iNumLevels; ++i)
    {
        for (auto& Pair : m_mapLayers[i])
        {
            Pair.second->Priority_Update(_fTimeDelta);
        }
    }
}


/*
이 사이에, 예를 들어서, 카메라 갱신이 종류되고 난 후에, 그 카메라에서 만들어낸 뷰, 투영 행렬을 저장해서, 
그 저장된 클래스에 접근해서 행렬들을 가져오는. 이야기.
*/


void CObject_Manager::Update(_float _fTimeDelta)
{
    for (size_t i = 0; i < m_iNumLevels; ++i)
    {
        for (auto& Pair : m_mapLayers[i])
        {
            Pair.second->Update(_fTimeDelta);
        }
    }
}

void CObject_Manager::Late_Update(_float _fTimeDelta)
{
    for (size_t i = 0; i < m_iNumLevels; ++i)
    {
        for (auto& Pair : m_mapLayers[i])
        {
            Pair.second->Late_Update(_fTimeDelta);
        }
    }
}

void CObject_Manager::Clear(_uint _iLevelIndex)
{
    for (auto& Pair : m_mapLayers[_iLevelIndex])
    {
        Safe_Release(Pair.second);
    }

    m_mapLayers[_iLevelIndex].clear();
}


// 사본들을 가져와서, 레이어에 추가하는 기능 "어떤 레벨에 있는 어떤 레이어에 넣어야 하는가!" 
// prototype manager에서 원형 객체를 가져온다. (prototype_Manager 클래스를 추가하는게 아닌, gameInstance를 추가해서 거기서 호출하는 식으로) 
HRESULT CObject_Manager::Add_GameObject_To_Layer(_uint _iProtoLevelIndex, const _wstring& _strPrototypeTag, _uint _iLevelIndex, const _wstring& _strLayerTag, void* _pArg)
{
    // Layer에 추가할 사본 객체를 복제해서 가져온다.
    // 몇 번째 LEVEL의 GAME_OBJECT를 , 어떤 원형 키값인지, ( prototype manager클래스의 find prototype 을 보면, 키 값으로 원형을 찾고 있음. ) , _pArg는 사본들의 특성? 이라고 보면 됨.
    CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, _iProtoLevelIndex, _strPrototypeTag, _pArg));
    if (nullptr == pGameObject)
        return E_FAIL;

    // 사본 객체를 추가할, 레이어를 찾는다. 
    CLayer* pLayer = Find_Layer(_iLevelIndex, _strLayerTag);

    // 사본 객체를 추가할 레이어가 없다는 것은, 지정한 레이어에 최초로 추가하기 위한 객체라는 뜻! 이고, 즉 레이어를 최초로 하나 만들면 된다.
    // 레이어 최초로 새로 만들고, GameObject 추가하기!
    if (nullptr == pLayer)
    {
        pLayer = CLayer::Create();

        pLayer->Add_GameObject( pGameObject, _strPrototypeTag);

        m_mapLayers[_iLevelIndex].emplace(_strLayerTag, pLayer);
    }
    else // 사본 객체를 추가할 레이어가 이미 있다면, 그 레이어에 사본객체를 그냥 추가한다.
    {
        pLayer->Add_GameObject(pGameObject, _strPrototypeTag);
    }

    return S_OK;
}

CComponent* CObject_Manager::Find_Component(_uint _iLevelIndex, const _wstring& _strLayerTag, const _wstring& _strComponetTag, _uint _iIndex)
{
    CLayer* pLayer = Find_Layer(_iLevelIndex, _strLayerTag);
    if (nullptr == pLayer)
        return nullptr;

    return pLayer->Find_Component(_strComponetTag, _iIndex);
}

CComponent* CObject_Manager::Find_Component(_uint _iLevelIndex, const _wstring& _strLayerTag, const _wstring& _strPartTag, const _wstring& _strComponetTag, _uint _iIndex)
{
    CLayer* pLayer = Find_Layer(_iLevelIndex, _strLayerTag);
    if (nullptr == pLayer)
        return nullptr;

    return pLayer->Find_Component(_strPartTag, _strComponetTag, _iIndex);
}

CGameObject* CObject_Manager::Get_GameObject(const _wstring& _strPrototypeTag, _uint _iLevelIndex, const _wstring& _strLayerTag)
{
    // 지정된 레벨과 레이어에서 게임 오브젝트를 찾음
    // 근데 이러면 
    CLayer* pLayer = Find_Layer(_iLevelIndex, _strLayerTag);
    if (pLayer == nullptr)
        return nullptr;  // 레이어가 없으면 nullptr 반환

    // 레이어에서 게임 오브젝트를 찾음
    return pLayer->Get_GameObject(_strPrototypeTag);
}

CComponent* CObject_Manager::Get_Component(const _wstring& _strPrototypeTag, _uint _iLevelIndex, const _wstring& _strLayerTag)
{
    CLayer* pLayer = Find_Layer(_iLevelIndex, _strLayerTag);
    if (pLayer == nullptr)
        return nullptr;

    return pLayer->Get_Component(_strPrototypeTag);
}

list<class CGameObject*>* CObject_Manager::Get_GameObjectList(_uint _iLevelIndex, const _wstring& _strLayerTag)
{
    CLayer* pLayer = Find_Layer(_iLevelIndex, _strLayerTag);
    if (pLayer == nullptr)
        return nullptr;

    return pLayer->Get_GameObjectList();
}

HRESULT CObject_Manager::Delete_GameObject(_uint _iLevelIndex, const _wstring& _strLayerTag, CGameObject* _pGameObject)
{
    CLayer* pLayer = Find_Layer(_iLevelIndex, _strLayerTag);

    if (nullptr == pLayer)
        return E_FAIL;

    if (nullptr == _pGameObject)
        return E_FAIL;

    HRESULT hr = pLayer->Delete_GameObject(_pGameObject);
    if (FAILED(hr))
        return E_FAIL;

    return S_OK;
}

//HRESULT CObject_Manager::Delete_GameObject(_uint _iLevelIndex, const _wstring& _strLayerTag, const _wstring& _strProtoTypeTag)
//{
//    CLayer* pLayer = Find_Layer(_iLevelIndex, _strLayerTag);
//
//    if (nullptr == pLayer)
//        return E_FAIL;
//
//    CGameObject* pGameObject = pLayer->Get_GameObject(_strProtoTypeTag);
//    if (nullptr == pGameObject)
//        return E_FAIL;
//
//    HRESULT hr = pLayer->Delete_GameObject(_strProtoTypeTag);
//    if (FAILED(hr))
//        return E_FAIL;
//
//    return S_OK;
//}


CLayer* CObject_Manager::Find_Layer(_uint _iLevelIndex, const _wstring& _strLayerTags)
{
    auto iter = m_mapLayers[_iLevelIndex].find(_strLayerTags);

    if (iter == m_mapLayers[_iLevelIndex].end())
    {
        return nullptr;
    }

    // 레이어의 주소를 반환함.
    return iter->second;
}

CObject_Manager* CObject_Manager::Create(_uint _iNumLevels)
{
    CObject_Manager* pInstance = new CObject_Manager();

    if (FAILED(pInstance->Initialize(_iNumLevels)))
    {
        MSG_BOX("Failed To Created : CObject_Manager");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CObject_Manager::Free()
{
    __super::Free();

    for (size_t i = 0; i < m_iNumLevels; ++i)
    {
        for (auto& Pair : m_mapLayers[i])
            Safe_Release(Pair.second);

        m_mapLayers[i].clear();
    }

    Safe_Release(m_pGameInstance);

    Safe_Delete_Array(m_mapLayers);
}
