#include "pch.h"
#include "FireRoad.h"
#include "GameInstance.h"

CFireRoad::CFireRoad(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CGameObject{ _pDevice , _pContext }
{
}

CFireRoad::CFireRoad(const CFireRoad& _Prototype)
    :CGameObject(_Prototype)
{
}

HRESULT CFireRoad::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CFireRoad::Initialize(void* _pArg)
{
    FIRE_ROAD_DESC* pDesc = static_cast<FIRE_ROAD_DESC*>(_pArg);

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_fFlowTime = 15.0f;

    m_vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, TEXT("Layer_Player")));


    return S_OK;
}

void CFireRoad::Priority_Update(_float _fTimeDelta)
{
  
}

void CFireRoad::Update(_float _fTimeDelta)
{
    if (!m_bActive)
        return;

    // 불 2천장 파바바박 하는 타이머변수
    m_fTimer += _fTimeDelta;

    // 얘는 불 길 지속시간 변수
    m_fFlowTime -= _fTimeDelta;

    // 5초~ 지나면 불길 사라짐.
    if (m_fFlowTime <= 0.0f)
    {
        m_fFlowTime = 15.0f;
        DeActivateFlow();
    }

    //=================================================
    // 불길 나올 때 부드럽게 하려고 추가한 bool 변수.
    if (m_bIncreasing)
        m_fLerpProgress += _fTimeDelta * 2.0f;

    if (m_fLerpProgress >= 1.0f)
    {
        m_bIncreasing = false;
    }

    _float fStartScale = 4.0f;
    _float fEndScale = 13.0f;
    _float fCurrentScale = fStartScale + (fEndScale - fStartScale) * m_fLerpProgress;

    m_pTransformCom->Scaling(_float3(fCurrentScale, 1.0f, fCurrentScale));
    //=================================================

    //if (!m_bActive)
    //{
    //    m_fFrame = 0.0f;
    //}

    //m_fFrame += 33.f * _fTimeDelta;

    //if (m_fFrame >= 33.f)
    //{
    //    m_fFrame = 0.f;
    //}

    m_fAnimationTime += _fTimeDelta * 12.0f;


    auto WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    m_pColliderCom->Update(WorldMatrix);

    if (m_bActive)
    {
        Check_Collision_With_Player();
    }
}

void CFireRoad::Late_Update(_float _fTimeDelta)
{
    m_pTargetCollider = static_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), 0));

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_GLOW, this);
}

HRESULT CFireRoad::Render()
{
#ifdef _DEBUG
    //m_pColliderCom->Render();
#endif
    if (FAILED(Bind_ShaderResource()))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    m_pShaderCom->Begin(1);
    m_pVIBufferCom->Bind_InputAssembler();
    m_pVIBufferCom->Render();

    if (FAILED(Bind_ShaderResource()))
        return E_FAIL;

    if (FAILED(m_pFrameTextureCom->Bind_ShaderResource(m_pShaderFireFrame, "g_FireFrameTexture", static_cast<_uint>(m_fFrame))))
        return E_FAIL;

    m_pShaderFireFrame->Begin(3);
    m_pFrameBufferCom->Bind_InputAssembler();
    m_pFrameBufferCom->Render();

    return S_OK;
}

HRESULT CFireRoad::Render_Glow()
{
    if (FAILED(Bind_ShaderResource()))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;

    m_pShaderCom->Begin(3);
    m_pVIBufferCom->Bind_InputAssembler();
    m_pVIBufferCom->Render();

   /* if (FAILED(Bind_ShaderResource()))
        return E_FAIL;

    if (FAILED(m_pFrameTextureCom->Bind_ShaderResource(m_pShaderFireFrame, "g_FireFrameTexture", static_cast<_uint>(m_fFrame))))
        return E_FAIL;

    m_pShaderFireFrame->Begin(3);
    m_pFrameBufferCom->Bind_InputAssembler();
    m_pFrameBufferCom->Render();*/

    return S_OK;
}

void CFireRoad::Set_Active(_bool _bActive)
{
    if (_bActive)
    {
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPosition);
        m_fLerpProgress = 0.0f; 
        m_bIncreasing = true;   
        
    }
    m_bActive = _bActive;
}

void CFireRoad::DeActivateFlow()
{
    if (m_bActive)
    {
        m_bActive = false;
        m_fLerpProgress = 1.0f;
    }
}

void CFireRoad::Set_Position(_vector _vPosition, _vector _vDirection)
{
    m_vPosition = _vPosition;
   
    _vDirection = XMVector3Normalize(_vDirection);
    //_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), _vDirection));

    //_matrix ScaleMatrix = XMMatrixScaling(3.0f, 1.0f, 3.0f);
    //_matrix WorldMatrix = XMMatrixIdentity();
    //WorldMatrix.r[0] = vRight * 1.0f; 
    //WorldMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f); 
    //WorldMatrix.r[2] = _vDirection * 1.0f; 
    //WorldMatrix.r[3] = _vPosition; 
    //WorldMatrix = XMMatrixMultiply(ScaleMatrix, WorldMatrix); 

    //XMFLOAT4X4 matWorldFloat;
    //XMStoreFloat4x4(&matWorldFloat, WorldMatrix);

    //m_pShaderCom->Bind_Matrix("g_WorldMatrix", &matWorldFloat);
}

void CFireRoad::Check_Collision_With_Player()
{
    if (!m_pColliderCom || !m_pTargetCollider)
        return;

    if (m_pColliderCom->Intersect(m_pTargetCollider))
    {
        if (m_pPlayer)
        {
            //static_cast<CPlayer*>(pPlayer)->TakeDamage(10);
            //cout << "플레이어 맞음!! 아프다!!" << endl;

            m_pPlayer->Be_Burned(10);
        }
    }
}

HRESULT CFireRoad::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FireRoad"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_FireFrame"), TEXT("Com_Texture_FireFrame"), reinterpret_cast<CComponent**>(&m_pFrameTextureCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_PosVerRect"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_FireFrame"), TEXT("Com_Shader_FireFrame"), reinterpret_cast<CComponent**>(&m_pShaderFireFrame))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_Ver"), TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_FramePoint"), TEXT("Com_VIBufferFrame"), reinterpret_cast<CComponent**>(&m_pFrameBufferCom))))
        return E_FAIL;


    /* Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom), &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CFireRoad::Bind_ShaderResource()
{
    // 월드 행렬 바인딩. 
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    // 월드 행렬 바인딩. 
    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderFireFrame, "g_WorldMatrix")))
        return E_FAIL;


    // 뷰 행렬 바인딩. 
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;

    // 뷰 행렬 바인딩. 
    if (FAILED(m_pShaderFireFrame->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;


    // 투영 행렬 바인딩. 
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    // 투영 행렬 바인딩. 
    if (FAILED(m_pShaderFireFrame->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;


    if (FAILED(m_pShaderCom->Bind_RawValue("g_Timer", &m_fTimer, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pShaderFireFrame->Bind_RawValue("g_vCameraPosition", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pShaderFireFrame->Bind_RawValue("g_fAnimationTime", &m_fAnimationTime, sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

CFireRoad* CFireRoad::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CFireRoad* pInstance = new CFireRoad(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CFireRoad");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CFireRoad::Clone(void* _pArg)
{
    CFireRoad* pInstance = new CFireRoad(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CFireRoad");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFireRoad::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pShaderFireFrame);

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pFrameTextureCom);

    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pFrameBufferCom);

    Safe_Release(m_pColliderCom);
}
