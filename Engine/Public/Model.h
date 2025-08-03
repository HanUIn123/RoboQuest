#pragma once
#include "Component.h"

BEGIN(Engine)
class ENGINE_DLL CModel final : public CComponent
{
public:
    enum MODEL  {MODEL_NONANIM, MODEL_ANIM, MODEL_END};

public:
    enum WORK_TYPE {WT_SAVE, WT_LOAD, WT_END};


private:
    CModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CModel(const CModel& _Prototype);
    virtual ~CModel() = default;

public:

    void                        SetUp_Animation(_uint _iAnimIndex, _bool _bLoop, _bool _bChange);
    virtual HRESULT             Initialize_Prototype(MODEL _eModelType, const _char* _ModelFilePath,  _fmatrix _PreTransformMatrix);
    virtual HRESULT             Initialize_Prototype_SaveData(MODEL _eModelType, HWND _pHwnd, const _char* _ModelFilePath, const _tchar* _SaveDataFilePath, _fmatrix _PreTransformMatrix);
    virtual HRESULT             Initialize_Prototype_LoadData(MODEL _eModelType, HWND _pHwnd, const _char* _ModelFilePath, const _tchar* _LoadDataFilePath, _fmatrix _PreTransformMatrix);

    virtual HRESULT             Initialize(void* _pArg) override;
    virtual HRESULT             Render(_uint _iMeshIndex);

public:
    HRESULT                     Bind_Material(class CShader* _pShader, _uint _iMeshIndex, aiTextureType _eTextureType, const _char* _pConstantName, _uint _iTextureIndex = 0);
    HRESULT                     Bind_BoneMatrices(class CShader* _pShader, _uint _iMeshIndex, const _char* _pConstantName);
    _bool                       Play_Animation(_float _fTimeDelta, _bool _bSkip = false);

public:
    _uint                       Get_NumMeshes() const{return m_iNumMeshes;}
    _uint                       Get_BoneIndex(const _char* _pBoneName) const;
    const _float4x4*            Get_BoneMatrix(const _char* _pBoneName) const;


private:
    const   aiScene*            m_pAIScene = { nullptr };
    Assimp::Importer            m_Importer = {};

private:
    _uint                       m_iNumMeshes = {};
    vector<class CMesh*>        m_vecMeshes;

public:
    vector<class CMesh*>        Get_Mesh() { return m_vecMeshes; }

    _float                      Get_CurrentTrackPosition() { return m_fCurrentTrackPosition; }
    void                        Set_CurrentTrackPosition(_float _fTrackPos) { m_fCurrentTrackPosition = _fTrackPos; }
private:
    _uint                       m_iNumMaterials = {};
    vector<class CMaterial*>    m_vecMaterials; 

private:
    vector<class CBone*>        m_vecBones;

private:
    MODEL                       m_eModelType = MODEL_END;

private:
    _uint                       m_iNumAnimations = {};
    vector<class CAnimation*>   m_vecAnimations;

    // 애니메이션 전환 전 필요한, (전) 애니메이션? 하고 (후) 애니메이션...??
    _uint                       m_iCurrentAnimIndex = {};
    _uint                       m_iPrevAnimIndex = { };
    _uint                       Get_PrevAnimIndex() { return m_iPrevAnimIndex; }
    _bool                       m_bisFinished = { false };
    _bool                       m_bIsAnims = { false };
    _bool                       m_bIsChange = { false };
    _bool                       m_bAnimLoop = {};

    _float                      m_fCurrentTrackPosition = {};
    _float4x4                   m_PreTransformMatrix;
    vector<vector<_uint>>       m_vecCurrentKeyFrameIndices;

private:
    WORK_TYPE                   m_eWorkType =  WT_END;

private:
    HRESULT                     Ready_Meshes(_fmatrix _PreTransformMatrix);
    HRESULT                     Ready_Materials(const _char* _pModelFilePath);
    HRESULT                     Ready_Bones(const aiNode* _pAINode, _int _iParentBoneIndex = -1);
    HRESULT                     Ready_Animations();

public:
    HRESULT                     Ready_Meshes_Save(_fmatrix _PreTransformMatrix, HANDLE _pFile);
    HRESULT                     Ready_Materials_Save(const _char* _pModelFilePath, HANDLE _pFile);
    HRESULT                     Ready_Bones_Save(const aiNode* _pAINode, HANDLE _pFile, _int _iParentBoneIndex = -1);
    HRESULT                     Ready_Animations_Save(HANDLE _pFile);

    HRESULT                     Ready_Meshes_Load(_fmatrix _PreTransformMatrix, HANDLE _pFile);
    HRESULT                     Ready_Materials_Load(const _char* _pModelFilePath, HANDLE _pFile);
    HRESULT                     Ready_Bones_Load(HANDLE _pFile, _int _iParentBoneIndex = -1);
    HRESULT                     Ready_Animations_Load(HANDLE _pFile);

public:
    void                        Set_AnimationSpeed(_float _fAnimSpeed, _uint _iAnimIndex = -1);
    _bool                       IsAnimOnceDone() const;


public:
    static CModel*              Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* _ModelFilePath, MODEL _eModelType, _fmatrix _PreTransformMatrix = XMMatrixIdentity());
    static CModel*              Create_ForSave(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* _ModelFilePath, HWND _pHwnd, const _tchar* _SaveDataFilePath, MODEL _eModelType, _fmatrix _PreTransformMatrix = XMMatrixIdentity());
    static CModel*              Create_ForLoad(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* _ModelFilePath, HWND _pHwnd, const _tchar* _LoadDataFilePath, MODEL _eModelType, _fmatrix _PreTransformMatrix = XMMatrixIdentity());


    virtual CComponent*         Clone(void* _pArg) override;
    virtual void                Free() override;
};

END