#include "../Public/Camera.h"
#include "GameInstance.h"

CCamera::CCamera(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGameObject{ _pDevice, _pContext }
{
}

CCamera::CCamera(const CCamera& _Prototype)
    :CGameObject(_Prototype)
{
}

HRESULT CCamera::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CCamera::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    CAMERA_DESC* pDesc = static_cast<CAMERA_DESC*>(_pArg);

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pDesc->_vEye), 1.0f));
    m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&pDesc->_vAt), 1.0f));


    m_fFov = pDesc->fFov;
    m_fNear = pDesc->fNear;
    m_fFar = pDesc->fFar;
    _uint2  vViewPortSize = m_pGameInstance->Get_ViewportSize();
    m_fAspect = vViewPortSize.x / (_float)vViewPortSize.y;

    return S_OK;
}

void CCamera::Priority_Update(_float _fTimeDelta)
{
    m_pGameInstance->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrix_Inverse());
    m_pGameInstance->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_fFov, m_fAspect, m_fNear, m_fFar));
}

void CCamera::Update(_float _fTimeDelta)
{
    //Picking_Mesh(g_hwnd)

}

void CCamera::Late_Update(_float _fTimeDelta)
{
}

HRESULT CCamera::Render()
{
    return S_OK;
}

_float3 CCamera::Picking_Terrain(HWND _hWnd, CVIBuffer_MapToolTerrain* _pMapToolTerrainBuffer)
{
    POINT       ptMouse = {};
    GetCursorPos(&ptMouse);
    ScreenToClient(_hWnd, &ptMouse);

    // 뷰 포트 -> 투영 
    _uint2  vViewPortSize = m_pGameInstance->Get_ViewportSize();
    XMVECTOR    vMousePos = XMVectorSet((ptMouse.x / (vViewPortSize.x * 0.5f) - 1.0f), (ptMouse.y / -(vViewPortSize.y * 0.5f) + 1.0f), 0.0f, 1.0f);

    // 투영 -> 뷰 스페이스 
    _float4x4       ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
    XMMATRIX        matProj = XMLoadFloat4x4(&ProjMatrix);
    XMMATRIX        matProjInverse = XMMatrixInverse(nullptr, matProj);
    vMousePos = XMVector3TransformCoord(vMousePos, matProjInverse);

    // 뷰 스페이스 -> 월드 
    _float4x4       ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
    XMMATRIX        matView = XMLoadFloat4x4(&ViewMatrix);
    XMMATRIX        matViewInverse = XMMatrixInverse(nullptr, matView);
    XMVECTOR        vRayPos = XMVector3TransformCoord(XMVectorZero(), matViewInverse);
    XMVECTOR        vRayDir = XMVector3TransformNormal(vMousePos, matViewInverse);
    vRayDir = XMVector3Normalize(vRayDir);

    // 월드 -> 로컬 
    XMMATRIX matWorldInverse = m_pTransformCom->Get_WorldMatrix_Inverse();
    XMVector3TransformCoord(vRayPos, matWorldInverse);
    XMVector3TransformNormal(vRayDir, matWorldInverse);

    XMVECTOR* pMapToolTerrainBufferPos = _pMapToolTerrainBuffer->Get_VertexPos();

    _uint iNumVerticesX = _pMapToolTerrainBuffer->Get_NumVerticesX();
    _uint iNumVerticesZ = _pMapToolTerrainBuffer->Get_NumVerticesZ();

    _ulong      dwVtxId[3] = {};
    _float      fU = {}, fV = {}, fDist = {};

    for (_ulong i = 0; i < iNumVerticesZ - 1; ++i)
    {
        for (_ulong j = 0; j < iNumVerticesX - 1; ++j)
        {
            _ulong  dwIndex = i * iNumVerticesX + j;

            // 오른쪽 위, 
            dwVtxId[0] = dwIndex + iNumVerticesX;
            dwVtxId[1] = dwIndex + iNumVerticesX + 1;
            dwVtxId[2] = dwIndex + 1;


            if (TriangleTests::Intersects(
                FXMVECTOR(vRayPos),
                FXMVECTOR(vRayDir),
                pMapToolTerrainBufferPos[dwVtxId[1]],
                pMapToolTerrainBufferPos[dwVtxId[2]],
                pMapToolTerrainBufferPos[dwVtxId[0]],
                fDist
            ))
            {
                _float3 vPickedPos =
                    XMFLOAT3
                    (
                        XMVectorGetX(pMapToolTerrainBufferPos[dwVtxId[1]]) + fU * (XMVectorGetX(pMapToolTerrainBufferPos[dwVtxId[2]]) - XMVectorGetX(pMapToolTerrainBufferPos[dwVtxId[1]])),
                        XMVectorGetY(pMapToolTerrainBufferPos[dwVtxId[1]]),
                        XMVectorGetZ(pMapToolTerrainBufferPos[dwVtxId[1]]) + fV * (XMVectorGetZ(pMapToolTerrainBufferPos[dwVtxId[0]]) - XMVectorGetZ(pMapToolTerrainBufferPos[dwVtxId[1]]))
                    );

                return vPickedPos;
                // 노말도 햇다....
            }

            // 왼쪽 아래, 
            dwVtxId[0] = dwIndex + iNumVerticesX;
            dwVtxId[1] = dwIndex + 1;
            dwVtxId[2] = dwIndex;

            if (TriangleTests::Intersects(
                FXMVECTOR(vRayPos),
                FXMVECTOR(vRayDir),
                pMapToolTerrainBufferPos[dwVtxId[2]],
                pMapToolTerrainBufferPos[dwVtxId[0]],
                pMapToolTerrainBufferPos[dwVtxId[1]],
                fDist
            ))
            {
                _float3 vPickedPos =
                    XMFLOAT3
                    (
                        XMVectorGetX(pMapToolTerrainBufferPos[dwVtxId[1]]) + fU * (XMVectorGetX(pMapToolTerrainBufferPos[dwVtxId[2]]) - XMVectorGetX(pMapToolTerrainBufferPos[dwVtxId[1]])),
                        XMVectorGetY(pMapToolTerrainBufferPos[dwVtxId[1]]),
                        XMVectorGetZ(pMapToolTerrainBufferPos[dwVtxId[1]]) + fV * (XMVectorGetZ(pMapToolTerrainBufferPos[dwVtxId[0]]) - XMVectorGetZ(pMapToolTerrainBufferPos[dwVtxId[1]]))
                    );
                return vPickedPos;
            }
        }
    }

    return _float3(0.0f, -0.5f, 0.0f);
}

