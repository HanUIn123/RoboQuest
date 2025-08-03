#pragma once
#include "Base.h"

BEGIN(Engine)
class CChannel final : public CBase
{
private:
    CChannel();
    virtual ~CChannel() = default;

public:
    HRESULT                 Initialize(const aiNodeAnim* _pAIChannel, const class CModel* _pModel);
    HRESULT                 Initialize_Save(const aiNodeAnim* _pAIChannel, const class CModel* _pModel, HANDLE _SaveFile);
    HRESULT                 Initialize_Load(const class CModel* _pModel, HANDLE _pLoadFile);

    void                    Update_TransformationMatrix(_float _fCurrentTrackPosition, _uint* _pCurrentKeyFrameIndex, const vector<class CBone*>& _vecBones);
    _bool                   Update_TransformationMatrixAnims(_float _fCurrentTrackPosition, _uint* _pCurrentKeyFrameIndex, const vector<class CBone*>& _vecBones/*, KEYFRAME _tagKeyFrame*/, _float _fSpeed);

private:
    char                    m_szName[MAX_PATH] = {};

    _uint                   m_iNumKeyFrames = {};
    vector<KEYFRAME>        m_vecKeyFrames;
    _uint                   m_iBoneIndex = {};

public:
    void                    Compute_matTransformation(const vector<class CBone*>& _vecBone);

public:
    KEYFRAME                Get_KeyFrame() { return m_vecKeyFrames.back(); }
    _float3                 m_vPreScale = {};
    _float4                 m_vPreRotation = {};
    _float3                 m_vPreTranslation = {};

public:
    static CChannel*        Create(const aiNodeAnim* _pAIChannel, const class CModel* _pModel);
    static CChannel*        Create_Save(const aiNodeAnim* _pAIChannel, const class CModel* _pModel, HANDLE _pSaveFile);
    static CChannel*        Create_Load(const class CModel* _pModel, HANDLE _pLoadFile);
    virtual void            Free() override;
};

END