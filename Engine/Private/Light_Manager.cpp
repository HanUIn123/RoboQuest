#include "../Public/Light_Manager.h"
#include "Light.h"

CLight_Manager::CLight_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice{ _pDevice }
    , m_pContext{ _pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

const LIGHT_DESC* CLight_Manager::Get_LightDesc(_uint _iIndex) const
{
    auto iter = m_listLights.begin();

    for (size_t i = 0; i < _iIndex; ++i)
    {
        ++iter;
    }

    return (*iter)->Get_LightDesc();
}

HRESULT CLight_Manager::Initialize()
{
    return S_OK;
}

HRESULT CLight_Manager::Add_Light(const LIGHT_DESC& _tagLightDesc)
{
    CLight* pLight = CLight::Create(m_pDevice, m_pContext, _tagLightDesc);

    if (nullptr == pLight)
        return E_FAIL;

    m_listLights.push_back(pLight);

    return S_OK;
}

void CLight_Manager::Render_Lights(CShader* _pShader, CVIBuffer_Rect* _pVIBuffer)
{
    for (auto& pLight : m_listLights)
        pLight->Render(_pShader, _pVIBuffer);
}

CLight_Manager* CLight_Manager::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CLight_Manager* pInstance = new CLight_Manager(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed To Created : CLight_Manager");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLight_Manager::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);


    for (auto& pLight : m_listLights)
        Safe_Release(pLight);

    m_listLights.clear();
}
