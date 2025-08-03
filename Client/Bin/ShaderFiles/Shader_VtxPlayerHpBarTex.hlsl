#include "../../../EngineSDK/Hlsl/Engine_Shader_Defines.hlsli"
float4x4        g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D       g_Texture;
int             g_TextureIndex;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;           // 여기에 월드의 위치를 담을 것이다 라고 픽셸세이더에게 약간 구라치는 느낌.
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT          Out  = (VS_OUT) 0;
    
    Out.vPosition = mul(float4(In.vPosition, 1.0f), g_WorldMatrix);
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

    if (g_TextureIndex == 0)
    {
        Out.vColor = float4(0.0f, 1.0f, 0.0f, 1.0f);
    }
    
    if (0.2f >= Out.vColor.a)
        discard;
    
    return Out;
}

PS_OUT PS_BOSS_HPBAR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (g_TextureIndex == 0)
    {
        Out.vColor = float4(0.96f, 0.20f, 1.0f, 1.0f);
    }
    
    if (0.2f >= Out.vColor.a)
        discard;
    
    return Out;
}

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

    pass BossHpBar
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BOSS_HPBAR();
    }
}