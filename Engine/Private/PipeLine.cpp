#include "../Public/PipeLine.h"

CPipeLine::CPipeLine()
{
}

HRESULT CPipeLine::Initialize()
{
    for (size_t i = 0; i < D3DTS_END; ++i)
    {
        XMStoreFloat4x4(&m_TransformationMatrixArray[i], XMMatrixIdentity());
        XMStoreFloat4x4(&m_TransformationInverseMatrixArray[i], XMMatrixIdentity());
    }

    m_vCameraPos = _float4(0.0f, 0.0f, 0.0f, 1.0f);

    return S_OK;
}

void CPipeLine::Update()
{
    for (size_t i = 0; i < D3DTS_END; ++i)
    {
        XMStoreFloat4x4(&m_TransformationInverseMatrixArray[i], XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformationMatrixArray[i])));
    }

    // ī�޶�� ���� ��� ���� ��ġ�ϱ�, ������� ������� ���ؼ�, camPosition�� �������ִ� ����. r[3]��, 4�� ��ġ���͸� �ǹ�.
    XMStoreFloat4(&m_vCameraPos, XMLoadFloat4x4(&m_TransformationInverseMatrixArray[D3DTS_VIEW]).r[3]);
}

CPipeLine* CPipeLine::Create()
{
    CPipeLine* pInstance = new CPipeLine();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed To Created : CPipeLine");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CPipeLine::Free()
{
    __super::Free();
}
