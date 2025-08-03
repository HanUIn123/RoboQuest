#include "Mesh.h"
#include "Shader.h"
#include "Bone.h"

CMesh::CMesh(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CVIBuffer{_pDevice, _pContext}
{
}

CMesh::CMesh(const CMesh& _Prototype)
    : CVIBuffer(_Prototype)
{
}

HRESULT CMesh::Initialize_Prototype(CModel::MODEL _eModelType, CModel* _pModel, const aiMesh* _pAIMesh, _fmatrix _PreTransformMatrix)
{
    //strcpy_s(m_szName, _pAIMesh->mName.data);

    m_iMaterialIndex = _pAIMesh->mMaterialIndex;
    m_iNumFaces = _pAIMesh->mNumFaces;
    m_iNumVertices = _pAIMesh->mNumVertices;
    m_iNumIndices = _pAIMesh->mNumFaces * 3;
    m_iIndexStride = 4;
    m_iNumVertexBuffers = 1;
    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEX_BUFFER 
    HRESULT     hr = CModel::MODEL_NONANIM == _eModelType ? Ready_VertexBuffer_ForNonAnim(_pAIMesh, _PreTransformMatrix) : Ready_VertexBuffer_ForAnim(_pModel, _pAIMesh);
    if (FAILED(hr))
        return E_FAIL;
#pragma endregion 

#pragma region INDEX_BUFFER 
    ZeroMemory(&m_tagBufferDesc, sizeof(m_tagBufferDesc));
    m_tagBufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
    m_tagBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_tagBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_tagBufferDesc.StructureByteStride = m_iIndexStride;
    m_tagBufferDesc.CPUAccessFlags = 0;
    m_tagBufferDesc.MiscFlags = 0;

    _uint* pIndices = new _uint[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

    _uint iIndicesIndex = { 0 };

    //m_pIndices = new _uint[_pAIMesh->mNumFaces * 3];

    for (_uint i = 0; i < _pAIMesh->mNumFaces; ++i)
    {
        pIndices[iIndicesIndex++] = _pAIMesh->mFaces[i].mIndices[0];
        pIndices[iIndicesIndex++] = _pAIMesh->mFaces[i].mIndices[1];
        pIndices[iIndicesIndex++] = _pAIMesh->mFaces[i].mIndices[2];
    }

    ZeroMemory(&m_tagInitialData, sizeof(m_tagInitialData));
    m_tagInitialData.pSysMem = pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;
    Safe_Delete_Array(pIndices);
#pragma endregion 

    return S_OK;
}

HRESULT CMesh::Initialize_Prototype_SaveData(CModel::MODEL _eModelType, CModel* _pModel, const aiMesh* _pAIMesh, HANDLE _pSaveData, _fmatrix _PreTransformMatrix)
{
    DWORD dwByte = 0;

    m_iMaterialIndex = _pAIMesh->mMaterialIndex;
    WriteFile(_pSaveData, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);

    m_iNumFaces = _pAIMesh->mNumFaces;
    WriteFile(_pSaveData, &m_iNumFaces, sizeof(_uint), &dwByte, nullptr);

    m_iNumVertices = _pAIMesh->mNumVertices;
    WriteFile(_pSaveData, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);

    m_iNumIndices = _pAIMesh->mNumFaces * 3;
    WriteFile(_pSaveData, &m_iNumIndices, sizeof(_uint), &dwByte, nullptr);

    m_iIndexStride = 4;
    m_iNumVertexBuffers = 1;
    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;



#pragma region VERTEX_BUFFER 
    HRESULT     hr = CModel::MODEL_NONANIM == _eModelType ? Ready_VertexBuffer_ForNonAnim_SaveData(_pSaveData,_pAIMesh, _PreTransformMatrix)
        : Ready_VertexBuffer_ForAnim_SaveData(_pSaveData, _pModel, _pAIMesh);

    if (FAILED(hr))
        return E_FAIL;
#pragma endregion 

#pragma region INDEX_BUFFER 
    ZeroMemory(&m_tagBufferDesc, sizeof(m_tagBufferDesc));
    m_tagBufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
    m_tagBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_tagBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_tagBufferDesc.StructureByteStride = m_iIndexStride;
    m_tagBufferDesc.CPUAccessFlags = 0;
    m_tagBufferDesc.MiscFlags = 0;

    _uint* pIndices = new _uint[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

    _uint iIndicesIndex = { 0 };

    for (_uint i = 0; i < _pAIMesh->mNumFaces; ++i)
    {
        pIndices[iIndicesIndex++] = _pAIMesh->mFaces[i].mIndices[0];
        _uint iTemp = _pAIMesh->mFaces[i].mIndices[0];
        WriteFile(_pSaveData, &iTemp, sizeof(_uint), &dwByte, nullptr);

        pIndices[iIndicesIndex++] = _pAIMesh->mFaces[i].mIndices[1];
        iTemp = _pAIMesh->mFaces[i].mIndices[1];
        WriteFile(_pSaveData, &iTemp, sizeof(_uint), &dwByte, nullptr);

        pIndices[iIndicesIndex++] = _pAIMesh->mFaces[i].mIndices[2];
        iTemp = _pAIMesh->mFaces[i].mIndices[2];
        WriteFile(_pSaveData, &iTemp, sizeof(_uint), &dwByte, nullptr);
    }

    ZeroMemory(&m_tagInitialData, sizeof(m_tagInitialData));
    m_tagInitialData.pSysMem = pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;
    Safe_Delete_Array(pIndices);
#pragma endregion 

    return S_OK;
}

HRESULT CMesh::Initialize_Prototype_LoadData(CModel::MODEL _eModelType, CModel* _pModel, HANDLE _pLoadData, _fmatrix _PreTransformMatrix)
{
    DWORD dwByte = 0;

    ReadFile(_pLoadData, &m_iMaterialIndex, sizeof(_uint), &dwByte, nullptr);

    ReadFile(_pLoadData, &m_iNumFaces, sizeof(_uint), &dwByte, nullptr);

    ReadFile(_pLoadData, &m_iNumVertices, sizeof(_uint), &dwByte, nullptr);

    ReadFile(_pLoadData, &m_iNumIndices, sizeof(_uint), &dwByte, nullptr);

    m_iIndexStride = 4;
    m_iNumVertexBuffers = 1;
    m_eIndexFormat = DXGI_FORMAT_R32_UINT;
    m_ePrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;


#pragma region VERTEX_BUFFER 
    HRESULT     hr = CModel::MODEL_NONANIM == _eModelType ? Ready_VertexBuffer_ForNonAnim_LoadData(_pLoadData, _PreTransformMatrix)
        : Ready_VertexBuffer_ForAnim_LoadData(_pLoadData, _pModel);

    if (FAILED(hr))
        return E_FAIL;
#pragma endregion 

#pragma region INDEX_BUFFER 
    ZeroMemory(&m_tagBufferDesc, sizeof(m_tagBufferDesc));
    m_tagBufferDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
    m_tagBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_tagBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    m_tagBufferDesc.StructureByteStride = m_iIndexStride;
    m_tagBufferDesc.CPUAccessFlags = 0;
    m_tagBufferDesc.MiscFlags = 0;

    _uint* pIndices = new _uint[m_iNumIndices];
    ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

    _uint iIndicesIndex = { 0 };

    for (_uint i = 0; i < m_iNumFaces; ++i)
    {
        _uint iIndices1 = 0;
        ReadFile(_pLoadData, &iIndices1, sizeof(_uint), &dwByte, nullptr);
        pIndices[iIndicesIndex++] = iIndices1;

        _uint iIndices2 = 0;
        ReadFile(_pLoadData, &iIndices2, sizeof(_uint), &dwByte, nullptr);
        pIndices[iIndicesIndex++] = iIndices2;

        _uint iIndices3 = 0;
        ReadFile(_pLoadData, &iIndices3, sizeof(_uint), &dwByte, nullptr);
        pIndices[iIndicesIndex++] = iIndices3;
    }

    ZeroMemory(&m_tagInitialData, sizeof(m_tagInitialData));
    m_tagInitialData.pSysMem = pIndices;

    if (FAILED(__super::Create_Buffer(&m_pIB)))
        return E_FAIL;
    Safe_Delete_Array(pIndices);
#pragma endregion 

    return S_OK;
}

HRESULT CMesh::Initialize(void* _pArg)
{
    return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(CShader* _pShader, const _char* _pConstantName, const vector<class CBone*>& _vecBones)
{
    for (size_t i = 0; i < m_iNumBones; ++i)
    {
        XMStoreFloat4x4(&m_BoneMatrices[i], XMLoadFloat4x4(&m_vecOffsetMatrices[i]) * _vecBones[m_vecBoneIndices[i]]->Get_CombinedTransformationMatrix());
    }

    return _pShader->Bind_Matrices(_pConstantName, m_BoneMatrices, m_iNumBones);
}

HRESULT CMesh::Save_AnimMesh(const aiMesh* _pAIMesh)
{
    const _tchar* pStrMeshFileName = L"../Bin/DataFiles/UnitFiles/UnitMeshData.txt";
    HANDLE hFile = CreateFile(pStrMeshFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        MSG_BOX("Failed To Create MeshFile!");
        return E_FAIL;
    }

    DWORD dwByte = 0;
    WriteFile(hFile, &m_iMaterialIndex, sizeof(m_iMaterialIndex ), &dwByte, nullptr);
    WriteFile(hFile, &m_iNumFaces, sizeof(m_iNumFaces), &dwByte, nullptr);
    WriteFile(hFile, &m_iNumVertices, sizeof(m_iNumVertices), &dwByte, nullptr);
    WriteFile(hFile, &m_iNumIndices, sizeof(m_iNumVertices), &dwByte, nullptr);

    //  Anim 쪽 
    for (_uint i = 0; i < m_iNumVertices; ++i)
    {
        //  _pAIMesh->mVertices
        WriteFile(hFile, &_pAIMesh->mVertices[i], sizeof(_float3), &dwByte, nullptr);

        //  _pAIMesh->mNormals
        WriteFile(hFile, &_pAIMesh->mNormals[i], sizeof(_float3), &dwByte, nullptr);

        //  _pAIMesh->mTextureCoords
        WriteFile(hFile, &_pAIMesh->mTextureCoords[0][i], sizeof(_float2), &dwByte, nullptr);

        //  _pAIMesh->mTangents
        WriteFile(hFile, &_pAIMesh->mTangents[i], sizeof(_float3), &dwByte, nullptr);
    }

    //         _pAIMesh->mNumBones;
    m_iNumBones = _pAIMesh->mNumBones;
    WriteFile(hFile, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);

    // 인덱스 배열 동적 할당
    _uint* pIndices = new _uint[_pAIMesh->mNumFaces * 3];
    ZeroMemory(pIndices, sizeof(_uint) * _pAIMesh->mNumFaces * 3);

    _uint iIndicesIndex = 0;  // 인덱스 배열에 대한 인덱스

    // 면별로 인덱스를 pIndices 배열에 복사
    for (_uint i = 0; i < _pAIMesh->mNumFaces; ++i)
    {
        pIndices[iIndicesIndex++] = _pAIMesh->mFaces[i].mIndices[0];
        pIndices[iIndicesIndex++] = _pAIMesh->mFaces[i].mIndices[1];
        pIndices[iIndicesIndex++] = _pAIMesh->mFaces[i].mIndices[2];
    }

    // pIndices 배열을 파일에 저장
    for (_uint i = 0; i < _pAIMesh->mNumFaces * 3; ++i)
    {
        WriteFile(hFile, &pIndices[i], sizeof(_uint), &dwByte, nullptr);
    }

    // 동적 할당된 메모리 해제
    //delete[] pIndices;
    Safe_Delete_Array(pIndices);

    
    //     _pAIMesh->mNumBones;
    _uint iNumBones = _pAIMesh->mNumBones;
    WriteFile(hFile, &iNumBones, sizeof(_uint), &dwByte, nullptr);

    VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

    //     _pAIMesh->mBones[i];
    for (_uint i = 0; i < iNumBones; ++i)
    {
        aiBone* pAIBone = _pAIMesh->mBones[i];
        const char* szBoneName = pAIBone->mName.C_Str();

        //    _pAIMesh->mName.data;
        _uint boneNameLength = (_uint)strlen(pAIBone->mName.C_Str());
        WriteFile(hFile, &boneNameLength, sizeof(_uint), &dwByte, nullptr);
        WriteFile(hFile, szBoneName, boneNameLength, &dwByte, nullptr);

        //     pAIBone->mOffsetMatrix
        WriteFile(hFile, &pAIBone->mOffsetMatrix, sizeof(_float4x4), &dwByte, nullptr);

        //      pAIBone->mWeights
        _uint iNumWeights = pAIBone->mNumWeights;
        WriteFile(hFile, &iNumWeights, sizeof(_uint), &dwByte, nullptr);

        for (_uint j = 0; j < iNumWeights; ++j)
        {
            aiVertexWeight	AIWeight = pAIBone->mWeights[j];
            _uint vertexId = AIWeight.mVertexId;

            WriteFile(hFile, &pVertices[vertexId].vBlendIndex.x, sizeof(_uint), &dwByte, nullptr);
            WriteFile(hFile, &pVertices[vertexId].vBlendWeight.x, sizeof(_uint), &dwByte, nullptr);
            WriteFile(hFile, &pVertices[vertexId].vBlendIndex.y, sizeof(_uint), &dwByte, nullptr);
            WriteFile(hFile, &pVertices[vertexId].vBlendWeight.y, sizeof(_uint), &dwByte, nullptr);
            WriteFile(hFile, &pVertices[vertexId].vBlendIndex.z, sizeof(_uint), &dwByte, nullptr);
            WriteFile(hFile, &pVertices[vertexId].vBlendWeight.z, sizeof(_uint), &dwByte, nullptr);
            WriteFile(hFile, &pVertices[vertexId].vBlendIndex.w, sizeof(_uint), &dwByte, nullptr);
            WriteFile(hFile, &pVertices[vertexId].vBlendWeight.w, sizeof(_uint), &dwByte, nullptr);
        }
    }

    Safe_Delete_Array(pVertices);
    CloseHandle(hFile);

    return S_OK;
}

#pragma region 기존 Anim or NonAnim.
HRESULT CMesh::Ready_VertexBuffer_ForNonAnim(const aiMesh* _pAIMesh, _fmatrix _PreTransformMatrix)
{
    // NonAnim 일 때는 그냥 VTXMESH 로. 이용.
    m_iVertexStride = sizeof(VTXMESH);
    ZeroMemory(&m_tagBufferDesc, sizeof m_tagBufferDesc);
    m_tagBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    m_tagBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_tagBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_tagBufferDesc.StructureByteStride = m_iVertexStride;
    m_tagBufferDesc.CPUAccessFlags = 0;
    m_tagBufferDesc.MiscFlags = 0;

    m_vertexPos = new XMVECTOR[m_iNumVertices];

    VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

    for (size_t i = 0; i < m_iNumVertices; i++)
    {
        memcpy(&pVertices[i].vPosition, &_pAIMesh->mVertices[i], sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), _PreTransformMatrix));

        memcpy(&pVertices[i].vNormal, &_pAIMesh->mNormals[i], sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), _PreTransformMatrix));

        memcpy(&pVertices[i].vTexCoord, &_pAIMesh->mTextureCoords[0][i], sizeof(_float2));
        memcpy(&pVertices[i].vTangent, &_pAIMesh->mTangents[i], sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), _PreTransformMatrix));


        m_vertexPos[i] = XMLoadFloat3(&pVertices[i].vPosition);
    }

    ZeroMemory(&m_tagInitialData, sizeof m_tagInitialData);
    m_tagInitialData.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

    return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_ForAnim(CModel* _pModel, const aiMesh* _pAIMesh)
{
    m_iVertexStride = sizeof(VTXANIMMESH);
    ZeroMemory(&m_tagBufferDesc, sizeof m_tagBufferDesc);
    m_tagBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    m_tagBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_tagBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_tagBufferDesc.StructureByteStride = m_iVertexStride;
    m_tagBufferDesc.CPUAccessFlags = 0;
    m_tagBufferDesc.MiscFlags = 0;

    m_vertexPos = new XMVECTOR[m_iNumVertices];

    VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

    for (size_t i = 0; i < m_iNumVertices; i++)
    {
        memcpy(&pVertices[i].vPosition, &_pAIMesh->mVertices[i], sizeof(_float3));
        memcpy(&pVertices[i].vNormal, &_pAIMesh->mNormals[i], sizeof(_float3));
        memcpy(&pVertices[i].vTexCoord, &_pAIMesh->mTextureCoords[0][i], sizeof(_float2));
        memcpy(&pVertices[i].vTangent, &_pAIMesh->mTangents[i], sizeof(_float3));

        m_vertexPos[i] = XMLoadFloat3(&pVertices[i].vPosition);
    }

    m_iNumBones = _pAIMesh->mNumBones;

    _float4x4	OffsetMatrix;
    if (m_iNumBones == 0)
    {
        m_iNumBones = 1;

        const _char* szNonAninName = _pAIMesh->mName.data;

        int boneIndex = _pModel->Get_BoneIndex(szNonAninName);

        _matrix		IndenTransformMatrix = XMMatrixIdentity();

        memcpy(&OffsetMatrix, &IndenTransformMatrix, sizeof(_float4x4));

        XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

        m_vecOffsetMatrices.push_back(OffsetMatrix);

        m_vecBoneIndices.push_back(boneIndex);
    }
    else
    {
        for (size_t i = 0; i < m_iNumBones; i++)
        {
            aiBone* pAIBone = _pAIMesh->mBones[i];

            memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

            XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

            m_vecOffsetMatrices.push_back(OffsetMatrix);

            m_vecBoneIndices.push_back(_pModel->Get_BoneIndex(pAIBone->mName.data));

            for (size_t j = 0; j < pAIBone->mNumWeights; j++)
            {
                aiVertexWeight	AIWeight = pAIBone->mWeights[j];

                if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.x)
                {
                    pVertices[AIWeight.mVertexId].vBlendIndex.x = (_uint)i;
                    pVertices[AIWeight.mVertexId].vBlendWeight.x = AIWeight.mWeight;
                }

                else if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.y)
                {
                    pVertices[AIWeight.mVertexId].vBlendIndex.y = (_uint)i;
                    pVertices[AIWeight.mVertexId].vBlendWeight.y = AIWeight.mWeight;
                }

                else if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.z)
                {
                    pVertices[AIWeight.mVertexId].vBlendIndex.z = (_uint)i;
                    pVertices[AIWeight.mVertexId].vBlendWeight.z = AIWeight.mWeight;
                }

                else if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.w)
                {
                    pVertices[AIWeight.mVertexId].vBlendIndex.w = (_uint)i;
                    pVertices[AIWeight.mVertexId].vBlendWeight.w = AIWeight.mWeight;
                }   
            }
          
        }
    }

    // 루프는 패스해버리고 m_iNumBones 를 1개로 강제로 늘리자.
    // 실제 뼈에 영향을 미치는 인덱스를 추가해야함. 
    // m_vecBoneIndices 에 push_back 까지.

    ZeroMemory(&m_tagInitialData, sizeof m_tagInitialData);
    m_tagInitialData.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

    return S_OK;
}
#pragma endregion

