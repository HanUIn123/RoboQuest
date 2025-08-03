#pragma once
#include "Client_Defines.h"
//#include "GameObject.h"

#include "MapToolMonster.h"
BEGIN(Engine)
class CShader;
class CModel;
//class CNavigation;
END

BEGIN(Client)
class CTestMonster final : public CMapToolMonster
{
public:
    struct TESTMONSTER_DESC : public CMapToolMonster::MAPTOOL_MONSTER_DESC
    {
        //_float3 vMonsterpos;
        //_float3 vMonsterRotation;
        //_float3 vMonsterScale;


    };


private:
    CTestMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CTestMonster(const CTestMonster& _Prototype);
    virtual ~CTestMonster() = default;

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
    //CNavigation*                m_pNavigationCom = { nullptr };


public:
  /*  _float3                     Get_TestMonsterPosition()     {return m_fPosition;}
    _float3                     Get_TestMonsterScale()       const { return m_fScale; }
    _float3                     Get_TestMonsterRotation()     { return m_fRotation; }
    _float3                     Get_TestMonsterTranslation() const { return m_fTranslation; }
    void                        Set_TestMonsterScale(_float3& _vScale) { m_fScale = _vScale; }
    void                        Set_TestMonsterTranslation(const _float3& _vTranslation) { m_fTranslation = _vTranslation; }
    void                        Set_TestMonsterPosition(_float3& _vPosition) { m_fPosition = _vPosition; }*/
    //void                        Set_TestMonsterRotation(_float3& _vRotation);
private:
    //_float3                      m_fScale = {0.001f,0.001f,0.001f};
    //_float3                      m_fRotation = {};
    //_float3                      m_fTranslation = {};
    //_float3                      m_fPosition = {};

public:
    HRESULT                     Ready_Component();
    HRESULT                     Bind_ShaderResource();

public:
    static CTestMonster* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void                Free() override;
};
END
