#include "Shadow.h"
#include "Shader.h"

CShadow::CShadow(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :m_pDevice{ _pDevice }
    ,m_pContext{ _pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CShadow::Initialize()
{
    return S_OK;
}

HRESULT CShadow::SetUp_ShadowLight(_fvector _vEye, _fvector _vAt, _float _fLightAngle, _float _fAspect, _float _fNear, _float _fFar)
{
    // XMVectorSet(0.0, 1.0 , 0.0 , 0.0 ) 한 게, 사실 Light View 광원을 의미함 광원의 위치라고 보면되겟음. ㅇㅅㅇ 
    XMStoreFloat4x4(&m_LightViewMatrix, XMMatrixLookAtLH(_vEye, _vAt, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
    XMStoreFloat4x4(&m_LightProjMatrix, XMMatrixPerspectiveFovLH(_fLightAngle, _fAspect, _fNear, _fFar));
    return S_OK;
}

HRESULT CShadow::Bind_Matrices(CShader* _pShader, const _char* _pConstViewMatrixName, const _char* _pConstProjMatrixName)
{
    if (FAILED(_pShader->Bind_Matrix(_pConstViewMatrixName, &m_LightViewMatrix)))
        return E_FAIL;

    if (FAILED(_pShader->Bind_Matrix(_pConstProjMatrixName, &m_LightProjMatrix)))
        return E_FAIL;

    return S_OK;
}

CShadow* CShadow::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CShadow* pInstance = new CShadow(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed To Created : CShadow");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CShadow::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
