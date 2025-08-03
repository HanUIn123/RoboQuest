#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Load_Manager.h"


BEGIN(Engine)
class CShader;
class CTexture;
class CTransform;
class CVIBuffer_Terrain;
class CNavigation;
END

BEGIN(Client)
class CBossTerrain final : public CGameObject
{
public:
    enum BOSS_TERRAIN_TEXTURE {BTT_DIFFUSE, BTT_MASK, BTT_END };

public:
    struct BOSS_TERRAIN_DESC : public CGameObject::GAMEOBJECT_DESC
    {

    };

private:
    CBossTerrain(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CBossTerrain(const CBossTerrain& _Prototype);
    virtual ~CBossTerrain() = default;

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

public:
    _float                          Compute_Height(const XMFLOAT3& _vPos);

private:
    CShader*                        m_pShaderCom = { nullptr };
    CTexture*                       m_pTextureCom[BTT_END] = { nullptr };
    CVIBuffer_Terrain*              m_pVITerrainBufferCom = { nullptr };
    CNavigation*                    m_pNavigationCom = { nullptr };

private:
    BOSS_TERRAIN_TEXTURE            m_eTerrainTexture = BTT_END;

public:
    static CBossTerrain*           Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*            Clone(void* _pArg) override;
    virtual void                    Free() override;
};
END