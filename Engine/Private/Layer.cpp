#include "../Public/Layer.h"
#include "GameObject.h"
#include "ContainerObject.h"

CLayer::CLayer()
{
}

HRESULT CLayer::Add_GameObject(CGameObject* _pGameObject, const _wstring& _strProtoTypeTag)
{
    // m_mapGameObjects 에 들어가는 애들 이름이 다 똑같았다. 4마리든 5마리든.. 
    // 그럼, 이 map container 를 이용할 때는,, 그냥 객체 하나! 일 때 만 쓰기. 
    //원래는 너가 생각한 01234 +c_str() 이걸로 차례대로 추가하면 가능할텐데
    // 내가 그 영역을 통달하지 못해서 저렇게 알아서 스트링값이 다르게 저장하는거를 못하겠더라고
    //이게되면 map Container 를 써도될거야

    //저의 얕은 지식 수준으로 인한. 
    //결국 여기서 제가 만든 겟겜옵의 한계가
    //드러난 순간이엇다.. ㅠㅠ.

    _pGameObject->Set_PrototypeTag(_strProtoTypeTag);


    m_mapGameObjects.emplace(_strProtoTypeTag, _pGameObject);
    m_listGameObjects.push_back(_pGameObject);

    //m_iObjectsNum++;

    return S_OK;
}

HRESULT CLayer::Delete_GameObject(CGameObject* _pGameObject)
{
    /*for (auto iter = m_listGameObjects.begin(); iter != m_listGameObjects.end();)
    {
        if (*iter == _pGameObject)
        {
            Safe_Release(*iter);
            iter = m_listGameObjects.erase(iter);
            return S_OK;
        }
        else
        {
            ++iter;
        }
    }*/

    for (auto& pGameObject : m_listGameObjects)
    {
        if (pGameObject == _pGameObject)
        {
            Safe_Release(pGameObject);
            pGameObject = nullptr;  
            return S_OK;
        }
    }

    return E_FAIL;  
}

CGameObject* CLayer::Get_GameObject(const _wstring& _strProtoTypeTag)
{
    auto iter = m_mapGameObjects.find(_strProtoTypeTag);
    if (iter != m_mapGameObjects.end())
        return iter->second;

    return nullptr;
}

void CLayer::Priority_Update(_float _fTimeDelta)
{
  /*  for (auto& pGameObject : m_listGameObjects)
    {
        pGameObject->Priority_Update(_fTimeDelta);
    }*/


    for (auto iter = m_listGameObjects.begin(); iter != m_listGameObjects.end();)
    {
        if (*iter == nullptr)
        {
            iter = m_listGameObjects.erase(iter);
            //continue;  // 다음 오브젝트로 건너뛰기
        }
        else
        {

            (*iter)->Priority_Update(_fTimeDelta);
            ++iter;
        }

    }
}

void CLayer::Update(_float _fTimeDelta)
{
   /* for (auto& pGameObject : m_listGameObjects)
    {
        pGameObject->Update(_fTimeDelta);
    }*/


    for (auto iter = m_listGameObjects.begin(); iter != m_listGameObjects.end();)
    {
        if (*iter == nullptr) 
        {
            iter = m_listGameObjects.erase(iter);
            //continue;  // 다음 오브젝트로 건너뛰기
        }
        else
        {
            (*iter)->Update(_fTimeDelta);
            ++iter;
        }


    }
}

void CLayer::Late_Update(_float _fTimeDelta)
{
  /*  for (auto& pGameObject : m_listGameObjects)
    {
        pGameObject->Late_Update(_fTimeDelta);
    }*/

    for (auto iter = m_listGameObjects.begin(); iter != m_listGameObjects.end();)
    {
        if (*iter == nullptr)
        {
            iter = m_listGameObjects.erase(iter);
            //continue;  // 다음 오브젝트로 건너뛰기
        }
        else
        {
            (*iter)->Late_Update(_fTimeDelta);
            ++iter;
        }


    }
}

CComponent* CLayer::Find_Component(const _wstring& _strComponetTag, _uint _iIndex)
{
    auto	iter = m_listGameObjects.begin();

    for (size_t i = 0; i < _iIndex; i++)
        ++iter;

    return (*iter)->Find_Component(_strComponetTag);
}

CComponent* CLayer::Find_Component(const _wstring& _strPartTag, const _wstring& _strComponetTag, _uint _iIndex)
{
    auto	iter = m_listGameObjects.begin();

    for (size_t i = 0; i < _iIndex; i++)
        ++iter;

    return dynamic_cast<CContainerObject*>(*iter)->Find_PartObject_Component(_strPartTag, _strComponetTag);
}

CComponent* CLayer::Get_Component(const _wstring& _strProtoTypeTag)
{
    auto iter = m_mapGameObjects.find(_strProtoTypeTag);

    if (iter == m_mapGameObjects.end())
        return nullptr;

    return iter->second->Find_Component(_strProtoTypeTag);
}

CLayer* CLayer::Create()
{
    return new CLayer();
}

void CLayer::Free()
{
    __super::Free();

    for (auto& pGameObject : m_listGameObjects)
    {
        Safe_Release(pGameObject);
    }
    m_listGameObjects.clear();

    //for (auto& Pair : m_mapGameObjects)
    //{
    //    Safe_Release(Pair.second);
    //}
    //m_mapGameObjects.clear();
}
