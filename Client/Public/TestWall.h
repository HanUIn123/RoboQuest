#pragma once
#include "Client_Defines.h"
#include "TestBuilding.h"
#include "Load_Manager.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CTestWall final : public CTestBuilding
{
public:
    enum WALL_TYPE {WT_BASIC, WT_BIG, WT_PILLAR, WT_CONCRETE, WT_END};

public:
    struct TEST_WALL_DESC : public CGameObject::GAMEOBJECT_DESC
    {
        _float3 vBigWallScale;
        _float3 vBigWallRotation;
        _float3 vBigWallPos;
        WALL_TYPE eWallType;

        _bool       bLoaded = false;
    };

public:
    void        Set_WallType(WALL_TYPE _eWallType) { m_eWallType = _eWallType; }

private:
    CTestWall(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CTestWall(const CTestWall& _Prototype);
    virtual ~CTestWall() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;
    virtual void                Priority_Update(_float _fTimeDelta)override;
    virtual void                Update(_float _fTimeDelta)override;
    virtual void                Late_Update(_float _fTimeDelta)override;
    virtual HRESULT             Render()override;

    void                        Set_WallType(_uint _iType) { m_iModelTypeIndex = _iType; }

private:
    CShader*                    m_pShaderCom = { nullptr };
    CModel*                     m_pModelCom = { nullptr };

    WALL_TYPE                   m_eWallType = WT_END;
    _uint                       m_iModelTypeIndex = {};
public:
    HRESULT                     Ready_Component(WALL_TYPE _eType);
    HRESULT                     Bind_ShaderResource();

public:
    static CTestWall*           Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*        Clone(void* _pArg) override;
    virtual void                Free() override;
};

END