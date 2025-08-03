#pragma once
#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

// prototype_Manager에 추가한 원형 객체들을 복제해, 이 오브젝트 매니져에 보관한다. 
// 보관할 때, 객체들은 각 레이어들에 분류해서 저장해놓는다.
// 보관하고 있는 객체들을, 반복적으로 Update 호출한다.
// 모아놓은 객체들의 렌더함수를 호출하는 것은 추후, Renderer 클래스에서 하겠다.

class CObject_Manager final : public CBase
{
private:
    CObject_Manager();
    virtual ~CObject_Manager() = default;

public:
    HRESULT Initialize(_uint _iNumLevels);
    void Priority_Update(_float _fTimeDelta);
    void Update(_float _fTimeDelta);
    void Late_Update(_float _fTimeDelta);
    
    void Clear(_uint _iLevelIndex);

public:
    // 원형 객체를 찾고, 복제하고, 지정한 레이어에 추가하는 역할의 함수.
    // 원형 레벨 인덱스 / 원형 키 값 / 레벨 인덱스 / 레이어 키 값 / 디폴트 인자 (생성되는 게임오브젝트가 어떤 역할을 할 지 모르기 때문에 void* 형으로 잡아줌) 
    HRESULT Add_GameObject_To_Layer(_uint _iProtoLevelIndex, const _wstring& _strPrototypeTag, _uint _iLevelIndex, const _wstring& _strLayerTag, void* _pArg = nullptr);

    class CComponent* Find_Component(_uint _iLevelIndex, const _wstring& _strLayerTag, const _wstring& _strComponetTag, _uint _iIndex);
    class CComponent* Find_Component(_uint _iLevelIndex, const _wstring& _strLayerTag, const _wstring& _strPartTag, const _wstring& _strComponetTag, _uint _iIndex);


    CGameObject* Get_GameObject(const _wstring& _strPrototypeTag, _uint _iLevelIndex, const _wstring& _strLayerTag);
    CComponent* Get_Component(const _wstring& _strPrototypeTag, _uint _iLevelIndex, const _wstring& _strLayerTag);
    list<class CGameObject*>* Get_GameObjectList(_uint _iLevelIndex, const _wstring& _strLayerTag);
    //HRESULT Delete_GameObject(_uint _iLevelIndex, const _wstring& _strLayerTag, const _wstring& _strProtoTypeTag);
    HRESULT Delete_GameObject(_uint _iLevelIndex, const _wstring& _strLayerTag, CGameObject* _pGameObject);

private:
    // 객체들을 리스트 형태로 받을 것임.
    map<const _wstring, class CLayer*>*     m_mapLayers = { nullptr };
    _uint                                   m_iNumLevels = {};
    class CGameInstance*                    m_pGameInstance = { nullptr };

private:
    class CLayer* Find_Layer(_uint _iLevelIndex, const _wstring& _strLayerTags);


public:
    static CObject_Manager* Create(_uint _iNumLevels);
    virtual void Free() override;

};

END