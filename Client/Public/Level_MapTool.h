      #pragma once
#include "Client_Defines.h"
#include "Level.h"
#include "Monster.h"
#include "ImGui_Manager.h"
#include "Camera_MapTool.h"
#include "MapToolTerrain.h"
#include "VIBuffer_MapToolTerrain.h"


#include "TestMonster.h"
#include "TestNorRobo.h"
#include "TestGoliath.h"
#include "TestLarva.h"
#include "TestSmall.h"
#include "TestDoor.h"
#include "TestBoss.h"
#include "TestBox.h"
#include "TestGate.h"

#include "MapToolMonster.h"


#include "TestBuilding.h"
#include "TestWall.h"


#include "TempCollider.h"
#include "Navigation.h"
#include "Cell.h"


#include "Load_Manager.h"

BEGIN(Engine)
class CShader;
class CNavigation;
END

BEGIN(Client)
class CLevel_MapTool final : public CLevel
{
    struct CELL_POINTS
    {
        _float3 fCellPoints[3];
        _float3 fPrevPoints[3];
    };

public:
    enum IMGUI_TEXTURE_TYPE { IMG_MONSTER, IMG_BUILDING, IMG_DIFFUSE, IMG_MASK, IMG_END };
    enum PICKING_TYPE { PT_MONSTER, PT_BUILDING, PT_TERRAIN_PART, PT_SPLATTING_TEXTURE, PT_CHOOSE_DIFFUSETEXTURE, PT_CHOOSE_MASKTEXTURE, PT_CELL, PT_COLLIDER, PT_END };

private:
    CLevel_MapTool(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CLevel_MapTool() = default;

public:
    virtual HRESULT                     Initialize() override;
    virtual void                        Update(_float _fTimeDelta) override;
    virtual HRESULT                     Render() override;

public:
    _float                              Get_HeightY() { return m_fHeightY; }

private:
    HRESULT                             Ready_Layer_MapToolTerrain(const _tchar* _pLayerTags);
    HRESULT                             Ready_Light(const _tchar* _pLayerTags); 
    HRESULT                             Ready_Layer_MapToolCamera(const _tchar* _pLayerTags);

    HRESULT                             Ready_Layer_MapToolMonster(const _tchar* _pLayerTags, CMapToolMonster::MONSTER_TYPE _eMonsterType, _uint _iLoadedDataTypeIndex, void* _pMonsterArg = nullptr);
    HRESULT                             Ready_Layer_MapToolBuilding(const _tchar* _pLayerTags, CTestBuilding::BUILDING_TYPE _eBuildingType, _uint _iLoadedDataTypeIndex, void* _pBuildingArg = nullptr);

    HRESULT                             Ready_Layer_TempCollider(const _tchar* _pLayerTags);

private:
    // MapTool 용 파일 입출력 관련 함수. 
    void                                OpenFileDialoge(const _tchar* _pDefaultFileName, const _tchar* _pFilter, std::wstring& outFileName);
    HRESULT                             Save_HeightMap(/*const _tchar* _strMapFileName*/);
    HRESULT                             Save_Level(const _tchar* _strMapFileName);
    HRESULT                             Save_MonsterData();
    HRESULT                             Save_BuildingData();
    HRESULT                             Save_Navi();

    //HRESULT                             Load_HeightMap();
    HRESULT                             Load_MonsterData();
    HRESULT                             Load_BuildingData();
    HRESULT                             Load_Navi();

   // HRESULT                             Load_WholeData(_uint _iStageIndex = 0);
    _uint                               m_iLoadIndex = 0;
    _bool                               m_bLoadComplete = { false };

    // Monster 등 Object 피킹용 함수. 
    HRESULT                             Pick_Monster(PICKING_TYPE _ePTType);
    // Object Pick 할 때, 자리 중복해서 생성되는 거 피하려고 만든 벡터멤버변수.
    // 피킹하고 피킹한 좌표와 이게 같아버리면, 생성이 안됨.


    HRESULT                             Pick_Building(PICKING_TYPE _ePTType);


    _bool                               Adjust_MaskImage();

    _bool                               Select_TerrainTexture(_uint _iFirstDiffuseIndex);

private:
    vector<_float3>                     m_vecCheckPosition;
    vector<_float3>                     m_vecCheckBuildingPosition;
    _uint                               m_iMonsterTypeIndex = {};
    _uint                               m_iBuildingTypeIndex = {};

    _uint                               m_iMaskImageIndex = {};
    _uint                               m_iDiffuseImageFirstIndex = {};
    _uint                               m_iDiffuseImageSecondIndex = {};

    // MapTool 에서 Terrain 에 브러시 영역 체크 용 함수.
    HRESULT                             Show_MouseRange(PICKING_TYPE _ePTType, _float _fTimeDelta);

    // MapTool ImGui Window 에 보여줄, 그림 설정.
    HRESULT                             Register_PreviewImage_In_ImGuiWindow(const _tchar* _pImageFilePath, IMGUI_TEXTURE_TYPE _eImguiTextureType, _uint _iTextureNumber);
    vector<ID3D11ShaderResourceView*>   m_vecMonsterSRVs;
    vector<ID3D11ShaderResourceView*>   m_vecBuildingSRVs;
    vector<ID3D11ShaderResourceView*>   m_vecMaksImageSRVs;
    vector<ID3D11ShaderResourceView*>   m_vecDiffuseImageSRVs;
    void                                Setting_MonsterList();
    void                                Setting_BuildingList();
    void                                Setting_MaskImages();
    void                                Setting_DiffuseImages();

private:
    // MapTool 에서 Navigation Mesh 설정하려고 만든 변수들과 함수... 
    HRESULT                             Picking_Points();

    //HRESULT                             Load_Navi();
    _float                              Compute_Cell_Distance(const XMFLOAT3& _NewPickingPoint, const XMFLOAT3& _PrevPickedPoint);
    _bool                               Is_CWTriangle(const XMVECTOR& _NearestCellPoint1, const XMVECTOR& _NearestCellPoint2, const XMVECTOR& _PickedNewPoint);
    pair<XMFLOAT3, XMFLOAT3>            Compute_NearPoints(const vector<CELL_POINTS>& _vecTagCells, const XMFLOAT3& _newPoints);
    pair<XMFLOAT3, _bool>                Find_NearestPoint(const vector<CELL_POINTS>& _vecTagCells, const XMFLOAT3& _newPoint);
    _bool                               Is_Point_InTriangle(const XMVECTOR& _Point, const XMVECTOR& _VertexPoint0, const XMVECTOR& _VertexPoint1, const XMVECTOR& _VertexPoint2);
    HRESULT                             Delete_Cell();
    HRESULT                             Delete_Cell_Mode();
    _bool                               m_bDeleteMode = { false };


private:
    vector<_float3>                     m_vecPickedPoints;
    _uint                               m_iNumCellCount = {};
    CELL_POINTS                         tagWholeCellPoints = {};
    vector<CELL_POINTS>                 m_vecWholeCellPoints;
    _bool                               bFirstPick = { true };

private:
    _bool                               m_bImGuiHovered = { false };
    _int                                m_iPickingType = {};
private:
    _bool                               m_bPickingMonsterActiveOn = false;
    _bool                               m_bPickingBuildingActiveOn = false;
    _bool                               m_bPickingTerrainActiveOn = false;
    _bool                               m_bPickingTextureActiveOn = false;
    _bool                               m_bPickingDiffuseTextureActiveOn = false;
    _bool                               m_bPickingMaskTexActiveOn = false;
    _bool                               m_bPickingCellActiveOn = false;
private:
    CCamera_MapTool*                    m_pMapToolCamera = { nullptr };
    CMapToolTerrain*                    m_pMapToolTerrain = { nullptr };
    CVIBuffer_MapToolTerrain*           m_pMapToolBuffer = { nullptr };
    CNavigation*                        m_pNavigation = { nullptr };





private:
    PICKING_TYPE                        m_ePickingType = { PT_END };
    _float3                             m_fPickPos = {};
    _float3                             m_fRotation = {};
    _float3                             m_fScale = {1.0f, 1.0f ,1.0f };
    _float3                             m_fTranslation = {-20.0f, 0.0f, 0.0f};
    _float                              m_fRange = {};
    _uint                               m_iNumMapToolTerrainNumVerticesX = { 0 };
    _uint                               m_iNumMapToolTerrainNumVerticesZ = { 0 };

    void                                RaiseUp_Terrain(_float _fTimeDelta);
    void                                RaiseDown_Terrain(_float _fTimeDelta);
    void                                Brushing_Terrain();

    void                                Set_TerrainTexture(_float _fX, _float _fY);
    void                                Show_MonsterList();
    void                                Show_BuildingList();

private:
    _float                              m_fTextureIndex = {};
    _bool                               m_bPicked = { false };
    _bool                               m_bBrushed = { false };
    _uint                               m_SelectedObjectIndex = {};
    vector<CGameObject*>                m_vecTestMonster;
    vector<CGameObject*>                m_vecTestBuilding;

private:
    ID3D11Texture2D*                    m_pMaskTexture = nullptr;              
    ID3D11ShaderResourceView*           m_pMaskSRV = nullptr;
public:
    _float                              m_fHeightY = {};
private:
    CTestMonster*                       m_pMonster = { nullptr };
    CTestNorRobo*                       m_pMonsterNorRobo = { nullptr };
    CGameObject*                        m_pMonsterGameObject = { nullptr };
    CGameObject*                        m_pBuildingGameObject = { nullptr };
    CMapToolMonster*                    m_pMapToolMonster= { nullptr };
    CTestBuilding*                      m_pMapToolBuilding = { nullptr };


    CTestWall*                          m_pTestWall = { nullptr };
private:
    CTempCollider*                      m_pTempCollider = { nullptr };
    vector<CTempCollider*>              m_vecTempCollider;

private:
    VTXNORTEX*                          m_pVertices = { nullptr };

public:
    static CLevel_MapTool*              Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual void                        Free() override;

};

END