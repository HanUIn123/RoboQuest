#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CBuilding : public CGameObject
{
public:
    enum INGAME_BUILDING_TYPE {BT_WALL, BT_BIG_WALL, BT_PILLAR, BT_CONCRETE, BT_END };

public:
    struct BUILDING_DESC : public CGameObject::GAMEOBJECT_DESC
    {
        _float3 vBuildingPos;
        _float3 vBuildingRotation;
        _float3 vBuildingScale;
    };

protected:
    CBuilding(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CBuilding(const CBuilding& _Prototype);
    virtual ~CBuilding() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;

    virtual void                Priority_Update(_float _fTimeDelta)override;
    virtual void                Update(_float _fTimeDelta)override;
    virtual void                Late_Update(_float _fTimeDelta)override;

    virtual HRESULT             Render()override;

protected:
    _float3                             m_fScale = { 0.001f,0.001f,0.001f };
    _float3                             m_fRotation = {};
    _float3                             m_fTranslation = {};
    _float3                             m_fPosition = {};

public:
    virtual CGameObject*        Clone(void* _pArg) = 0;
    virtual void                Free() override;

};

END