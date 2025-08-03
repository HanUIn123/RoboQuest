#include "Bone.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(const aiNode* _pAINode, _int _iParentBoneIndex)
{
    strcpy_s(m_szName, _pAINode->mName.data);

    memcpy(&m_TransformationMatrix, &_pAINode->mTransformation, sizeof(_float4x4));

    XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));

    XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

    m_iParentBoneIndex = _iParentBoneIndex;

    return S_OK;
}

HRESULT CBone::Initialize_Save(const aiNode* _pAINode, HANDLE _pSaveFile, _int _iParentBoneIndex)
{
    DWORD dwByte = 0;

    // _pAINode->mName.data ����, m_szName ��� ������ �̸� �� ���������,
    // m_szName �� szAINodeName �� �ؼ� �ϴ� ���� �� �ϰ�,...
    strcpy_s(m_szName, _pAINode->mName.data);

    const _char* szAINodeName = m_szName;
    _uint iNodeNameLength = (_uint)strlen(szAINodeName);
    WriteFile(_pSaveFile, &iNodeNameLength, sizeof(_uint), &dwByte, nullptr);
    for (_uint i = 0; i < iNodeNameLength; ++i)
    {
        WriteFile(_pSaveFile, &szAINodeName[i], sizeof(_char), &dwByte, nullptr);
    }

    memcpy(&m_TransformationMatrix, &_pAINode->mTransformation, sizeof(_float4x4));
    XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
    WriteFile(_pSaveFile, &m_TransformationMatrix, sizeof(_float4x4), &dwByte, nullptr);

    XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

    m_iParentBoneIndex = _iParentBoneIndex;

    return S_OK;
}

HRESULT CBone::Initialize_Load(HANDLE _pLoadData, _int _iParentBoneIndex)
{
    DWORD dwByte = 0;

    // ���� ������ �̸� ( �� �� �̸��� ��� ������ ����) -> �׷��� �����؊x�� m_szName ��� ���ٰ�, �ٽ� ������ ������ �����س���
    // �̸��� �ٽ� ����� �־. �ҷ�����. 

    _uint iNodeNameLength = 0;
    ReadFile(_pLoadData, &iNodeNameLength, sizeof(_uint), &dwByte, nullptr);

    for (_uint i = 0; i < iNodeNameLength; ++i)
    {
        ReadFile(_pLoadData, &m_szName[i], sizeof(_char), &dwByte, nullptr);
    }

    _float4x4 matTransformation = {};
    ReadFile(_pLoadData, &matTransformation, sizeof(_float4x4), &dwByte, nullptr);
    m_TransformationMatrix = matTransformation;

    //ReadFile(_pLoadData, &m_TransformationMatrix, sizeof(_float4x4), &dwByte, nullptr);

    XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMMatrixIdentity());

    m_iParentBoneIndex = _iParentBoneIndex;

    return S_OK;
}

void CBone::Update_CombinedTransformMatrix(const vector<CBone*>& _vecBones, _fmatrix _PreTransformMatrix)
{
    if (-1 == m_iParentBoneIndex)
    {
        //m_CombinedTransformationMatrix = m_TransformationMatrix;
        XMStoreFloat4x4(&m_CombinedTransformationMatrix, _PreTransformMatrix * XMLoadFloat4x4(&m_TransformationMatrix));
    }
    else
        XMStoreFloat4x4(&m_CombinedTransformationMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&_vecBones[m_iParentBoneIndex]->m_CombinedTransformationMatrix));
}

CBone* CBone::Create(const aiNode* _pAINode, _int _iParentBoneIndex)
{
    CBone* pInstance = new CBone();

    if (FAILED(pInstance->Initialize(_pAINode, _iParentBoneIndex)))
    {
        MSG_BOX("Failed To Created : CBone");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CBone* CBone::Create_SaveData(const aiNode* _pAINode, HANDLE _pSaveFile, _int _iParentBoneIndex)
{
    CBone* pInstance = new CBone();

    if (FAILED(pInstance->Initialize_Save(_pAINode, _pSaveFile, _iParentBoneIndex)))
    {
        MSG_BOX("Failed To Created : CBone Save");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CBone* CBone::Create_LoadData(HANDLE _pLoadFile, _int _iParentBoneIndex)
{
    CBone* pInstance = new CBone();

    if (FAILED(pInstance->Initialize_Load(_pLoadFile, _iParentBoneIndex)))
    {
        MSG_BOX("Failed To Created : CBone Load");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CBone* CBone::Clone()
{
    return new CBone(*this);
}

void CBone::Free()
{
    __super::Free();
}
