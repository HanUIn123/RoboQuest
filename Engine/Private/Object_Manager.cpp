#include "../Public/Object_Manager.h"
#include "Layer.h"

#include "GameInstance.h"
#include "GameObject.h"

CObject_Manager::CObject_Manager()
    : m_pGameInstance(CGameInstance::GetInstance())     // GameInstance�� �ִ� Prototype Manager�� ����ϱ� ���ؼ�, 
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CObject_Manager::Initialize(_uint _iNumLevels)
{
    m_iNumLevels = _iNumLevels;

    m_mapLayers = new map<const _wstring, CLayer*>[_iNumLevels];

    return S_OK;
}

// Priority_Update ���� �ξ� ȣ���ϰ� ������, Update �Լ��� ȣ�� �ϰ� �� ������ ����, Late_Update �Լ��� �Ҿ� ȣ���ϴ� ������ �� ��.
// ���� �� ������Ʈ��, LEVEL �� 2��, ( LEVEL_STATIC + LEVEL_LOGO ) / ( LEVEL_STATIC + LEVEL_GAMEPLAY )


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
�� ���̿�, ���� ��, ī�޶� ������ �����ǰ� �� �Ŀ�, �� ī�޶󿡼� ���� ��, ���� ����� �����ؼ�, 
�� ����� Ŭ������ �����ؼ� ��ĵ��� ��������. �̾߱�.
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


// �纻���� �����ͼ�, ���̾ �߰��ϴ� ��� "� ������ �ִ� � ���̾ �־�� �ϴ°�!" 
// prototype manager���� ���� ��ü�� �����´�. (prototype_Manager Ŭ������ �߰��ϴ°� �ƴ�, gameInstance�� �߰��ؼ� �ű⼭ ȣ���ϴ� ������) 
HRESULT CObject_Manager::Add_GameObject_To_Layer(_uint _iProtoLevelIndex, const _wstring& _strPrototypeTag, _uint _iLevelIndex, const _wstring& _strLayerTag, void* _pArg)
{
    // Layer�� �߰��� �纻 ��ü�� �����ؼ� �����´�.
    // �� ��° LEVEL�� GAME_OBJECT�� , � ���� Ű������, ( prototype managerŬ������ find prototype �� ����, Ű ������ ������ ã�� ����. ) , _pArg�� �纻���� Ư��? �̶�� ���� ��.
    CGameObject* pGameObject = dynamic_cast<CGameObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, _iProtoLevelIndex, _strPrototypeTag, _pArg));
    if (nullptr == pGameObject)
        return E_FAIL;

    // �纻 ��ü�� �߰���, ���̾ ã�´�. 
    CLayer* pLayer = Find_Layer(_iLevelIndex, _strLayerTag);

    // �纻 ��ü�� �߰��� ���̾ ���ٴ� ����, ������ ���̾ ���ʷ� �߰��ϱ� ���� ��ü��� ��! �̰�, �� ���̾ ���ʷ� �ϳ� ����� �ȴ�.
    // ���̾� ���ʷ� ���� �����, GameObject �߰��ϱ�!
    if (nullptr == pLayer)
    {
        pLayer = CLayer::Create();

        pLayer->Add_GameObject( pGameObject, _strPrototypeTag);

        m_mapLayers[_iLevelIndex].emplace(_strLayerTag, pLayer);
    }
    else // �纻 ��ü�� �߰��� ���̾ �̹� �ִٸ�, �� ���̾ �纻��ü�� �׳� �߰��Ѵ�.
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
    // ������ ������ ���̾�� ���� ������Ʈ�� ã��
    // �ٵ� �̷��� 
    CLayer* pLayer = Find_Layer(_iLevelIndex, _strLayerTag);
    if (pLayer == nullptr)
        return nullptr;  // ���̾ ������ nullptr ��ȯ

    // ���̾�� ���� ������Ʈ�� ã��
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

    // ���̾��� �ּҸ� ��ȯ��.
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