#pragma region Save 용 Anim or NonAnim
HRESULT CMesh::Ready_VertexBuffer_ForNonAnim_SaveData(HANDLE _pSaveData, const aiMesh* _pAIMesh, _fmatrix _PreTransformMatrix)
{
    m_iVertexStride = sizeof(VTXMESH);
    ZeroMemory(&m_tagBufferDesc, sizeof m_tagBufferDesc);
    m_tagBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    m_tagBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_tagBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_tagBufferDesc.StructureByteStride = m_iVertexStride;
    m_tagBufferDesc.CPUAccessFlags = 0;
    m_tagBufferDesc.MiscFlags = 0;

    m_vertexPos = new XMVECTOR[m_iNumVertices];

    VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

    // 파일 쓰기
    DWORD dwByte = 0;

    for (size_t i = 0; i < m_iNumVertices; i++)
    {
        memcpy(&pVertices[i].vPosition, &_pAIMesh->mVertices[i], sizeof(_float3));
        WriteFile(_pSaveData, &_pAIMesh->mVertices[i], sizeof(_float3),  &dwByte, nullptr);
        XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), _PreTransformMatrix));


        memcpy(&pVertices[i].vNormal, &_pAIMesh->mNormals[i], sizeof(_float3));
        WriteFile(_pSaveData, &_pAIMesh->mNormals[i], sizeof(_float3), &dwByte, nullptr);
        XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), _PreTransformMatrix));

        memcpy(&pVertices[i].vTexCoord, &_pAIMesh->mTextureCoords[0][i], sizeof(_float2));
        WriteFile(_pSaveData, &_pAIMesh->mTextureCoords[0][i], sizeof(_float2), &dwByte, nullptr);

        memcpy(&pVertices[i].vTangent, &_pAIMesh->mTangents[i], sizeof(_float3));
        XMStoreFloat3(&pVertices[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), _PreTransformMatrix));
        WriteFile(_pSaveData, &_pAIMesh->mTangents[i], sizeof(_float3), &dwByte, nullptr);

        m_vertexPos[i] = XMLoadFloat3(&pVertices[i].vPosition);
    }

    ZeroMemory(&m_tagInitialData, sizeof m_tagInitialData);
    m_tagInitialData.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

    return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_ForAnim_SaveData(HANDLE _pSaveData, CModel* _pModel, const aiMesh* _pAIMesh)
{
    m_iVertexStride = sizeof(VTXANIMMESH);
    ZeroMemory(&m_tagBufferDesc, sizeof m_tagBufferDesc);
    m_tagBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    m_tagBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_tagBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_tagBufferDesc.StructureByteStride = m_iVertexStride;
    m_tagBufferDesc.CPUAccessFlags = 0;
    m_tagBufferDesc.MiscFlags = 0;

    m_vertexPos = new XMVECTOR[m_iNumVertices];

    VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

    // 파일 쓰기
    DWORD dwByte = 0;

    for (size_t i = 0; i < m_iNumVertices; i++)
    {
        memcpy(&pVertices[i].vPosition, &_pAIMesh->mVertices[i], sizeof(_float3));
        WriteFile(_pSaveData, &_pAIMesh->mVertices[i], sizeof(_float3), &dwByte, nullptr);

        memcpy(&pVertices[i].vNormal, &_pAIMesh->mNormals[i], sizeof(_float3));
        WriteFile(_pSaveData, &_pAIMesh->mNormals[i], sizeof(_float3), &dwByte, nullptr);

        memcpy(&pVertices[i].vTexCoord, &_pAIMesh->mTextureCoords[0][i], sizeof(_float2));
        WriteFile(_pSaveData, &_pAIMesh->mTextureCoords[0][i], sizeof(_float2), &dwByte, nullptr);

        memcpy(&pVertices[i].vTangent, &_pAIMesh->mTangents[i], sizeof(_float3));
        WriteFile(_pSaveData, &_pAIMesh->mTangents[i], sizeof(_float3), &dwByte, nullptr);

        m_vertexPos[i] = XMLoadFloat3(&pVertices[i].vPosition);
    }

    m_iNumBones = _pAIMesh->mNumBones;
    WriteFile(_pSaveData, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);

    _float4x4	OffsetMatrix;
    if (m_iNumBones == 0)
    {
        m_iNumBones = 1;

        const _char* szNonAnimName = _pAIMesh->mName.data;
        _uint boneNameLength = (_uint)strlen(szNonAnimName);
        WriteFile(_pSaveData, &boneNameLength, sizeof(_uint), &dwByte, nullptr);

        for (_uint i = 0; i < boneNameLength; ++i)
        {
            WriteFile(_pSaveData, &szNonAnimName[i], sizeof(_char), &dwByte, nullptr);
        }

        int boneIndex = _pModel->Get_BoneIndex(szNonAnimName);
        WriteFile(_pSaveData, &boneIndex, sizeof(int), &dwByte, nullptr);

        _matrix		IndenTransformMatrix = XMMatrixIdentity();

        memcpy(&OffsetMatrix, &IndenTransformMatrix, sizeof(_float4x4));
        XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));
        WriteFile(_pSaveData, &OffsetMatrix, sizeof(_float4x4), &dwByte, nullptr);

        m_vecOffsetMatrices.push_back(OffsetMatrix);
        m_vecBoneIndices.push_back(boneIndex);

    }
    else
    {
        for (size_t i = 0; i < m_iNumBones; i++)
        {
            aiBone* pAIBone = _pAIMesh->mBones[i];

            memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
            XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));
            WriteFile(_pSaveData, &OffsetMatrix, sizeof(_float4x4), &dwByte, nullptr);

            m_vecOffsetMatrices.push_back(OffsetMatrix);

            _uint iTemp = _pModel->Get_BoneIndex(pAIBone->mName.data);
            WriteFile(_pSaveData, &iTemp, sizeof(_uint), &dwByte, nullptr);
            m_vecBoneIndices.push_back(_pModel->Get_BoneIndex(pAIBone->mName.data));
            
            _uint iNumWeights = pAIBone->mNumWeights;
            WriteFile(_pSaveData, &iNumWeights, sizeof(_uint), &dwByte, nullptr);
            for (size_t j = 0; j < iNumWeights; j++)
            {
                _uint iBlendCase = 0;

                aiVertexWeight	AIWeight = pAIBone->mWeights[j];
                WriteFile(_pSaveData, &AIWeight.mVertexId, sizeof(_uint), &dwByte, nullptr);
                // AIWeight : 이새끼의 [mVertexId] 를 저장. 하고나서,
                // iBlendCase 라고~ 내가 만든 구분문 변수를 저장 해줌. 

                if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.x)
                {
                    iBlendCase = 1;
                    WriteFile(_pSaveData, &iBlendCase, sizeof(_uint), &dwByte, nullptr);

                    pVertices[AIWeight.mVertexId].vBlendIndex.x = (_uint)i;
                    WriteFile(_pSaveData, &pVertices[AIWeight.mVertexId].vBlendIndex.x, sizeof(_uint), &dwByte, nullptr);
                    pVertices[AIWeight.mVertexId].vBlendWeight.x = AIWeight.mWeight;
                    WriteFile(_pSaveData, &pVertices[AIWeight.mVertexId].vBlendWeight.x, sizeof(float), &dwByte, nullptr);
                }

                else if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.y)
                {
                    iBlendCase = 2;
                    WriteFile(_pSaveData, &iBlendCase, sizeof(_uint), &dwByte, nullptr);

                    pVertices[AIWeight.mVertexId].vBlendIndex.y = (_uint)i;
                    WriteFile(_pSaveData, &pVertices[AIWeight.mVertexId].vBlendIndex.y, sizeof(_uint), &dwByte, nullptr);
                    pVertices[AIWeight.mVertexId].vBlendWeight.y = AIWeight.mWeight;
                    WriteFile(_pSaveData, &pVertices[AIWeight.mVertexId].vBlendWeight.y, sizeof(float), &dwByte, nullptr);
                }

                else if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.z)
                {
                    iBlendCase = 3;
                    WriteFile(_pSaveData, &iBlendCase, sizeof(_uint), &dwByte, nullptr);

                    pVertices[AIWeight.mVertexId].vBlendIndex.z = (_uint)i;
                    WriteFile(_pSaveData, &pVertices[AIWeight.mVertexId].vBlendIndex.z, sizeof(_uint), &dwByte, nullptr);
                    pVertices[AIWeight.mVertexId].vBlendWeight.z = AIWeight.mWeight;
                    WriteFile(_pSaveData, &pVertices[AIWeight.mVertexId].vBlendWeight.z, sizeof(float), &dwByte, nullptr);
                }

                else if (0.f == pVertices[AIWeight.mVertexId].vBlendWeight.w)
                {
                    iBlendCase = 4;
                    WriteFile(_pSaveData, &iBlendCase, sizeof(_uint), &dwByte, nullptr);

                    pVertices[AIWeight.mVertexId].vBlendIndex.w = (_uint)i;
                    WriteFile(_pSaveData, &pVertices[AIWeight.mVertexId].vBlendIndex.w, sizeof(_uint), &dwByte, nullptr);
                    pVertices[AIWeight.mVertexId].vBlendWeight.w = AIWeight.mWeight;
                    WriteFile(_pSaveData, &pVertices[AIWeight.mVertexId].vBlendWeight.w, sizeof(float), &dwByte, nullptr);
                }
            }
        }
    }

    // 루프는 패스해버리고 m_iNumBones 를 1개로 강제로 늘리자.
    // 실제 뼈에 영향을 미치는 인덱스를 추가해야함. 
    // m_vecBoneIndices 에 push_back 까지.

    ZeroMemory(&m_tagInitialData, sizeof m_tagInitialData);
    m_tagInitialData.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

    return S_OK;
}
#pragma endregion

