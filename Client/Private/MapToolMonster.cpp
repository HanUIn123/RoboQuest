#include "pch.h"
#include "MapToolMonster.h"

CMapToolMonster::CMapToolMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGameObject{_pDevice, _pContext}
{
    
}

CMapToolMonster::CMapToolMonster(const CMapToolMonster& _Prototype)
    :CGameObject(_Prototype)
{
}

void CMapToolMonster::Set_TestMonsterRotation(_float3& _vRotation)
{
    m_fRotation = _vRotation;
}

void CMapToolMonster::Free()
{
    __super::Free();
}
