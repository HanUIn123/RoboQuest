#pragma once
#include "Client_Defines.h"
#include "Body_Gate.h"
#include "Monster.h"

BEGIN(Engine)
class CCollider;
class CModel;
class CShader;
END

BEGIN(Client)
class CGate final: public CMonster
{
    enum COLLIDERTYPE { COLL_AABB, COLL_OBB, COLL_SPHERE, COLL_END };

    enum GATE_STATE {
        GATE_CLOSE = 0x00000001,
        GATE_OPEN = 0x00000002,
        GATE_UNLOCK = 0x00000003,
        GATE_UNLOCK2 = 0x00000004,
        GATE_UNLOCK3 = 0x00000005,
        GATE_END = 0x00000006
    };

public:
    struct GATE_DESC final : public CGameObject::GAMEOBJECT_DESC
    {
        _float3 vMonsterpos;
        _float3 vMonsterRotation;
        _float3 vMonsterScale;
    };

private:
    CGate(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CGate(const CGate& _Prototype);
    virtual ~CGate() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;

    virtual void                Priority_Update(_float _fTimeDelta)override;
    virtual void                Update(_float _fTimeDelta)override;
    virtual void                Late_Update(_float _fTimeDelta)override;

    virtual HRESULT             Render()override;

public:
    _bool                       Check_Collision_With_Ray(const _vector& _vRayStart, const _vector& _vRayDirection, _float& _fDistance);
    void                        Be_Hited(_uint _iCurrentGunDamage);

private:
    CCollider*                  m_pColliderCom[COLL_END] = { nullptr };

public:
    virtual HRESULT             Ready_Component();
    virtual HRESULT             Ready_PartObjects();

    //void Activate_Boss();
private:
    _uint						m_iState = {};
    CBody_Gate*                 m_pBody_Gate = { nullptr };

    //_bool                       m_bBossSpawn = { false };


private:
    _bool m_bShowInteractionText = false; // 텍스트 표시 여부
    _float3 m_vTextPosition = {};        // 텍스트 표시 위치
    _float m_fTextScreenTime = 1.0f;     // 텍스트 화면에 표시 시간 (옵션)


public:
    static CMonster*            Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*        Clone(void* _pArg) override;
    virtual void                Free() override;
};
END