#pragma region Load 용 Anim or NonAnim
HRESULT CMesh::Ready_VertexBuffer_ForNonAnim_LoadData(HANDLE _pLoadData, _fmatrix _PreTransformMatrix)
{
    m_iVertexStride = sizeof(VTXMESH);
    ZeroMemory(&m_tagBufferDesc, sizeof m_tagBufferDesc);
    m_tagBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    m_tagBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_tagBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_tagBufferDesc.StructureByteStride = m_iVertexStride;
    m_tagBufferDesc.CPUAccessFlags = 0;
    m_tagBufferDesc.MiscFlags = 0;

    m_vertexPos = new XMVECTOR[m_iNumVertices];

    VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

    // 파일 쓰기
    DWORD dwByte = 0;

    for (size_t i = 0; i < m_iNumVertices; i++)
    {
        ReadFile(_pLoadData, &pVertices[i].vPosition, sizeof(_float3), &dwByte, nullptr);
        XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), _PreTransformMatrix));

        ReadFile(_pLoadData, &pVertices[i].vNormal, sizeof(_float3), &dwByte, nullptr);
        XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), _PreTransformMatrix));

        ReadFile(_pLoadData, &pVertices[i].vTexCoord, sizeof(_float2), &dwByte, nullptr);

        ReadFile(_pLoadData, &pVertices[i].vTangent, sizeof(_float3), &dwByte, nullptr);

        m_vertexPos[i] = XMLoadFloat3(&pVertices[i].vPosition);
    }

    ZeroMemory(&m_tagInitialData, sizeof m_tagInitialData);
    m_tagInitialData.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

    return S_OK;
}

