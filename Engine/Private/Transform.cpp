#include "../Public/Transform.h"
#include "Shader.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CComponent{ _pDevice, _pContext }
{
}

//CTransform::CTransform(const CTransform& _Prototype)
//    //: CComponent{ _Prototype.m_pDevice, _Prototype.m_pContext }
//    : CComponent(_Prototype)
//    , m_WorldMatrix{ _Prototype.m_WorldMatrix }
//{
//}

HRESULT CTransform::Initialize_Prototype()
{
    // transform ���� ������, ���� ����� �׵���ķ� �����ߴ�. 
    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

    return S_OK;
}

HRESULT CTransform::Initialize(void* _pArg)
{
    TRANSFORM_DESC* pDesc = static_cast<TRANSFORM_DESC*>(_pArg);

    if (nullptr == pDesc)
        return S_OK;

    m_fSpeedPerSec = pDesc->fSpeedPerSec;
    m_fRotationPerSec = pDesc->fRotationPerSec;

    return S_OK;
}

void CTransform::Go_Straight(_float _fTimeDelta, CNavigation* _pNavigation)
{
    _vector vPosition = Get_State(STATE_POSITION);
    _vector vLook = Get_State(STATE_LOOK);

    vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * _fTimeDelta;

    if (_pNavigation)
    {
        if (_pNavigation->bIsMove(vPosition))
        {
            Set_State(STATE_POSITION, vPosition);
        }
        else
        {
            _vector vSliderPosition = _pNavigation->Setting_SlidingMove(vPosition);
    
            Set_State(STATE_POSITION, vSliderPosition);
        }
    }
    else
    {
        Set_State(STATE_POSITION, vPosition);
    }
}

void CTransform::Go_Backward(_float _fTimeDelta, CNavigation* _pNavigation)
{
    _vector vPosition = Get_State(STATE_POSITION);
    _vector vLook = Get_State(STATE_LOOK);

    vPosition -= XMVector3Normalize(vLook) * m_fSpeedPerSec * _fTimeDelta;

    if (_pNavigation)
    {
        if (_pNavigation->bIsMove(vPosition))
        {
            Set_State(STATE_POSITION, vPosition);
        }
        else
        {
            _vector vSliderPosition = _pNavigation->Setting_SlidingMove(vPosition);

            Set_State(STATE_POSITION, vSliderPosition);
        }
    }
    else
    {
        Set_State(STATE_POSITION, vPosition);
    }
}                                

void CTransform::Go_Left(_float _fTimeDelta, CNavigation* _pNavigation)
{
    _vector vPosition = Get_State(STATE_POSITION);
    _vector vRight = Get_State(STATE_RIGHT);

    vPosition -= XMVector3Normalize(vRight) * m_fSpeedPerSec * _fTimeDelta;                                                                     
                           
    if (_pNavigation)
    {
        if (_pNavigation->bIsMove(vPosition))
        {
            Set_State(STATE_POSITION, vPosition);
        }
    }
    else
    {
        Set_State(STATE_POSITION, vPosition);
    }

}

void CTransform::Go_Right(_float _fTimeDelta, CNavigation* _pNavigation)
{
    _vector vPosition = Get_State(STATE_POSITION);
    _vector vRight = Get_State(STATE_RIGHT);

    vPosition += XMVector3Normalize(vRight) * m_fSpeedPerSec * _fTimeDelta;

    if (_pNavigation)
    {
        if (_pNavigation->bIsMove(vPosition))
        {
            Set_State(STATE_POSITION, vPosition);
        }
    }
    else
    {
        Set_State(STATE_POSITION, vPosition);
    }
}

