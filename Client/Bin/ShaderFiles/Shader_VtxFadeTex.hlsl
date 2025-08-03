#include "../../../EngineSDK/Hlsl/Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;
Texture2D g_GaugeTexture;

Texture2D g_DepthTexture;

float g_FadeAlpha; // 0.0f (���� ����) ~ 1.0f (���� ������)
float g_Time;


float g_CurrentBarSize; 
float g_MaxBarSize; 

float g_LerpTime;


Texture2D g_DissolveTexture;

float g_DissolveValue;

float g_fAnimationTime;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1; 
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
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
    
    float vDepth : SV_DEPTH; // ���̰� �߰�
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    Out.vColor.a *= g_FadeAlpha;

    if (Out.vColor.a <= 0.01f)
        discard;
    
    return Out;
}

//==============================================================================================

PS_OUT PS_DASH_EFFECT(PS_IN In)
{
    /*
    �� ������ ���� ��鸲 �� g_Time * 10.0f
    �� ���� ������ ��鸲 �� intensity = 0.12f
    ������ �ٲ�� �ڿ������� ��鸲 �� sin * cos �߰�
    */
    
    
    /*
    speed�� �ð��� ���� ���ϴ� ���̹Ƿ�, cos(speed * 2.0f)�� �ð��� ������ ���� -1 ~ 1 ���̿��� �ݺ���.
    speed * 2.0f �̹Ƿ� �Ϲ� cos()���� 2�� ������ ������.
    ��������� Y���� 2�� ������ ��鸮�� ����
    */
    
    /*
     cos(speed * 2.0f)�� ���Ʒ��� ������ �����ϰ�,
    sin(speed * 1.5f)�� �� �� �ε巴�� ������.
    �� ���� ���ϸ� UV�� �� �ڿ������� �ұ�Ģ�ϰ� ��鸮�� ��.
    */
    
    PS_OUT Out = (PS_OUT) 0;
    
    float intensity = 0.02f; // ��鸲 ����
    float speed = g_Time * 4.0f; // �ð� ��ȭ �ӵ�

    float2 animatedUV = In.vTexcoord;
    
    animatedUV.x += sin(speed * 3.0f) * intensity * cos(speed * 2.0f);
    animatedUV.y += cos(speed * 2.0f) * intensity * sin(speed * 1.5f);

    Out.vColor = g_Texture.Sample(LinearSampler_Clamp, animatedUV);
    
    Out.vColor.a *= g_FadeAlpha;

    if (Out.vColor.a <= 0.01f)
        discard;
    
    return Out;
}
//==============================================================================================

PS_OUT PS_HURT_EFFECT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float intensity = 0.05f; // ��鸲 ������ ���� ����
    float speed = g_Time * 5.0f; // �ӵ� ����

    float2 animatedUV = In.vTexcoord;
    
    animatedUV.x += sin(speed * 3.0f) * intensity * cos(speed * 2.0f);
    animatedUV.y += cos(speed * 2.0f) * intensity * sin(speed * 1.5f);

    float4 sampledColor = g_Texture.Sample(LinearSampler_Clamp, animatedUV);
    
    float3 redTint = float3(2.0f, 0.05f + sin(g_Time * 3.0f) * 0.1f, 0.05f);
    float3 finalColor = lerp(sampledColor.rgb, redTint, 1.2f);

    finalColor = pow(finalColor, 0.9f);
    
    finalColor *= 2.8f; 

    Out.vColor = float4(finalColor, sampledColor.a * g_FadeAlpha * 1.5f);

    if (Out.vColor.a <= 0.01f)
        discard;
    
    return Out;
}

//==============================================================================================

PS_OUT PS_SCOPE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (Out.vColor.a <= 0.01f)
        discard;
    
    return Out;
}

PS_OUT PS_SCOPE_BAR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    float fBarProgressRatio = g_CurrentBarSize / g_MaxBarSize;

    if (In.vTexcoord.x < fBarProgressRatio)
    {
        Out.vColor += float4(1.0f, 0.0f, 0.0f, 1.0f); 
    }

    return Out;
}


PS_OUT PS_UI_BOSS(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 sampledColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    float dissolveFactor = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord).r;
    
    if (dissolveFactor < g_DissolveValue)
        discard;

    float depthOffset = 0.01f;
    Out.vDepth = (In.vPosition.z / In.vPosition.w) + depthOffset;

    Out.vColor = float4(sampledColor.rgb, sampledColor.a * (1.0f - g_DissolveValue));

    if (Out.vColor.a <= 0.01f)
        discard;

    return Out;
}

