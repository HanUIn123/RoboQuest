#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

class CTestBuilding : public CGameObject
{
public:
    enum BUILDING_TYPE { BT_WALL , BT_BIGWALL, BT_PILLAR, BT_CONCRETE, BT_END };

public:
    BUILDING_TYPE Get_BuildingType() const { return m_eBuildingType; }
    void Set_BuildingType(BUILDING_TYPE _eType) { m_eBuildingType = _eType; }

protected:
    CTestBuilding(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CTestBuilding(const CTestBuilding& _Prototype);
    virtual ~CTestBuilding() = default;

public:
    virtual _float3                     Get_TestBuildingPosition() { return m_fPosition; }
    virtual _float3                     Get_TestBuildingScale()       const { return m_fScale; }
    virtual _float3                     Get_TestBuildingRotation() { return m_fRotation; }
    virtual _float3                     Get_TestBuildingTranslation() const { return m_fTranslation; }
    virtual void                        Set_TestBuildingScale(_float3& _vScale) { m_fScale = _vScale; }
    virtual void                        Set_TestBuildingRotation(_float3& _vRotation);
    virtual void                        Set_TestBuildingTranslation(const _float3& _vTranslation) { m_fTranslation = _vTranslation; }
    virtual void                        Set_TestBuildingPosition(_float3& _vPosition) { m_fPosition = _vPosition; }

protected:
    //_float3                             m_fScale = { 0.001f,0.001f,0.001f };
    _float3                             m_fScale = { };
    _float3                             m_fRotation = {};
    _float3                             m_fTranslation = {};
    _float3                             m_fPosition = {};

private:
    BUILDING_TYPE                        m_eBuildingType;

};

