#include "Model.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
#include "Bone.h"
#include "Animation.h"

CModel::CModel(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : CComponent{ _pDevice, _pContext }
{
}

CModel::CModel(const CModel& _Prototype)
    : CComponent(_Prototype)
    , m_iNumMeshes{ _Prototype.m_iNumMeshes }
    , m_vecMeshes{ _Prototype.m_vecMeshes }
    , m_iNumMaterials{ _Prototype.m_iNumMaterials }
    , m_vecMaterials{ _Prototype.m_vecMaterials }
    //, m_vecBones{_Prototype.m_vecBones}
    , m_eModelType{ _Prototype.m_eModelType }
    , m_iNumAnimations{ _Prototype.m_iNumAnimations }
    , m_vecAnimations{ _Prototype.m_vecAnimations }
    , m_vecCurrentKeyFrameIndices{ _Prototype.m_vecCurrentKeyFrameIndices }
    , m_PreTransformMatrix{ _Prototype.m_PreTransformMatrix }
{
    for (auto& pMesh : m_vecMeshes)
        Safe_AddRef(pMesh);

    for (auto& pMaterial : m_vecMaterials)
        Safe_AddRef(pMaterial);

    //for (auto& pBone : m_vecBones)
    //    Safe_AddRef(pBone);

    for (auto& pPrototypeBone : _Prototype.m_vecBones)
    {
        m_vecBones.push_back(pPrototypeBone->Clone());
    }

    for (auto& pAnimation : m_vecAnimations)
        Safe_AddRef(pAnimation);
}

HRESULT CModel::Initialize_Prototype(MODEL _eModelType, const _char* _ModelFilePath, _fmatrix _PreTransformMatrix)
{
    _uint   iFlags = {};

    iFlags = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

    if (MODEL_NONANIM == _eModelType)
        iFlags |= aiProcess_PreTransformVertices;

    m_pAIScene = m_Importer.ReadFile(_ModelFilePath, iFlags);
    if (nullptr == m_pAIScene)
        return E_FAIL;

    // 멤버 변수에 저장해주자. 안하면 큰일남 적용이 안되어서.
    XMStoreFloat4x4(&m_PreTransformMatrix, _PreTransformMatrix);

    // 여기다가 저장안하면 무조건 모델 타입이. Non _ anim 으로 가버림;
    m_eModelType = _eModelType;

    if (FAILED(Ready_Bones(m_pAIScene->mRootNode)))
        return E_FAIL;

    if (FAILED(Ready_Meshes(_PreTransformMatrix)))
        return E_FAIL;

    if (FAILED(Ready_Materials(_ModelFilePath)))
        return E_FAIL;

    if (FAILED(Ready_Animations()))
        return E_FAIL;

    return S_OK;
}

HRESULT CModel::Initialize_Prototype_SaveData(MODEL _eModelType, HWND _pHwnd, const _char* _ModelFilePath, const _tchar* _SaveDataFilePath, _fmatrix _PreTransformMatrix)
{
    _uint   iFlags = {};

    iFlags = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

    if (MODEL_NONANIM == _eModelType)
        iFlags |= aiProcess_PreTransformVertices;

    m_pAIScene = m_Importer.ReadFile(_ModelFilePath, iFlags);
    if (nullptr == m_pAIScene)
        return E_FAIL;

    XMStoreFloat4x4(&m_PreTransformMatrix, _PreTransformMatrix);

    m_eModelType = _eModelType;

    HANDLE hFile = CreateFile(_SaveDataFilePath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX("Failed To Create File!!");
        return E_FAIL;
    }
    // 파일 쓰기
    DWORD dwByte = 0;

    if (FAILED(Ready_Bones_Save(m_pAIScene->mRootNode, hFile)))
        return E_FAIL;

    if (FAILED(Ready_Meshes_Save(_PreTransformMatrix, hFile)))
        return E_FAIL;

    if (FAILED(Ready_Materials_Save(_ModelFilePath, hFile)))
        return E_FAIL;

    if (FAILED(Ready_Animations_Save(hFile)))
        return E_FAIL;

    CloseHandle(hFile);

    return S_OK;
}

HRESULT CModel::Initialize_Prototype_LoadData(MODEL _eModelType, HWND _pHwnd, const _char* _ModelFilePath, const _tchar* _LoadDataFilePath, _fmatrix _PreTransformMatrix)
{
    //_uint   iFlags = {};

    //iFlags = aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast;

    //if (MODEL_NONANIM == _eModelType)
    //    iFlags |= aiProcess_PreTransformVertices;

    //m_pAIScene = m_Importer.ReadFile(_ModelFilePath, iFlags);
    //if (nullptr == m_pAIScene)
    //    return E_FAIL;

    XMStoreFloat4x4(&m_PreTransformMatrix, _PreTransformMatrix);

    m_eModelType = _eModelType;

    //_phFile = CreateFile(_SaveDataFilePath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    HANDLE hFile = CreateFile(_LoadDataFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX("Failed To Create File!!");
        return E_FAIL;
    }
    // 파일 쓰기
    DWORD dwByte = 0;

    if (FAILED(Ready_Bones_Load(hFile)))
        return E_FAIL;

    if (FAILED(Ready_Meshes_Load(_PreTransformMatrix, hFile)))
        return E_FAIL;

    if (FAILED(Ready_Materials_Load(_ModelFilePath, hFile)))
        return E_FAIL;

    if (FAILED(Ready_Animations_Load(hFile)))
        return E_FAIL;

    CloseHandle(hFile);

    return S_OK;
}

