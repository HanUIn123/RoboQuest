#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)
class CLevelChangeCollider final : public CGameObject
{
    enum LC_COLLIDERTYPE { LC_COLL_AABB, LC_COLL_OBB, LC_COLL_SPHERE, LC_COLL_END };
public:
    struct LC_DESC : public CGameObject::GAMEOBJECT_DESC
    {

    };

private:
    CLevelChangeCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CLevelChangeCollider(const CLevelChangeCollider& _Prototype);
    virtual ~CLevelChangeCollider() = default;

public:
    virtual HRESULT										Initialize_Prototype() override;
    virtual HRESULT										Initialize(void* _pArg) override;

    virtual void										Priority_Update(_float _fTimeDelta) override;
    virtual void										Update(_float _fTimeDelta) override;
    virtual void										Late_Update(_float _fTimeDelta) override;

    virtual HRESULT										Render() override;

public:
    HRESULT                                             Ready_Components();

    _bool                                               Check_Collision_With_Player();
private:
    CCollider*                                          m_pColliderCom = { nullptr };
public:
    static CLevelChangeCollider* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void        Free() override;
};
END