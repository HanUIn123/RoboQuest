#include "../Public/Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(const aiAnimation* _pAIAnimation, const CModel* _pModel, vector<_uint>& _vecCurrentKeyFrameIndices)
{
    m_fDuration = static_cast<_float>(_pAIAnimation->mDuration);

    m_fTickPerSecond = static_cast<_float>(_pAIAnimation->mTicksPerSecond);

    m_iNumChannels = _pAIAnimation->mNumChannels;

    _vecCurrentKeyFrameIndices.resize(m_iNumChannels);

    for (size_t i = 0; i < m_iNumChannels; i++)
    {
        CChannel* pChannel = CChannel::Create(_pAIAnimation->mChannels[i], _pModel);
        if (nullptr == pChannel)
            return E_FAIL;

        m_vecChannels.push_back(pChannel);
    }

    return S_OK;
}

HRESULT CAnimation::Initialize_Save(const aiAnimation* _pAIAnimation, const CModel* _pModel, HANDLE _pSaveFile, vector<_uint>& _vecCurrentKeyFrameIndices)
{
    DWORD       dwByte = {};

    m_fDuration = static_cast<_float>(_pAIAnimation->mDuration);
    WriteFile(_pSaveFile, &m_fDuration, sizeof(_float), &dwByte, nullptr);

    m_fTickPerSecond = static_cast<_float>(_pAIAnimation->mTicksPerSecond);
    WriteFile(_pSaveFile, &m_fTickPerSecond, sizeof(_float), &dwByte, nullptr);

    m_iNumChannels = _pAIAnimation->mNumChannels;
    WriteFile(_pSaveFile, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr);

    _vecCurrentKeyFrameIndices.resize(m_iNumChannels);

    for (size_t i = 0; i < m_iNumChannels; i++)
    {
        CChannel* pChannel = CChannel::Create_Save(_pAIAnimation->mChannels[i], _pModel, _pSaveFile);
        if (nullptr == pChannel)
            return E_FAIL;

        m_vecChannels.push_back(pChannel);
    }

    return S_OK;
}

HRESULT CAnimation::Initialize_Load(const CModel* _pModel, HANDLE _pLoadFile, vector<_uint>& _vecCurrentKeyFrameIndices)
{
    DWORD       dwByte = {};

    ReadFile(_pLoadFile, &m_fDuration, sizeof(_float), &dwByte, nullptr);

    ReadFile(_pLoadFile, &m_fTickPerSecond, sizeof(_float), &dwByte, nullptr);

    ReadFile(_pLoadFile, &m_iNumChannels, sizeof(_uint), &dwByte, nullptr);

    _vecCurrentKeyFrameIndices.resize(m_iNumChannels);

    for (size_t i = 0; i < m_iNumChannels; i++)
    {
        CChannel* pChannel = CChannel::Create_Load(_pModel, _pLoadFile);
        if (nullptr == pChannel)
            return E_FAIL;

        m_vecChannels.push_back(pChannel);
    }

    return S_OK;
}

_bool CAnimation::Update_TransformationMatrix(_float _fTimeDelta, const vector<class CBone*>& _vecBones, _float* _pCurrentTrackPosition, vector<_uint>& _vecCurrentKeyFrameIndices, _bool _bLoop)
{
    // 결국 m_fCurrentTrackPosition 이게 ++ 되면서, 애니메이션 드드드드득~ 실행됨.
    *_pCurrentTrackPosition += m_fTickPerSecond * _fTimeDelta * m_fAnimationCurrentSpeed;

    // 루프 도는 애니메이션이고, 끝나면 다시 0으로 빠꾸.
    if ((true == _bLoop) && (*_pCurrentTrackPosition >= m_fDuration))
    {
        *_pCurrentTrackPosition = 0.0f;
    }

    // 넘어가면 그냥 끝남.
    if (*_pCurrentTrackPosition >= m_fDuration)
    {
        m_bAnimFinished = true;
        return true;
    }

    m_bAnimFinished = false;
    _uint               iNumChannels = {};

    for (auto& pChannel : m_vecChannels)
    {
        pChannel->Update_TransformationMatrix(*_pCurrentTrackPosition, &_vecCurrentKeyFrameIndices[iNumChannels++], _vecBones);
    }

    return false;
}

_bool CAnimation::Update_TransformationMatrix_Anims(_float _fTimeDelta, const vector<class CBone*>& _vecBones, _float* _pCurrentTrackPosition,
    vector<_uint>& _vecCurrentKeyFrameIndices, _bool _bLoop, class CAnimation* _pPreAnimation)
{
    _bool bIsChange = {};

    *_pCurrentTrackPosition += m_fTickPerSecond * _fTimeDelta * 3.0f * m_fAnimationCurrentSpeed;

    _uint               iNumChannels = {};

    for (auto& pChannel : m_vecChannels)
    {
        pChannel->Compute_matTransformation(_vecBones);
        bIsChange = pChannel->Update_TransformationMatrixAnims(*_pCurrentTrackPosition, &_vecCurrentKeyFrameIndices[iNumChannels], _vecBones, /*(_pPreAnimation->Get_Channels())[iNumChannels]->Get_KeyFrame(), */m_fTickPerSecond);
        iNumChannels++;
    }

    if (*_pCurrentTrackPosition >= m_fTickPerSecond)
    {
        *_pCurrentTrackPosition = 0.0f;
        m_bAnimFinished = true;
        return true;
    }

    m_bAnimFinished = false;
    return bIsChange;
}

void CAnimation::Set_AnimationSpeed(_float _fSpeed)
{
    m_fAnimationCurrentSpeed = _fSpeed;
}

CAnimation* CAnimation::Create(const aiAnimation* _pAIAnimation, const CModel* _pModel, vector<_uint>& _vecCurrentKeyFrameIndices)
{
    CAnimation* pInstance = new CAnimation();
    if (FAILED(pInstance->Initialize(_pAIAnimation, _pModel, _vecCurrentKeyFrameIndices)))
    {
        MSG_BOX("Failed To Created : CAnimation");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CAnimation* CAnimation::Create_Save(const aiAnimation* _pAIAnimation, const CModel* _pModel, HANDLE _pSaveFile, vector<_uint>& _vecCurrentKeyFrameIndices)
{
    CAnimation* pInstance = new CAnimation();
    if (FAILED(pInstance->Initialize_Save(_pAIAnimation, _pModel, _pSaveFile, _vecCurrentKeyFrameIndices)))
    {
        MSG_BOX("Failed To Created : CAnimation");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CAnimation* CAnimation::Create_Load(const CModel* _pModel, HANDLE _pLoadFile, vector<_uint>& _vecCurrentKeyFrameIndices)
{
    CAnimation* pInstance = new CAnimation();
    if (FAILED(pInstance->Initialize_Load(_pModel, _pLoadFile, _vecCurrentKeyFrameIndices)))
    {
        MSG_BOX("Failed To Created : CAnimation");
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CAnimation::Free()
{
    __super::Free();

    for (auto& pChannel : m_vecChannels)
        Safe_Release(pChannel);

    m_vecChannels.clear();
}