//==============================================================================================

PS_OUT PS_BURN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 fFrameSize = float2(1.0f / 8, 1.0f / 2); 
    
    int iTotalFrames = 16;
    
    int iFrameIndex = (int) g_fAnimationTime % iTotalFrames; 
    
    float2 uvOffset = float2((iFrameIndex % 8) * fFrameSize.x, (iFrameIndex / 8) * fFrameSize.y);
    
    float2 animationUV = In.vTexcoord * fFrameSize + uvOffset;
    
    float4 fireColor = g_Texture.Sample(LinearSampler, animationUV);

    if (fireColor.a < 0.2f)
        discard;
    
    Out.vColor = fireColor * g_FadeAlpha;

    return Out;
}

//==============================================================================================

PS_OUT PS_DYING_EXPLODE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 fFrameSize = float2(1.0f / 7, 1.0f / 1);
    
    int iTotalFrames = 7;
    
    int iFrameIndex = (int) g_fAnimationTime % iTotalFrames;
    
    float2 uvOffset = float2((iFrameIndex % 7) * fFrameSize.x, (iFrameIndex / 7) * fFrameSize.y);
    
    float2 animationUV = In.vTexcoord * fFrameSize + uvOffset;
    
    float4 fireColor = g_Texture.Sample(LinearSampler, animationUV);

    if (fireColor.a < 0.2f)
        discard;
    
    Out.vColor = fireColor;

    return Out;
}

PS_OUT PS_BREAK_EFFECT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float intensity = 0.05f; // ��鸲 ������ ���� ����
    float speed = g_Time * 5.0f; // �ӵ� ����

    float2 animatedUV = In.vTexcoord;
    
    animatedUV.x += sin(speed * 3.0f) * intensity * cos(speed * 2.0f);
    animatedUV.y += cos(speed * 2.0f) * intensity * sin(speed * 1.5f);

    float4 sampledColor = g_Texture.Sample(LinearSampler_Clamp, animatedUV);
    
    //float3 redTint = float3(2.0f, 0.05f + sin(g_Time * 3.0f) * 0.1f, 0.05f);
    float3 finalColor = lerp(sampledColor.rgb, 0.9f, 1.2f);

    //finalColor = pow(finalColor, 0.9f);
    //
    //finalColor *= 2.8f;

    Out.vColor = float4(finalColor, sampledColor.a * g_FadeAlpha * 1.5f);

    if (Out.vColor.a <= 0.01f)
        discard;
    
    return Out;
}


PS_OUT PS_HEAL_EFFECT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float intensity = 0.05f; // ��鸲 ����
    float speed = g_Time * 5.0f; // �ӵ� ����

    float2 animatedUV = In.vTexcoord;
    
    animatedUV.x += sin(speed * 3.0f) * intensity * cos(speed * 2.0f);
    animatedUV.y += cos(speed * 2.0f) * intensity * sin(speed * 1.5f);

    float4 sampledColor = g_Texture.Sample(LinearSampler_Clamp, animatedUV);
    
    float3 greenTint = float3(0.1f, 2.5f, 0.1f); // R = ����, G = ���ϰ�, B = ����
    float3 finalColor = lerp(sampledColor.rgb, greenTint, 0.8f); // �ʷϻ����� ����

    finalColor = pow(finalColor, 1.0f);
    finalColor *= 2.8f;

    // ���� ���� ����
    Out.vColor = float4(finalColor, sampledColor.a * g_FadeAlpha * 1.5f);

    if (Out.vColor.a <= 0.01f)
        discard;
    
    return Out;

}

//==============================================================================================

technique11 DefaultTechnique
{
    pass DefaultPass
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass DashDeffect
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DASH_EFFECT();
    }

    pass Hurteffect
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HURT_EFFECT();
    }

    pass Scope
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SCOPE();
    }

    pass ScopeBar
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_SCOPE_BAR();
    }

    pass UIBOSS
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_UI, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_UI_BOSS();
    }

    pass Burn
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BURN();
    }

    pass Dying_Explode
    {
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_DYING_EXPLODE();
    }


    pass BreakEffect
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_BREAK_EFFECT();
    }

    pass HealEffect
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_HEAL_EFFECT();
    }

}