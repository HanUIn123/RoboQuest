#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

class CMapToolMonster : public CGameObject
{
public:
    struct MAPTOOL_MONSTER_DESC: public CGameObject::GAMEOBJECT_DESC
    {
        _float3 vMonsterpos;
        _float3 vMonsterRotation;
        _float3 vMonsterScale;
    };

public:
    enum MONSTER_TYPE { MT_ROBO_L, MT_ROBO_B, MT_ROBO_G, MT_ROBO_V, MT_ROBO_S, MT_DOOR, MT_ROBO_BOSS, MT_ITEM_BOX, MT_GATE, MT_ROBO_END };

public:
    MONSTER_TYPE Get_MonsterType() const {return m_eMonsterType;}
    void Set_MonsterType(MONSTER_TYPE _eType) { m_eMonsterType = _eType; }

protected:
    CMapToolMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CMapToolMonster(const CMapToolMonster& _Prototype);
    virtual ~CMapToolMonster() = default;


public:
    virtual _float3                     Get_TestMonsterPosition() { return m_fPosition; }
    virtual _float3                     Get_TestMonsterScale()       const { return m_fScale; }
    virtual _float3                     Get_TestMonsterRotation() { return m_fRotation; }
    virtual _float3                     Get_TestMonsterTranslation() const { return m_fTranslation; }
    virtual void                        Set_TestMonsterScale(_float3& _vScale) { m_fScale = _vScale; }
    virtual void                        Set_TestMonsterRotation(_float3& _vRotation);
    virtual void                        Set_TestMonsterTranslation(const _float3& _vTranslation) { m_fTranslation = _vTranslation; }
    virtual void                        Set_TestMonsterPosition(_float3& _vPosition) { m_fPosition = _vPosition; }

protected:
    _float3                             m_fScale = { 0.001f,0.001f,0.001f };
    _float3                             m_fRotation = {};
    _float3                             m_fTranslation = {};
    _float3                             m_fPosition = {};


private:
    MONSTER_TYPE                        m_eMonsterType;

public:
    virtual CGameObject*                Clone(void* _pArg) = 0;
    virtual void                        Free() override;

};

