#include "pch.h"
#include "Robo_Billy.h"
#include "GameInstance.h"

#include "Body_Robo_Billy.h"

CRobo_Billy::CRobo_Billy(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CMonster{_pDevice, _pContext}
{
}

CRobo_Billy::CRobo_Billy(const CRobo_Billy& _Prototype)
    :CMonster(_Prototype)
{
}

HRESULT CRobo_Billy::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRobo_Billy::Initialize(void* _pArg)
{
    ROBO_BILLY_DESC* pDesc = static_cast<ROBO_BILLY_DESC*>(_pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    if (FAILED(Ready_Component()))
        return E_FAIL;

    if (FAILED(Ready_PartObjects()))
        return E_FAIL;

    // 현재 얕복 상태라서, 맵툴에서 피킹하면, 이니셜 다시 호출되면서, 애니메이션이 바껴버리네.
    //m_pModelCom->SetUp_Animation(rand() % 20, true, false);

    // Monster Poistion 값 Loading 해온 값으로 세팅. 
    _vector vMonsterPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    vMonsterPos = XMLoadFloat3(&pDesc->vMonsterpos);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vMonsterPos, 1.0f));

    // Monster Rotation 값 Loading 해온 값으로 세팅. 
    _vector vMonsterRotation = XMLoadFloat3(&pDesc->vMonsterRotation);
    _matrix matRotation = XMMatrixRotationRollPitchYawFromVector(vMonsterRotation);

    XMFLOAT3 right = XMFLOAT3{ 1.0f, 0.0f, 0.0f };
    XMFLOAT3 up = XMFLOAT3{ 0.0f, 1.0f, 0.0f };
    XMFLOAT3 look = XMFLOAT3{ 0.0f, 0.0f, 1.0f };

    _vector vRight = XMVector3TransformNormal(XMLoadFloat3(&right), matRotation);
    _vector vUp = XMVector3TransformNormal(XMLoadFloat3(&up), matRotation);
    _vector vLook = XMVector3TransformNormal(XMLoadFloat3(&look), matRotation);

    m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVectorSetW(vRight, 0.0f));
    m_pTransformCom->Set_State(CTransform::STATE_UP, XMVectorSetW(vUp, 0.0f));
    m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVectorSetW(vLook, 0.0f));

    // Monster Scale 값 Loading 해온 값으로 세팅. 
    m_pTransformCom->Scaling(pDesc->vMonsterScale);

    return S_OK;
}

void CRobo_Billy::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}

void CRobo_Billy::Update(_float _fTimeDelta)
{
    //if (true == m_pModelCom->Play_Animation(_fTimeDelta))
    //    int a = 10;

    __super::Update(_fTimeDelta);
}

void CRobo_Billy::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CRobo_Billy::Render()
{
#ifdef _DEBUG
    //m_pNavigationCom->Render();
#endif


    return S_OK;
}

_bool CRobo_Billy::Check_Collision_With_Ray(const _vector& _vRayStart, const _vector& _vRayDirection, _float& _fDistance)
{
    return _bool();
}

void CRobo_Billy::Be_Hited(_uint _iCurrentGunDamage)
{
}

HRESULT CRobo_Billy::Ready_Component()
{
    /* Com_Navigation */
    CNavigation::NAVIGATION_DESC		NaviDesc{};

    NaviDesc.iCurrentCellIndex = 0;

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Navigation"), TEXT("Com_Navigation"), reinterpret_cast<CComponent**>(&m_pNavigationCom), &NaviDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CRobo_Billy::Ready_PartObjects()
{
#pragma region BODY_BILLY
    // BODY_BILLY 를 만들자. 
    CBody_Robo_Billy::BODY_BILLY_DESC       BodyDesc{};
    BodyDesc.pParentState = &m_iState;
    BodyDesc.pParentWorldMatrix = m_pTransformCom->Get_WorldMatrix_Ptr();
    BodyDesc.fSpeedPerSec = 0.0f;
    BodyDesc.fRotationPerSec = 0.0f;

    if (FAILED(__super::Add_PartObject(TEXT("Part_Body"), LEVEL_GAMEPLAY, TEXT("Prototype_GameObject_Body_RoboBilly"), &BodyDesc)))
        return E_FAIL;
#pragma endregion

    return S_OK;
}

CMonster* CRobo_Billy::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CRobo_Billy* pInstance = new CRobo_Billy(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed To Created : CRobo_Billy");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CRobo_Billy::Clone(void* _pArg)
{
    CRobo_Billy* pInstance = new CRobo_Billy(*this);
    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed To Cloned : CRobo_Billy");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRobo_Billy::Free()
{
    __super::Free();

    //Safe_Release(m_pShaderCom);
    //Safe_Release(m_pModelCom);

    Safe_Release(m_pNavigationCom);
}
