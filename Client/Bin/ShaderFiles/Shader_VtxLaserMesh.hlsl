#include "../../../EngineSDK/Hlsl/Engine_Shader_Defines.hlsli"
float4x4        g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D       g_DiffuseTexture;
Texture2D       g_NormalTexture;

float g_Timer;
float4 g_vCameraPosition;
float g_RimPower = 0.5f;
float4 g_RimColor = { 1.0f, 0.0f, 0.0f, 1.0f };



struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;           
    float4 vProjPos : TEXCOORD2;
    
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT          Out  = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    // 모델의 Normal 벡터 구하려고 하는데 0 ~ 1 로하려고
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.0f), g_WorldMatrix));
    
    // 바이노말이 y축 너낌이라햇잖슴? 그 바이노말은 그래서 z축 하고 x축 하고 외적하면 구할 수 있음. 
    Out.vBinormal = vector(normalize(cross(Out.vNormal.xyz, Out.vTangent.xyz)), 0.0f);
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};

struct PS_OUT
{
    //float4 vColor : SV_TARGET0;
    
    float4 vDiffuse : SV_TARGET0;
    float4 vNormal : SV_TARGET1;
    float4 vDepth : SV_TARGET2;
    
    float4 vPickDepth : SV_TARGET3;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexcoord = float2((In.vTexcoord.y + g_Timer * 0.03f) + pow(2, g_Timer), In.vTexcoord.x + g_Timer * 0.05f);
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
    
    float3 LarvaColor = lerp(float3(1.0f, 0.3f, 0.0f), float3(1.0f, 0.8f, 0.0f), vMtrlDiffuse.r);
    
    if (vMtrlDiffuse.r < 0.1f && vMtrlDiffuse.g < 0.1f && vMtrlDiffuse.b < 0.1f)
        discard;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    float3 vNormal = vNormalDesc.xyz * 2.0f - 1.0f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    
    vNormal = normalize(mul(vNormal, WorldMatrix));
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 1.f, 0.f);

    return Out;
}

//==========================================================================================

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
    
    float fRim = 1.0 - saturate(dot(In.vNormal.xyz, vDirection));
    fRim = pow(fRim, g_RimPower);
    
    Out.vColor.rgb = fRim * g_RimColor.rgb;
    Out.vColor.a = 1.0f;

    return Out;
}

//==========================================================================================


PS_OUT PS_LASER_DEFAULT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 vTexcoord = float2((In.vTexcoord.y + g_Timer * 1.0f) + pow(2, g_Timer), In.vTexcoord.x + g_Timer * 2.0f);
    float4 vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, vTexcoord);
    
    float3 LarvaColor = lerp(float3(1.0f, 0.3f, 0.0f), float3(1.0f, 0.8f, 0.0f), vMtrlDiffuse.r);
    
    if (vMtrlDiffuse.r < 0.1f && vMtrlDiffuse.g < 0.1f && vMtrlDiffuse.b < 0.1f)
        discard;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    float3 vNormal = vNormalDesc.xyz * 2.0f - 1.0f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    
    vNormal = normalize(mul(vNormal, WorldMatrix));
    
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 1.f, 0.f);

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


    pass RIM
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_RIMLIGHT();
    }

    pass LaserDefault
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_LASER_DEFAULT();
    }
}