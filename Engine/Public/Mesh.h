#pragma once
#include "VIBuffer.h"
#include "Model.h"

BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
    CMesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CMesh(const CMesh& _Prototype);
    virtual ~CMesh() = default;

public:
    _uint       Get_MaterialIndex() const {
        return m_iMaterialIndex;
    }

public:
    XMVECTOR* Get_VertexPos() const{return m_vertexPos;}
    _uint*    Get_Indices() { return m_pIndices; }
    _uint   Get_Triangles() { return m_iNumFaces; }

public:
    virtual HRESULT Initialize_Prototype(CModel::MODEL _eModelType, CModel* _pModel, const aiMesh* _pAIMesh, _fmatrix _PreTransformMatrix);
    virtual HRESULT Initialize_Prototype_SaveData(CModel::MODEL _eModelType, CModel* _pModel, const aiMesh* _pAIMesh, HANDLE _pSaveData, _fmatrix _PreTransformMatrix);
    virtual HRESULT Initialize_Prototype_LoadData(CModel::MODEL _eModelType, CModel* _pModel, HANDLE _pLoadData, _fmatrix _PreTransformMatrix);
     
    virtual HRESULT Initialize(void* _pArg) override;

public:
    HRESULT             Bind_BoneMatrices(class CShader* _pShader, const _char* _pConstantName, const vector<class CBone*>& _vecBones);
public:
    HRESULT             Save_AnimMesh(const aiMesh* _pAIMesh);



private:
    _char               m_szName[MAX_PATH] = {};

    _uint               m_iMaterialIndex = {};
private:
    // Mesh 에 영향을 주는 뼈의 갯수.
    _uint               m_iNumBones = {};
    // 영향을 주는 뼈들의 인덱스(모델이 가지고 있는 전체 뼈 기준의 인덱스)
    vector<_uint>       m_vecBoneIndices;

    _float4x4                   m_BoneMatrices[256] = {};
    vector<_float4x4>           m_vecOffsetMatrices;

private:
    _uint               m_iNumFaces = {};
    _uint*              m_pIndices= {nullptr};
    XMVECTOR*           m_vertexPos = {};


private:
#pragma region 기존.
    HRESULT         Ready_VertexBuffer_ForNonAnim(const aiMesh* _pAIMesh, _fmatrix _PreTransformMatrix);
    HRESULT         Ready_VertexBuffer_ForAnim(CModel* _pModel, const aiMesh* _pAIMesh);
#pragma endregion

#pragma region Save.
    HRESULT         Ready_VertexBuffer_ForNonAnim_SaveData(HANDLE _pSaveData, const aiMesh* _pAIMesh, _fmatrix _PreTransformMatrix);
    HRESULT         Ready_VertexBuffer_ForAnim_SaveData(HANDLE _pSaveData, CModel* _pModel, const aiMesh* _pAIMesh);
#pragma endregion

#pragma region Load.
    HRESULT         Ready_VertexBuffer_ForNonAnim_LoadData(HANDLE _pLoadData,_fmatrix _PreTransformMatrix);
    HRESULT         Ready_VertexBuffer_ForAnim_LoadData(HANDLE _pLoadData, CModel* _pModel);
#pragma endregion


public:
    static CMesh* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CModel::MODEL _eModelType, CModel* _pModel, const aiMesh* _pAIMesh, _fmatrix _PreTransformMatrix);
    static CMesh* CreateSaveData(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CModel::MODEL _eModelType, CModel* _pModel, const aiMesh* _pAIMesh, HANDLE _pSaveFile, _fmatrix _PreTransformMatrix);
    static CMesh* CreateLoadData(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CModel::MODEL _eModelType, CModel* _pModel, HANDLE _pLoadFile, _fmatrix _PreTransformMatrix);


    virtual CVIBuffer* Clone(void* _pArg) override;
    virtual void Free() override;
};

END