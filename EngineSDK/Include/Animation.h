#pragma once
#include "Base.h"

BEGIN(Engine)
class CAnimation final : public CBase
{
private:
    CAnimation();
    virtual ~CAnimation() = default;

public:
    HRESULT Initialize(const aiAnimation* _pAIAnimation, const class CModel* _pModel, vector<_uint>& _vecCurrentKeyFrameIndices);
    HRESULT Initialize_Save(const aiAnimation* _pAIAnimation, const class CModel* _pModel, HANDLE _pSaveFile, vector<_uint>& _vecCurrentKeyFrameIndices);
    HRESULT Initialize_Load(const class CModel* _pModel, HANDLE _pLoadFile, vector<_uint>& _vecCurrentKeyFrameIndices);

    _bool Update_TransformationMatrix(_float _fTimeDelta, const vector<class CBone*>& _vecBones, _float* _pCurrentTrackPosition, vector<_uint>& _vecCurrentKeyFrameIndices, _bool _bLoop);


    _bool Update_TransformationMatrix_Anims(_float _fTimeDelta, const vector<class CBone*>& _vecBones, 
        _float* _pCurrentTrackPosition, vector<_uint>& _vecCurrentKeyFrameIndices, _bool _bLoop, class CAnimation* _pPreAnimation);

public:
    _float Get_Duration() const { return m_fDuration; }

private:
    _float                      m_fDuration = {};
    _float                      m_fTickPerSecond = {};

    // 현재 재생위치 
    _uint                       m_iNumChannels = {};

    _float                      m_fAnimationCurrentSpeed = 1.0f;

    _bool                       m_bAnimFinished = { false };
public:
    _bool                       IsAnimFinished() const { return m_bAnimFinished; }


public:
    vector<class CChannel*>&    Get_Channels() { return m_vecChannels; }

    vector<class CChannel*>     m_vecChannels;

    _float                      m_fTime = {};

public:
    void                        Set_AnimationSpeed(_float _fSpeed);


public:
    static CAnimation* Create(const aiAnimation* _pAIAnimation, const class CModel* _pModel, vector<_uint>& _vecCurrentKeyFrameIndices);
    static CAnimation* Create_Save(const aiAnimation* _pAIAnimation, const class CModel* _pModel, HANDLE _pSaveFile, vector<_uint>& _vecCurrentKeyFrameIndices);
    static CAnimation* Create_Load(const class CModel* _pModel, HANDLE _pLoadFile, vector<_uint>& _vecCurrentKeyFrameIndices);
    virtual void Free() override;
};

END