#pragma once
#include "Client_Defines.h"
#include "MapToolMonster.h"

BEGIN(Engine)
class CShader;
class CModel;
END


BEGIN(Client)
class CTestGoliath final: public CMapToolMonster
{
public:
    struct TESTGOLIATH_DESC : public CMapToolMonster::MAPTOOL_MONSTER_DESC
    {
        //_float3 vMonsterpos;
        //_float3 vMonsterRotation;
        //_float3 vMonsterScale;
    };

private:
    CTestGoliath(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CTestGoliath(const CTestGoliath& _Prototype);
    virtual ~CTestGoliath() = default;

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
    static CTestGoliath*        Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*        Clone(void* _pArg) override;
    virtual void                Free() override;
};
END