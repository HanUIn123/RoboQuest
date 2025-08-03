#pragma once
#include "Base.h"

BEGIN(Engine)


/* 1.화면에 그려져야할 객체들을 모아둔다.  */
/* 1_1. 내가 그리고자하는 순서대로 객체들을 분류하여 모아둔다. */

/* 2.순서대로 모아놓은 객체들을 저장해놓은 순서대로 돌면서 렌더함수를 호출(렌더콜, 드로우콜)해준다. */


class CRenderer final : public CBase
{
public:
    enum RENDERGROUP { RG_PRIORITY, RG_SHADOW, RG_NONBLEND, RG_GLOW, RG_NONLIGHT, RG_BLEND, RG_BLEND_UI, RG_UI, RG_END };

private:
    CRenderer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual ~CRenderer() = default;

public:
    HRESULT Initialize();
    HRESULT Add_RenderGroup(RENDERGROUP _eRenderGroupID, class CGameObject* _pGameObject);
    HRESULT Render();

private:
    ID3D11Device*                   m_pDevice = { nullptr };
    ID3D11DeviceContext*            m_pContext = { nullptr };
    class CGameInstance*            m_pGameInstance = { nullptr };

    class CShader*                  m_pShader = { nullptr };
    class CVIBuffer_Rect*           m_pVIBuffer = { nullptr };

    _float4x4                       m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};

    list<class CGameObject*>        m_listRenderObjects[RG_END];

private:
    HRESULT Render_Priority();
    HRESULT Render_Shadow();
    HRESULT Render_NonBlend();
    HRESULT Render_NonLight();
    HRESULT Render_LightAcc();
    HRESULT Render_Deferred();

    HRESULT Render_BlurX();
    HRESULT Render_BlurY();
    HRESULT Render_Final();

    HRESULT Render_Glow();
    HRESULT Render_GlowX();
    HRESULT Render_GlowY();

    HRESULT Render_Blend();
    HRESULT Render_Blend_UI();
    HRESULT Render_UI();

#ifdef _DEBUG
private:
    // 사각형이라는 것을 그릴 때에 필요한 텍스쳐가, RenderTarget 클래스에 있으니, 디버깅용 RenderTarget 을 하나 추가해서 작업하는 것이다. 
    HRESULT Render_DebugMode();
#endif

public:
    static CRenderer* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual void Free() override;

};

END