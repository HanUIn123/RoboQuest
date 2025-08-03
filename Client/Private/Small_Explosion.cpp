#include "pch.h"
#include "Small_Explosion.h"
#include "GameInstance.h"


CSmall_Explosion::CSmall_Explosion(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CEffectPartObject{ _pDevice, _pContext }
{
}

CSmall_Explosion::CSmall_Explosion(const CSmall_Explosion& _Prototype)
    :CEffectPartObject(_Prototype)
{
}

HRESULT CSmall_Explosion::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSmall_Explosion::Initialize(void* _pArg)
{
    SMALL_EFFECT_DESC* pDesc = static_cast<SMALL_EFFECT_DESC*>(_pArg);

    m_pSocketMatrix = pDesc->pSocketMatrix;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    _vector vExplosionPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    // 아래로 1만큼 내리고.. 오른쪽으로 3만큼 이동시킴. 
    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.0f, 0.0f, 2.8f, 1.0f));

    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.0f, 0.0f, 1.6f, 1.0f));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vExplosionPos);

    //m_pTransformCom->Scaling((_float3(3.5f, 3.5f, 3.5f)));
    //m_pTransformCom->Scaling((_float3(2.5f, 2.5f, 2.5f)));

    return S_OK;
}

void CSmall_Explosion::Priority_Update(_float _fTimeDelta)
{
}

void CSmall_Explosion::Update(_float _fTimeDelta)
{
    if (!m_bActivate)
    {
        m_fFrame = 0.0f;
        m_bFinish = false;
        return;
    }

    m_fFrame += 90.0f * _fTimeDelta;
    
    if (m_fFrame >= 90.0f)
    {
        m_fFrame = 0.f;
        m_bFinish = true;
    }


    //m_fAnimationTime += _fTimeDelta * 12.0f;


    __super::Compute_CamDistance();
}

void CSmall_Explosion::Late_Update(_float _fTimeDelta)
{
    if (m_bActivate)
    {
        Setting_BillBoard();
        __super::Late_Update(_fTimeDelta);
    }
}

HRESULT CSmall_Explosion::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShaderCom, "g_DepthTexture")))
        return E_FAIL;
    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", static_cast<_uint>(m_fFrame))))
        return E_FAIL;

    //if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture", 0)))
    //    return E_FAIL;

    //if (FAILED(m_pShaderCom->Bind_RawValue("g_fAnimationTime", &m_fAnimationTime, sizeof(_float))))
    //    return E_FAIL;

    m_pShaderCom->Begin(1);

    m_pVIBufferCom->Bind_InputAssembler();

    m_pVIBufferCom->Render();

    return S_OK;
}

void CSmall_Explosion::Setting_BillBoard()
{
    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (size_t i = 0; i < 3; ++i)
    {
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
    }

    _float4x4 ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);

    _float4x4 ViewMatrixInverse;
    XMStoreFloat4x4(&ViewMatrixInverse, XMMatrixInverse(nullptr, XMLoadFloat4x4(&ViewMatrix)));

    _float4x4 BillBoardMatrix;
    XMStoreFloat4x4(&BillBoardMatrix, XMMatrixIdentity());

    BillBoardMatrix =
    {
           ViewMatrix._11,   0.0f,   ViewMatrix._13,  0.0f,
            0.0f,            1.0f,             0.0f,  0.0f,
           ViewMatrix._31,   0.0f,   ViewMatrix._33,  0.0f,
                     0.0f,   0.0f,             0.0f,  1.0f,
    };

    _float4x4 BillBoardMatrixInverse;
    XMStoreFloat4x4(&BillBoardMatrixInverse, XMMatrixInverse(nullptr, XMLoadFloat4x4(&BillBoardMatrix)));

    _vector vScale{}, vRotation{}, vTranslation{};

    XMMatrixDecompose(&vScale, &vRotation, &vTranslation, XMLoadFloat4x4(m_pParentWorldMatrix));

    _float4x4 matWithOutLookMatrix;
    XMStoreFloat4x4(&matWithOutLookMatrix, XMMatrixAffineTransformation(vScale, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f), vTranslation));

    _float4x4 ExplosionMatrix;
    XMStoreFloat4x4(&ExplosionMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * SocketMatrix * XMLoadFloat4x4(&BillBoardMatrixInverse) * XMLoadFloat4x4(&matWithOutLookMatrix));

    Set_CombineMatrix(&ExplosionMatrix);

    //m_pGameInstance->Add_RenderGroup(CRenderer::RG_UI, this);
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_BLEND, this);
}

void CSmall_Explosion::DeActivate_Effect()
{
    m_bActivate = false;
}

void CSmall_Explosion::Set_Position(_vector _vCurrentPos)
{
    _vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

    _vCurrentPos = vCurrentPos;
}

void CSmall_Explosion::Hold_Position(_vector _vCurrentPos)
{
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(_vCurrentPos, 1.0f));
}


HRESULT CSmall_Explosion::Ready_Component()
{
    /* Com_Texture */
    //if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Explosion"),
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_SmallExplosion"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;

    // Main APP 클래스에서 바로 그냥 쉐이더랑, POSTEX 버퍼 (사각형) 미리 등록해놨음. 
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxExplosionTex"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    /* Com_VIBuffer */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom))))
        return E_FAIL;

    return S_OK;
}

CSmall_Explosion* CSmall_Explosion::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CSmall_Explosion* pInstance = new CSmall_Explosion(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CSmall_Explosion");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CSmall_Explosion::Clone(void* _pArg)
{
    CSmall_Explosion* pInstance = new CSmall_Explosion(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CSmall_Explosion");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSmall_Explosion::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTextureCom);
}
