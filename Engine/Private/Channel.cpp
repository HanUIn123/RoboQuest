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

// 여기서 커트포 거꾸로 하는거 생각해봐야할듯. (Door 용도 땜에 )
void CChannel::Update_TransformationMatrix(_float _fCurrentTrackPosition, _uint* _pCurrentKeyFrameIndex, const vector<class CBone*>& _vecBones)
{
    // 애니메 바뀔때마다. 아예 다른 상태의 애니메이션 딱 진입하면서 저게 0 됨.
    if (0.0f == _fCurrentTrackPosition)
        *_pCurrentKeyFrameIndex = 0;

    // 현재 재생 위치에 맞는 상태를 구한다.
    _vector                 vScale, vRotation, vTranslation;

    KEYFRAME                tagLastKeyFrame = m_vecKeyFrames.back();

    // 애니메이션 10개잇다치면 거기서 애니메이션1 의 그 하나의 애니메이션에서 투 두 둑 할 때 투 - 두 & 두 - 둑 & 둑 - 투(loop true 일때) 사이 선형보간.
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

        // ex) 키프레임의 한 간격을 떼서 그게  1 ~ 10 이라 치면, 빨간색 currentTrack 이것이 7 정도 가면
        // 7 / 10 해서 0.7비율
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
