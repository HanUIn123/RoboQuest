#include "../../../EngineSDK/Hlsl/Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;
Texture2D g_NoiseTexture;
Texture2D g_FireFrameTexture;

float4 g_vCameraPosition;
float g_fDuration;

float g_fAnimationTime;

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

PS_OUT PS_FIRE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    // �Ҳ� �ؽ�ó ���ø�
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexCoord);

    if (0.2f >= Out.vColor.a)
        discard;

    float fCorrectValue = (In.vLifeTime.x - In.vLifeTime.y) * 5.f;
    fCorrectValue = saturate(fCorrectValue);

    Out.vColor = float4(fCorrectValue * 2.f, fCorrectValue * 1.5f, 0.f, 1.f);
    
    if (In.vLifeTime.y >= In.vLifeTime.x)
        discard;

    return Out;
}

PS_OUT PS_FIREGLOWDISSOLVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 Texcoord1 = In.vTexCoord;
    float2 Texcoord2 = In.vTexCoord;
    
    // smoothstep : durtaion �� 3 -> 0 �̾, �����ִ� ���� ( 0 ~ 1 ) 
    float fTimer = smoothstep(0.f, 1.f, g_fDuration * 0.35f);
    
    Texcoord1.x *= fTimer * 0.5f;
    Texcoord2.y *= fTimer * 0.75f;
    
    Texcoord1.x *= 2.0f;
    //Texcoord2.y *= 4.0f;
    
    vector vMaterialFire = g_Texture.Sample(LinearSampler_Clamp, Texcoord1);
    vector vMaterialNoiseFire = g_NoiseTexture.Sample(LinearSampler_Clamp, Texcoord2);
    vector vResultMaterial = vMaterialFire + vMaterialNoiseFire;
    
    float fResultX = vResultMaterial.x;
    if(fTimer < fResultX)
        discard;
    
    // �Ҳ� �ؽ�ó ���ø�
    //Out.vColor = g_Texture.Sample(LinearSampler, In.vTexCoord);

    //if (0.2f >= Out.vColor.a)
    //    discard;

    float fCorrectValue = (In.vLifeTime.x - In.vLifeTime.y) * 5.f;
    fCorrectValue = saturate(fCorrectValue);

    //Out.vColor = float4(fCorrectValue * 4.f, fCorrectValue * 2.f, 0.f, 1.f);

    //Out.vColor = vResultMaterial; 
    //Out.vColor.r -= fCorrectValue * 6.0f;
    //Out.vColor.g -= fCorrectValue * 8.0f;
    Out.vColor = float4(fCorrectValue * 4.f, 0.0f, 0.f, 1.f);
    
    return Out;
}


PS_OUT PS_Fire_Frame(PS_IN In)
{
    //PS_OUT Out = (PS_OUT) 0;
    //
    //Out.vColor = g_FireFrameTexture.Sample(LinearSampler, In.vTexCoord);
    //
    //if (Out.vColor.r < 0.1f && Out.vColor.g < 0.1f && Out.vColor.b < 0.1f)
    //    discard;
    //
    //if (In.vLifeTime.y >= In.vLifeTime.x)
    //    discard;
    //
    //return Out;
    
    
    PS_OUT Out = (PS_OUT) 0;
    
    float2 fFrameSize = float2(0.5f, 0.5f);
    
    int iTotalFrames = 4;
    
    float fAnimationSpeed = 20.0f;
    
    int iFrameIndex = (int) g_fAnimationTime % iTotalFrames;
    
    float2 uvOffset = float2((iFrameIndex % 2) * fFrameSize.x, (iFrameIndex / 2) * fFrameSize.y);
    
    float2 animationUV = In.vTexCoord * fFrameSize + uvOffset;
    
    float4 fireColor = g_FireFrameTexture.Sample(LinearSampler, animationUV);
    
    if(fireColor.a < 0.2f)
        discard;
    
    float fCorrectValue = (In.vLifeTime.x - In.vLifeTime.y) * 4.0f;
    fCorrectValue = saturate(fCorrectValue);
    
    Out.vColor = float4(fireColor.rgb * fCorrectValue, fireColor.a * fCorrectValue);
    
    if (In.vLifeTime.y >= In.vLifeTime.x)
        discard;
    
    return Out;
}

