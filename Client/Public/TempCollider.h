#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Cube;
class CShader;
END

BEGIN(Client)
class CTempCollider final: public CGameObject
{
public:
    struct TEMP_COLLIDER_DESC : public CGameObject::GAMEOBJECT_DESC
    {
        _float3 vTempColliderpos;
        _float3 vTempColliderScale;
    };

public:
    const XMFLOAT3& Get_Position() const {
        return m_vPosition;
    }

    void Set_Position(const XMFLOAT3& _vPosition) {
        m_vPosition = _vPosition;
    }

private:
    CTempCollider(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CTempCollider(const CTempCollider& _Prototype);
    virtual ~CTempCollider() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;

    virtual void                Priority_Update(_float _fTimeDelta)override;
    virtual void                Update(_float _fTimeDelta)override;
    virtual void                Late_Update(_float _fTimeDelta)override;

    virtual HRESULT             Render()override;

private:
    CVIBuffer_Cube*             m_pVIBufferCom = { nullptr };
    CShader*                    m_pShaderCom = { nullptr };

private:
    XMFLOAT3                    m_vPosition = {};

public:
    HRESULT                     Ready_Component();
    HRESULT                     Bind_ShaderResources();
public:
    static CTempCollider*       Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject*        Clone(void* _pArg) override;
    virtual void                Free() override;

};

END