void CTransform::Monster_Go(_float _fTimeDelta, CNavigation* _pNavigation)
{
    _vector vPosition = Get_State(STATE_POSITION);
    _vector vLook = Get_State(STATE_LOOK);
    _vector vOffset = { -0.1f, -0.0f, -0.1f, 0.0f };

    vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * _fTimeDelta;

    if (_pNavigation)
    {
        if (_pNavigation->bIsMove(vPosition))
        {
            Set_State(STATE_POSITION, vPosition);
        }
        else
        {
            //_vector vSliderPosition = _pNavigation->Setting_SlidingMove(vPosition);
            
            //Set_State(STATE_POSITION, vPosition);

            //Go_Backward(_fTimeDelta, _pNavigation);
            //_pNavigation

            //Set_State(STATE_POSITION, vPosition + vOffset);

        }
    }
    else
    {
        Set_State(STATE_POSITION, vPosition);
    }
}

void CTransform::LookAt(_fvector _vAt)
{
    _float3 vScale = Compute_Scaled();

    // 1. Look ���� ���ϱ�
    _vector vLook = _vAt - Get_State(STATE_POSITION);

    // 2. Right ���� ���ϱ�
    // XMVectorSet �̶�� �Լ���, ���ͷ� �ٷ� ������ִ� �Լ��ε�, �̰ɷ� 0,1,0,0 �� Y���� �Լ��� ���ؼ�, 
    // ��������� Ư���� �̿���, ���� ���س���, Look ���Ϳ� �����Ͽ� Right ���͸� ���Ѵ�.
    _vector vRight = XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), vLook);

    // Up ���� ���ϱ�
    _vector vUp = XMVector3Cross(vLook, vRight);

    // ������ ������ �ϵ�, ���̴� ���� ���� �״�� ����, scale ���� ������ �߻����� ����
    // Compute_Scaled �Լ���, ���� scale�� �����ִ� ��.
    // ���ؿ� scale�� x y z ��, ������ look right up ���͸� ����ȭ�� �κп� �����༭, ������ �Ѵ�

    Set_State(STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
    Set_State(STATE_UP, XMVector3Normalize(vUp) * vScale.y);
    Set_State(STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::Go_Bullet(_float _fTimeDelta, const _vector& _vLook, CNavigation* _pNavigation)
{
    /*
      _vector vPosition = Get_State(STATE_POSITION);
    _vector vLook = Get_State(STATE_LOOK);

    vPosition += XMVector3Normalize(vLook) * m_fSpeedPerSec * _fTimeDelta;

    //Set_State(STATE_POSITION, vPosition);

    if (nullptr == _pNavigation || true == _pNavigation->bIsMove(vPosition))
        Set_State(STATE_POSITION, vPosition);
    */



    _vector vPosition = Get_State(STATE_POSITION);
    //_vector vLook = Get_State(STATE_LOOK);

    vPosition += XMVector3Normalize(_vLook) * m_fSpeedPerSec * _fTimeDelta;

    if (nullptr == _pNavigation || true == _pNavigation->bIsMove(vPosition))
        Set_State(STATE_POSITION, vPosition);
}

void CTransform::Turn(_fvector _vAxis, _float _fTimeDelta)
{
    // ������ right look up ���͸� �����´�.
    _vector     vRight = Get_State(CTransform::STATE_RIGHT);
    _vector     vLook = Get_State(CTransform::STATE_LOOK);
    _vector     vUp = Get_State(CTransform::STATE_UP);

    // ȸ���� ���� ȸ����� ���� �����ִ� �۾�.
    // XMLoadFloat3 : float ����, fvector �÷� �ٲ��ֱ�.
    _matrix     RotationMatrix = XMMatrixRotationAxis(_vAxis, m_fRotationPerSec * _fTimeDelta);


    // ����� w������, 1 �̸� Coord , 0 �̸� Normal �ε�,
    // ���� ���� : w -> 0  (Normal) : ���� ��ȯ
    // ��ġ ���� : w -> 1  (Coord) : �� ��ȯ 
    Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
    Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
    Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));

    // �������� ����� 4�� ������ ��� ���Ϳ� ���Խ�Ű�ڴ�. 
}

