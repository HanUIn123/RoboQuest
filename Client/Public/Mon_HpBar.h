#pragma once
#include "Client_Defines.h"
#include "UIPartObject.h"

BEGIN(Client)
class CMon_HpBar : public CUIPartObject
{
public:
    struct MONSTER_HPBAR_DESC : public CUIPartObject::UIPARTOBJECT_DESC
    {
        //const _uint* pParentState = { nullptr };

        const _int* pMonsterHp = nullptr;
    };

public:
    void Set_WorldMatrix(_float4x4* _pWorldMatrix) { m_CombinedWorldMatrix = *_pWorldMatrix; }

//private:
protected:
    CMon_HpBar(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CMon_HpBar(const CMon_HpBar& _Prototype);
    virtual ~CMon_HpBar() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;
    virtual void                Priority_Update(_float _fTimeDelta) override;
    virtual void                Update(_float _fTimeDelta) override;
    virtual void                Late_Update(_float _fTimeDelta) override;
    virtual HRESULT             Render() override;

    void                        Decrease_Hp(_uint _iCurrentHp);
    void                        Set_DeltaTime(_float _fDeltaTime){m_fDeltaTime = _fDeltaTime;}

    void                        Show_HealthBar(_bool _bShowHealthBar) { m_bShowHealthBar = _bShowHealthBar; }


protected:
    _float                      m_fSizeX, m_fSizeY = {};
    _float                      m_fX, m_fY, m_fZ = {};
    _float                      m_fFar, m_fNear = {};

protected:
    _float                      m_fCurrentHp = {};
    _float                      m_fBackgroundHp = {};
    _float                      m_fDeltaTime = {};
    _float                      m_fMaxHp = {};

    _bool                       m_bShowHealthBar = { false };

public:
    static CMon_HpBar* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual CGameObject* Clone(void* _pArg) override;
    virtual void Free() override;
};
END
