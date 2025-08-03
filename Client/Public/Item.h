#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
//#include "Player.h"

BEGIN(Client)
class CPlayer;
class CItem : public CGameObject
{
public:
    enum ITEM_TYPE {ITEM_WEAPON,ITEM_HEALTH,ITEM_AMMO};

public:
    struct ITEM_DESC : public CGameObject::GAMEOBJECT_DESC
    {
        ITEM_TYPE   eItemType = ITEM_WEAPON; 
        _uint       iItemID = 0;            
        _float3     vScale = { 1.0f, 1.0f, 1.0f }; 
        _bool       bCollected = false;    
        _uint       iUseCount = 1;            
        CPlayer*    pPlayer = nullptr; 
        _vector     vPosition;
      
    };

protected:
    CItem(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CItem(const CItem& _Prototype);
    virtual ~CItem() = default;
        
public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;

    virtual void                Priority_Update(_float _fTimeDelta)override;
    virtual void                Update(_float _fTimeDelta)override;
    virtual void                Late_Update(_float _fTimeDelta)override;

    virtual HRESULT             Render()override;

    virtual void                Use(class CPlayer* _pPlayer) = 0; 

    _bool                       IsCollected() const { return m_bCollected; }
    void                        SetCollected(_bool _bCollected) { m_bCollected = _bCollected; }

    _vector                     Get_ItemPos() { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }

protected:
    ITEM_TYPE                   m_eItemType = ITEM_WEAPON;
    _bool                       m_bCollected = false;
    CPlayer*                    m_pPlayer = nullptr;   

public:
    virtual CGameObject*        Clone(void* _pArg) = 0;
    virtual void                Free() override;
};

END
