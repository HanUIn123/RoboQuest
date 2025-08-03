#include "../../../EngineSDK/Hlsl/Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;

Texture2D g_DepthTexture;

Texture2D g_TrailTexture;

float g_FrameIndex;

float g_Opacity = 1.0f;

float g_Alpha;

float g_Timer;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1; // ���⿡ ������ ��ġ�� ���� ���̴� ��� �ȼм��̴����� �ణ ����ġ�� ����.
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    Out.vPosition = mul(float4(In.vPosition, 1.0f), g_WorldMatrix);
    // ���� �ȼ� ���̴�����, ���� ������ �˰� �ʹ�. �ϸ� ���� ���� ���̴� �ʿ��� �߰��� �̾ƹ���.
    Out.vWorldPos = Out.vPosition;
    Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
    Out.vPosition = mul(Out.vPosition, g_ProjMatrix);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // Out.vColor = 1.0f;
    
    /* Sample : g_Texture�� ���� ������ ��ǥ�� ���� ������ ������� ���´�.*/
	// g_Texture.Sample(��� ���ð���(Sampler_State), ��� ���� �����ð���)
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
        
    //Out.vColor.a *= g_Opacity;
    //if (In.vTexcoord.x < 0.5f)
    //    Out.vColor.rgb = 1.f;

    if (0.2f >= Out.vColor.a)
        discard;
    
    
    return Out;
}

//==============================================================================================
PS_OUT PS_BULLET_COUNT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // Out.vColor = 1.0f;
    
    /* Sample : g_Texture�� ���� ������ ��ǥ�� ���� ������ ������� ���´�.*/
	// g_Texture.Sample(��� ���ð���(Sampler_State), ��� ���� �����ð���)
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.r <= 0.5f || Out.vColor.g <= 0.5f || Out.vColor.b <= 0.5f)
        discard;
    
    return Out;
}
//==============================================================================================

struct VS_OUT_EFFECT
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

VS_OUT_EFFECT VS_MAIN_EFFECT(VS_IN In)
{
    VS_OUT_EFFECT Out = (VS_OUT_EFFECT) 0;
    
    Out.vPosition = mul(float4(In.vPosition, 1.0f), g_WorldMatrix);
    Out.vWorldPos = Out.vPosition;
    
    Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
    Out.vPosition = mul(Out.vPosition, g_ProjMatrix);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

struct PS_IN_EFFECT
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

PS_OUT PS_MAIN_EFFECT(PS_IN_EFFECT In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    float2 vTexcoord;
    
    // ( -1 , 1 ) ~ ( 1, -1 ) �� ���� ������ x y �� ���Ӱ� ������. ( 0, 0 ) ~ ( 1 , 1 )
    // w������ ���� �����ؾ� ����� �� ��.
    vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;
    
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, vTexcoord);
    
    // In.vProjPos.w �� �佺���̽� ���� z�� �ǹ���. 
    //Out.vColor.a = Out.vColor.a * (vDepthDesc.y - In.vProjPos.w);
    Out.vColor.a = Out.vColor.a * saturate(vDepthDesc.y - In.vProjPos.w);
    
    return Out;
}

//==============================================================================================

//PS_OUT PS_SMALL_EXPLOSION_EFFECT(PS_IN_EFFECT In)
//{
//    PS_OUT Out = (PS_OUT) 0;
    
//    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
//    float2 vTexcoord;
    
//    // ( -1 , 1 ) ~ ( 1, -1 ) �� ���� ������ x y �� ���Ӱ� ������. ( 0, 0 ) ~ ( 1 , 1 )
//    // w������ ���� �����ؾ� ����� �� ��.
//    vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
//    vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;
    
//    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, vTexcoord);
    
//    // In.vProjPos.w �� �佺���̽� ���� z�� �ǹ���. 
//    //Out.vColor.a = Out.vColor.a * (vDepthDesc.y - In.vProjPos.w);
//    Out.vColor.a = Out.vColor.a * saturate(vDepthDesc.y - In.vProjPos.w);
    
//    return Out;
//}



PS_OUT PS_IMPACT_EFFECT(PS_IN_EFFECT In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.r < 0.1f && Out.vColor.g < 0.1f && Out.vColor.b < 0.1f)
        discard;

    
    float2 vTexcoord;
    
    // ( -1 , 1 ) ~ ( 1, -1 ) �� ���� ������ x y �� ���Ӱ� ������. ( 0, 0 ) ~ ( 1 , 1 )
    // w������ ���� �����ؾ� ����� �� ��.
    vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;
    
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, vTexcoord);
    
  
    // In.vProjPos.w �� �佺���̽� ���� z�� �ǹ���. 
    //Out.vColor.a = Out.vColor.a * (vDepthDesc.y - In.vProjPos.w);
    Out.vColor.a = Out.vColor.a * saturate(vDepthDesc.y - In.vProjPos.w);
    
    return Out;
   
}