HRESULT CModel::Initialize(void* _pArg)
{
    return S_OK;
}

HRESULT CModel::Render(_uint _iMeshIndex)
{
    //for (auto& pMesh : m_vecMeshes)
    //{
    //    pMesh->Bind_InputAssembler();
    //    pMesh->Render();
    //}

    // 이제, 각 메쉬에서 작업 
    m_vecMeshes[_iMeshIndex]->Bind_InputAssembler();
    m_vecMeshes[_iMeshIndex]->Render();

    return S_OK;
}

HRESULT CModel::Bind_Material(CShader* _pShader, _uint _iMeshIndex, aiTextureType _eTextureType, const _char* _pConstantName, _uint _iTextureIndex)
{
    _uint iMaterialIndex = m_vecMeshes[_iMeshIndex]->Get_MaterialIndex();

    return m_vecMaterials[iMaterialIndex]->Bind_Material(_pShader, _eTextureType, _pConstantName, _iTextureIndex);
}

HRESULT CModel::Bind_BoneMatrices(CShader* _pShader, _uint _iMeshIndex, const _char* _pConstantName)
{
    // 내가 들고 있는 메쉬들 중에서, 내가 그릴려고하는 메쉬 인덱스 몇번째 꺼! 
    // 이 메쉬는 자기가 이용해야할 뼈의 배열을 다 들고 있다.
    // 메쉬가 들고 있는 건 뼈의 인덱스만 들고있어서, 뼈 행렬을 찾지못하니, 모델이 들고 있는
    // 모델의 전체 뼈 배열을 던져줘야할듯!! 벡터를 말이지. 

    if (_iMeshIndex >= m_iNumMeshes)
        return E_FAIL;

    return m_vecMeshes[_iMeshIndex]->Bind_BoneMatrices(_pShader, _pConstantName, m_vecBones);
}

void CModel::SetUp_Animation(_uint _iAnimIndex, _bool _bLoop, _bool _bChange)
{
    m_iPrevAnimIndex = m_iCurrentAnimIndex;

    m_iCurrentAnimIndex = _iAnimIndex;

    m_bIsChange = _bChange;

    m_bAnimLoop = _bLoop;
}

