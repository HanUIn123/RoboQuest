#pragma once
#include "Client_Defines.h"
#include "MapToolMonster.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CTestNorRobo final : public CMapToolMonster
{
public:
    struct TEST_NOR_ROBO : public CMapToolMonster::MAPTOOL_MONSTER_DESC
    {
        //_float3 vMonsterpos;
        //_float3 vMonsterRotation;
        //_float3 vMonsterScale;
    };


private:
    CTestNorRobo(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CTestNorRobo(const CTestNorRobo& _Prototype);
    virtual ~CTestNorRobo() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;

    virtual void                Priority_Update(_float _fTimeDelta)override;
    virtual void                Update(_float _fTimeDelta)override;
    virtual void                Late_Update(_float _fTimeDelta)override;

    virtual HRESULT             Render()override;

private:
    CShader*                    m_pShaderCom = { nullptr };
    CModel*                     m_pModelCom = { nullptr };

public:
    HRESULT                     Ready_Component();
    HRESULT                     Bind_ShaderResource();

public:
    static CTestNorRobo*        Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*        Clone(void* _pArg) override;
    virtual void                Free() override;
};
END
