#include "pch.h"
#include "Gun.h"
#include "GameInstance.h"

#include "Player.h"

CGun::CGun(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :CPartObject{ _pDevice, _pContext }
{
}

CGun::CGun(const CGun& _Prototype)
    :CPartObject(_Prototype)
{
}

HRESULT CGun::Initialize_Prototype()
{
    if (FAILED(__super::Initialize_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CGun::Initialize(void* _pArg)
{
    GUN_DESC* pDesc = static_cast<GUN_DESC*>(_pArg);

    if (FAILED(__super::Initialize(pDesc)))
        return E_FAIL;

    // �ʱ� ���ü� ����
    m_bVisible = true;

    //m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, TEXT("Layer_Player")));


    return S_OK;
}

void CGun::Priority_Update(_float _fTimeDelta)
{
    if (!m_bVisible)  
        return;
}

void CGun::Update(_float _fTimeDelta)
{
    //Activate_ReloadUI(_fTimeDelta);

    if (!m_bVisible) 
        return;

}

void CGun::Late_Update(_float _fTimeDelta)
{
    if (!m_bVisible)  
        return;

    m_pGameInstance->Add_RenderGroup(CRenderer::RG_NONBLEND, this);
}

HRESULT CGun::Render()
{
    if (!m_bVisible) 
        return S_OK;

    return S_OK;
}

void CGun::Activate_ReloadUI(_float _fTimeDelta)
{
}

void CGun::Shoot_Ray()
{
    CPlayer* pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_GameObject(TEXT("Prototype_GameObject_Player"), LEVEL_GAMEPLAY, TEXT("Layer_Player")));

    if (nullptr != pPlayer)
    {
        
        pPlayer->Shoot_Ray();
    }
}

void CGun::Update_RayCooldown(_float _fTimeDelta)
{
    // m_fCurrentRayTime ���� ����
    m_fCurrentRayTime += _fTimeDelta;

    // ��ٿ��� ���� ������ �ʾҴٸ� �ٷ� �����ع���
    if (m_fCurrentRayTime < m_fRayCoolDown)
        return;

    // ��ٿ��� ���� ���¶�� Ÿ�̸� �ʱ�ȭ�ع���
    m_fCurrentRayTime = 0.0f;

    // Ray �߻礡��
    Shoot_Ray();
}

void CGun::Free()
{
    __super::Free();
}
