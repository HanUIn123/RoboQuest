#include "pch.h"
#include "Body_Door.h"
#include "GameInstance.h"
#include "Monster.h"

CBody_Door::CBody_Door(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CPartObject{_pDevice, _pContext}
{
}

CBody_Door::CBody_Door(const CBody_Door& _Prototype)
    :CPartObject(_Prototype)
{
}

HRESULT CBody_Door::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Door::Initialize(void* _pArg)
{
    CBody_Door::BODY_DOOR_DESC* pDesc = static_cast<CBody_Door::BODY_DOOR_DESC*>(_pArg);

    m_pParentState = pDesc->pParentState;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_bIsEffectiveObject = false;

    m_pModelCom->SetUp_Animation(1, false, true);

    //_bool bPlayAnimation = m_pModelCom->Play_Animation(_)
    //m_pModelCom->set


    return S_OK;
}

void CBody_Door::Priority_Update(_float _fTimeDelta)
{
    if (*m_pParentState == 3)
    {
        if (m_pModelCom->Play_Animation(_fTimeDelta ,true) == false)
        {
            return;
        }
    }
    else
    {
        if (*m_pParentState != m_iPreAnimationState)
        {
            if (*m_pParentState == 1)
            {
                m_pModelCom->SetUp_Animation(0, false, true);
                m_pModelCom->Set_AnimationSpeed(1.0f, 1);

            }
            else if (*m_pParentState == 2)
            {
                m_pModelCom->SetUp_Animation(1, false, true);
                m_pModelCom->Set_AnimationSpeed(1.0f, 1);
            }

            m_iPreAnimationState = *m_pParentState;
        }

        m_pModelCom->Play_Animation(_fTimeDelta);
    }

    XMStoreFloat4x4(&m_CombinedWorldMatrix, XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()) * XMLoadFloat4x4(m_pParentWorldMatrix));
}

void CBody_Door::Update(_float _fTimeDelta)
{
}

void CBody_Door::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_GLOW, this);
}

HRESULT CBody_Door::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture", 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        m_pShaderCom->Begin(0);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CBody_Door::Render_Glow()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (_uint i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture", 0)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, i, "g_BoneMatrices")))
            return E_FAIL;

        m_pShaderCom->Begin(3);
        m_pModelCom->Render(i);
    }

    return S_OK;
}

_bool CBody_Door::IsAnimationFinished() const
{
    return m_pModelCom->IsAnimOnceDone();
}

HRESULT CBody_Door::Ready_Components()
{
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_DoorMask"), TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom))))
        return E_FAIL;


    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"), TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Door"), TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CBody_Door::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_EffectiveObject", &m_bIsEffectiveObject, sizeof(_bool))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_MaskTexture", 0)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_RawValue("g_vCameraPosition", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
        return E_FAIL;

    //if (FAILED(m_pShaderCom->Bind_RawValue("g_RimPower", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
    //    return E_FAIL;

    //if (FAILED(m_pShaderCom->Bind_RawValue("g_vCameraPosition", &m_pGameInstance->Get_CameraPos(), sizeof(_float4))))
    //    return E_FAIL;



    return S_OK;
}

CBody_Door* CBody_Door::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CBody_Door* pInstance = new CBody_Door(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CBody_Door");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CBody_Door::Clone(void* _pArg)
{
    CBody_Door* pInstance = new CBody_Door(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CBody_Door");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBody_Door::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pTextureCom);

}
