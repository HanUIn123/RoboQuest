#include "../Public/Renderer.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "BlendObject.h"

CRenderer::CRenderer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    : m_pDevice(_pDevice)
    , m_pContext(_pContext)
    , m_pGameInstance(CGameInstance::GetInstance())
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pDevice);
}

HRESULT CRenderer::Initialize()
{
    _uint                   iNumViewports = { 1 };
    D3D11_VIEWPORT          ViewportDesc = {};

    // context 로 부터, viewport 의 정보를 가져와서 세팅해줘. 
    m_pContext->RSGetViewports(&iNumViewports, &ViewportDesc);

    /* 디퍼드 렌더링을 위한, 렌더타겟을 추가하자.  */

    /* Target_Diffuse */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Diffuse"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;

    /* Target_Normal */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Normal"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
    	return E_FAIL;

    // 깊이 비교용 렌더타겟.에서 far 값을 1000 으로하자. 
    // 여기 Depth의 y가 뷰스페이스 상의 깊이..
    /* Target_Depth */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Depth"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 1000.f, 0.f, 0.f))))
        return E_FAIL;

    /* Target_PickDepth */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_PickDepth"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* Target_Glow */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Glow"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* Target_Shade */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shade"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
    	return E_FAIL;

    /* Target_Specular */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Specular"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* Target_Shadow */
    if(FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Shadow"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(1.0f, 1000.0f, 1.0f, 1.0f))))
        return E_FAIL;

    /* Target_Final */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_Final"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f))))
        return E_FAIL;

    /* Target_BlurX */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlurX"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* Target_BlurY */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_BlurY"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;

    /* Target_GlowX */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_GlowX"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;

    /* Target_GlowY */
    if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_GlowY"), (_uint)ViewportDesc.Width, (_uint)ViewportDesc.Height, DXGI_FORMAT_R8G8B8A8_UNORM, _float4(0.0f, 0.0f, 0.0f, 0.0f))))
        return E_FAIL;


    /* MRT_GameObjects */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Diffuse"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Normal"))))
    	return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_Depth"))))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GameObjects"), TEXT("Target_PickDepth"))))
        return E_FAIL;

    /* MRT_Glow */ // Glow 에서의 final.. 
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Glow"), TEXT("Target_Glow"))))
        return E_FAIL;



    /* MRT_LightAcc */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Shade"))))
    	return E_FAIL;
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_LightAcc"), TEXT("Target_Specular"))))
        return E_FAIL;

    /* MRT_Shadow */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Shadow"), TEXT("Target_Shadow"))))
        return E_FAIL;

    /* MRT_Final */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_Final"), TEXT("Target_Final"))))
        return E_FAIL;

    /* MRT_BlurX */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlurX"), TEXT("Target_BlurX"))))
        return E_FAIL;

    /* MRT_BlurY */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_BlurY"), TEXT("Target_BlurY"))))
        return E_FAIL;

    /* MRT_GlowX */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GlowX"), TEXT("Target_GlowX"))))
        return E_FAIL;
    /* MRT_GlowY */
    if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_GlowY"), TEXT("Target_GlowY"))))
        return E_FAIL;



    m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../EngineSDK/Hlsl/Shader_Deferred.hlsl"), VTXPOSTEX::tagElements, VTXPOSTEX::iNumElements);
    if (nullptr == m_pShader)
        return E_FAIL;
    
    m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
    if (nullptr == m_pVIBuffer)
        return E_FAIL;

    XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.0f));
    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.0f, 1.0f));

    // Ready_RenderTarget_DebugMode
#ifdef _DEBUG
    //if (FAILED(m_pGameInstance->Ready_RenderTarget_DebugMode(TEXT("Target_Diffuse"), 100.f, 100.f, 200.f, 200.f)))
    //    return E_FAIL;
    //if (FAILED(m_pGameInstance->Ready_RenderTarget_DebugMode(TEXT("Target_Normal"), 100.f, 300.f, 200.f, 200.f)))
    //    return E_FAIL;
    //if (FAILED(m_pGameInstance->Ready_RenderTarget_DebugMode(TEXT("Target_Depth"), 100.f, 500.f, 200.f, 200.f)))
    //    return E_FAIL;
    //if (FAILED(m_pGameInstance->Ready_RenderTarget_DebugMode(TEXT("Target_Shade"), 300.f, 100.f, 200.f, 200.f)))
    //    return E_FAIL;
    //if (FAILED(m_pGameInstance->Ready_RenderTarget_DebugMode(TEXT("Target_Specular"), 300.f, 300.f, 200.f, 200.f)))
    //    return E_FAIL;
    //if (FAILED(m_pGameInstance->Ready_RenderTarget_DebugMode(TEXT("Target_Shadow"), 300.0f, 500.0f, 200.f, 200.0f)))
    //    return E_FAIL;

    //if (FAILED(m_pGameInstance->Ready_RenderTarget_DebugMode(TEXT("Target_Glow"), 100.0f, 300.0f, 200.0f, 200.0f)))
    //    return E_FAIL;

