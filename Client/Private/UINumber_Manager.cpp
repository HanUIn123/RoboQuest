#include "pch.h"
#include "UINumber_Manager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CUINumber_Manager)

CUINumber_Manager::CUINumber_Manager()
{
}

CUINumber_Manager::~CUINumber_Manager()
{

}

HRESULT CUINumber_Manager::Initialize()
{

    return S_OK;
}

HRESULT CUINumber_Manager::Add_NumberUI(_uint uiID, CUINumber::UINUMBER_DESC& _Desc)
{
    CUINumber* pNumberUI = CUINumber::Create(m_pDevice, m_pContext);
    if (!pNumberUI)
        return E_FAIL;

    if (FAILED(pNumberUI->Initialize(static_cast<CUINumber::UINUMBER_DESC*>(&_Desc))))
    {
        Safe_Release(pNumberUI);
        return E_FAIL;
    }

    m_mapNumberUI[uiID].push_back(pNumberUI);

    return S_OK;
}


void CUINumber_Manager::Set_DigitAt(_uint uiID, _int _iRealNum)
{
    if (m_mapNumberUI.find(uiID) == m_mapNumberUI.end())
        return;

    auto& vecNumbers = m_mapNumberUI[uiID];
    _uint iTotalDigits = static_cast<_uint>(vecNumbers.size());

    for (auto* pNumberUI : vecNumbers)
    {
        if (pNumberUI)
            pNumberUI->Set_Digit(0);
    }

    _uint iDigitIndex = iTotalDigits - 1;

    while (_iRealNum > 0 && iDigitIndex < iTotalDigits)
    {
        if (uiID == 5 || uiID == 6)
        {
            vecNumbers[iDigitIndex]->Set_Digit(10);
            break;
        }
       


        else
        {
            vecNumbers[iDigitIndex]->Set_Digit(_iRealNum % 10);
            _iRealNum /= 10;
            --iDigitIndex;
        }
    }
}

void CUINumber_Manager::Update(_float _fDeltaTime)
{
    for (auto& pair : m_mapNumberUI)
    {
        for (auto& pNumberUI : pair.second)
        {
            if (pNumberUI)
                pNumberUI->Update(_fDeltaTime);
        }
    }

}

void CUINumber_Manager::Late_Update(_float _fDeltaTime)
{
    for (auto& pair : m_mapNumberUI)
    {
        for (auto& pNumberUI : pair.second)
        {
            if (pNumberUI)
                pNumberUI->Late_Update(_fDeltaTime);
        }
    }

}

void CUINumber_Manager::Render()
{
    if (m_bVisible)
    {
        for (auto& pair : m_mapNumberUI)
        {
            for (auto& pNumberUI : pair.second)
            {
                if (pNumberUI)
                    pNumberUI->Render();
            }
        }
    }


}

void CUINumber_Manager::Free()
{
    __super::Free();

    for (auto& pair : m_mapNumberUI)
    {
        for (auto& pNumberUI : pair.second)
        {
            Safe_Release(pNumberUI);
        }
        pair.second.clear();
    }
    m_mapNumberUI.clear();
}
