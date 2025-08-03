#include "../../../EngineSDK/Hlsl/Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;
Texture2D g_NoiseTexture;
Texture2D g_FireFrameTexture;

float4 g_vCameraPosition;
float g_fDuration;
float g_fAnimationTime;

float g_RimPower = 1.0f;
float4 g_RimColor = { 1.0f, 0.0f, 0.0f, 1.0f };


struct VS_IN
{
    float3 vPosition : POSITION;
    float fPointSize : PSIZE;

    row_major float4x4 Transformatrix : WORLD;
    float2 vLifeTime : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : POSITION;
    float fPointSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    // �� 1 2 3 4 �࿡ ���. 
    //float4x4 TransformationMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
	/* ���ý����̽������� �������� ��������. */ 
    // TransformationMatrix�� ���ÿ��� �� �� ����. ���� �����̽� ������ �������� ��������. 
    vector vPosition = mul(vector(In.vPosition, 1.f), In.Transformatrix);

    Out.vPosition = mul(vPosition, g_WorldMatrix);
    Out.fPointSize = length(In.Transformatrix._11_12_13);
    Out.vLifeTime = In.vLifeTime;
    
    return Out;
}

struct GS_IN
{
    float4 vPosition : POSITION;
    float fPointSize : PSIZE;
    float2 vLifeTime : TEXCOORD0;
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

// ������Ʈ�� ���̴� : ���� ���̴� 
[maxvertexcount(6)] // :  ������Ʈ������ ������� �ϴ� ������ �ִ� 6���̴�.
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT              Out[4];
    
    // Look Right Up ���͸� ��������. 
    // In[0].vPosition ������ POINT �� �� �ϳ��� ��. �簢���� �׳� center �߽����� ��������. 
    float3 vLook = g_vCameraPosition.xyz - In[0].vPosition.xyz;
    float3 vRight = normalize(cross(float3(0.0f, 1.0f, 0.0f), vLook)) * (In[0].fPointSize * 0.5f);
    float3 vUp = normalize(cross(vLook, vRight)) * (In[0].fPointSize * 0.5f);
    
    float4x4 matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    Out[0].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.0f);            // ������� �ϸ� �׳� ���� ���� ��ǥ.
    Out[0].vPosition = mul(Out[0].vPosition, matVP);                                // ���� �佺���̽� ��ȯ ���� ó��. �ؼ� �ٽ� position ��������.
    Out[0].vTexCoord = float2(0.0f, 0.0f);
    Out[0].vLifeTime = In[0].vLifeTime;
    
    Out[1].vPosition = float4(In[0].vPosition.xyz - vRight + vUp, 1.0f);
    Out[1].vPosition = mul(Out[1].vPosition, matVP);
    Out[1].vTexCoord = float2(1.0f, 0.0f);
    Out[1].vLifeTime = In[0].vLifeTime;
    
    Out[2].vPosition = float4(In[0].vPosition.xyz - vRight - vUp, 1.0f);
    Out[2].vPosition = mul(Out[2].vPosition, matVP);
    Out[2].vTexCoord = float2(1.0f, 1.0f);
    Out[2].vLifeTime = In[0].vLifeTime;
    
    Out[3].vPosition = float4(In[0].vPosition.xyz + vRight - vUp, 1.0f);
    Out[3].vPosition = mul(Out[3].vPosition, matVP);
    Out[3].vTexCoord = float2(0.0f, 1.0f);
    Out[3].vLifeTime = In[0].vLifeTime;
    
    DataStream.Append(Out[0]);
    DataStream.Append(Out[1]);
    DataStream.Append(Out[2]);
    DataStream.RestartStrip();
    
    DataStream.Append(Out[0]);
    DataStream.Append(Out[2]);
    DataStream.Append(Out[3]);
    DataStream.RestartStrip();
}



struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexCoord : TEXCOORD0;
    float2 vLifeTime : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

	/* Sample : g_Texture�� ���� ������ ��ǥ�� ���� ������ ������� ���´�.*/
	// g_Texture.Sample(��� ���ð���(Sampler_State), ��� ���� �����ð���)
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexCoord);

    if (0.2f >= Out.vColor.a)
        discard;
    
    float fCorrectValue = (In.vLifeTime.x - In.vLifeTime.y) * 3.f;
    fCorrectValue = saturate(fCorrectValue);

    Out.vColor = float4(fCorrectValue * 2.f, fCorrectValue * 1.5f, 0.f, 1.f);

    if(In.vLifeTime.y >= In.vLifeTime.x)
        discard;

    return Out;
}

PS_OUT PS_SMOKE(PS_IN In)
{   
    PS_OUT Out = (PS_OUT) 0;

    // 5x5 �ؽ�ó ��Ʋ�� ����
    float2 frameSize = float2(0.2, 0.2);
    int totalFrames = 25;
    float animationSpeed = 6.0; // �ʴ� ������ ���� �ӵ� ����

    // ���� ������ �ε��� ���
    int frameIndex = (int) (g_fAnimationTime * animationSpeed) % totalFrames;

    // UV ������ ���
    float2 uvOffset = float2((frameIndex % 5) * frameSize.x, (frameIndex / 5) * frameSize.y);
    float2 animatedUV = In.vTexCoord * frameSize + uvOffset;

    // �ؽ�ó ���ø�
    float4 smokeColor = g_Texture.Sample(LinearSampler, animatedUV);

    if (smokeColor.a < 0.15f)
        discard; // ���� �ּҰ� ���缭 ������ ����

    //  ������ ���� ���� (���� �� ��Ȳ �� ��� �� ���)
    float3 explosionColor = lerp(float3(1.0f, 0.0f, 0.0f), //  ������ ������
                                 float3(0.5f, 0.0f, 0.0f), //  ��ο� ������
                                 smokeColor.r);

    explosionColor = lerp(explosionColor, float3(0.1f, 0.0f, 0.0f), smokeColor.g); //  ����� ����

    // �ε巯�� ���̵� ���� (������ ���������)
    float fCorrectValue = (In.vLifeTime.x - In.vLifeTime.y) * 3.0f;
    fCorrectValue = saturate(fCorrectValue);
    float fadeFactor = smoothstep(0.2, 1.0, fCorrectValue);

    Out.vColor = float4(explosionColor * fadeFactor, smokeColor.a * fadeFactor);

    if (In.vLifeTime.y >= In.vLifeTime.x)
        discard;

    return Out;
}

PS_OUT PS_RIMLIGHT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    if (Out.vColor.r < 0.1f && Out.vColor.g < 0.1f && Out.vColor.b < 0.1f)
        discard;
    
    float3 vDirection = normalize(g_vCameraPosition.xyz - In.vPosition.xyz);
    
    float fRim = saturate(1.0f - vDirection);
    fRim = pow(fRim, g_RimPower);
    
    Out.vColor.rgb = fRim * g_RimColor.rgb;
    Out.vColor.a = 1.0f;

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

        GeometryShader = compile gs_5_0 GS_MAIN();

        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass SmokePass
    {
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(1.f, 0.5f, 0.2f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_SMOKE(); 
    }

    pass GlowPass
    {
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(1.0f, 0.3f, 0.1f, 1.0f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_RIMLIGHT();
    }
}
