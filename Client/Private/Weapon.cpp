#include "pch.h"
#include "../Public/Weapon.h"
#include "GameInstance.h"


CWeapon::CWeapon(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CPartObject{ _pDevice, _pContext }
{
}

CWeapon::CWeapon(const CWeapon& _Prototype)
    :CPartObject(_Prototype)
{
}

HRESULT CWeapon::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CWeapon::Initialize(void* _pArg)
{
    WEAPON_DESC* pDesc = static_cast<WEAPON_DESC*>(_pArg);

    m_pSocketMatrix = pDesc->pSocketMatrix;

    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    if (FAILED(Ready_Components()))
        return E_FAIL;

    m_pTransformCom->Scaling(_float3(0.005f, 0.005f, 0.005f));
    //
    m_pTransformCom->Rotation(127.45f, 118.509f, 163.23f);
    //
    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-0.040f, -0.031f, -0.034f, 1.0f));



    //m_pTransformCom->Scaling(_float3(0.1f, 0.1f, 0.1f));
    //m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(90.f));
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f));


    return S_OK;
}

void CWeapon::Priority_Update(_float _fTimeDelta)
{
    int a = 10;
    //
    // _float CameraSpeed = m_pTransformCom->Get_Speed();
    // ImGui::SliderFloat("##1", &(CameraSpeed), 10.0f, 40.0f);
    // m_pTransformCom->Set_Speed(CameraSpeed);

    //ImGui::SliderFloat("s", &fX, 0, 360.0f);
    //ImGui::SliderFloat("r", &fY, 0, 360.0f);
    //ImGui::SliderFloat("t", &fZ, 0, 360.0f);

    //m_pTransformCom->Rotation(fX, fY, fZ);
    // 플레이어의 위치를 저장할 변수
    //_float fWeaponPosX = XMVectorGetX(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
    //_float fWeaponPosY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
    //_float fWeaponPosZ = XMVectorGetZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
    //
    //// ImGui 슬라이더로 X, Y, Z 값을 조정
    //ImGui::SliderFloat("Translation X", &fWeaponPosX, -1.0f, 1.0f); // X축
    //ImGui::SliderFloat("Translation Y", &fWeaponPosY, -1.0f, 1.0f); // Y축
    //ImGui::SliderFloat("Translation Z", &fWeaponPosZ, -1.0f, 1.0f); // Z축

    // 변경된 값을 다시 m_pTransformCom에 설정
    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(fWeaponPosX, fWeaponPosY, fWeaponPosZ, 1.0f));
}

void CWeapon::Update(_float _fTimeDelta)
{
    _matrix SocketMatrix = XMLoadFloat4x4(m_pSocketMatrix);

    for (size_t i = 0; i < 3; ++i)
    {
        // 행 3개를 길이 1로 만듬.
        SocketMatrix.r[i] = XMVector3Normalize(SocketMatrix.r[i]);
    }

    XMStoreFloat4x4(&m_CombinedWorldMatrix,
        SocketMatrix *
        XMLoadFloat4x4(m_pParentWorldMatrix));
        //XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr()));
}

void CWeapon::Late_Update(_float _fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CWeapon::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint			iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, (_uint)i, aiTextureType_DIFFUSE, "g_DiffuseTexture", 0)))
            return E_FAIL;

        m_pShaderCom->Begin(0);
        m_pModelCom->Render((_uint)i);
    }

    return S_OK;
}

HRESULT CWeapon::Ready_Components()
{
    /* Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxMesh"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom))))
        return E_FAIL;

    ///* Com_Model */
    //if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_ForkLift"),
    //    TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
    //    return E_FAIL;

    /* Com_Shotgun */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Model_Shotgun"),
        TEXT("Com_Model"), reinterpret_cast<CComponent**>(&m_pModelCom))))
        return E_FAIL;

    return S_OK;
}

HRESULT CWeapon::Bind_ShaderResources()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", &m_CombinedWorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    return S_OK;
}

CWeapon* CWeapon::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CWeapon* pInstance = new CWeapon(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CWeapon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CWeapon::Clone(void* _pArg)
{
    CWeapon* pInstance = new CWeapon(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CWeapon");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWeapon::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
}
