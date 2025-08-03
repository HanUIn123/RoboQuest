#pragma once
#include "Client_Defines.h"
#include "PartObject.h"


BEGIN(Engine)
class CCollider;
END

BEGIN(Client)
class CBoss_Fire final : public CPartObject
{
public:
    struct BOSS_FIRE_DESC : public CPartObject::PARTOBJECT_DESC
    {
        const _float4x4* pSocketMatrix = { nullptr };
    };

public:

    //_vector Get_FirePos() const { return m_MortalPos; }
    _vector Get_FirePos() const { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }
    _vector Get_FireUp() const { return m_pTransformCom->Get_State(CTransform::STATE_UP); }

    _float4x4 Get_CombinedMatrix() const { return m_CombinedWorldMatrix; }

private:
    CBoss_Fire(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CBoss_Fire(const CBoss_Fire& _Prototype);
    virtual ~CBoss_Fire() = default;

public:
    virtual HRESULT										Initialize_Prototype() override;
    virtual HRESULT										Initialize(void* _pArg) override;

    virtual void										Priority_Update(_float _fTimeDelta) override;
    virtual void										Update(_float _fTimeDelta) override;
    virtual void										Late_Update(_float _fTimeDelta) override;

    virtual HRESULT										Render() override;
public:
    HRESULT                                             Ready_Components();

private:
    const _float4x4*                                    m_pSocketMatrix = { nullptr };
    CCollider*                                          m_pColliderCom = { nullptr };
    _vector                                             m_MortalPos = {};

private:
    _bool                                               m_bIsActive = { false };

public:
    static CBoss_Fire*                                  Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*                                Clone(void* _pArg) override;
    virtual void										Free() override;
};
END
