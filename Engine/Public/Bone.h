#pragma once
#include "Base.h"

BEGIN(Engine)
class CBone final : public CBase
{
private:
    CBone();
    virtual ~CBone() = default;

public:
    const _char* Get_Name() const
    {
        return m_szName;
    }

    _matrix Get_CombinedTransformationMatrix() const
    {
        return XMLoadFloat4x4(&m_CombinedTransformationMatrix);
    }

    const _float4x4* Get_CombinedTransformationMatrixPtr() const {
        return &m_CombinedTransformationMatrix;
    }

    _matrix Get_TransformationMatrix() const
    {
        return XMLoadFloat4x4(&m_TransformationMatrix);
    }

public:
    void Set_TransformationMatrix(_fmatrix _TransformationMatrix) {
        XMStoreFloat4x4(&m_TransformationMatrix, _TransformationMatrix);
    }

public:
    HRESULT Initialize(const aiNode* _pAINode, _int _iParentBoneIndex);
    HRESULT Initialize_Save(const aiNode* _pAINode, HANDLE _pSaveData, _int _iParentBoneIndex);
    HRESULT Initialize_Load(HANDLE _pLoadData, _int _iParentBoneIndex);

    void Update_CombinedTransformMatrix(const vector<CBone*>& _vecBones, _fmatrix _PreTransformMatrix);

private:
    _char           m_szName[MAX_PATH] = {};

    _float4x4       m_TransformationMatrix = {};
    _float4x4       m_CombinedTransformationMatrix = {};

    _int            m_iParentBoneIndex = { -1 };


public:
    static CBone* Create(const aiNode* _pAINode, _int _iParentBoneIndex);
    static CBone* Create_SaveData(const aiNode* _pAINode, HANDLE _pSaveFile, _int _iParentBoneIndex);
    static CBone* Create_LoadData(HANDLE _pLoadFile, _int _iParentBoneIndex);

    //  Bone을 return 하는 clone 함수.
    CBone* Clone();


    virtual void Free() override;
};
END