PS_OUT PS_WEAPON_UI(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // Out.vColor = 1.0f;
    
    /* Sample : g_Texture�� ���� ������ ��ǥ�� ���� ������ ������� ���´�.*/
	// g_Texture.Sample(��� ���ð���(Sampler_State), ��� ���� �����ð���)
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
        
    if (Out.vColor.r < 0.01f && Out.vColor.g < 0.01f && Out.vColor.b < 0.01f)
        discard;
    
    Out.vColor.a *= g_Opacity;
    //if (In.vTexcoord.x < 0.5f)
    //    Out.vColor.rgb = 1.f;

    if (0.2f >= Out.vColor.a)
        discard;
    
    
    return Out;
}

PS_OUT PS_INTERACTION_UI(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // Out.vColor = 1.0f;
    
    /* Sample : g_Texture�� ���� ������ ��ǥ�� ���� ������ ������� ���´�.*/
	// g_Texture.Sample(��� ���ð���(Sampler_State), ��� ���� �����ð���)
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
        
    Out.vColor.a *= g_Opacity;
    //if (In.vTexcoord.x < 0.5f)
    //    Out.vColor.rgb = 1.f;

    if (0.2f >= Out.vColor.a)
        discard;
    
    
    return Out;
}

PS_OUT PS_DAMAGE_NUMBER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (Out.vColor.r < 0.1f && Out.vColor.g < 0.1f && Out.vColor.b < 0.1f)
        discard;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    //Out.vColor.a *= g_Alpha;

    if (Out.vColor.a <= 0.05f)
        discard;
    
    return Out;
}


PS_OUT PS_LETTER(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (Out.vColor.r < 0.1f && Out.vColor.g < 0.1f && Out.vColor.b < 0.1f)
        discard;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    //Out.vColor.a *= g_Alpha;

    if (Out.vColor.a <= 0.05f)
        discard;
    
    return Out;
}

PS_OUT PS_TRAIL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // Out.vColor = 1.0f;
    
    /* Sample : g_Texture�� ���� ������ ��ǥ�� ���� ������ ������� ���´�.*/
	// g_Texture.Sample(��� ���ð���(Sampler_State), ��� ���� �����ð���)
    Out.vColor = g_TrailTexture.Sample(LinearSampler, In.vTexcoord);
        
    //Out.vColor.a *= g_Opacity;
    //if (In.vTexcoord.x < 0.5f)
    //    Out.vColor.rgb = 1.f;
    if (Out.vColor.r < 0.1f && Out.vColor.g < 0.1f && Out.vColor.b < 0.1f)
        discard;
    
    
    
    //if (0.2f >= Out.vColor.a)
    //    discard;
    
    
    return Out;
}

PS_OUT PS_BUS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // Out.vColor = 1.0f;
    
    /* Sample : g_Texture�� ���� ������ ��ǥ�� ���� ������ ������� ���´�.*/
	// g_Texture.Sample(��� ���ð���(Sampler_State), ��� ���� �����ð���)
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
        
    //Out.vColor.a *= g_Opacity;
    //if (In.vTexcoord.x < 0.5f)
    //    Out.vColor.rgb = 1.f;

    if (0.2f >= Out.vColor.a)
        discard;
    
    
    return Out;
}


PS_OUT PS_BACKGROUND(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // Out.vColor = 1.0f;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord + g_Timer * float2(0.1, 0.0));
    
    /* Sample : g_Texture�� ���� ������ ��ǥ�� ���� ������ ������� ���´�.*/
	// g_Texture.Sample(��� ���ð���(Sampler_State), ��� ���� �����ð���)
    //Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
        
    //Out.vColor.a *= g_Opacity;
    //if (In.vTexcoord.x < 0.5f)
    //    Out.vColor.rgb = 1.f;

    if (0.02f >= Out.vColor.a)
        discard;
    
    
    return Out;
}




//==============================================================================================
technique11 DefaultTechnique
{
    pass DefaultPass
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass PS_MAIN_EFFECT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
    }


    pass PS_BULLET_COUNT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BULLET_COUNT();
    }

    pass PS_IMPACT_EFFECT
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_IMPACT_EFFECT();
    }

    pass PS_WEAPON_UI
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_WEAPON_UI();
    }

    pass PS_INTERACTION_UI
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_INTERACTION_UI();
    }

    pass PS_DAMAGE_NUMBER
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DAMAGE_NUMBER();
    }

    pass PS_LETTER
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LETTER();
    }

    pass PS_TRAIL
    {
        SetRasterizerState(RS_No_Cull);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_TRAIL();
    }

    pass BUS
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BUS();
    }

    pass BACKGROUND
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BACKGROUND();
    }
}