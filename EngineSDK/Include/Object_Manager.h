#pragma once
#include "Base.h"
#include "GameObject.h"

BEGIN(Engine)

// prototype_Manager�� �߰��� ���� ��ü���� ������, �� ������Ʈ �Ŵ����� �����Ѵ�. 
// ������ ��, ��ü���� �� ���̾�鿡 �з��ؼ� �����س��´�.
// �����ϰ� �ִ� ��ü����, �ݺ������� Update ȣ���Ѵ�.
// ��Ƴ��� ��ü���� �����Լ��� ȣ���ϴ� ���� ����, Renderer Ŭ�������� �ϰڴ�.

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
    // ���� ��ü�� ã��, �����ϰ�, ������ ���̾ �߰��ϴ� ������ �Լ�.
    // ���� ���� �ε��� / ���� Ű �� / ���� �ε��� / ���̾� Ű �� / ����Ʈ ���� (�����Ǵ� ���ӿ�����Ʈ�� � ������ �� �� �𸣱� ������ void* ������ �����) 
    HRESULT Add_GameObject_To_Layer(_uint _iProtoLevelIndex, const _wstring& _strPrototypeTag, _uint _iLevelIndex, const _wstring& _strLayerTag, void* _pArg = nullptr);

    class CComponent* Find_Component(_uint _iLevelIndex, const _wstring& _strLayerTag, const _wstring& _strComponetTag, _uint _iIndex);
    class CComponent* Find_Component(_uint _iLevelIndex, const _wstring& _strLayerTag, const _wstring& _strPartTag, const _wstring& _strComponetTag, _uint _iIndex);


    CGameObject* Get_GameObject(const _wstring& _strPrototypeTag, _uint _iLevelIndex, const _wstring& _strLayerTag);
    CComponent* Get_Component(const _wstring& _strPrototypeTag, _uint _iLevelIndex, const _wstring& _strLayerTag);
    list<class CGameObject*>* Get_GameObjectList(_uint _iLevelIndex, const _wstring& _strLayerTag);
    //HRESULT Delete_GameObject(_uint _iLevelIndex, const _wstring& _strLayerTag, const _wstring& _strProtoTypeTag);
    HRESULT Delete_GameObject(_uint _iLevelIndex, const _wstring& _strLayerTag, CGameObject* _pGameObject);

private:
    // ��ü���� ����Ʈ ���·� ���� ����.
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