PS_OUT PS_MUZZLEFALSH(PS_IN In)
{
 //   PS_OUT Out = (PS_OUT) 0;

	///* Sample : g_Texture�� ���� ������ ��ǥ�� ���� ������ ������� ���´�.*/
	//// g_Texture.Sample(��� ���ð���(Sampler_State), ��� ���� �����ð���)
 //   Out.vColor = g_Texture.Sample(LinearSampler, In.vTexCoord);

 //   if (0.2f >= Out.vColor.a)
 //       discard;
    
 //   float fCorrectValue = (In.vLifeTime.x - In.vLifeTime.y) * 3.f;
 //   fCorrectValue = saturate(fCorrectValue);

 //   Out.vColor = float4(fCorrectValue * 2.f, fCorrectValue * 1.5f, 0.f, 1.f);

 //   if (In.vLifeTime.y >= In.vLifeTime.x)
 //       discard;
    
    
    
    
    PS_OUT Out = (PS_OUT) 0;
    
    float2 fFrameSize = float2(0.2f, 1.0f);
    //float2 fFrameSize = float2(0.25f, 0.05f);
    
    int iTotalFrames = 5;
    
    //float fAnimationSpeed = 20.0f;
    
    int iFrameIndex = (int) g_fAnimationTime % iTotalFrames;
    
    float2 uvOffset = float2((iFrameIndex % 5) * fFrameSize.x, (iFrameIndex / 1) * fFrameSize.y);
    //float2 uvOffset = float2((iFrameIndex % 4) * fFrameSize.x, (iFrameIndex / 2) * fFrameSize.y);
    
    float2 animationUV = In.vTexCoord * fFrameSize + uvOffset;
    
    Out.vColor = g_Texture.Sample(LinearSampler, animationUV);
    
    if (0.05f >= Out.vColor.a)
        discard;
    
    //float fCorrectValue = (In.vLifeTime.x - In.vLifeTime.y) * 4.0f;
    //fCorrectValue = saturate(fCorrectValue);
    
    //Out.vColor = float4(fCorrectValue * 2.f, fCorrectValue * 1.5f, 0.f, 1.f);
    
    if (In.vLifeTime.y >= In.vLifeTime.x)
        discard;

    return Out;
}


PS_OUT PS_MortalExplosion(PS_IN In)
{
   PS_OUT Out = (PS_OUT) 0;
    
    //float2 fFrameSize = float2(1.0f/ 4 , 1.0f /4);
    //float2 fFrameSize = float2(1.0f/ 7 , 1.0f /1);
    float2 fFrameSize = float2(1.0f/ 4 , 1.0f /2);
    
    //int iTotalFrames = 16;
    //int iTotalFrames = 7;
    int iTotalFrames = 8;
    
    float fAnimationSpeed = 10.0f;
    
    int iFrameIndex = (int) (g_fAnimationTime * fAnimationSpeed) % iTotalFrames;
    
    //float2 uvOffset = float2((iFrameIndex % 4) * fFrameSize.x, (iFrameIndex / 4) * fFrameSize.y);
    //float2 uvOffset = float2((iFrameIndex % 7) * fFrameSize.x, (iFrameIndex / 7) * fFrameSize.y);
    float2 uvOffset = float2((iFrameIndex % 4) * fFrameSize.x, (iFrameIndex / 4) * fFrameSize.y);
    
    float2 animationUV = In.vTexCoord * fFrameSize + uvOffset;
    
    float4 fireColor = g_Texture.Sample(LinearSampler, animationUV);
    
    if (fireColor.a < 0.2f)
        discard;
    
    Out.vColor = fireColor;

    if (In.vLifeTime.y >= In.vLifeTime.x)
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

        GeometryShader = compile gs_5_0 GS_MAIN();

        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass FlameGlowPass
    {
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(1.f, 0.5f, 0.2f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_FIRE(); 
    }

    pass FlameGlowDissolvePass
    {
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(1.f, 0.5f, 0.2f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_FIREGLOWDISSOLVE();
    }

    pass FIRE_FRAMEPass
    {
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(1.f, 0.5f, 0.2f, 1.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        PixelShader = compile ps_5_0 PS_Fire_Frame();
    }

    pass MuzzlePass
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();

        GeometryShader = compile gs_5_0 GS_MAIN();

        PixelShader = compile ps_5_0 PS_MUZZLEFALSH();
    }

    pass MortalExplosion
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();

        GeometryShader = compile gs_5_0 GS_MAIN();

        PixelShader = compile ps_5_0 PS_MortalExplosion();
    }
}