_float3 CCamera::Picking_Terrain_Elaborated(HWND _hWnd, CVIBuffer_MapToolTerrain* _pMapToolTerrainBuffer)
{
    POINT       ptMouse = {};
    GetCursorPos(&ptMouse);
    ScreenToClient(_hWnd, &ptMouse);

    // 뷰 포트 -> 투영 
    _uint2  vViewPortSize = m_pGameInstance->Get_ViewportSize();
    XMVECTOR    vMousePos = XMVectorSet((ptMouse.x / (vViewPortSize.x * 0.5f) - 1.0f), (ptMouse.y / -(vViewPortSize.y * 0.5f) + 1.0f), 0.0f, 1.0f);

    // 투영 -> 뷰 스페이스 
    _float4x4       ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
    XMMATRIX        matProj = XMLoadFloat4x4(&ProjMatrix);
    XMMATRIX        matProjInverse = XMMatrixInverse(nullptr, matProj);
    vMousePos = XMVector3TransformCoord(vMousePos, matProjInverse);

    // 뷰 스페이스 -> 월드 
    _float4x4       ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
    XMMATRIX        matView = XMLoadFloat4x4(&ViewMatrix);
    XMMATRIX        matViewInverse = XMMatrixInverse(nullptr, matView);
    XMVECTOR        vRayPos = XMVector3TransformCoord(XMVectorZero(), matViewInverse);
    XMVECTOR        vRayDir = XMVector3TransformNormal(vMousePos, matViewInverse);
    vRayDir = XMVector3Normalize(vRayDir);

    // 월드 -> 로컬 
    XMMATRIX matWorldInverse = m_pTransformCom->Get_WorldMatrix_Inverse();
    XMVector3TransformCoord(vRayPos, matWorldInverse);
    XMVector3TransformNormal(vRayDir, matWorldInverse);

    XMVECTOR* pMapToolTerrainBufferPos = _pMapToolTerrainBuffer->Get_VertexPos();

    _uint iNumVerticesX = _pMapToolTerrainBuffer->Get_NumVerticesX();
    _uint iNumVerticesZ = _pMapToolTerrainBuffer->Get_NumVerticesZ();

    _ulong      dwVtxId[3] = {};
    _float      fU = {}, fV = {}, fDist = {};

    for (_ulong i = 0; i < iNumVerticesZ - 1; ++i)
    {
        for (_ulong j = 0; j < iNumVerticesX - 1; ++j)
        {
            _ulong  dwIndex = i * iNumVerticesX + j;

            // 오른쪽 위, 
            dwVtxId[0] = dwIndex + iNumVerticesX;
            dwVtxId[1] = dwIndex + iNumVerticesX + 1;
            dwVtxId[2] = dwIndex + 1;


            if (TriangleTests::Intersects(
                FXMVECTOR(vRayPos),
                FXMVECTOR(vRayDir),
                pMapToolTerrainBufferPos[dwVtxId[1]],
                pMapToolTerrainBufferPos[dwVtxId[2]],
                pMapToolTerrainBufferPos[dwVtxId[0]],
                fDist
            ))
            {
                _float3 vPickedPos =
                    XMFLOAT3
                    (
                        XMVectorGetX(pMapToolTerrainBufferPos[dwVtxId[1]]) + fU * (XMVectorGetX(pMapToolTerrainBufferPos[dwVtxId[2]]) - XMVectorGetX(pMapToolTerrainBufferPos[dwVtxId[1]])),
                        XMVectorGetY(pMapToolTerrainBufferPos[dwVtxId[1]]),
                        XMVectorGetZ(pMapToolTerrainBufferPos[dwVtxId[1]]) + fV * (XMVectorGetZ(pMapToolTerrainBufferPos[dwVtxId[0]]) - XMVectorGetZ(pMapToolTerrainBufferPos[dwVtxId[1]]))
                    );

                return vPickedPos;
                // 노말도 햇다....
            }

            // 왼쪽 아래, 
            dwVtxId[0] = dwIndex + iNumVerticesX;
            dwVtxId[1] = dwIndex + 1;
            dwVtxId[2] = dwIndex;

            if (TriangleTests::Intersects(
                FXMVECTOR(vRayPos),
                FXMVECTOR(vRayDir),
                pMapToolTerrainBufferPos[dwVtxId[2]],
                pMapToolTerrainBufferPos[dwVtxId[0]],
                pMapToolTerrainBufferPos[dwVtxId[1]],
                fDist
            ))
            {
                _float3 vPickedPos =
                    XMFLOAT3
                    (
                        XMVectorGetX(pMapToolTerrainBufferPos[dwVtxId[1]]) + fU * (XMVectorGetX(pMapToolTerrainBufferPos[dwVtxId[2]]) - XMVectorGetX(pMapToolTerrainBufferPos[dwVtxId[1]])),
                        XMVectorGetY(pMapToolTerrainBufferPos[dwVtxId[1]]),
                        XMVectorGetZ(pMapToolTerrainBufferPos[dwVtxId[1]]) + fV * (XMVectorGetZ(pMapToolTerrainBufferPos[dwVtxId[0]]) - XMVectorGetZ(pMapToolTerrainBufferPos[dwVtxId[1]]))
                    );
                return vPickedPos;
            }
        }
    }

    return _float3(0.0f, -0.5f, 0.0f);
}