#endif
    return S_OK;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP _eRenderGroupID, CGameObject* _pGameObject)
{
    if ((nullptr == _pGameObject) || (_eRenderGroupID >= RG_END))
        return E_FAIL;

    m_listRenderObjects[_eRenderGroupID].push_back(_pGameObject);

    // Object_Manager의 객체를 가져다 쓴거라서 또 Add_Ref해야한다.
    Safe_AddRef(_pGameObject);

    return S_OK;
}

HRESULT CRenderer::Render()
{
    if (FAILED(Render_Priority()))
        return E_FAIL;

    if (FAILED(Render_Shadow()))
        return E_FAIL;


    if (FAILED(Render_NonBlend()))
        return E_FAIL;

    //if (FAILED(Render_Blend_UI()))
    //    return E_FAIL;

    if (FAILED(Render_LightAcc()))
        return E_FAIL;



    if (FAILED(Render_Glow()))
        return E_FAIL;

    if (FAILED(Render_Deferred()))
        return E_FAIL;

    if (FAILED(Render_BlurX()))
        return E_FAIL;

    if (FAILED(Render_BlurY()))
        return E_FAIL;


    if (FAILED(Render_GlowX()))
        return E_FAIL;

    if (FAILED(Render_GlowY()))
        return E_FAIL;

    if (FAILED(Render_Final()))
        return E_FAIL;

    if (FAILED(Render_NonLight()))
        return E_FAIL;
    if (FAILED(Render_Blend()))
        return E_FAIL;


 


    if (FAILED(Render_UI()))
        return E_FAIL;



#ifdef _DEBUG
    if (FAILED(Render_DebugMode()))
        return E_FAIL;
#endif
    return S_OK;
}

