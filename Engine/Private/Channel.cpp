#include "Channel.h"
#include "Model.h"
#include "Bone.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(const aiNodeAnim* _pAIChannel, const CModel* _pModel)
{
    strcpy_s(m_szName, _pAIChannel->mNodeName.data);

    m_iBoneIndex = _pModel->Get_BoneIndex(m_szName);

    m_iNumKeyFrames = max(_pAIChannel->mNumScalingKeys, _pAIChannel->mNumRotationKeys);
    m_iNumKeyFrames = max(m_iNumKeyFrames, _pAIChannel->mNumPositionKeys);

    _float3         vScale = {};
    _float4         vRotation = {};
    _float3         vTranslation = {};

    for (size_t i = 0; i < m_iNumKeyFrames; i++)
    {
        KEYFRAME            tagKeyFrame = {};

        if (_pAIChannel->mNumScalingKeys > i)
        {
            memcpy(&vScale, &_pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
            tagKeyFrame.fTrackPosition = (_float)(_pAIChannel->mScalingKeys[i].mTime);
        }

        if (_pAIChannel->mNumRotationKeys > i)
        {
            vRotation.x = _pAIChannel->mRotationKeys[i].mValue.x;
            vRotation.y = _pAIChannel->mRotationKeys[i].mValue.y;
            vRotation.z = _pAIChannel->mRotationKeys[i].mValue.z;
            vRotation.w = _pAIChannel->mRotationKeys[i].mValue.w;

            tagKeyFrame.fTrackPosition = (_float)(_pAIChannel->mRotationKeys[i].mTime);
        }

        if (_pAIChannel->mNumPositionKeys > i)
        {
            memcpy(&vTranslation, &_pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
            tagKeyFrame.fTrackPosition = (_float)(_pAIChannel->mPositionKeys[i].mTime);
        }

        tagKeyFrame.vScale = vScale;
        tagKeyFrame.vRotation = vRotation;
        tagKeyFrame.vTranslation = vTranslation;

        m_vecKeyFrames.push_back(tagKeyFrame);
    }

    return S_OK;
}

HRESULT CChannel::Initialize_Save(const aiNodeAnim* _pAIChannel, const CModel* _pModel, HANDLE _SaveFile)
{
    DWORD           dwByte = {};
    strcpy_s(m_szName, _pAIChannel->mNodeName.data);

    const _char* szNodeName = m_szName;
    _uint iLength = (_uint)strlen(szNodeName);

    WriteFile(_SaveFile, &iLength, sizeof(_uint), &dwByte, nullptr);
    for (_uint i = 0; i < iLength; ++i)
    {
        WriteFile(_SaveFile, &szNodeName[i], sizeof(_char), &dwByte, nullptr);
    }

    m_iBoneIndex = _pModel->Get_BoneIndex(m_szName);

    _uint iNumScalingKeys = _pAIChannel->mNumScalingKeys;
    _uint iNumRoatationKeys = _pAIChannel->mNumRotationKeys;

    WriteFile(_SaveFile, &iNumScalingKeys, sizeof(_uint), &dwByte, nullptr);
    WriteFile(_SaveFile, &iNumRoatationKeys, sizeof(_uint), &dwByte, nullptr);

    m_iNumKeyFrames = max(iNumScalingKeys, iNumRoatationKeys);

    _uint iNumPositionKeys = _pAIChannel->mNumPositionKeys;
    WriteFile(_SaveFile, &iNumPositionKeys, sizeof(_uint), &dwByte, nullptr);

    m_iNumKeyFrames = max(m_iNumKeyFrames, iNumPositionKeys);

    _float3         vScale = {};
    _float4         vRotation = {};
    _float3         vTranslation = {};

    for (size_t i = 0; i < m_iNumKeyFrames; i++)
    {
        KEYFRAME            tagKeyFrame = {};

        if (_pAIChannel->mNumScalingKeys > i)
        {
            memcpy(&vScale, &_pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
            WriteFile(_SaveFile, &vScale, sizeof(_float3), &dwByte, nullptr);

            tagKeyFrame.fTrackPosition = (_float)(_pAIChannel->mScalingKeys[i].mTime);
            WriteFile(_SaveFile, &tagKeyFrame.fTrackPosition, sizeof(_float), &dwByte, nullptr);
        }

        if (_pAIChannel->mNumRotationKeys > i)
        {
            vRotation.x = _pAIChannel->mRotationKeys[i].mValue.x;
            vRotation.y = _pAIChannel->mRotationKeys[i].mValue.y;
            vRotation.z = _pAIChannel->mRotationKeys[i].mValue.z;
            vRotation.w = _pAIChannel->mRotationKeys[i].mValue.w;
            WriteFile(_SaveFile, &vRotation, sizeof(_float4), &dwByte, nullptr);

            tagKeyFrame.fTrackPosition = (_float)(_pAIChannel->mRotationKeys[i].mTime);
            WriteFile(_SaveFile, &tagKeyFrame.fTrackPosition, sizeof(float), &dwByte, nullptr);
        }

        if (_pAIChannel->mNumPositionKeys > i)
        {
            memcpy(&vTranslation, &_pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
            WriteFile(_SaveFile, &vTranslation, sizeof(_float3), &dwByte, nullptr);

            tagKeyFrame.fTrackPosition = (_float)(_pAIChannel->mPositionKeys[i].mTime);
            WriteFile(_SaveFile, &tagKeyFrame.fTrackPosition, sizeof(float), &dwByte, nullptr);
        }

        tagKeyFrame.vScale = vScale;
        tagKeyFrame.vRotation = vRotation;
        tagKeyFrame.vTranslation = vTranslation;

        m_vecKeyFrames.push_back(tagKeyFrame);
    }

    return S_OK;
}

HRESULT CChannel::Initialize_Load(const CModel* _pModel, HANDLE _pLoadFile)
{
    DWORD           dwByte = {};

    _uint iLength = 0;
    ReadFile(_pLoadFile, &iLength, sizeof(_uint), &dwByte, nullptr);

    for (_uint i = 0; i < iLength; ++i)
    {
        ReadFile(_pLoadFile, &m_szName[i], sizeof(_char), &dwByte, nullptr);
    }

    m_iBoneIndex = _pModel->Get_BoneIndex(m_szName);

    _uint iNumScalingKeys = 0;
    _uint iNumRoatationKeys = 0;

    ReadFile(_pLoadFile, &iNumScalingKeys, sizeof(_uint), &dwByte, nullptr);
    ReadFile(_pLoadFile, &iNumRoatationKeys, sizeof(_uint), &dwByte, nullptr);

    m_iNumKeyFrames = max(iNumScalingKeys, iNumRoatationKeys);

    _uint iNumPositionKeys = 0;
    ReadFile(_pLoadFile, &iNumPositionKeys, sizeof(_uint), &dwByte, nullptr);

    m_iNumKeyFrames = max(m_iNumKeyFrames, iNumPositionKeys);

    _float3         vScale = {};
    _float4         vRotation = {};
    _float3         vTranslation = {};

    for (size_t i = 0; i < m_iNumKeyFrames; i++)
    {
        KEYFRAME            tagKeyFrame = {};

        if (iNumScalingKeys > i)
        {
            ReadFile(_pLoadFile, &vScale, sizeof(_float3), &dwByte, nullptr);

            ReadFile(_pLoadFile, &tagKeyFrame.fTrackPosition, sizeof(_float), &dwByte, nullptr);
        }

        if (iNumRoatationKeys > i)
        {
            ReadFile(_pLoadFile, &vRotation, sizeof(_float4), &dwByte, nullptr);

            ReadFile(_pLoadFile, &tagKeyFrame.fTrackPosition, sizeof(_float), &dwByte, nullptr);
        }

        if (iNumPositionKeys > i)
        {
            ReadFile(_pLoadFile, &vTranslation, sizeof(_float3), &dwByte, nullptr);
            ReadFile(_pLoadFile, &tagKeyFrame.fTrackPosition, sizeof(_float), &dwByte, nullptr);
        }

        tagKeyFrame.vScale = vScale;
        tagKeyFrame.vRotation = vRotation;
        tagKeyFrame.vTranslation = vTranslation;

        m_vecKeyFrames.push_back(tagKeyFrame);
    }

    return S_OK;
}

// ���⼭ ĿƮ�� �Ųٷ� �ϴ°� �����غ����ҵ�. (Door �뵵 ���� )
void CChannel::Update_TransformationMatrix(_float _fCurrentTrackPosition, _uint* _pCurrentKeyFrameIndex, const vector<class CBone*>& _vecBones)
{
    // �ִϸ� �ٲ𶧸���. �ƿ� �ٸ� ������ �ִϸ��̼� �� �����ϸ鼭 ���� 0 ��.
    if (0.0f == _fCurrentTrackPosition)
        *_pCurrentKeyFrameIndex = 0;

    // ���� ��� ��ġ�� �´� ���¸� ���Ѵ�.
    _vector                 vScale, vRotation, vTranslation;

    KEYFRAME                tagLastKeyFrame = m_vecKeyFrames.back();

    // �ִϸ��̼� 10���մ�ġ�� �ű⼭ �ִϸ��̼�1 �� �� �ϳ��� �ִϸ��̼ǿ��� �� �� �� �� �� �� - �� & �� - �� & �� - ��(loop true �϶�) ���� ��������.
    if (_fCurrentTrackPosition >= tagLastKeyFrame.fTrackPosition)
    {
        vScale = XMLoadFloat3(&tagLastKeyFrame.vScale);
        vRotation = XMLoadFloat4(&tagLastKeyFrame.vRotation);
        vTranslation = XMVectorSetW(XMLoadFloat3(&tagLastKeyFrame.vTranslation), 1.0f);
    }
    else
    {
        while (_fCurrentTrackPosition >= m_vecKeyFrames[*_pCurrentKeyFrameIndex + 1].fTrackPosition)
            ++*_pCurrentKeyFrameIndex;

        // ex) Ű�������� �� ������ ���� �װ�  1 ~ 10 �̶� ġ��, ������ currentTrack �̰��� 7 ���� ����
        // 7 / 10 �ؼ� 0.7����
        _float fRatio = (_fCurrentTrackPosition - m_vecKeyFrames[*_pCurrentKeyFrameIndex].fTrackPosition) /
            (m_vecKeyFrames[*_pCurrentKeyFrameIndex + 1].fTrackPosition - m_vecKeyFrames[*_pCurrentKeyFrameIndex].fTrackPosition);

        vScale = XMVectorLerp(XMLoadFloat3(&m_vecKeyFrames[*_pCurrentKeyFrameIndex].vScale), XMLoadFloat3(&m_vecKeyFrames[*_pCurrentKeyFrameIndex + 1].vScale), fRatio);
        vRotation = XMQuaternionSlerp(XMLoadFloat4(&m_vecKeyFrames[*_pCurrentKeyFrameIndex].vRotation), XMLoadFloat4(&m_vecKeyFrames[*_pCurrentKeyFrameIndex + 1].vRotation), fRatio);
        vTranslation = XMVectorLerp(XMLoadFloat3(&m_vecKeyFrames[*_pCurrentKeyFrameIndex].vTranslation), XMLoadFloat3(&m_vecKeyFrames[*_pCurrentKeyFrameIndex + 1].vTranslation), fRatio);
    }

    _matrix  TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), vRotation, vTranslation);

    _vecBones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);
}

_bool CChannel::Update_TransformationMatrixAnims(_float _fCurrentTrackPosition, _uint* _pCurrentKeyFrameIndex,
    const vector<class CBone*>& _vecBones/*, KEYFRAME _tagKeyFrame*/, _float _fSpeed)
{
    KEYFRAME                tagFirstKeyFrame = m_vecKeyFrames.front();

    if (_fSpeed <= _fCurrentTrackPosition)
    {
        *_pCurrentKeyFrameIndex = 0;
        //_fCurrentTrackPosition = 0.0f;
        _matrix  TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&tagFirstKeyFrame.vScale), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMLoadFloat4(&tagFirstKeyFrame.vRotation), XMLoadFloat3(&tagFirstKeyFrame.vTranslation));

        _vecBones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);

        return true;
    }

    _vector                 vScale, vRotation, vTranslation;
    _vector                 vCombiScale, vCombiRotation, vCombiTranslation;

    vScale = XMLoadFloat3(&tagFirstKeyFrame.vScale);
    vRotation = XMLoadFloat4(&tagFirstKeyFrame.vRotation);
    vTranslation = XMVectorSetW(XMLoadFloat3(&tagFirstKeyFrame.vTranslation), 1.0f);

    _float fRatio = _fCurrentTrackPosition / _fSpeed;

    vCombiScale = XMVectorLerp(XMLoadFloat3(&m_vPreScale), vScale, fRatio);
    vCombiRotation = XMQuaternionSlerp(XMLoadFloat4(&m_vPreRotation), vRotation, fRatio);
    vCombiTranslation = XMVectorLerp(XMLoadFloat3(&m_vPreTranslation), vTranslation, fRatio);

    _matrix  TransformationMatrix = XMMatrixAffineTransformation(vCombiScale, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), vCombiRotation, vCombiTranslation);

    _vecBones[m_iBoneIndex]->Set_TransformationMatrix(TransformationMatrix);

    return false;
}

