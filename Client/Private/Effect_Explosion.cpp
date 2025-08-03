#include "pch.h"
#include "Effect_Explosion.h"
#include "GameInstance.h"

CEffect_Explosion::CEffect_Explosion(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CBlendObject{ _pDevice, _pContext }
{
}

CEffect_Explosion::CEffect_Explosion(const CEffect_Explosion& _Prototype)
    :CBlendObject(_Prototype)
{
}

HRESULT CEffect_Explosion::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CEffect_Explosion::Initialize(void* _pArg)
{
    GAMEOBJECT_DESC				Desc{};

    Desc.fSpeedPerSec = 5.f;
    Desc.fRotationPerSec = XMConvertToRadians(90.f);

    if (FAILED(__super::Initialize(&Desc)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(rand() % 10, 0.f, rand() % 10, 1.f));

    return S_OK;
}

void CEffect_Explosion::Priority_Update(_float _fTimeDelta)
{
    int a = 10;
}

void CEffect_Explosion::Update(_float _fTimeDelta)
{
    m_fFrame += 32.0f * _fTimeDelta;

    if (m_fFrame >= 32.0f)
        m_fFrame = 0.f;

    __super::Compute_CamDistance();
}

void CEffect_Explosion::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_BLEND, this);
}

HRESULT CEffect_Explosion::Render()
{
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

    m_pShaderCom->Begin(1);

    m_pVIBufferCom->Bind_InputAssembler();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CEffect_Explosion::Setting_BillBoard()
{
   /* _float4x4 ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);

    _float4x4 ViewMatrixInverse;
    XMStoreFloat4x4(&ViewMatrixInverse, XMMatrixInverse(nullptr, XMLoadFloat4x4(&ViewMatrix)));

    _float4x4   BillBoardMatrix;
    XMStoreFloat4x4(&BillBoardMatrix, XMMatrixIdentity());

    BillBoardMatrix =
    {
        ViewMatrix._11, 0.0f, ViewMatrix._13, 0.0f,
        0.0f,           1.0f,           0.0f, 0.0f,
        ViewMatrix._31, 0.0f, ViewMatrix._33, 0.0f,
                  0.0f, 0.0f,           0.0f, 1.0f
    };

    _float4x4 BillBoardMatrixInverse;
    XMStoreFloat4x4(&BillBoardMatrixInverse, XMMatrixInverse(nullptr, XMLoadFloat4x4(&BillBoardMatrix)));

    _vector vScale{}, vRotation{}, vTranslation{};
    XMMatrixDecompose(&vScale, &vRotation, &vTranslation, XMLoadFloat4x4(m_pparent))*/

}

HRESULT CEffect_Explosion::Ready_Components()
{
    /* Com_Texture */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    // Main APP 클래스에서 바로 그냥 쉐이더랑, POSTEX 버퍼 (사각형) 미리 등록해놨음. 
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

CEffect_Explosion* CEffect_Explosion::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CEffect_Explosion* pInstance = new CEffect_Explosion(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CEffect_Explosion");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_Explosion::Clone(void* _pArg)
{
    CEffect_Explosion* pInstance = new CEffect_Explosion(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CEffect_Explosion");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_Explosion::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
}