HRESULT CRenderer::Render_Priority()
{
    // 스카이박스는, Final 타겟에 포함해서 그리겠어.
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Final"))))
        return E_FAIL;


    // 깊이 비교 끄고 안전하게 덮을 수 있게 스카이박스 같은거. renderstate를 설정안해놨기 떄문임 아직.
    for (auto& pRenderObject : m_listRenderObjects[RG_PRIORITY])
    {
        if (FAILED(pRenderObject->Render()))
            return E_FAIL;

        Safe_Release(pRenderObject);
    }

    m_listRenderObjects[RG_PRIORITY].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Shadow()
{
    /* Shadow */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Shadow"))))
        return E_FAIL;

    for (auto& pRendertarget : m_listRenderObjects[RG_SHADOW])
    {
        if (FAILED(pRendertarget->Render_Shadow()))
            return E_FAIL;

        Safe_Release(pRendertarget);
    }
    
    // 여기서 클리어 싸악해서 비워주자 다음 후처리 렌더링 쪽 가기전에 ㅇㅈ?
    m_listRenderObjects[RG_SHADOW].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    m_pGameInstance->Clear_DepthStencil_View();

    return S_OK;
}

HRESULT CRenderer::Render_NonBlend()
{
    /* Diffuse + Normal */
    /*
    여기서 이제 렌타뷰 매니져의 Begin_MRT 해서 Render_Priority 하고나서 NonBlend 로 오기전에, 디퓨즈용 렌타뷰랑 노말용 렌타뷰를 같이 바인딩해줘야함.
    */

    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GameObjects"))))
        return E_FAIL;

    for (auto& pRenderObject : m_listRenderObjects[RG_NONBLEND])
    {
        if (FAILED(pRenderObject->Render()))
            return E_FAIL;

        Safe_Release(pRenderObject);
    }

    m_listRenderObjects[RG_NONBLEND].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_NonLight()
{
    for (auto& pRenderObject : m_listRenderObjects[RG_NONLIGHT])
    {
        if (FAILED(pRenderObject->Render()))
            return E_FAIL;

        Safe_Release(pRenderObject);
    }

    m_listRenderObjects[RG_NONLIGHT].clear();

    return S_OK;
}

HRESULT CRenderer::Render_LightAcc()
{
    /* Shade */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_LightAcc"))))
        return E_FAIL;

    /* m_pShader->Bind_SRV(노말텍스쳐) */
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Normal"), m_pShader, "g_NormalTexture")))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
        return E_FAIL;

    // 월 뷰 투 ㄱㄱ 
    m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix);
    m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix);
    m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix);

    // 빛 연산할 때 역행렬 정보도 같이 넘겨줌.
    _float4x4       ViewMatrixInverse, ProjMatrixInverse;
    ViewMatrixInverse = m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW);
    ProjMatrixInverse = m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ);

    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrixInverse", &ViewMatrixInverse)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrixInverse", &ProjMatrixInverse)))
        return E_FAIL;

    // 카메라 도 같이, 카메라 위치 던져서 픽셀위치와의 방향벡터를 구할 수 있게 됨. 

    _float4         CameraPos = {};
    CameraPos = m_pGameInstance->Get_CameraPos();
    if (FAILED(m_pShader->Bind_RawValue("g_vCameraPosition", &CameraPos, sizeof(_float4))))
        return E_FAIL;

    m_pGameInstance->Render_Lights(m_pShader, m_pVIBuffer);

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Deferred()
{
    // 렌더 디퍼드 때는, Final 에 그리기 할 때, clear 안하겠읍니다~ 라는 의미로 false 불변수를 하나 추가해놨음. 
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Final"), false)))
        return E_FAIL;

    // Deffered 쉐이더 파일에서, 그 중에서도 PS_MAIN_DEFERRED(PS_IN In) 에서 쓸 텍스쳐 두 장 던져줌.
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Diffuse"), m_pShader, "g_DiffuseTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Shade"), m_pShader, "g_ShadeTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Specular"), m_pShader, "g_SpecularTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Shadow"), m_pShader, "g_ShadowTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Depth"), m_pShader, "g_DepthTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Glow"), m_pShader, "g_GlowTexture")))
        return E_FAIL;

    if(FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;


    // 쉐도우 그거를 그릴려고 광원용 뷰와 투영 행렬을 던져주는 작업을 추가해줌. 
    if (FAILED(m_pGameInstance->Bind_Shadow_Matrices(m_pShader, "g_LightViewMatrix", "g_LightProjMatrix")))
        return E_FAIL;

    _float4x4       ViewMatrixInverse, ProjMatrixInverse;
    ViewMatrixInverse = m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_VIEW);
    ProjMatrixInverse = m_pGameInstance->Get_Transform_Float4x4_Inverse(CPipeLine::D3DTS_PROJ);

    m_pShader->Bind_Matrix("g_ViewMatrixInverse", &ViewMatrixInverse);
    m_pShader->Bind_Matrix("g_ProjMatrixInverse", &ProjMatrixInverse);


    m_pShader->Begin(3);

    m_pVIBuffer->Bind_InputAssembler();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_BlurX()
{
    // BlurX 는 BlurX 타겟에 그리겠어.
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BlurX"))))
        return E_FAIL;

    // Target Final 의 final Texture 의 픽셀을 던지겠어.
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Final"), m_pShader, "g_FinalTexture")))
        return E_FAIL;

    // 월튜뷰 하겠어.
    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    m_pShader->Begin(4);

    m_pVIBuffer->Bind_InputAssembler();
    m_pVIBuffer->Render();

    // 다했으니 일단 끝.
    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_BlurY()
{
    // 이거는 BlurX 를 하고 나서의 일이니, MRT_BlurY 라는 곳에 그리겠어!
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_BlurY"))))
        return E_FAIL;

    // FinalTexture 조져놓은 상태의, g_BlurXTexture 의 픽셀을 던지겠어.
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_BlurX"), m_pShader, "g_BlurXTexture")))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    m_pShader->Begin(5);

    m_pVIBuffer->Bind_InputAssembler();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Final()
{
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_BlurY"), m_pShader, "g_BlurYTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_GlowY"), m_pShader, "g_GlowYTexture")))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Final"), m_pShader, "g_FinalTexture")))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    m_pShader->Begin(6);

    m_pVIBuffer->Bind_InputAssembler();
    m_pVIBuffer->Render();

    return S_OK;
}

HRESULT CRenderer::Render_Glow()
{
    /* Glow */
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_Glow"))))
        return E_FAIL;

    for (auto& pRendertarget : m_listRenderObjects[RG_GLOW])
    {
        if (FAILED(pRendertarget->Render_Glow()))
            return E_FAIL;

        Safe_Release(pRendertarget);
    }

    // 여기서 클리어 싸악해서 비워주자 다음 후처리 렌더링 쪽 가기전에 ㅇㅈ?
    m_listRenderObjects[RG_GLOW].clear();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;




    return S_OK;
}

