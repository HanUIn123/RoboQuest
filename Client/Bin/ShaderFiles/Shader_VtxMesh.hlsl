#include "../../../EngineSDK/Hlsl/Engine_Shader_Defines.hlsli"
float4x4        g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D       g_DiffuseTexture;
Texture2D       g_NormalTexture;
          
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
    float4 vWorldPos : TEXCOORD1;           // 여기에 월드의 위치를 담을 것이다 라고 픽셸세이더에게 약간 구라치는 느낌.
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

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.1f)
        discard;
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    
    // 기존 빛 연산할 때는, 노말은 0 ~ 1 로 한다고 해서 그 속상값, 을 밑에처럼 하고 있었단 말이지. 
    // 근데 디자이너들이 노말 텍스쳐에 노말 저장할 때는 unorm 으로 보통한단다. 그래서 여기서 쓰는 
    // g_NormalTexture 가 0 ~ 1 로 되어있음. 
    // 그래서 다시 그 normal 을 -1 ~ 1로 변경해야함. 

   
    /*
       Target_Normal 
    if (FAILED(m_pGameInstance - > Add_RenderTarget(TEXT("Target_Normal"), (_uint) ViewportDesc.Width, (_uint) ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    */
    
    // 탄젠트 노말이라고,  0 ~ 1 을 -1 ~ 1 로 함.
    // 여기서의 normalt texture 는 각 픽셀마다의, 로컬 기준에서의, xyz 축이야. 
    // 이걸 모델의 월드 좌표랑 곱해서 처리해야함. 
    float3 vNormal = vNormalDesc.xyz * 2.0f -1.0f;
    
    // vTangent 가 x축 너낌, biTangent 즉 BiNormal 이 y 축 너낌, 그 노말이 z 축 너낌임.
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    
    // 그래서 노말 (z축) 이랑 그 모델의 곱하면 됨.
    vNormal = normalize(mul(vNormal, WorldMatrix));
    

    Out.vDiffuse = vMtrlDiffuse;
    //Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);
    
    Out.vPickDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 1.f, 0.f);

    return Out;
}

PS_OUT PS_WALL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.r < 0.1f && vMtrlDiffuse.g < 0.1f && vMtrlDiffuse.b < 0.1f)
        discard;
    
    //if (vMtrlDiffuse.a < 0.1f)
    //    discard;
    
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


    pass Wall
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_WALL();
    }
}