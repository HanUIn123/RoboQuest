#pragma once
#include "Client_Defines.h"
#include "MapToolMonster.h"

BEGIN(Engine)
class CShader;
class CModel;
END

BEGIN(Client)
class CTestSmall final : public CMapToolMonster
{
public:
    struct TESTSMALL_DESC : public CMapToolMonster::MAPTOOL_MONSTER_DESC
    {
        //_float3 vMonsterpos;
        //_float3 vMonsterRotation;
        //_float3 vMonsterScale;
    };

private:
    CTestSmall(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CTestSmall(const CTestSmall& _Prototype);
    virtual ~CTestSmall() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;

    virtual void                Priority_Update(_float _fTimeDelta)override;
    virtual void                Update(_float _fTimeDelta)override;
    virtual void                Late_Update(_float _fTimeDelta)override;

    virtual HRESULT             Render()override;

private:
    CShader* m_pShaderCom = { nullptr };
    CModel* m_pModelCom = { nullptr };

public:
    HRESULT                     Ready_Component();
    HRESULT                     Bind_ShaderResource();

public:
    static CTestSmall* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void                Free() override;
};

END