HRESULT CRenderer::Render_GlowX()
{
    // GlowX 는 GlowX 타겟에 그리겠어.
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GlowX"))))
        return E_FAIL;

    // Target_GlowX 의 g_GlowTexture 의 픽셀을 던지겠어.
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_Glow"), m_pShader, "g_GlowTexture")))
        return E_FAIL;

    // 월튜뷰 하겠어.
    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    m_pShader->Begin(7);

    m_pVIBuffer->Bind_InputAssembler();
    m_pVIBuffer->Render();

    // 다했으니 일단 끝.
    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_GlowY()
{
    // 이거는 GlowX 를 하고 나서의 일이니, MRT_GlowY 라는 곳에 그리겠어!
    if (FAILED(m_pGameInstance->Begin_MRT(TEXT("MRT_GlowY"))))
        return E_FAIL;

    // FinalTexture 조져놓은 상태의, g_BlurXTexture 의 픽셀을 던지겠어.
    if (FAILED(m_pGameInstance->Bind_RenderTarget_ShaderResource(TEXT("Target_GlowX"), m_pShader, "g_GlowXTexture")))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_WorldMatrix", &m_WorldMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;
    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    m_pShader->Begin(8);

    m_pVIBuffer->Bind_InputAssembler();
    m_pVIBuffer->Render();

    if (FAILED(m_pGameInstance->End_MRT()))
        return E_FAIL;

    return S_OK;
}

HRESULT CRenderer::Render_Blend()
{
    /*m_RenderObjects[RG_BLEND].sort(Compare);*/
    m_listRenderObjects[RG_BLEND].sort([](CGameObject* _pSour, CGameObject* _pDest)->_bool
        {
            return static_cast<CBlendObject*>(_pSour)->Get_CamDistance() > static_cast<CBlendObject*>(_pDest)->Get_CamDistance();
        });

    for (auto& pRenderObject : m_listRenderObjects[RG_BLEND])
    {
        if (FAILED(pRenderObject->Render()))
            return E_FAIL;

        Safe_Release(pRenderObject);
    }

    m_listRenderObjects[RG_BLEND].clear();

    return S_OK;
}

HRESULT CRenderer::Render_Blend_UI()
{

    for (auto& pRenderObject : m_listRenderObjects[RG_BLEND_UI])
    {
        if (FAILED(pRenderObject->Render()))
            return E_FAIL;
        Safe_Release(pRenderObject);
    }
    m_listRenderObjects[RG_BLEND_UI].clear();


    return S_OK;
}

HRESULT CRenderer::Render_UI()
{
    for (auto& pRenderObject : m_listRenderObjects[RG_UI])
    {
        if (FAILED(pRenderObject->Render()))
            return E_FAIL;

        Safe_Release(pRenderObject);
    }

    m_listRenderObjects[RG_UI].clear();

    return S_OK;
}

HRESULT CRenderer::Render_DebugMode()
{
    if (FAILED(m_pShader->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShader->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Render_RenderTarget_DebugMode(TEXT("MRT_GameObjects"), m_pShader, m_pVIBuffer)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Render_RenderTarget_DebugMode(TEXT("MRT_LightAcc"), m_pShader, m_pVIBuffer)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Render_RenderTarget_DebugMode(TEXT("MRT_Shadow"), m_pShader, m_pVIBuffer)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Render_RenderTarget_DebugMode(TEXT("MRT_Glow"), m_pShader, m_pVIBuffer)))
        return E_FAIL;

    if (FAILED(m_pGameInstance->Render_RenderTarget_DebugMode(TEXT("MRT_Final"), m_pShader, m_pVIBuffer)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Render_RenderTarget_DebugMode(TEXT("MRT_BlurX"), m_pShader, m_pVIBuffer)))
        return E_FAIL;
    if (FAILED(m_pGameInstance->Render_RenderTarget_DebugMode(TEXT("MRT_BlurY"), m_pShader, m_pVIBuffer)))
        return E_FAIL;

    return S_OK;
}

CRenderer* CRenderer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CRenderer* pInstance = new CRenderer(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX("Failed To Created : CRenderer");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CRenderer::Free()
{
    __super::Free();

    Safe_Release(m_pShader);
    Safe_Release(m_pVIBuffer);

    for (auto& RenderObjects : m_listRenderObjects)
    {
        for (auto& pRenderObjects : RenderObjects)
        {
            Safe_Release(pRenderObjects);
        }
        RenderObjects.clear();
    }

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}

