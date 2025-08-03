#pragma once
#include "Client_Defines.h"
#include "Missile.h"

BEGIN(Engine)
class CRectangle_Cube;
class CShader;
class CTexture;
class CCollider;
END

BEGIN(Client)
class CLaser_Larva final : public CMissile
{
public:
    struct LASER_DESC : public CGameObject::GAMEOBJECT_DESC
    {

    };

public:
    _vector                         Get_LaserUp() { return m_pTransformCom->Get_State(CTransform::STATE_UP); }
    _vector                         Get_StartPosition() const { return m_vLaser_StartPosition; }
    _vector                         Get_Direction() const { return m_vLaser_Direction; }
    _float                          Get_Speed() const { return m_fSpeed; }
    _float                          Get_MaxRange() const { return m_fMaxRange; }
    _float                          Get_FlightTime() const { return m_fFlightTime; }
    void                            Add_FlightTime(_float _fTimeDelta) { m_fFlightTime += _fTimeDelta; }
    void                            Set_Position(const _vector& vPosition) { m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition); }

private:
    CLaser_Larva(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CLaser_Larva(const CLaser_Larva& _Prototype);
    virtual ~CLaser_Larva() = default;

public:
    virtual HRESULT                     Initialize_Prototype() override;
    virtual HRESULT                     Initialize(void* _pArg) override;

    virtual void                        Priority_Update(_float _fTimeDelta)override;
    virtual void                        Update(_float _fTimeDelta) override;
    virtual void                        Late_Update(_float _fTimeDelta)override;

    virtual HRESULT                     Render()override;


    void                                Activate_Laser(const _vector& _vPosition, const _vector& _vDirection);

    void                                Check_Collision_With_Player();
public:
    HRESULT                             Ready_Component();
    HRESULT                             Bind_ShaderResource();


    void                                Curve_Laser(_float _fTimeDelta);
private:
    CShader* m_pShaderCom = { nullptr };
    CRectangle_Cube* m_pVIBufferCom = { nullptr };
    CTexture* m_pTextureCom = { nullptr };
    CCollider* m_pColliderCom = { nullptr };
    CCollider* m_pTargetCollider = { nullptr };




private:
    _float m_fCurveWidth = {};  // �־����� �� ���α��� ��������.
    _float m_fCurveHeight = {}; // �־����� �� ���α��� ��������.

private:
    _vector m_vLaser_StartPosition;         // ���� ��ġ
    _vector m_vLaser_Direction;             // ������ ����
    _float m_fSpeed = {};                // ������ �ӵ�
    _float m_fFlightTime = {};            // ���� �ð�
    _float m_fMaxRange = {};             // �ִ� ��Ÿ�

public:
    static CLaser_Larva* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void    Free() override;
};
END