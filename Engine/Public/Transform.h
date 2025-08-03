#pragma once
#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
    //struct TRANSFORM_DESC
    //{
    //    _float      fSpeedPerSec = {};
    //    _float      fRotationPerSec = {};
    //};

    typedef struct tagTransformDesc
    {
        _float      fSpeedPerSec = {};
        _float      fRotationPerSec = {};
    }TRANSFORM_DESC;

    enum STATE {STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

private:
    CTransform(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);

    // Transform 컴포넌트는, 복제화 해서 사용하지 않고, 예를 들어, backGround 원형 객체 만들 때, 
    // 이런 저런 패킹 등의 과정을 통해 다 데이터가 준비되어 있을 것이기 때문에, 
    // background 의 원형 객체를 사본화 해서 initialize할 때, 그 때 함께 추가해버리겠다.
    //CTransform(const CTransform& _Prototype);
    virtual ~CTransform() = default;

public:
    _vector Get_State(STATE _eState)
    {
        return XMLoadFloat4x4(&m_WorldMatrix).r[_eState];
    }

    void Set_State(STATE _eState, _fvector _vState)
    {
        XMStoreFloat4(reinterpret_cast<_float4*>(&m_WorldMatrix.m[_eState]), _vState);
    }

    _matrix     Get_WorldMatrix_Inverse() const {
        return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix));
    }

    // 부모의 월드행렬에, 트랜스폼의 행렬을 담아주기 위해서 만든 월드 행렬의 주소를 가져오는 함수를 추가함.
    const _float4x4* Get_WorldMatrix_Ptr() const {
        return &m_WorldMatrix;
    }


public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize(void* _pArg) override;

public:
    void Go_Straight(_float _fTimeDelta,class CNavigation* _pNavigation = nullptr);
    void Go_Backward(_float _fTimeDelta, class CNavigation* _pNavigation = nullptr);
    void Go_Left(_float _fTimeDelta, class CNavigation* _pNavigation = nullptr);
    void Go_Right(_float _fTimeDelta, class CNavigation* _pNavigation = nullptr);
    void Monster_Go(_float _fTimeDelta, class CNavigation* _pNavigation = nullptr);
    void LookAt(_fvector _vAt);

    void Go_Bullet(_float _fTimeDelta, const _vector& _vLook, class CNavigation* _pNavigation = nullptr);

    // 실시간 회전 
    void Turn(_fvector _vAxis, _float _fTimeDelta);
    // 고정 회전
    void Rotation(_fvector _vAxis, _float _fRadians);
    // 사원수 회지ㅓㄴ
    void Rotation(_float _fX, _float _fY, _float _fZ);
    

public:
    HRESULT Bind_ShaderResource(class CShader* _pShader, const _char* _pConstantName);

public:
    void Scaling(const _float3& _vScale);
    _float3 Compute_Scaled();

    void Set_Speed(_float _fSpeed) { m_fSpeedPerSec = _fSpeed; }
    _float Get_Speed() { return m_fSpeedPerSec; }


private:
    // 저장용 행렬 
    _float4x4               m_WorldMatrix = {};
    _float                  m_fSpeedPerSec = { 0.0f };
    _float                  m_fRotationPerSec = { 0.0f };

public:
    static CTransform* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CComponent* Clone(void* _pArg) { return nullptr; }; // Transform 컴포넌트는, clone화 해서 만들지는 않을 것이라서, 지금 당장 안쓸거라, nullptr로 
    virtual void Free() override;
};

END