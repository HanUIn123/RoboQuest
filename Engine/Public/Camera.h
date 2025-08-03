#pragma once
#include "GameObject.h"
#include "VIBuffer_MapToolTerrain.h"
#include "Mesh.h"

BEGIN(Engine)
class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
    struct SquareInfo
    {
        _float3 vCenter;
        _float3 vSize;
        _bool bPicked;
    };

public:
    struct CAMERA_DESC : public GAMEOBJECT_DESC
    {
        _float3 _vEye;
        _float3 _vAt;

        _float fFov;
        _float fNear;
        _float fFar;
    };
protected:
    CCamera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CCamera(const CCamera& _Prototype);
    virtual ~CCamera() = default;

public:
    virtual HRESULT Initialize_Prototype() override;            
    virtual HRESULT Initialize(void* _pArg)override;            

    virtual void Priority_Update(_float _fTimeDelta)override;
    virtual void Update(_float _fTimeDelta)override;
    virtual void Late_Update(_float _fTimeDelta)override;

    virtual HRESULT Render()override;

    virtual _float3    Picking_Terrain(HWND _hWnd, CVIBuffer_MapToolTerrain* _pMapToolTerrainBuffer);
    virtual _float3    Picking_Terrain_Elaborated(HWND _hWnd, CVIBuffer_MapToolTerrain* _pMapToolTerrainBuffer);


    // virtual _float3    Picking_Mesh(HWND _hWnd, CMesh* _pMeshBuffer);
    

    _float              Compute_Height(_fvector _vPosition, HWND _hWnd, CVIBuffer_MapToolTerrain* _pMapToolTerrainBuffer);

    void Set_Height(_float _fHeight) { m_fHeight = _fHeight; }


private:
    _float              m_fHeight = {};

private:
    _float  m_fFov{}, m_fNear{}, m_fFar{}, m_fAspect{};
public:
    virtual CGameObject* Clone(void* _pArg) = 0;
    virtual void Free() override;
};

END