#include "../Public/Light.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

CLight::CLight(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice{_pDevice}
    , m_pContext{_pContext}
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);

}

HRESULT CLight::Initialize(const LIGHT_DESC& _tagLightDesc)
{
    m_tagLightDesc = _tagLightDesc;

    return S_OK;
}

void CLight::Render(CShader* _pShader, CVIBuffer_Rect* _pVIBuffer)
{
    _uint iPassIndex = {};

    if (LIGHT_DESC::LT_DIRECTIONAL == m_tagLightDesc.eState)
    {
        // 1 번 패스이면 방향성 광원을 쓰겠다는 소리라서, 그 때 g_vLightDir 빛 방향 ㄱㄱ 함. 
        iPassIndex = 1;

        if (FAILED(_pShader->Bind_RawValue("g_vLightDir", &m_tagLightDesc.vDirection, sizeof(_float4))))
            return;
    }
    else if (LIGHT_DESC::LT_POINT == m_tagLightDesc.eState)
    {
        iPassIndex = 2;

        if (FAILED(_pShader->Bind_RawValue("g_vLightPos", &m_tagLightDesc.vPosition, sizeof(_float4))))
            return;
        if (FAILED(_pShader->Bind_RawValue("g_fRange", &m_tagLightDesc.fRange, sizeof(_float))))
            return;
    }



    // 여기서 Light Diffuse 랑 Ambient 처리 해줄꺼임. 
    if(FAILED(_pShader->Bind_RawValue("g_vLightDiffuse", &m_tagLightDesc.vDiffuse , sizeof(_float4))))
        return;

    if (FAILED(_pShader->Bind_RawValue("g_vLightAmbient", &m_tagLightDesc.vAmbient, sizeof(_float4))))
        return;

    if(FAILED(_pShader->Bind_RawValue("g_vLightSpecular", &m_tagLightDesc.vSpecular, sizeof(_float4))))
        return;


    // 2 번 째 패스 쓸꺼셈. 여기다가 기본 빛 정보를 던질꺼셈. 
    _pShader->Begin(iPassIndex);

    _pVIBuffer->Bind_InputAssembler();
    _pVIBuffer->Render();
}

CLight* CLight::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const LIGHT_DESC& _tagLightDesc)
{
    CLight* pInstance = new CLight(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize(_tagLightDesc)))
    {
        MSG_BOX("Failed To Created : CLight");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLight::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
