#pragma once
#include "PartObject.h"
#include "UIPartObject.h"
#include "EffectPartObject.h"

BEGIN(Engine)

class ENGINE_DLL CContainerObject abstract : public CGameObject
{
public:
	struct CONTAINEROBJECT_DESC : public CGameObject::GAMEOBJECT_DESC
	{

	};

protected:
	CContainerObject(ID3D11Device* _pDeivce, ID3D11DeviceContext* _pContext);
	CContainerObject(const CContainerObject& _Prototype);
	virtual ~CContainerObject() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;
	virtual void Priority_Update(_float _fTimeDelta) override;
	virtual void Update(_float _fTimeDelta) override;
	virtual void Late_Update(_float _fTimeDelta) override;
	virtual HRESULT Render() override;

	CComponent* Find_PartObject_Component(const _wstring& _strPartTag, const _wstring& _strComponentTag);
	CComponent* Find_UIPartObject_Component(const _wstring& _strPartTag, const _wstring& _strComponentTag);
	CComponent* Find_EffectPartObject_Component(const _wstring& _strPartTag, const _wstring& _strComponentTag);

protected:
	// ���� ������Ʈ���� ��� �ִ� map Container �� ������ ����. 
	// �÷��̾� �ٵ� / ���� / ����Ʈ / ������ ��.. �̶�� �������� ����ִ� �� �����̳�.
	map<const _wstring, CPartObject*>		m_mapPartObjects;
	map<const _wstring, CUIPartObject*>		m_mapUIPartObjects;
	map<const _wstring, CEffectPartObject*> m_mapEffectPartObjects;

protected:
	HRESULT Add_PartObject(const _wstring& _strPartTag, _uint _ePrototypeLevelID, const _wstring& _strPrototypeTag, CPartObject::PARTOBJECT_DESC* _pDesc = nullptr);
	HRESULT	Add_UIPartObject(const _wstring& _strUIPartTag, _uint _ePrototypeLevelID, const _wstring& _strPrototypeTag, CUIPartObject::UIPARTOBJECT_DESC* _pDesc = nullptr);
	HRESULT Add_Effect_PartObject(const _wstring& _strEffectPartTag, _uint _ePrototypeLevelID, const _wstring& _strPrototypeTag, CEffectPartObject::EFFECT_PARTOBJECT_DESC* _pDesc = nullptr);
	CPartObject* Find_PartObject(const _wstring& _strPartTag);
	CUIPartObject* Find_UIPartObject(const _wstring& _strUIPartTag);
	CEffectPartObject* Find_Effect_PartObject(const _wstring& _strEffectPartTag);

public:
	virtual CGameObject* Clone(void* _pArg) = 0;
	virtual void Free() override;
};

END