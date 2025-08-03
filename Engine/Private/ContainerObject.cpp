#include "ContainerObject.h"
#include "GameInstance.h"


CContainerObject::CContainerObject(ID3D11Device* _pDeivce, ID3D11DeviceContext* _pContext)
    :CGameObject{ _pDeivce, _pContext, }
{
}

CContainerObject::CContainerObject(const CContainerObject& _Prototype)
    : CGameObject(_Prototype)
{
}

HRESULT CContainerObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CContainerObject::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}

void CContainerObject::Priority_Update(_float _fTimeDelta)
{
    for (auto& Pair : m_mapPartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Priority_Update(_fTimeDelta);
    }

    for (auto& Pair : m_mapUIPartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Priority_Update(_fTimeDelta);
    }

    for (auto& Pair : m_mapEffectPartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Priority_Update(_fTimeDelta);
    }

}

void CContainerObject::Update(_float _fTimeDelta)
{
    for (auto& Pair : m_mapPartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Update(_fTimeDelta);
    }

    for (auto& Pair : m_mapUIPartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Update(_fTimeDelta);
    }

    for (auto& Pair : m_mapEffectPartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Update(_fTimeDelta);
    }

}

void CContainerObject::Late_Update(_float _fTimeDelta)
{
    for (auto& Pair : m_mapPartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Late_Update(_fTimeDelta);
    }

    for (auto& Pair : m_mapUIPartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Late_Update(_fTimeDelta);
    }

    for (auto& Pair : m_mapEffectPartObjects)
    {
        if (nullptr != Pair.second)
            Pair.second->Late_Update(_fTimeDelta);
    }
}

HRESULT CContainerObject::Render()
{
    //for (auto& Pair : m_mapPartObjects)
    //{
    //    if (nullptr != Pair.second)
    //        Pair.second->Render();
    //}

    return S_OK;
}

HRESULT CContainerObject::Add_PartObject(const _wstring& _strPartTag, _uint _ePrototypeLevelID, const _wstring& _strPrototypeTag, CPartObject::PARTOBJECT_DESC* _pDesc)
{
    // Part Object 에 레이어 추가가 아닌, 복제해 와서 실제 이 Player 가 가지고 있는 Part Object 들이라는 Map 컨테이너에 푸쉬하려고 한다.
    // 그래서 Clone_Prototype 함수로 처리 ㄱㄱ 
    // 어떤 레벨에 있는, 어떤 원형을 복제한다라는 기능의 함수이다.

    CPartObject* pPartObject = { nullptr };

    pPartObject = dynamic_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, _ePrototypeLevelID, _strPrototypeTag, _pDesc));
    if (nullptr == pPartObject)
        return E_FAIL;

    m_mapPartObjects.emplace(_strPartTag, pPartObject);

    return S_OK;
}

HRESULT CContainerObject::Add_UIPartObject(const _wstring& _strUIPartTag, _uint _ePrototypeLevelID, const _wstring& _strPrototypeTag, CUIPartObject::UIPARTOBJECT_DESC* _pDesc)
{
    CUIPartObject* pUIPartObject = { nullptr };

    pUIPartObject = dynamic_cast<CUIPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, _ePrototypeLevelID, _strPrototypeTag, _pDesc));
    if (nullptr == pUIPartObject)
        return E_FAIL;

    m_mapUIPartObjects.emplace(_strUIPartTag, pUIPartObject);

    return S_OK;
}

HRESULT CContainerObject::Add_Effect_PartObject(const _wstring& _strEffectPartTag, _uint _ePrototypeLevelID, const _wstring& _strPrototypeTag, CEffectPartObject::EFFECT_PARTOBJECT_DESC* _pDesc)
{
    CEffectPartObject* pEffectPartObject = { nullptr };

    pEffectPartObject = dynamic_cast<CEffectPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::TYPE_GAMEOBJECT, _ePrototypeLevelID, _strPrototypeTag, _pDesc));
    if (nullptr == pEffectPartObject)
        return E_FAIL;

    m_mapEffectPartObjects.emplace(_strEffectPartTag, pEffectPartObject);

    return S_OK;
}

CComponent* CContainerObject::Find_PartObject_Component(const _wstring& _strPartTag, const _wstring& _strComponentTag)
{
    auto iter = m_mapPartObjects.find(_strPartTag);
    if (iter == m_mapPartObjects.end())
        return nullptr;

    return (iter->second)->Find_Component(_strComponentTag);
}

CComponent* CContainerObject::Find_UIPartObject_Component(const _wstring& _strPartTag, const _wstring& _strComponentTag)
{
    auto iter = m_mapUIPartObjects.find(_strPartTag);
    if (iter == m_mapUIPartObjects.end())
        return nullptr;

    return (iter->second)->Find_Component(_strComponentTag);
}

CComponent* CContainerObject::Find_EffectPartObject_Component(const _wstring& _strPartTag, const _wstring& _strComponentTag)
{
    auto iter = m_mapEffectPartObjects.find(_strPartTag);
    if (iter == m_mapEffectPartObjects.end())
        return nullptr;

    return (iter->second)->Find_Component(_strComponentTag);
}

CPartObject* CContainerObject::Find_PartObject(const _wstring& _strPartTag)
{
    auto iter = m_mapPartObjects.find(_strPartTag);

    if (iter != m_mapPartObjects.end())
        return iter->second;;

    return nullptr;
}

CUIPartObject* CContainerObject::Find_UIPartObject(const _wstring& _strUIPartTag)
{
    auto iter = m_mapUIPartObjects.find(_strUIPartTag);

    if (iter != m_mapUIPartObjects.end())
        return iter->second;

    return nullptr;
}

CEffectPartObject* CContainerObject::Find_Effect_PartObject(const _wstring& _strEffectPartTag)
{
    auto iter = m_mapEffectPartObjects.find(_strEffectPartTag);

    if (iter != m_mapEffectPartObjects.end())
        return iter->second;

    return nullptr;
}

void CContainerObject::Free()
{
    __super::Free();

    for (auto& Pair : m_mapPartObjects)
    {
        Safe_Release(Pair.second);
    }

    m_mapPartObjects.clear();

    for (auto& Pair : m_mapUIPartObjects)
    {
        Safe_Release(Pair.second);
    }

    m_mapUIPartObjects.clear();

    for (auto& Pair : m_mapEffectPartObjects)
    {
        Safe_Release(Pair.second);
    }

    m_mapEffectPartObjects.clear();

}
