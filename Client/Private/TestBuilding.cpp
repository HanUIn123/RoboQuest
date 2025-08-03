#include "pch.h"
#include "TestBuilding.h"

CTestBuilding::CTestBuilding(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGameObject{_pDevice, _pContext}
{
}

CTestBuilding::CTestBuilding(const CTestBuilding& _Prototype)
    :CGameObject(_Prototype)
{
}

void CTestBuilding::Set_TestBuildingRotation(_float3& _vRotation)
{
    m_fRotation = _vRotation;
}
