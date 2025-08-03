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
    // transform 원본 생성시, 월드 행렬을 항등행렬로 저장했다. 
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

    // 1. Look 벡터 구하기
    _vector vLook = _vAt - Get_State(STATE_POSITION);

    // 2. Right 벡터 구하기
    // XMVectorSet 이라는 함수는, 벡터로 바로 만들어주는 함수인데, 이걸로 0,1,0,0 의 Y축의 함수를 구해서, 
    // 직교행렬의 특성을 이용해, 위에 구해놨던, Look 벡터와 외적하여 Right 벡터를 구한다.
    _vector vRight = XMVector3Cross(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), vLook);

    // Up 벡터 구하기
    _vector vUp = XMVector3Cross(vLook, vRight);

    // 방향은 갱신을 하되, 길이는 기존 길이 그대로 가야, scale 관련 문제가 발생하지 않음
    // Compute_Scaled 함수로, 원래 scale을 구해주는 것.
    // 구해온 scale의 x y z 와, 각각의 look right up 벡터를 정규화한 부분에 곱해줘서, 저장을 한다

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
    // 각각의 right look up 벡터를 가져온다.
    _vector     vRight = Get_State(CTransform::STATE_RIGHT);
    _vector     vLook = Get_State(CTransform::STATE_LOOK);
    _vector     vUp = Get_State(CTransform::STATE_UP);

    // 회전을 위한 회전행렬 만들어서 곱해주는 작업.
    // XMLoadFloat3 : float 형을, fvector 꼴로 바꿔주기.
    _matrix     RotationMatrix = XMMatrixRotationAxis(_vAxis, m_fRotationPerSec * _fTimeDelta);


    // 행렬의 w성분이, 1 이면 Coord , 0 이면 Normal 인데,
    // 방향 벡터 : w -> 0  (Normal) : 벡터 변환
    // 위치 벡터 : w -> 1  (Coord) : 점 변환 
    Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
    Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
    Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));

    // 곱해지는 행렬의 4행 정보를 결과 벡터에 포함시키겠다. 
}

void CTransform::Rotation(_fvector _vAxis, _float _fRadians)
{
    // Rotation 함수는, 항등 회전 행렬을 기준으로 돌기에, 각각의 right, look, up 도 항등 행렬로 가져와야한다. (XMVector)

    // 마찬가지로, scale 관련 문제가 나타기에, 기존의 scale값을 곱해주어 right look up 벡터를 구해줌.
    _float3     vScaled = Compute_Scaled();

    _vector     vRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f) * vScaled.x;
    _vector     vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) * vScaled.y;
    _vector     vLook = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f) * vScaled.z;

    // 내가 회전시킬 _fRadians 각도 만큼.
    _matrix     RotationMatrix = XMMatrixRotationAxis(_vAxis, _fRadians);

    Set_State(STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
    Set_State(STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
    Set_State(STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));
}

void CTransform::Rotation(_float _fX, _float _fY, _float _fZ)
{
    // 사원수 회전.
// 극좌표 : 복소수를 극좌표계로 표현할 수 있다. (각도와 길이 로) 
// 원래 복소수 / 곱할 복소수의 값은, 각도 길이 인데, 각도가 둘의 합! 길이가 둘의 곱으로 되더라.
// 그래서 곱할 복소수에 노말라이즈를 해서 1로 만든 다음, 하면 길이는 그대론데 회전만 잘 처리되는 방식임

    _float3         vScaled = Compute_Scaled();

    _vector         vRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f) * vScaled.x;
    _vector         vUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) * vScaled.y;
    _vector         vLook = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f) * vScaled.z;

    // 함수는 시발 롤 - 피치 - 요 라는데, 인자는 순서 뒤바껴잇음ㅋㅋㅋ (Pitch 가 X Yaw 가 Y Roll 이 Z )
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
    // 각 행의 길이 꺼내와서 1로 만들고, 스케일 곱하기! 
    // 진짜 1 * 3  해서 3 이 아닌, 3으로 변경한다는 의미임.
    Set_State(STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * _vScale.x);
    Set_State(STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * _vScale.y);
    Set_State(STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * _vScale.z);

}

_float3 CTransform::Compute_Scaled()
{
    // 내 행렬에 저장되어 있는, Right 벡터를 꺼내와서 , 이 길이가 내 scale 의 x이다. 
    // 벡터의 길이는 float 형태인데, Get_State가 벡터로 return 하고 있으므로, 
    // 연산용 벡터를 이용해서 ( 2 , 2 ,2 ,2 ) 로 되어있다면, 이 중에서, 아무 2를 뽑아 오겠다. 
    return _float3(
        XMVectorGetX(XMVector3Length(Get_State(STATE_RIGHT))),  //벡터의 각성분은 이 길이로 채워져잇다 (xmvectorgety / z 로해도됨)
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
