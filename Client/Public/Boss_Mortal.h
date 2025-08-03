#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)
class CBoss_Mortal final : public CPartObject
{
public:
    struct BOSS_MORTAL_DESC : public CPartObject::PARTOBJECT_DESC
    {
        const _float4x4* pSocketMatrix = { nullptr };
    };

public:

    _vector Get_MortalPos() const { return m_MortalPos; }
    _vector Get_MortalUp() const { return m_pTransformCom->Get_State(CTransform::STATE_UP); }

    _float4x4 Get_CombinedMatrix() const { return m_CombinedWorldMatrix; }

private:
    CBoss_Mortal(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CBoss_Mortal(const CBoss_Mortal& _Prototype);
    virtual ~CBoss_Mortal() = default;

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

public:
    static CBoss_Mortal*                                Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*                                Clone(void* _pArg) override;
    virtual void										Free() override;

};
END
