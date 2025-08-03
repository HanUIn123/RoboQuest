#pragma once
//#include "Base.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
    struct GAMEOBJECT_DESC : public CTransform::TRANSFORM_DESC
    {
        //_float fSpeed;
    };
protected: 
    CGameObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CGameObject(const CGameObject& _Prototype);
    virtual ~CGameObject() = default;

public:
    // ¿øÇü °´Ã¼¿ë + »çº» °´Ã¼¿ë 
    virtual HRESULT Initialize_Prototype();
    virtual HRESULT Initialize(void* _pArg);

    virtual void Priority_Update(_float _fTimeDelta);
    virtual void Update(_float _fTimeDelta);
    virtual void Late_Update(_float _fTimeDelta);

    virtual HRESULT Render();

    virtual HRESULT Render_Shadow() { return S_OK; }

    virtual HRESULT Render_Glow() { return S_OK; }

    class CComponent* Find_Component(const _wstring& _strComponentTag);

public:
    void Set_PrototypeTag(const _wstring& _strPrototypeTag) { m_strPrototypeTag = _strPrototypeTag; }
    const _wstring& Get_PrototypeTag() const { return m_strPrototypeTag; }
protected:
    ID3D11Device*                           m_pDevice = { nullptr };
    ID3D11DeviceContext*                    m_pContext = { nullptr };
    class CGameInstance*                    m_pGameInstance = { nullptr };
    class CTransform*                       m_pTransformCom = { nullptr };

    map<const _wstring, class CComponent*>  m_mapComponent;
    
    _wstring                                m_strPrototypeTag;

protected:
    HRESULT Add_Component(_uint _iPrototypeLevelIndex, const _wstring& _strPrototypeTag, const _wstring& _strComponentTag, CComponent** _ppOut, void* _pArg = nullptr);

public:
    virtual CGameObject* Clone(void* _pArg) = 0;
    virtual void Free() override;

};

END