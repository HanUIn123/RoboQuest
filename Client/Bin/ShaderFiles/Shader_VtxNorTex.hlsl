#include "../../../EngineSDK/Hlsl/Engine_Shader_Defines.hlsli"
float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float3 g_PickedCoord;
float g_BrushRange;
bool g_Brushed;
float g_fImageTypeX;
float g_fImageTypeY;

Texture2D   g_DiffuseTexture[8];
uint        g_DiffuseTextureFirstIndex;
uint        g_DiffuseTextureSecondIndex;

Texture2D   g_MouseTexture;
Texture2D   g_BrushTexture;
vector      g_vBrushPosition = vector(20.f, 0.f, 20.f, 1.f);

Texture2D   g_MaskTexture;

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
    float4 vWorldPos : TEXCOORD2; // 여기에 월드의 위치를 담을 것이다 라고 픽셸세이더에게 약간 구라치는 느낌.
    float4 vProjPos : TEXCOORD3;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    Out.vPosition = mul(float4(In.vPosition, 1.0f), g_WorldMatrix);
    
    Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
    Out.vPosition = mul(Out.vPosition, g_ProjMatrix);
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vTexIndex = float4(In.vTexIndex, 1.0f);
    Out.vWorldPos = mul(float4(In.vPosition, 1.0f), g_WorldMatrix);
    
    // 여기서의 Out.vPosition 은 월->뷰->투영 까지 변환하고 난 상태의 position 을 vProjPos 에 담아주는 거임. 
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vTexIndex : TEXCOORD1;
    float4 vWorldPos : TEXCOORD2;
    float4 vProjPos : TEXCOORD3;
};

struct PS_OUT
{
    //float4 vColor : SV_TARGET0;
    float4 vDiffuse : SV_Target0;
    float4 vNormal : SV_Target1;
    
    float4 vDepth : SV_Target2;
    
    
    float4 vPickDepth : SV_TARGET3;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vFirstDiffuse;
    vector vSecondDiffuse = g_DiffuseTexture[3].Sample(LinearSampler, In.vTexcoord * 30.0f);
    vector vMouseDiffuse = g_MouseTexture.Sample(LinearSampler, In.vTexcoord * 30.0f);

    if (g_DiffuseTextureFirstIndex == 0)
    {
        vFirstDiffuse = g_DiffuseTexture[0].Sample(LinearSampler, In.vTexcoord * 30.0f);
    }
    
    if (g_DiffuseTextureFirstIndex == 1)
    {
        vFirstDiffuse = g_DiffuseTexture[1].Sample(LinearSampler, In.vTexcoord * 30.0f);
    }
    
    if (g_DiffuseTextureFirstIndex == 2)
    {
        vFirstDiffuse = g_DiffuseTexture[2].Sample(LinearSampler, In.vTexcoord * 30.0f);
    }
    
    if (g_DiffuseTextureFirstIndex == 3)
    {
        vFirstDiffuse = g_DiffuseTexture[3].Sample(LinearSampler, In.vTexcoord * 30.0f);
    }
    
    if (g_DiffuseTextureFirstIndex == 4)
    {
        vFirstDiffuse = g_DiffuseTexture[4].Sample(LinearSampler, In.vTexcoord * 30.0f);
    }
    

    float fDistance = distance(In.vWorldPos.xyz, g_PickedCoord);
    
    vector vMask = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);
    vector vMtrlDiffuse = vFirstDiffuse * vMask + vSecondDiffuse * (1.f - vMask);
    
    //vector vShade = max(dot(normalize(g_vLightDir) * -1.0f, normalize(In.vNormal)), 0.0f) + (g_vLightAmbient * g_vMaterialAmbient);
    //vector vReflect = reflect(normalize(g_vLightDir), normalize(In.vNormal));
    //vector vLook = In.vWorldPos - g_vCameraPosition;
    
    //float fSpecular = pow(max(dot(normalize(vReflect) * -1.0f, normalize(vLook)), 0.0f), 50.0f);
    

    if (In.vTexIndex.x > 0.1f)
    {
        //Out.vColor = vMtrlDiffuse;
        //Out.vColor = vSecondDiffuse;
  
        Out.vDiffuse = vMtrlDiffuse;
        //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    }
    else
    {
        //Out.vColor = (g_vLightDiffuse * vFirstDiffuse) * saturate(vShade) + fSpecular * (g_vLightSpecular * g_vMaterialSpecular);
        
        Out.vDiffuse = vFirstDiffuse;
    }
    
    if (fDistance < g_BrushRange)
    {
        vMouseDiffuse = vector(1.f, 0.0f, 0.0f, 1.0f);
        //Out.vColor = vMouseDiffuse;
        
        Out.vDiffuse = vMouseDiffuse;
    }
    
    //Out.vDiffuse = vector(vMtrlDiffuse.rgb, 1.f);
    Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    
    // In.vProjPos.z  는 최소 0 ~ 최대 Far 까지 그 값임. 
    // 투영 공간상에서의 z 는 0 ~ 1 이기 때문에, z / w 하자.
    
    // y 가 디퍼드 쉐이더의 fViewZ 를 vWorldPos.xyzw 곱한거를 끌고 온 것. 
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.0f, 0.0f);

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
}