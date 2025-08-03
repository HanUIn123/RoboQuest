#pragma once
#include "Client_Defines.h"
#include "Building.h"


BEGIN(Engine)
class CShader;
class CModel;
class CCollider;
END

BEGIN(Client)
class CWall : public CBuilding
{
public: 
    enum INGAME_WALL_TYPE {IG_BASIC_WALL, IG_BIG_WALL, IG_PILLAR, IG_CONCRETE, IG_END };

public:
    struct INGAME_WALL_DESC : public CGameObject::GAMEOBJECT_DESC
    {
        _float3 vWallScale;
        _float3 vWallRotation;
        _float3 vWallPos;
        INGAME_WALL_TYPE eWallType;
    };

private:
    CWall(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CWall(const CWall& _Prototype);
    virtual ~CWall() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;
    virtual void                Priority_Update(_float _fTimeDelta)override;
    virtual void                Update(_float _fTimeDelta)override;
    virtual void                Late_Update(_float _fTimeDelta)override;
    virtual HRESULT             Render()override;

    _vector                     Get_CenterPosition() const 
    { 
        return m_pTransformCom->Get_State(CTransform::STATE_POSITION);
      
    }

private:
    CShader* m_pShaderCom = { nullptr };
    CModel* m_pModelCom = { nullptr };
    CCollider* m_pColliderCom = { nullptr };

public:
    HRESULT                     Ready_Component(_uint _iWallTypeIndex);
    HRESULT                     Bind_ShaderResource();

public:
    static CWall*               Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*        Clone(void* _pArg) override;
    virtual void                Free() override;

};
END