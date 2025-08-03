#include "pch.h"
#include "Gate.h"
#include "GameInstance.h"

CGate::CGate(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CMonster{ _pDevice, _pContext }
{
}

CGate::CGate(const CGate& _Prototype)
    :CMonster(_Prototype)
{
}

HRESULT CGate::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CGate::Initialize(void* _pArg)
{
    GATE_DESC* pDesc = static_cast<GATE_DESC*>(_pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    m_iState = GATE_END;

    // Monster Position 값 Loading 해온 값으로 세팅
    _vector vMonsterPos = XMLoadFloat3(&pDesc->vMonsterpos);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vMonsterPos, 1.0f));

    // Monster Rotation 값 Loading 해온 값으로 세팅
    _matrix matRotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&pDesc->vMonsterRotation));
    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, matRotation.r[0]);
    m_pTransformCom->Set_State(CTransform::STATE_UP, matRotation.r[1]);
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, matRotation.r[2]);

    // Monster Scale 값 Loading 해온 값으로 세팅
    m_pTransformCom->Scaling(pDesc->vMonsterScale);




    return S_OK;
}

void CGate::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CGate::Update(_float _fTimeDelta)
{
    auto WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrix_Ptr());
    m_pColliderCom[COLL_AABB]->Update(WorldMatrix);
    m_pColliderCom[COLL_SPHERE]->Update(WorldMatrix);

    __super::Update(_fTimeDelta);
}

void CGate::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);

    CCollider* pPlayerCollider = static_cast<CCollider*>(m_pGameInstance->Find_Component(LEVEL_GAMEPLAY, TEXT("Layer_Player"), TEXT("Com_Collider"), 0));

    if (pPlayerCollider && m_pColliderCom[COLL_SPHERE]->Intersect(pPlayerCollider))
    {
        m_bShowInteractionText = true;

        _vector vDoorPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
        XMStoreFloat3(&m_vTextPosition, vDoorPosition);

        if (m_iState == GATE_END)
        {
            if (m_pGameInstance->Get_DIKeyState(DIK_E) & 0x80)
            {
                m_iState = GATE_CLOSE;
               /* if (!m_bBossSpawn)
                {
                    Activate_Boss();
                    m_bBossSpawn = true;
                }*/
            }
        }
    }
    else
    {
        m_bShowInteractionText = false;
    }

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_BLEND, this);
}

HRESULT CGate::Render()
{
#ifdef _DEBUG
    //m_pColliderCom[COLL_AABB]->Render();
    //m_pColliderCom[COLL_SPHERE]->Render();
#endif

    if (m_bShowInteractionText)
    {
        _float2 screenPosition = m_pGameInstance->Convert_WorldToScreen(m_vTextPosition);

        _uint2 viewportSize = m_pGameInstance->Get_ViewportSize();

        screenPosition.x = viewportSize.x * 0.5f;
        screenPosition.y = viewportSize.y * 0.5f;

        // 텍스트 렌더링
        // const _wstring& _strFontTag, const _tchar* _pText, const _float2& _vPosition, _fvector _vColor, _float _fRotation, const _float2& _vOrigin, const _float _fScale
        m_pGameInstance->Render_Font(TEXT("Noto_San_KR"), TEXT("E: 문 열기"), screenPosition, XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f), 0.0f, _float2(0.0f, 0.0f), 0.5f);
    }


    return S_OK;
}

_bool CGate::Check_Collision_With_Ray(const _vector& _vRayStart, const _vector& _vRayDirection, _float& _fDistance)
{
    return _bool();
}

void CGate::Be_Hited(_uint _iCurrentGunDamage)
{
}

HRESULT CGate::Ready_Component()
{
    /* Com_Collider */
    CBounding_AABB::BOUNDING_AABB_DESC ColliderDesc{};
    ColliderDesc.vExtents = _float3(0.40f, 0.55f, 0.40f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vExtents.y, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_AABB"), TEXT("Com_Collider_AABB"), reinterpret_cast<CComponent**>(&m_pColliderCom[COLL_AABB]), &ColliderDesc)))
        return E_FAIL;


    CBounding_Sphere::BOUNDING_SPHERE_DESC  ColliderSphereDesc{};
    ColliderSphereDesc.fRadius = 5.0f;
    ColliderSphereDesc.vCenter = _float3(0.f, ColliderSphereDesc.fRadius - 5.0f, 0.f);
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider_SPHERE"), reinterpret_cast<CComponent**>(&m_pColliderCom[COLL_SPHERE]), &ColliderSphereDesc)))
        return E_FAIL;


    return S_OK;
}

HRESULT CGate::Ready_PartObjects()
{
#pragma region BODY_GATE
     //BODY_GATE 을 만들자. 
    CBody_Gate::BODY_GATE_DESC       GateDesc{};
    GateDesc.pParentState = &m_iState;
    GateDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    GateDesc.fSpeedPerSec = 0.0f;
    GateDesc.fRotationPerSec = 0.0f;

    if (FAILED(__super::Add_PartObject(TEXT("Part_Body"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Body_Gate"), &GateDesc)))
        return E_FAIL;

    m_pBody_Gate = static_cast<CBody_Gate*>(Find_PartObject(TEXT("Part_Body")));
    if (nullptr == m_pBody_Gate)
        return E_FAIL;
#pragma endregion

    return S_OK;
}

CMonster* CGate::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CGate* pInstance = new CGate(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CGate");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CGate::Clone(void* _pArg)
{
    CGate* pInstance = new CGate(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CGate");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGate::Free()
{
    __super::Free();

    Safe_Release(m_pColliderCom[COLL_AABB]);
    Safe_Release(m_pColliderCom[COLL_SPHERE]);
}
