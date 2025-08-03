#pragma once
#include "Base.h"

// 사본 객체들을 분류해놓은, 하나의 묶음 역할을 하는 클래스 .

BEGIN(Engine)

class CLayer final: public CBase
{
private:
    CLayer();
    virtual ~CLayer() = default;

public:
    HRESULT Add_GameObject(class CGameObject* _pGameObject, const _wstring& _strProtoTypeTag);
    //HRESULT Delete_GameObject(const _wstring& _strProtoTypeTag);
    HRESULT Delete_GameObject(CGameObject* _pGameObject);
public:
    CGameObject* Get_GameObject(const _wstring& _strProtoTypeTag);

    void Priority_Update(_float _fTimeDelta);
    void Update(_float _fTimeDelta);
    void Late_Update(_float _fTimeDelta);

public:
    class CComponent* Find_Component(const _wstring& _strComponetTag, _uint _iIndex);
    class CComponent* Find_Component(const _wstring& _strPartTag, const _wstring& _strComponetTag, _uint _iIndex);



    class CComponent* Get_Component(const _wstring& _strProtoTypeTag);

    list<class CGameObject*>*            Get_GameObjectList() { return &m_listGameObjects; }

private:
    list<class CGameObject*>            m_listGameObjects;

    map<const _wstring, class CGameObject*> m_mapGameObjects;

    _uint                               m_iObjectsNum = {};

public:
    static CLayer* Create();
    virtual void Free() override;

};

END