//_float3 CCamera::Picking_Mesh(HWND _hWnd, CMesh* _pMeshBuffer)
//{
//    POINT       ptMouse = {};
//    GetCursorPos(&ptMouse);
//    ScreenToClient(_hWnd, &ptMouse);
//
//    // 뷰 포트 -> 투영 
//    _uint2  vViewPortSize = m_pGameInstance->Get_ViewportSize();
//    XMVECTOR    vMousePos = XMVectorSet((ptMouse.x / (vViewPortSize.x * 0.5f) - 1.0f), (ptMouse.y / -(vViewPortSize.y * 0.5f) + 1.0f), 0.0f, 1.0f);
//
//    // 투영 -> 뷰 스페이스 
//    _float4x4       ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
//    XMMATRIX        matProj = XMLoadFloat4x4(&ProjMatrix);
//    XMMATRIX        matProjInverse = XMMatrixInverse(nullptr, matProj);
//    vMousePos = XMVector3TransformCoord(vMousePos, matProjInverse);
//
//    // 뷰 스페이스 -> 월드 
//    _float4x4       ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
//    XMMATRIX        matView = XMLoadFloat4x4(&ViewMatrix);
//    XMMATRIX        matViewInverse = XMMatrixInverse(nullptr, matView);
//    XMVECTOR        vRayPos = XMVector3TransformCoord(XMVectorZero(), matViewInverse);
//    XMVECTOR        vRayDir = XMVector3TransformNormal(vMousePos, matViewInverse);
//    vRayDir = XMVector3Normalize(vRayDir);
//
//    // 월드 -> 로컬 
//    XMMATRIX matWorldInverse = m_pTransformCom->Get_WorldMatrix_Inverse();
//    XMVector3TransformCoord(vRayPos, matWorldInverse);
//    XMVector3TransformNormal(vRayDir, matWorldInverse);
//
//    XMVECTOR* pMeshBufferPos = _pMeshBuffer->Get_VertexPos();
//    _uint   iNumTriangles = _pMeshBuffer->Get_Triangles();
//    _uint* pIndices = _pMeshBuffer->Get_Indices();
//
//    _ulong dwVtxId[3] = {};
//    _float fDist = 0.0f;
//
//    for (_uint i = 0; i < iNumTriangles; ++i)
//    {
//        dwVtxId[0] = pIndices[i * 3];
//        dwVtxId[1] = pIndices[i * 3 + 1];
//        dwVtxId[2] = pIndices[i * 3 + 2];
//
//        if (TriangleTests::Intersects(
//            FXMVECTOR(vRayPos),
//            FXMVECTOR(vRayDir),
//            pMeshBufferPos[dwVtxId[0]],
//            pMeshBufferPos[dwVtxId[1]],
//            pMeshBufferPos[dwVtxId[2]],
//            fDist))
//        {
//
//
//        }
//    }
//
//    return _float3(0.0f, -1.0f, 0.0f);
//}

_float CCamera::Compute_Height(_fvector _vPosition, HWND _hWnd, CVIBuffer_MapToolTerrain* _pMapToolTerrainBuffer)
{
    return (-Picking_Terrain(_hWnd, _pMapToolTerrainBuffer).x * XMVectorGetX(_vPosition) - (Picking_Terrain(_hWnd, _pMapToolTerrainBuffer).z * XMVectorGetZ(_vPosition)) - 1.0f) / Picking_Terrain(_hWnd, _pMapToolTerrainBuffer).y;
}

void CCamera::Free()
{
    __super::Free();
}
