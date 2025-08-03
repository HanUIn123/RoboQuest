#pragma once
#include "Client_Defines.h"
#include "PartObject.h"
#include "Camera.h"

class CPlayer_Camera final : public CPartObject
{
public:
    struct PLAYER_CAMERA_DESC : public CPartObject::PARTOBJECT_DESC
    {
        _float fFov;
        _float fNear;
        _float fFar;
        //const _float4x4* pParentWorldMatrix;
    };

public:
    struct Ray 
    {
        _vector vStartPosition;
        _vector vRayDirection;
    };

public:
    const XMMATRIX Get_ViewMatrix() const;

    const XMMATRIX Get_ProjMatrix() const;

public:
    _vector Get_LookVector() const { return XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)); }
    _vector Get_RightVector() const { return XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)); }
    _vector Get_UpVector() const { return XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)); }
    _vector Get_PlayerCameraPos() const { return m_pTransformCom->Get_State(CTransform::STATE_POSITION); }

private:
    explicit CPlayer_Camera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    explicit CPlayer_Camera(const CPlayer_Camera& _Prototype);
    virtual ~CPlayer_Camera() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* _pArg) override;
    virtual void Priority_Update(_float _fTimeDelta) override;
    virtual void Update(_float _fTimeDelta) override;
    virtual void Late_Update(_float _fTimeDelta) override;
    virtual HRESULT Render() override;

    _bool IsActive() { return m_bActive; }
    void Set_Active(_bool _bActive) { m_bActive = _bActive; }
    Ray Create_Ray();

    void        Set_CameraPosition(_vector _vPosition, _vector _vDirection);
    void        Set_Fov(_float _fFov) { m_fFov = _fFov; }

    void        Update_Camera(_float _fCurrentFov, _float _fLastFov, _float _fTimeDelta, _float _fSpeed);

    void        Start_CameraShake(_float _fDuration, _float _fIntensity,_float _fFrequency);

private:
    _float                                  m_fFov = {};
    _float                                  m_fNear = {};
    _float                                  m_fFar = {};
    _float                                  m_fAspect = {};
    //const _float4x4*                        m_pParentWorldMatrix = { nullptr };
    //_float4x4                               m_CombinedWorldMatrix = { nullptr };
    _bool                                   m_bActive = { false };

    _bool m_bShakeActive = false;
    _float m_fShakeDuration = 0.0f;
    _float m_fShakeTimer = 0.0f;
    _float m_fShakeIntensity = 0.1f;
    _float m_fShakeFrequency = 20.0f; // Ω¶¿Ã≈∑ ¡÷±‚
    _float m_fShakeTimeElapsed = 0.0f;




public:
    static CPlayer_Camera* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void Free() override;
};

