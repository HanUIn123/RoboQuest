#include "../../../EngineSDK/Hlsl/Engine_Shader_Defines.hlsli"
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float3 g_PickedCoord;
float g_BrushRange;
float g_fImageTypeX;
float g_fImageTypeY;
bool g_Brushed;

Texture2D g_DiffuseTexture[8];
uint g_DiffuseTextureFirstIndex;
uint g_DiffuseTextureSecondIndex;

Texture2D g_MouseTexture;
Texture2D g_BrushTexture;
vector g_vBrushPosition = vector(20.f, 0.f, 20.f, 1.f);
float g_fBrushRange = 10.f;

Texture2D g_MaskTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTexIndex : TEXCOORD1;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vTexIndex : TEXCOORD1;
    float4 vWorldPos : TEXCOORD2;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    Out.vPosition = mul(float4(In.vPosition, 1.0f), g_WorldMatrix);
    Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
    Out.vPosition = mul(Out.vPosition, g_ProjMatrix);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.0f), g_WorldMatrix));
    
    Out.vTexcoord = In.vTexcoord;
    Out.vTexIndex = float4(In.vTexIndex, 1.0f);
    Out.vWorldPos = mul(float4(In.vPosition, 1.0f), g_WorldMatrix);

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vTexIndex : TEXCOORD1;
    float4 vWorldPos : TEXCOORD2;
};

struct PS_OUT
{
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vSourDiffuse = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexcoord * 30.f);
    vector vDestDiffuse = g_DiffuseTexture[1].Sample(LinearSampler, In.vTexcoord * 30.f);

    vector vMask = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);
    vector vBrush = 0.f;

    if (g_vBrushPosition.x - g_fBrushRange <= In.vWorldPos.x && In.vWorldPos.x < g_vBrushPosition.x + g_fBrushRange &&
		g_vBrushPosition.z - g_fBrushRange <= In.vWorldPos.z && In.vWorldPos.z < g_vBrushPosition.z + g_fBrushRange)
    {
        float2 vTexcoord = 0.f;

        vTexcoord.x = (In.vWorldPos.x - (g_vBrushPosition.x - g_fBrushRange)) / (2.f * g_fBrushRange);
        vTexcoord.y = ((g_vBrushPosition.z + g_fBrushRange) - In.vWorldPos.z) / (2.f * g_fBrushRange);

        vBrush = g_BrushTexture.Sample(LinearSampler, vTexcoord);
    }

    vector vMtrlDiffuse = vDestDiffuse * vMask + vSourDiffuse * (1.f - vMask) + vBrush;

    Out.vDiffuse = vMtrlDiffuse;

	/* 0 ~ 1 */   /* -1 ~ 1 */
	/* -1 -> 0, 1 -> 1 */
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);

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
}