_bool CModel::Play_Animation(_float _fTimeDelta, _bool _bSkip)
{
    _bool bIsFinish = { false }, bIsChange = { false };

    if (_bSkip == true)
    {
        for (auto& pBone : m_vecBones)
        {
            pBone->Update_CombinedTransformMatrix(m_vecBones, XMLoadFloat4x4(&m_PreTransformMatrix));
        }
        return true;
    }

    if (m_bIsChange)
    {
        bIsChange = m_vecAnimations[m_iCurrentAnimIndex]->Update_TransformationMatrix_Anims(_fTimeDelta, m_vecBones, &m_fCurrentTrackPosition,
            m_vecCurrentKeyFrameIndices[m_iCurrentAnimIndex], m_bAnimLoop, m_vecAnimations[m_iPrevAnimIndex]);
        if (true == bIsChange)
            m_bIsChange = false;
    }
    if (!m_bIsChange)
    {
        bIsFinish = m_vecAnimations[m_iCurrentAnimIndex]->Update_TransformationMatrix(_fTimeDelta, m_vecBones, &m_fCurrentTrackPosition,
            m_vecCurrentKeyFrameIndices[m_iCurrentAnimIndex], m_bAnimLoop);
    }

    for (auto& pBone : m_vecBones)
    {
        pBone->Update_CombinedTransformMatrix(m_vecBones, XMLoadFloat4x4(&m_PreTransformMatrix));
    }

    return bIsFinish;
}

_uint CModel::Get_BoneIndex(const _char* _pBoneName) const
{
    _uint iBoneIndex = {};

    auto iter = find_if(m_vecBones.begin(), m_vecBones.end(), [&](CBone* pBone)->_bool
        {
            if (false == strcmp(pBone->Get_Name(), _pBoneName))
                return true;

            ++iBoneIndex;
            return false;
        }
    );

    return iBoneIndex;
}

const _float4x4* CModel::Get_BoneMatrix(const _char* _pBoneName) const
{
    auto iter = find_if(m_vecBones.begin(), m_vecBones.end(), [&](CBone* pBone)->_bool {
        if (false == strcmp(pBone->Get_Name(), _pBoneName))
            return true;
        return false;
        });

    if (iter == m_vecBones.end())
        return nullptr;

    return (*iter)->Get_CombinedTransformationMatrixPtr();
}

#pragma region 기존 Ready함수들
HRESULT CModel::Ready_Meshes(_fmatrix _PreTransformMatrix)
{
    m_iNumMeshes = m_pAIScene->mNumMeshes;

    for (size_t i = 0; i < m_iNumMeshes; ++i)
    {
        CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eModelType, this, m_pAIScene->mMeshes[i], _PreTransformMatrix);
        if (nullptr == pMesh)
            return E_FAIL;

        m_vecMeshes.push_back(pMesh);
    }
    return S_OK;
}

HRESULT CModel::Ready_Materials(const _char* _pModelFilePath)
{
    m_iNumMaterials = m_pAIScene->mNumMaterials;

    for (size_t i = 0; i < m_iNumMaterials; ++i)
    {
        CMaterial* pAIMaterial = CMaterial::Create(m_pDevice, m_pContext, m_pAIScene->mMaterials[i], _pModelFilePath);
        if (nullptr == pAIMaterial)
            return E_FAIL;

        m_vecMaterials.push_back(pAIMaterial);
    }

    return S_OK;
}

HRESULT CModel::Ready_Bones(const aiNode* _pAINode, _int _iParentBoneIndex)
{
    CBone* pBone = CBone::Create(_pAINode, _iParentBoneIndex);
    if (nullptr == pBone)
        return E_FAIL;

    m_vecBones.push_back(pBone);

    _int iParentBoneIndex = (_int)(m_vecBones.size() - 1);
    for (size_t i = 0; i < _pAINode->mNumChildren; ++i)
    {
        Ready_Bones(_pAINode->mChildren[i], iParentBoneIndex);
    }

    return S_OK;
}

HRESULT CModel::Ready_Animations()
{
    m_iNumAnimations = m_pAIScene->mNumAnimations;

    m_vecCurrentKeyFrameIndices.resize(m_iNumAnimations);

    for (size_t i = 0; i < m_iNumAnimations; ++i)
    {
        CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], this, m_vecCurrentKeyFrameIndices[i]);

        if (nullptr == pAnimation)
            return E_FAIL;

        m_vecAnimations.push_back(pAnimation);
    }

    return S_OK;
}
#pragma endregion