HRESULT CMesh::Ready_VertexBuffer_ForAnim_LoadData(HANDLE _pLoadData, CModel* _pModel)
{
    m_iVertexStride = sizeof(VTXANIMMESH);
    ZeroMemory(&m_tagBufferDesc, sizeof m_tagBufferDesc);
    m_tagBufferDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    m_tagBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    m_tagBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    m_tagBufferDesc.StructureByteStride = m_iVertexStride;
    m_tagBufferDesc.CPUAccessFlags = 0;
    m_tagBufferDesc.MiscFlags = 0;

    m_vertexPos = new XMVECTOR[m_iNumVertices];

    VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
    ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

    // 파일 쓰기
    DWORD dwByte = 0;

    for (size_t i = 0; i < m_iNumVertices; i++)
    {
        ReadFile(_pLoadData, &pVertices[i].vPosition, sizeof(_float3), &dwByte, nullptr);

        ReadFile(_pLoadData, &pVertices[i].vNormal, sizeof(_float3), &dwByte, nullptr);

        ReadFile(_pLoadData, &pVertices[i].vTexCoord, sizeof(_float2), &dwByte, nullptr);

        ReadFile(_pLoadData, &pVertices[i].vTangent, sizeof(_float3), &dwByte, nullptr);

        m_vertexPos[i] = XMLoadFloat3(&pVertices[i].vPosition);
    }

    ReadFile(_pLoadData, &m_iNumBones, sizeof(_uint), &dwByte, nullptr);

    _float4x4	OffsetMatrix;
    if (m_iNumBones == 0)
    {
        m_iNumBones = 1;

        const _char* szNonAnimName[MAX_PATH] = {};
        _uint boneNameLength = 0;
        ReadFile(_pLoadData, &boneNameLength, sizeof(_uint), &dwByte, nullptr);

        for (_uint i = 0; i < boneNameLength; ++i)
        {
            ReadFile(_pLoadData, &szNonAnimName[i], sizeof(_char), &dwByte, nullptr);
        }

        int boneIndex = 0;
        ReadFile(_pLoadData, &boneIndex, sizeof(int), &dwByte, nullptr);

        ReadFile(_pLoadData, &OffsetMatrix, sizeof(_float4x4), &dwByte, nullptr);

        m_vecOffsetMatrices.push_back(OffsetMatrix);
        m_vecBoneIndices.push_back(boneIndex);
    }
    else
    {
        for (size_t i = 0; i < m_iNumBones; i++)
        {
            ReadFile(_pLoadData, &OffsetMatrix, sizeof(_float4x4), &dwByte, nullptr);

            m_vecOffsetMatrices.push_back(OffsetMatrix);

            _uint iTemp = 0;
            ReadFile(_pLoadData, &iTemp, sizeof(_uint), &dwByte, nullptr);
            m_vecBoneIndices.push_back(iTemp);

            _uint iNumWeights = 0;
            ReadFile(_pLoadData, &iNumWeights, sizeof(_uint), &dwByte, nullptr);

            // 불러온 iNumWeights 만큼. 루프돌면서, 
            for (size_t j = 0; j < iNumWeights; j++)
            {
                // AIWeight : 이새끼의 [mVertexId] 를 저장해놨던 거부터 불러서 mVertexId -> mVertexIndex 라고 내가 임의 변수 만듬.  
                // mVertexIndex 얘부터 불러와서~ iBlendCase (내가 만든 분기나누는 임시변수) 도 불러오고 
                // 두 값부터 다 불러오고 나서 그대로 다시 그 분기변수 대로, 리드파일 시작 .

                _uint mVertexIndex = 0;
                ReadFile(_pLoadData, &mVertexIndex, sizeof(_uint), &dwByte, nullptr);

                _uint iBlendCase = 0;
                ReadFile(_pLoadData, &iBlendCase, sizeof(_uint), &dwByte, nullptr);

                if (iBlendCase == 1)
                {
                    uint32_t iTemp = 0;
                    ReadFile(_pLoadData, &iTemp, sizeof(_uint), &dwByte, nullptr);
                    pVertices[mVertexIndex].vBlendIndex.x = iTemp;

                    float iTemp2 = 0;
                    ReadFile(_pLoadData, &iTemp2, sizeof(float), &dwByte, nullptr);
                    pVertices[mVertexIndex].vBlendWeight.x = iTemp2;
                }

                else if (iBlendCase == 2)
                {
                    uint32_t iTemp = 0;
                    ReadFile(_pLoadData, &iTemp, sizeof(_uint), &dwByte, nullptr);
                    pVertices[mVertexIndex].vBlendIndex.y = iTemp;

                    float iTemp2 = 0;
                    ReadFile(_pLoadData, &iTemp2, sizeof(float), &dwByte, nullptr);
                    pVertices[mVertexIndex].vBlendWeight.y = iTemp2;
                }

                else if (iBlendCase == 3)
                {
                    uint32_t iTemp = 0;
                    ReadFile(_pLoadData, &iTemp, sizeof(_uint), &dwByte, nullptr);
                    pVertices[mVertexIndex].vBlendIndex.z = iTemp;

                    float iTemp2 = 0;
                    ReadFile(_pLoadData, &iTemp2, sizeof(float), &dwByte, nullptr);
                    pVertices[mVertexIndex].vBlendWeight.z = iTemp2;
                }

                else if (iBlendCase == 4)
                {
                    uint32_t iTemp = 0;
                    ReadFile(_pLoadData, &iTemp, sizeof(_uint), &dwByte, nullptr);
                    pVertices[mVertexIndex].vBlendIndex.w = iTemp;

                    float iTemp2 = 0;
                    ReadFile(_pLoadData, &iTemp2, sizeof(float), &dwByte, nullptr);
                    pVertices[mVertexIndex].vBlendWeight.w = iTemp2;
                }
            }
        }
    }

    // 루프는 패스해버리고 m_iNumBones 를 1개로 강제로 늘리자.
    // 실제 뼈에 영향을 미치는 인덱스를 추가해야함. 
    // m_vecBoneIndices 에 push_back 까지.

    ZeroMemory(&m_tagInitialData, sizeof m_tagInitialData);
    m_tagInitialData.pSysMem = pVertices;

    if (FAILED(__super::Create_Buffer(&m_pVB)))
        return E_FAIL;

    Safe_Delete_Array(pVertices);

    return S_OK;
}
#pragma endregion

