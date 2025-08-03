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

    // 각 1 2 3 4 행에 담기. 
    //float4x4 TransformationMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);
    
	/* 로컬스페이스내에서 움직임을 가져간다. */ 
    // TransformationMatrix를 로컬에서 한 번 담자. 로컬 스페이스 내에서 움직임을 가져간다. 
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

// 지오메트리 쉐이더 : 기하 쉐이더 
[maxvertexcount(6)] // :  지오메트리에서 만들고자 하는 정점은 최대 6개이다.
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> DataStream)
{
    GS_OUT              Out[4];
    
    // Look Right Up 벡터를 구해주자. 
    // In[0].vPosition 어차피 POINT 라서 점 하나임 쭉. 사각형의 그냥 center 중심점을 생각하자. 
    float3 vLook = g_vCameraPosition.xyz - In[0].vPosition.xyz;
    float3 vRight = normalize(cross(float3(0.0f, 1.0f, 0.0f), vLook)) * (In[0].fPointSize * 0.5f);
    float3 vUp = normalize(cross(vLook, vRight)) * (In[0].fPointSize * 0.5f);
    
    float4x4 matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    Out[0].vPosition = float4(In[0].vPosition.xyz + vRight + vUp, 1.0f);            // 여기까지 하면 그냥 월드 상의 좌표.
    Out[0].vPosition = mul(Out[0].vPosition, matVP);                                // 투영 뷰스페이스 변환 까지 처리. 해서 다시 position 정해주자.
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

	/* Sample : g_Texture로 부터 지정된 좌표의 색을 지정한 방식으로 얻어온다.*/
	// g_Texture.Sample(어떻게 얻어올건지(Sampler_State), 어디 색을 가져올건지)
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

    // 불꽃 텍스처 샘플링
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
    
    // smoothstep : durtaion 이 3 -> 0 이어서, 맞춰주는 느낌 ( 0 ~ 1 ) 
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
    
    // 불꽃 텍스처 샘플링
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

	///* Sample : g_Texture로 부터 지정된 좌표의 색을 지정한 방식으로 얻어온다.*/
	//// g_Texture.Sample(어떻게 얻어올건지(Sampler_State), 어디 색을 가져올건지)
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