void CTransform::Rotation(_fvector _vAxis, _float _fRadians)
{
    // Rotation �Լ���, �׵� ȸ�� ����� �������� ���⿡, ������ right, look, up �� �׵� ��ķ� �����;��Ѵ�. (XMVector)

    // ����������, scale ���� ������ ��Ÿ�⿡, ������ scale���� �����־� right look up ���͸� ������.
    _float3     vScaled = Compute_Scaled();

    _vector     vRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f) * vScaled.x;
    _vector     vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) * vScaled.y;
    _vector     vLook = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f) * vScaled.z;

    // ���� ȸ����ų _fRadians ���� ��ŭ.
    _matrix     RotationMatrix = XMMatrixRotationAxis(_vAxis, _fRadians);

    Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
    Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
    Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(_float _fX, _float _fY, _float _fZ)
{
    // ����� ȸ��.
// ����ǥ : ���Ҽ��� ����ǥ��� ǥ���� �� �ִ�. (������ ���� ��) 
// ���� ���Ҽ� / ���� ���Ҽ��� ����, ���� ���� �ε�, ������ ���� ��! ���̰� ���� ������ �Ǵ���.
// �׷��� ���� ���Ҽ��� �븻����� �ؼ� 1�� ���� ����, �ϸ� ���̴� �״�е� ȸ���� �� ó���Ǵ� �����

    _float3         vScaled = Compute_Scaled();

    _vector         vRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f) * vScaled.x;
    _vector         vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) * vScaled.y;
    _vector         vLook = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f) * vScaled.z;

    // �Լ��� �ù� �� - ��ġ - �� ��µ�, ���ڴ� ���� �ڹٲ����������� (Pitch �� X Yaw �� Y Roll �� Z )
    _vector         vQuaternion = XMQuaternionRotationRollPitchYaw(_fX, _fY, _fZ);

    _matrix         RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);

    Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
    Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
    Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));

}

HRESULT CTransform::Bind_ShaderResource(CShader* _pShader, const _char* _pConstantName)
{
    return _pShader->Bind_Matrix(_pConstantName, &m_WorldMatrix);
}

void CTransform::Scaling(const _float3& _vScale)
{
    // �� ���� ���� �����ͼ� 1�� �����, ������ ���ϱ�! 
    // ��¥ 1 * 3  �ؼ� 3 �� �ƴ�, 3���� �����Ѵٴ� �ǹ���.
    Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * _vScale.x);
    Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * _vScale.y);
    Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * _vScale.z);

}

_float3 CTransform::Compute_Scaled()
{
    // �� ��Ŀ� ����Ǿ� �ִ�, Right ���͸� �����ͼ� , �� ���̰� �� scale �� x�̴�. 
    // ������ ���̴� float �����ε�, Get_State�� ���ͷ� return �ϰ� �����Ƿ�, 
    // ����� ���͸� �̿��ؼ� ( 2 , 2 ,2 ,2 ) �� �Ǿ��ִٸ�, �� �߿���, �ƹ� 2�� �̾� ���ڴ�. 
    return _float3(
        XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT))),  //������ �������� �� ���̷� ä�����մ� (xmvectorgety / z ���ص���)
        XMVectorGetY(XMVector3Length(Get_State(STATE_UP))),
        XMVectorGetZ(XMVector3Length(Get_State(STATE_LOOK)))
    );
}

CTransform* CTransform::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CTransform* pInstance = new CTransform(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CTransform");
        Safe_Release(pInstance);
    }

    return pInstance;
}

//CComponent* CTransform::Clone(void* _pArg)
//{
//    CTransform* pInstance = new CTransform(*this);
//
//    if (FAILED(pInstance->Initialize(_pArg)))
//    {
//        MSG_BOX("Failed To Cloned : CTransform");
//        Safe_Release(pInstance);
//    }
//
//    return pInstance;
//}

void CTransform::Free()
{
    __super::Free();
}
