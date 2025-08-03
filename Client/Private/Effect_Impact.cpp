#include "pch.h"
#include "Effect_Impact.h"
#include "GameInstance.h"
#include "Player.h"

CEffect_Impact::CEffect_Impact(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CBlendObject{ _pDevice, _pContext }
{
}

CEffect_Impact::CEffect_Impact(const CEffect_Impact& _Prototype)
    :CBlendObject(_Prototype)
{
}

HRESULT CEffect_Impact::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Impact::Initialize(void* _pArg)
{
    GAMEOBJECT_DESC Desc{};
    Desc.fSpeedPerSec = 5.f;
    Desc.fRotationPerSec = XMConvertToRadians(90.f);

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    if (_pArg)
    {
        _float3 vHitPos = *static_cast<_float3*>(_pArg);

        CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, TEXT("Layer_Player")));
        if (!pPlayer)
            return E_FAIL;

        CPlayer_Camera* pCamera = pPlayer->Get_PlayerCamera();
        if (!pCamera)
            return E_FAIL;

        _vector vCameraLook = XMVector3Normalize(pCamera->Get_LookVector());

        vHitPos.x -= XMVectorGetX(vCameraLook) * 1.0f;
        vHitPos.y += 0.0f; 
        vHitPos.z -= XMVectorGetZ(vCameraLook) * 1.0f;

        m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vHitPos), 1.0f));
    }

    m_pTransformCom->Scaling((_float3(1.0f,1.0f,1.0f)));

    return S_OK;
}

void CEffect_Impact::Priority_Update(_float _fTimeDelta)
{
}

void CEffect_Impact::Update(_float _fTimeDelta)
{
    if (!m_bActive)
    {
        m_fFrame = 0.0f;
        return;
    }

    m_fFrame += 5.f * 0.1f;

    if (m_fFrame >= 5.f)
    {
        m_fFrame = 0.f;
        m_bActive = false;
    }

    __super::Compute_CamDistance();
}

void CEffect_Impact::Late_Update(_float _fTimeDelta)
{
    if (m_bActive)
    {
        Setting_BillBoard();
        m_pGameInstance->Add_RenderGroup(CRenderer::RG_BLEND, this);
    }
}

HRESULT CEffect_Impact::Render()
{
   /* if (!m_bActive)  
        return S_OK;*/

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", static_cast<_uint>(m_fFrame))))
        return E_FAIL;

    m_pShaderCom->Begin(3);

    m_pVIBufferCom->Bind_InputAssembler();
    m_pVIBufferCom->Render();

    return S_OK;
}

void CEffect_Impact::Setting_BillBoard()
{
    _float4x4 ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);

    // 빌보드 행렬 생성
    _float4x4 BillBoardMatrix;
    XMStoreFloat4x4(&BillBoardMatrix, XMMatrixIdentity());

    BillBoardMatrix =
    {
        ViewMatrix._11, 0.0f, ViewMatrix._13, 0.0f,
        0.0f,           1.0f,         0.0f,  0.0f,
        ViewMatrix._31, 0.0f, ViewMatrix._33, 0.0f,
        0.0f,           0.0f,         0.0f,  1.0f
    };

    _matrix BillboardMatrixXM = XMLoadFloat4x4(&BillBoardMatrix);

    // 기존 변환 행렬을 가져옴
    _matrix WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());

    // 월드 행렬에서 스케일 값을 분리하여 저장
    _vector vScale, vRotation, vTranslation;
    XMMatrixDecompose(&vScale, &vRotation, &vTranslation, WorldMatrix);

    // 기존 위치 정보 유지
    _vector vRight = XMVector3Normalize(BillboardMatrixXM.r[0]) * XMVectorGetX(vScale);
    _vector vUp = XMVector3Normalize(BillboardMatrixXM.r[1]) * XMVectorGetY(vScale);
    _vector vLook = XMVector3Normalize(BillboardMatrixXM.r[2]) * XMVectorGetZ(vScale);

    // 변환 적용
    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
    m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);

}

void CEffect_Impact::Activate_ImpactEffect(const _float3& _vHitPos)
{
    m_bActive = true;  
}

HRESULT CEffect_Impact::Ready_Components()
{
    /* Com_Texture (T_Impact_3Channel_Flip 사용) */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Impact"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CEffect_Impact* CEffect_Impact::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CEffect_Impact* pInstance = new CEffect_Impact(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CEffect_Impact");
        Safe_Release(pInstance);
    }

    return pInstance;

}

CGameObject* CEffect_Impact::Clone(void* _pArg)
{
    CEffect_Impact* pInstance = new CEffect_Impact(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CEffect_Impact");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_Impact::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
}
