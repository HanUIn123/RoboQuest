#pragma once
#include "Client_Defines.h"
#include "PartObject.h"

BEGIN(Engine)
class CCollider;
END

BEGIN(Client)
class CMortal final : public CPartObject
{
public:
    struct MORTAL_DESC : public CPartObject::PARTOBJECT_DESC
    {
        const _float4x4* pSocketMatrix = { nullptr };
    };

public:

    //_vector Get_MortalPos() const { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }
    _vector Get_MortalPos() const { return m_MortalPos; }
    _vector Get_MortalUp() const { return m_pTransformCom->Get_State(CTransform::STATE_UP); }
    //_vector Get_MortalLook() const {return}

    _float4x4 Get_CombinedMatrix() const { return m_CombinedWorldMatrix; }

    //const _float4x4* Get_SocketMatrix() { return m_pSocketMatrix; }
    //const _float4x4* Get_ParentWorldMatrix() { return m_pParentWorldMatrix; }

private:
    CMortal(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CMortal(const CMortal& _Prototype);
    virtual ~CMortal() = default;

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
    CCollider* m_pColliderCom = { nullptr };
    _vector                                               m_MortalPos = {};

public:
    static CMortal*                                     Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*                                Clone(void* _pArg) override;
    virtual void										Free() override;

};

END