void CChannel::Compute_matTransformation(const vector<class CBone*>& _vecBone)
{
    _vector vScale{}, vRotation{}, vTranslation{};

    XMMatrixDecompose(&vScale, &vRotation, &vTranslation, _vecBone[m_iBoneIndex]->Get_TransformationMatrix());

    XMStoreFloat3(&m_vPreScale, vScale);
    XMStoreFloat4(&m_vPreRotation, vRotation);
    XMStoreFloat3(&m_vPreTranslation, vTranslation);
    XMVectorSetW(XMLoadFloat3(&m_vPreTranslation), 1.f);
}

CChannel* CChannel::Create(const aiNodeAnim* _pAIChannel, const CModel* _pModel)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize(_pAIChannel, _pModel)))
    {
        MSG_BOX("Failed To Created : CChannel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CChannel* CChannel::Create_Save(const aiNodeAnim* _pAIChannel, const CModel* _pModel, HANDLE _pSaveFile)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize_Save(_pAIChannel, _pModel, _pSaveFile)))
    {
        MSG_BOX("Failed To Created : CChannel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CChannel* CChannel::Create_Load(const CModel* _pModel, HANDLE _pLoadFile)
{
    CChannel* pInstance = new CChannel();

    if (FAILED(pInstance->Initialize_Load(_pModel, _pLoadFile)))
    {
        MSG_BOX("Failed To Created : CChannel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CChannel::Free()
{
    __super::Free();
}
