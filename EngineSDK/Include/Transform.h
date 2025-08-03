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

    // Transform ������Ʈ��, ����ȭ �ؼ� ������� �ʰ�, ���� ���, backGround ���� ��ü ���� ��, 
    // �̷� ���� ��ŷ ���� ������ ���� �� �����Ͱ� �غ�Ǿ� ���� ���̱� ������, 
    // background �� ���� ��ü�� �纻ȭ �ؼ� initialize�� ��, �� �� �Բ� �߰��ع����ڴ�.
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

    // �θ��� ������Ŀ�, Ʈ�������� ����� ����ֱ� ���ؼ� ���� ���� ����� �ּҸ� �������� �Լ��� �߰���.
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

    // �ǽð� ȸ�� 
    void Turn(_fvector _vAxis, _float _fTimeDelta);
    // ���� ȸ��
    void Rotation(_fvector _vAxis, _float _fRadians);
    // ����� ȸ���ä�
    void Rotation(_float _fX, _float _fY, _float _fZ);
    

public:
    HRESULT Bind_ShaderResource(class CShader* _pShader, const _char* _pConstantName);

public:
    void Scaling(const _float3& _vScale);
    _float3 Compute_Scaled();

    void Set_Speed(_float _fSpeed) { m_fSpeedPerSec = _fSpeed; }
    _float Get_Speed() { return m_fSpeedPerSec; }


private:
    // ����� ��� 
    _float4x4               m_WorldMatrix = {};
    _float                  m_fSpeedPerSec = { 0.0f };
    _float                  m_fRotationPerSec = { 0.0f };

public:
    static CTransform* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CComponent* Clone(void* _pArg) { return nullptr; }; // Transform ������Ʈ��, cloneȭ �ؼ� �������� ���� ���̶�, ���� ���� �Ⱦ��Ŷ�, nullptr�� 
    virtual void Free() override;
};

END