#pragma region Save 용도 Ready 함수들
HRESULT CModel::Ready_Bones_Save(const aiNode* _pAINode, HANDLE _pFile, _int _iParentBoneIndex)
{
    DWORD       dwByte = { 0 };
    CBone* pBone = CBone::Create_SaveData(_pAINode, _pFile, _iParentBoneIndex);
    if (nullptr == pBone)
        return E_FAIL;

    m_vecBones.push_back(pBone);

    _int iParentBoneIndex = (_int)(m_vecBones.size() - 1);

    _uint iNumChildren = _pAINode->mNumChildren;
    WriteFile(_pFile, &iNumChildren, sizeof(_uint), &dwByte, nullptr);


    for (size_t i = 0; i < iNumChildren; ++i)
    {
        Ready_Bones_Save(_pAINode->mChildren[i], _pFile, iParentBoneIndex);
    }

    return S_OK;
}

HRESULT CModel::Ready_Meshes_Save(_fmatrix _PreTransformMatrix, HANDLE _pFile)
{
    DWORD       dwByte = { 0 };
    m_iNumMeshes = m_pAIScene->mNumMeshes;
    WriteFile(_pFile, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);

    for (size_t i = 0; i < m_iNumMeshes; ++i)
    {
        CMesh* pMesh = CMesh::CreateSaveData(m_pDevice, m_pContext, m_eModelType, this, m_pAIScene->mMeshes[i], _pFile, _PreTransformMatrix);
        if (nullptr == pMesh)
            return E_FAIL;

        m_vecMeshes.push_back(pMesh);
    }
    return S_OK;
}

HRESULT CModel::Ready_Materials_Save(const _char* _pModelFilePath, HANDLE _pFile)
{
    DWORD       dwByte = {};
    m_iNumMaterials = m_pAIScene->mNumMaterials;
    WriteFile(_pFile, &m_iNumMaterials, sizeof(_uint), &dwByte, nullptr);

    for (size_t i = 0; i < m_iNumMaterials; ++i)
    {
        CMaterial* pAIMaterial = CMaterial::Create_SaveData(m_pDevice, m_pContext, m_pAIScene->mMaterials[i], _pFile, _pModelFilePath);
        if (nullptr == pAIMaterial)
            return E_FAIL;

        m_vecMaterials.push_back(pAIMaterial);
    }

    return S_OK;
}

HRESULT CModel::Ready_Animations_Save(HANDLE _pFile)
{
    DWORD       dwByte = {};
    m_iNumAnimations = m_pAIScene->mNumAnimations;
    WriteFile(_pFile, &m_iNumAnimations, sizeof(_uint), &dwByte, nullptr);

    m_vecCurrentKeyFrameIndices.resize(m_iNumAnimations);

    for (size_t i = 0; i < m_iNumAnimations; ++i)
    {
        CAnimation* pAnimation = CAnimation::Create_Save(m_pAIScene->mAnimations[i], this, _pFile, m_vecCurrentKeyFrameIndices[i]);

        if (nullptr == pAnimation)
            return E_FAIL;

        m_vecAnimations.push_back(pAnimation);
    }

    return S_OK;
}
#pragma endregion

#pragma region Load 용도 Ready 함수들
HRESULT CModel::Ready_Bones_Load(HANDLE _pFile, _int _iParentBoneIndex)
{
    DWORD       dwByte = { 0 };
    CBone* pBone = CBone::Create_LoadData(_pFile, _iParentBoneIndex);
    if (nullptr == pBone)
        return E_FAIL;

    m_vecBones.push_back(pBone);

    _int iParentBoneIndex = (_int)(m_vecBones.size() - 1);

    _uint iNumChildren = 0;
    ReadFile(_pFile, &iNumChildren, sizeof(_uint), &dwByte, nullptr);

    for (size_t i = 0; i < iNumChildren; ++i)
    {
        Ready_Bones_Load(_pFile, iParentBoneIndex);
    }

    return S_OK;
}

HRESULT CModel::Ready_Meshes_Load(_fmatrix _PreTransformMatrix, HANDLE _pFile)
{
    DWORD       dwByte = { 0 };
    ReadFile(_pFile, &m_iNumMeshes, sizeof(_uint), &dwByte, nullptr);

    for (size_t i = 0; i < m_iNumMeshes; ++i)
    {
        CMesh* pMesh = CMesh::CreateLoadData(m_pDevice, m_pContext, m_eModelType, this, _pFile, _PreTransformMatrix);
        if (nullptr == pMesh)
            return E_FAIL;

        m_vecMeshes.push_back(pMesh);
    }
    return S_OK;
}

