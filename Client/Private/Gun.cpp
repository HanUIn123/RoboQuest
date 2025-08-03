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

    // 초기 가시성 설정
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
    // m_fCurrentRayTime 갱신 ㄱㄱ
    m_fCurrentRayTime += _fTimeDelta;

    // 쿨다운이 아직 끝나지 않았다면 바로 리턴해벌힘
    if (m_fCurrentRayTime < m_fRayCoolDown)
        return;

    // 쿨다운이 끝난 상태라면 타이머 초기화해벌힘
    m_fCurrentRayTime = 0.0f;

    // Ray 발사ㄱㄱ
    Shoot_Ray();
}

void CGun::Free()
{
    __super::Free();
}
