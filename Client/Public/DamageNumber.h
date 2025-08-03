#pragma once
#include "Client_Defines.h"
#include "UIPartObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CDamageNumber final :public CUIPartObject
{
public:
    struct DAMAGE_NUM_DESC : public CUIPartObject::UIPARTOBJECT_DESC
    {
        _int iNumber;       
        _float fLifeTime;
        _uint iIndex;
        //_float fY;
    };

public:
    void Set_WorldMatrix(_float4x4* _pWorldMatrix) { m_CombinedWorldMatrix = *_pWorldMatrix; }

private:
    CDamageNumber(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    CDamageNumber(const CDamageNumber& _Prototype);
    virtual ~CDamageNumber() = default;

public:
    virtual HRESULT             Initialize_Prototype() override;
    virtual HRESULT             Initialize(void* _pArg) override;
    virtual void                Priority_Update(_float _fTimeDelta) override;
    virtual void                Update(_float _fTimeDelta) override;
    virtual void                Late_Update(_float _fTimeDelta) override;
    virtual HRESULT             Render() override;

    void                        Set_DamageNumberDigit(_uint _iDigit, _uint _iIndex);
    void                        Set_LifeTime(_float _fTime);
    _bool                       IsActive() { return m_fLifeTime > 0.0f; }

private:
    HRESULT                     Ready_Components();
private:
    CShader*                    m_pShaderCom = { nullptr };
    CTexture*                   m_pTextureCom = { nullptr };
    CVIBuffer_Rect*             m_pVIBufferCom = { nullptr };

private:
    _uint                       m_iDigit = {};
    _uint                       m_iIndex = { };
    _float                      m_fLifeTime = {1.0f};
    _bool                       m_bShow = { false };
    _float                      m_fVerticalOffset = 0.0f;  
    _float                      m_fAlpha = 1.0f;
    _float                      m_fHeight = {};

public:
    static CDamageNumber*       Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual                     CGameObject* Clone(void* _pArg) override;
    virtual void                Free() override;
};
END