HRESULT CModel::Ready_Materials_Load(const _char* _pModelFilePath, HANDLE _pFile)
{
    DWORD       dwByte = {};
    //m_iNumMaterials = m_pAIScene->mNumMaterials;
    _uint iNumMaterials = 0;
    ReadFile(_pFile, &iNumMaterials, sizeof(_uint), &dwByte, nullptr);

    for (size_t i = 0; i < iNumMaterials; ++i)
    {
        CMaterial* pAIMaterial = CMaterial::Create_LoadData(m_pDevice, m_pContext, _pFile, _pModelFilePath);
        if (nullptr == pAIMaterial)
            return E_FAIL;

        m_vecMaterials.push_back(pAIMaterial);
    }

    return S_OK;
}

HRESULT CModel::Ready_Animations_Load(HANDLE _pFile)
{
    DWORD       dwByte = {};

    _uint iNumAnimations = 0;
    ReadFile(_pFile, &iNumAnimations, sizeof(_uint), &dwByte, nullptr);

    m_vecCurrentKeyFrameIndices.resize(iNumAnimations);

    for (size_t i = 0; i < iNumAnimations; ++i)
    {
        CAnimation* pAnimation = CAnimation::Create_Load(this, _pFile, m_vecCurrentKeyFrameIndices[i]);

        if (nullptr == pAnimation)
            return E_FAIL;

        m_vecAnimations.push_back(pAnimation);
    }

    return S_OK;
}

#pragma endregion

void CModel::Set_AnimationSpeed(_float _fAnimSpeed, _uint _iAnimIndex)
{
    m_vecAnimations[_iAnimIndex - 1]->Set_AnimationSpeed(_fAnimSpeed);
}

_bool CModel::IsAnimOnceDone() const
{
    if (m_iCurrentAnimIndex >= m_vecAnimations.size())
        return true;

    return m_vecAnimations[m_iCurrentAnimIndex]->IsAnimFinished();
}

CModel* CModel::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* _ModelFilePath, MODEL _eModelType, _fmatrix _PreTransformMatrix)
{
    CModel* pInstance = new CModel(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype(_eModelType, _ModelFilePath, _PreTransformMatrix)))
    {
        MSG_BOX("Failed To Created : CModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CModel* CModel::Create_ForSave(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* _ModelFilePath, HWND _pHwnd, const _tchar* _SaveDataFilePath, MODEL _eModelType, _fmatrix _PreTransformMatrix)
{
    CModel* pInstance = new CModel(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype_SaveData(_eModelType, _pHwnd, _ModelFilePath, _SaveDataFilePath, _PreTransformMatrix)))
    {
        MSG_BOX("Failed To Loaded : CModel Save");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CModel* CModel::Create_ForLoad(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _char* _ModelFilePath, HWND _pHwnd, const _tchar* _LoadDataFilePath, MODEL _eModelType, _fmatrix _PreTransformMatrix)
{
    CModel* pInstance = new CModel(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype_LoadData(_eModelType, _pHwnd, _ModelFilePath, _LoadDataFilePath, _PreTransformMatrix)))
    {
        MSG_BOX("Failed To Loaded : CModel Load");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CModel::Clone(void* _pArg)
{
    CModel* pInstance = new CModel(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Created : CModel");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CModel::Free()
{
    __super::Free();

    for (auto& pBone : m_vecBones)
        Safe_Release(pBone);
    m_vecBones.clear();

    for (auto& pMesh : m_vecMeshes)
        Safe_Release(pMesh);
    m_vecMeshes.clear();

    for (auto& pMaterial : m_vecMaterials)
        Safe_Release(pMaterial);
    m_vecMaterials.clear();

    for (auto& pAnimation : m_vecAnimations)
        Safe_Release(pAnimation);
    m_vecAnimations.clear();
}
