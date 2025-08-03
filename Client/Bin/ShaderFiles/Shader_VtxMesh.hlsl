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
    float4 vWorldPos : TEXCOORD1;           // ���⿡ ������ ��ġ�� ���� ���̴� ��� �ȼм��̴����� �ణ ����ġ�� ����.
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
    // ���� Normal ���� ���Ϸ��� �ϴµ� 0 ~ 1 ���Ϸ���
    Out.vNormal = normalize(mul(float4(In.vNormal, 0.f), g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(float4(In.vPosition, 1.f), g_WorldMatrix);
    Out.vProjPos = Out.vPosition;
    
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.0f), g_WorldMatrix));
    
    // ���̳븻�� y�� �ʳ��̶����ݽ�? �� ���̳븻�� �׷��� z�� �ϰ� x�� �ϰ� �����ϸ� ���� �� ����. 
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
    
    // ���� �� ������ ����, �븻�� 0 ~ 1 �� �Ѵٰ� �ؼ� �� �ӻ�, �� �ؿ�ó�� �ϰ� �־��� ������. 
    // �ٵ� �����̳ʵ��� �븻 �ؽ��Ŀ� �븻 ������ ���� unorm ���� �����Ѵܴ�. �׷��� ���⼭ ���� 
    // g_NormalTexture �� 0 ~ 1 �� �Ǿ�����. 
    // �׷��� �ٽ� �� normal �� -1 ~ 1�� �����ؾ���. 

   
    /*
       Target_Normal 
    if (FAILED(m_pGameInstance - > Add_RenderTarget(TEXT("Target_Normal"), (_uint) ViewportDesc.Width, (_uint) ViewportDesc.Height, DXGI_FORMAT_R16G16B16A16_UNORM, _float4(0.f, 0.f, 0.f, 0.f))))
        return E_FAIL;
    */
    
    // ź��Ʈ �븻�̶��,  0 ~ 1 �� -1 ~ 1 �� ��.
    // ���⼭�� normalt texture �� �� �ȼ�������, ���� ���ؿ�����, xyz ���̾�. 
    // �̰� ���� ���� ��ǥ�� ���ؼ� ó���ؾ���. 
    float3 vNormal = vNormalDesc.xyz * 2.0f -1.0f;
    
    // vTangent �� x�� �ʳ�, biTangent �� BiNormal �� y �� �ʳ�, �� �븻�� z �� �ʳ���.
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    
    // �׷��� �븻 (z��) �̶� �� ���� ���ϸ� ��.
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