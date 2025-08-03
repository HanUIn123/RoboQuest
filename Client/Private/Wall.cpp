#include "pch.h"
#include "Wall.h"
#include "GameInstance.h"

CWall::CWall(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CBuilding{_pDevice, _pContext}
{
}

CWall::CWall(const CWall& _Prototype)
    :CBuilding(_Prototype)
{
}

HRESULT CWall::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CWall::Initialize(void* _pArg)
{
    if (nullptr != _pArg)
    {
        INGAME_WALL_DESC* pDesc = static_cast<INGAME_WALL_DESC*>(_pArg);

        if (FAILED(__super::Initialize(pDesc)))
            return E_FAIL;

        if (FAILED(Ready_Component(pDesc->eWallType)))
            return E_FAIL;

        m_fPosition = pDesc->vWallPos;
        m_fRotation = pDesc->vWallRotation;
        m_fScale = pDesc->vWallScale;

        _vector  vWallPos = XMLoadFloat3(&m_fPosition);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vWallPos, 1.0f));

        _matrix matRotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&pDesc->vWallRotation));
        m_pTransformCom->Set_State(CTransform::STATE_RIGHT, matRotation.r[0]);
        m_pTransformCom->Set_State(CTransform::STATE_UP, matRotation.r[1]);
        m_pTransformCom->Set_State(CTransform::STATE_LOOK, matRotation.r[2]);

        m_pTransformCom->Scaling(pDesc->vWallScale);
    }
    return S_OK;
}

void CWall::Priority_Update(_float _fTimeDelta)
{
}

void CWall::Update(_float _fTimeDelta)
{
    auto WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());

    if(m_pColliderCom)
        m_pColliderCom->Update(WorldMatrix);

    __super::Update(_fTimeDelta);
}

void CWall::Late_Update(_float _fTimeDelta)
{
    //if (true == m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 10.0f))
    {
        m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
    }
}

HRESULT CWall::Render()
{
    if (FAILED(Bind_ShaderResource()))
        return E_FAIL;

    _uint           iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture", 0)))
            return E_FAIL;

        m_pShaderCom->Begin(1);

        m_pModelCom->Render(i);
    }

    //if (true == m_pGameInstance->isIn_Frustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 1.5f))
    {
#ifdef _DEBUG
        //if (m_pColliderCom)
        //    m_pColliderCom->Render();
#endif
    }
    return S_OK;
}

HRESULT CWall::Ready_Component(_uint _iWallTypeIndex)
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    switch (_iWallTypeIndex)
    {
        case INGAME_WALL_TYPE::IG_BASIC_WALL:
        {
            if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BasicWall"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
                return E_FAIL;
        }
        break;
        case INGAME_WALL_TYPE::IG_BIG_WALL:
        {
            if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_BigWall"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
                return E_FAIL;
        }
        break;
        case INGAME_WALL_TYPE::IG_PILLAR:
        {
            if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Pillar"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
                return E_FAIL;

            CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
            ColliderDesc.vExtents = _float3(1.0f, 3.0f, 1.0f);
            ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);


            if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
                return E_FAIL;
        }
        break;

        case INGAME_WALL_TYPE::IG_CONCRETE:
        {
            if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Concrete"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
                return E_FAIL;

            CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
            ColliderDesc.vExtents = _float3(1.0f, 3.0f, 1.0f);
            ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);


            if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
                return E_FAIL;
        }
        break;
    }

    return S_OK;
}

HRESULT CWall::Bind_ShaderResource()
{
    // 월드 행렬 바인딩. 
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    // 뷰 행렬 바인딩. 
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    // 투영 행렬 바인딩. 
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    return S_OK;
}

CWall* CWall::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CWall* pInstance = new CWall(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CWall");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CWall::Clone(void* _pArg)
{
    CWall* pInstance = new CWall(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CWall");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWall::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
}