CMesh* CMesh::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CModel::MODEL _eModelType, CModel* _pModel, const aiMesh* _pAIMesh, _fmatrix _PreTransformMatrix)
{
    CMesh* pInstance = new CMesh(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype(_eModelType, _pModel, _pAIMesh, _PreTransformMatrix)))
    {
        MSG_BOX("Failed To Created : CMesh");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CMesh* CMesh::CreateSaveData(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CModel::MODEL _eModelType, CModel* _pModel, const aiMesh* _pAIMesh, HANDLE _pSaveFile, _fmatrix _PreTransformMatrix)
{
    CMesh* pInstance = new CMesh(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype_SaveData(_eModelType, _pModel, _pAIMesh, _pSaveFile, _PreTransformMatrix)))
    {
        MSG_BOX("Failed To Created : CMesh");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CMesh* CMesh::CreateLoadData(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, CModel::MODEL _eModelType, CModel* _pModel, HANDLE _pLoadFile, _fmatrix _PreTransformMatrix)
{
    CMesh* pInstance = new CMesh(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype_LoadData(_eModelType, _pModel, _pLoadFile, _PreTransformMatrix)))
    {
        MSG_BOX("Failed To Created : CMesh");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CVIBuffer* CMesh::Clone(void* _pArg)
{
    return nullptr;
}

void CMesh::Free()
{
    __super::Free();

    if (false == m_bClone)
    {
        Safe_Delete_Array(m_vertexPos);
        Safe_Delete_Array(m_pIndices);
    }
}
