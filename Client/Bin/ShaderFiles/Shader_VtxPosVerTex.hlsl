#include "../../../EngineSDK/Hlsl/Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;

Texture2D g_DepthTexture;

float g_FrameIndex;

float g_Opacity = 1.0f;

float g_Timer;
float4 g_vCameraPosition;
float g_RimPower = 1.0f;
float4 g_RimColor = { 1.0f, 0.0f, 0.0f, 1.0f };


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1; // 여기에 월드의 위치를 담을 것이다 라고 픽셸세이더에게 약간 구라치는 느낌.
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    Out.vPosition = mul(float4(In.vPosition, 1.0f), g_WorldMatrix);
    // 만약 픽셸 셰이더에서, 월드 정보를 알고 싶다. 하면 여기 정점 셰이더 쪽에서 중간에 뽑아버림.
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
    
    /* Sample : g_Texture로 부터 지정된 좌표의 색을 지정한 방식으로 얻어온다.*/
	// g_Texture.Sample(어떻게 얻어올건지(Sampler_State), 어디 색을 가져올건지)
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
        
    //Out.vColor.a *= g_Opacity;
    //if (In.vTexcoord.x < 0.5f)
    //    Out.vColor.rgb = 1.f;

    if (0.2f >= Out.vColor.a)
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

    float4 texColor = g_Texture.Sample(LinearSampler, In.vTexcoord + g_Timer * float2(0.1, 0.2));

    float3 lavaColor = lerp(float3(1.0, 0.3, 0.0), float3(1.0, 0.8, 0.0), texColor.r);
    Out.vColor = float4(lavaColor, texColor.a);
    
    if (texColor.r < 0.1f && texColor.g < 0.1f && texColor.b < 0.1f)
        discard;

    return Out;

}

//==============================================================================================

PS_OUT PS_IMPACT_EFFECT(PS_IN_EFFECT In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    //Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    //if (Out.vColor.r < 0.1f && Out.vColor.g < 0.1f && Out.vColor.b < 0.1f)
    //    discard;

    float fTImer = smoothstep(0.0f, 1.0f, g_Timer * 1.0f);
    
    float2 vTexcoord;
    float2 vDiffuseTexcoord = In.vTexcoord;
    
    vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;

    vDiffuseTexcoord.x += fTImer;
    
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler_Clamp, vTexcoord);
    Out.vColor = g_Texture.Sample(LinearSampler_Clamp, vDiffuseTexcoord);

    Out.vColor.a = Out.vColor.a * saturate(vDepthDesc.y - In.vProjPos.w);
    
    return Out;
   
}

struct PS_RIM_OUT
{
    float4 vColor : SV_TARGET0;
};


PS_RIM_OUT PS_RIMLIGHT(PS_IN In)
{
    PS_RIM_OUT Out = (PS_RIM_OUT) 0;
    
    //if (Out.vColor.r < 0.1f && Out.vColor.g < 0.1f && Out.vColor.b < 0.1f)
    //    discard;

    float3 vDirection = normalize(g_vCameraPosition.xyz - In.vWorldPos.xyz);
    
    //float rim = 1.0 - saturate(dot(In.vNormal.xyz, vDirection));
    float rim = saturate(1.0f - vDirection);
    rim = pow(rim, g_RimPower);
    
    Out.vColor.rgb = rim * g_RimColor.rgb;
    Out.vColor.a = 1.0f;

    return Out;
}

PS_OUT PS_MORTALIMPACT(PS_IN_EFFECT In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.r <= 0.1f && Out.vColor.g <= 0.1f && Out.vColor.b <= 0.1f)
        discard;

    float3 fTargetColor = float3(1.0f, 0.2f, 0.2f);
    float fLerpValue = smoothstep(0.9f, 1.0f, Out.vColor.r); 

    Out.vColor.rgb = lerp(Out.vColor.rgb, fTargetColor, fLerpValue);
    
    return Out;
}

//==============================================================================================
technique11 DefaultTechnique
{
    pass DefaultPass
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass PS_MAIN_EFFECT
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
    }

    pass PS_IMPACT_EFFECT
    {
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_IMPACT_EFFECT();
    }

    pass RIM
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RIMLIGHT();
    }


    pass PS_MORTALIMPACT
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MORTALIMPACT();
    }
}