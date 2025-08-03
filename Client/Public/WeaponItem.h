#pragma once
#include "Client_Defines.h"
#include "Item.h"

BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)
class CWeaponItem final : public CItem
{
public:
    struct WEAPON_ITEM_DESC : public CItem::ITEM_DESC
    {
        const _uint* pParentState = { nullptr };

        _uint     iMaxAmmo = 30;         
        _float    fDamage = 10.0f;       
        _float    fRange = 50.0f;       

        _vector     vPosition;

    };

    enum WEAPON_STATE {
        STATE_IDLE = 0x00000001
        //STATE_SHOOT = 0x00000002,
        //STATE_IDLE = 0x00000003,
    };

private:
    CWeaponItem(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CWeaponItem(const CWeaponItem& _Prototype);
    virtual ~CWeaponItem() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;
    virtual void                Priority_Update(_float _fTimeDelta) override;
    virtual void                Update(_float _fTimeDelta) override;
    virtual void                Late_Update(_float _fTimeDelta) override;
    virtual HRESULT             Render() override;

    virtual void                Use(class CPlayer* _pPlayer) override;

    void                        Set_Position();

    _uint                       Get_WeaponTypeIndex() { return m_iWeaponTypeIndex; }

    void                        Set_Position(_vector _vPosition);

private:
    CShader*                    m_pShaderCom = { nullptr };
    CModel*                     m_pModelCom = { nullptr };
    CCollider*                  m_pColliderCom = { nullptr };

private:
    _uint                       m_iMaxAmmo = 0;           
    _uint                       m_iCurrentAmmo = 0;       
    _float                      m_fDamage = 0.0f;         
    _float                      m_fRange = 0.0f;          
    _bool                       m_bEquipped = false;     
private:
    const _uint*                m_pParentState = { nullptr };

    _uint                       m_iWeaponTypeIndex = {};

    _uint                       m_iState;

public:
    HRESULT                     Ready_Components();
    HRESULT                     Bind_ShaderResources();

public:
    static CWeaponItem*         Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*        Clone(void* _pArg) override;
    virtual void                Free() override;
};
END
