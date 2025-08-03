#pragma once
#include "Client_Defines.h"
#include "UIObject.h"

#include "Player.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CVIBuffer_Rect;
END


BEGIN(Client)
class CBulletCount final: public CUIObject
{
//public:
//    enum DIGIT_TEXTURE {ONE_DIGIT, TEN_DIGIT, HUN_DIGT, DT_END };
//
//public:
//    struct BULLET_COUNT_DESC : public CUIObject::UIOBJECT_DESC
//    {
//
//    };
//
//private:
//    CBulletCount(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
//    CBulletCount(const CBulletCount& _Prototype);
//    virtual ~CBulletCount() = default;
//
//public:
//    virtual HRESULT Initialize_Prototype() override;
//    virtual HRESULT Initialize(void* _pArg)override;
//
//    virtual void Priority_Update(_float _fTimeDelta)override;
//    virtual void Update(_float _fTimeDelta)override;
//    virtual void Late_Update(_float _fTimeDelta)override;
//
//    virtual HRESULT Render()override;
//
//private:
//    CShader* m_pShaderCom = { nullptr };
//    CVIBuffer_Rect* m_pVIBufferCom = { nullptr };
//    //CTexture* m_pTextureCom[DT_END] = {nullptr};
//    CTexture* m_pTextureCom = {nullptr};
//
//public:
//    HRESULT     Ready_Component();
//
//private:
//    _uint                   m_iTextureIndex = {};
//
//private:
//    _uint                   m_iHunDigit = {};
//    _uint                   m_iTenDigit = {};
//    _uint                   m_iOneDigit = {};
//
//    _float                  m_iNumHeight = {};
//
//private:
//    CPlayer*                m_pPlayer = { nullptr };
//
//
//    vector<_uint>           m_vecNumber;
//
//    DIGIT_TEXTURE       m_eDigitTexture = DT_END;
//    _float                  m_fGap = {};
//    _float                  m_fX{}, m_fY{};
//
//    _vector                 m_vDigitPosition[3] = {};
//
//public:
//    static CBulletCount* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
//    virtual CGameObject* Clone(void* _pArg) override;
//    virtual void Free() override;
};

END