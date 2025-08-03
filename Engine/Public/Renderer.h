#pragma once
#include "Base.h"

BEGIN(Engine)


/* 1.ȭ�鿡 �׷������� ��ü���� ��Ƶд�.  */
/* 1_1. ���� �׸������ϴ� ������� ��ü���� �з��Ͽ� ��Ƶд�. */

/* 2.������� ��Ƴ��� ��ü���� �����س��� ������� ���鼭 �����Լ��� ȣ��(������, ��ο���)���ش�. */


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
    // �簢���̶�� ���� �׸� ���� �ʿ��� �ؽ��İ�, RenderTarget Ŭ������ ������, ������ RenderTarget �� �ϳ� �߰��ؼ� �۾��ϴ� ���̴�. 
    HRESULT Render_DebugMode();
#endif

public:
    static CRenderer* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
    virtual void Free() override;

};

END