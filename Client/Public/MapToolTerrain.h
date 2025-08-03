#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "ImGui_Manager.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CTransform;
class CVIBuffer_Terrain;
class CVIBuffer_MapToolTerrain;
class CNavigation;
class CCollider;
END

BEGIN(Client)
class CMapToolTerrain final : public CGameObject
{
public:
    enum TERRAIN_TEXTURE { TT_DIFFUSE, TT_MASK, TT_MOUSE, /*TT_BRUSH,*/ TT_END };

public:
    struct MT_TERRAIN_DESC : public CGameObject::GAMEOBJECT_DESC
    {

    };

private:
    CMapToolTerrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CMapToolTerrain(const CMapToolTerrain& _Prototype);
    virtual ~CMapToolTerrain() = default;

public:
    virtual HRESULT                 Initialize_Prototype() override;
    virtual HRESULT                 Initialize(void* _pArg) override;



    virtual void                    Priority_Update(_float _fTimeDelta) override;
    virtual void                    Update(_float _fTimeDelta) override;
    virtual void                    Late_Update(_float _fTimeDelta) override;

    virtual HRESULT                 Render() override;

public:
    HRESULT                         Ready_Component();
    HRESULT                         Bind_ShaderResource();
    HRESULT                         Bind_Terrain_Texture(_float _fX, _float _fY);

    HRESULT                         Bind_DiffuseTextureIndex(_uint _iFirstIndex, _uint _iSecondIndex);

public:
    //_uint                           Get_TextureIndex() { return m_eTerrainTexture; }
    
    _uint                           Get_MaskTextureIndex() { return m_iMaskImageIndex; }
    void                            Set_MaskTextureIndex(_uint _iTextureIndex) { m_iMaskImageIndex = _iTextureIndex; }

    _uint                           Get_DiffuseTextureIndex() { return m_iDiffuseImageIndex; }
    void                            Set_DiffuseTextureIndex(_uint _iTextureIndex) { m_iDiffuseImageIndex = _iTextureIndex; }


    // Terrain Splatter 관련 변수 및 함수들.
public:
    void                            Set_TerrainPickPos(_float3 _fPickPos, _float _fRange, _bool _bPicked, _uint _iTextureTypeNum = 0)
    {
        m_fPickPos = _fPickPos,
            m_fRange = _fRange,
            m_bPicked = _bPicked;
            //m_iImageTypeNum = _iTextureTypeNum;
    }
private:
    _float                          m_fRange = {};
    _float3                         m_fPickPos = {};
    _bool                           m_bPicked = { false };
    _uint                           m_iImageTypeNumX = {};
    _uint                           m_iImageTypeNumY = {};
    // Texture Splatter 관련 변수 및 함수들.
public:
    void                            Set_TextureBrush(_bool _bBrushed) { m_bBrushed = _bBrushed; }
private:
    _bool                           m_bBrushed = { false };
private:
    TERRAIN_TEXTURE                 m_eTerrainTexture = TT_END;

private:
    _uint                           m_iMaskImageIndex = {};
    _uint                           m_iDiffuseImageIndex = {};


private:
    CShader*                        m_pShaderCom = { nullptr };
    CTexture*                       m_pTextureCom[TT_END] = { nullptr };
    CVIBuffer_MapToolTerrain*       m_pVITerrainBufferCom = { nullptr };
    CNavigation*                    m_pNavigationCom = { nullptr };
    CCollider*                      m_pColliderCom = { nullptr };

public:
    static CMapToolTerrain*         Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*            Clone(void* _pArg) override;
    virtual void                    Free() override;
};

END