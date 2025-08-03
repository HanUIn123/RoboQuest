#include "../../../EngineSDK/Hlsl/Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
Texture2D g_Texture;

Texture2D g_DepthTexture;

float g_FrameIndex;

float g_Opacity = 1.0f;

float g_GlobalTime;

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
    float4 vWorldPos : TEXCOORD1; // ���⿡ ������ ��ġ�� ���� ���̴� ��� �ȼм��̴����� �ణ ����ġ�� ����.
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    Out.vPosition = mul(float4(In.vPosition, 1.0f), g_WorldMatrix);
    // ���� �ȼ� ���̴�����, ���� ������ �˰� �ʹ�. �ϸ� ���� ���� ���̴� �ʿ��� �߰��� �̾ƹ���.
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
    
    // Out.vColor = 1.0f;
    
    /* Sample : g_Texture�� ���� ������ ��ǥ�� ���� ������ ������� ���´�.*/
	// g_Texture.Sample(��� ���ð���(Sampler_State), ��� ���� �����ð���)
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
        
    //Out.vColor.a *= g_Opacity;
    //if (In.vTexcoord.x < 0.5f)
    //    Out.vColor.rgb = 1.f;

    if (0.2f >= Out.vColor.a)
        discard;
    
    
    return Out;
}

//==============================================================================================
PS_OUT PS_BULLET_COUNT(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    // Out.vColor = 1.0f;
    
    /* Sample : g_Texture�� ���� ������ ��ǥ�� ���� ������ ������� ���´�.*/
	// g_Texture.Sample(��� ���ð���(Sampler_State), ��� ���� �����ð���)
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.r <= 0.5f || Out.vColor.g <= 0.5f || Out.vColor.b <= 0.5f)
        discard;
    
    return Out;
}
//==============================================================================================

struct VS_OUT_EFFECT
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

VS_OUT_EFFECT VS_MAIN_EFFECT(VS_IN In)
{
    VS_OUT_EFFECT Out = (VS_OUT_EFFECT) 0;
    
    Out.vPosition = mul(float4(In.vPosition, 1.0f), g_WorldMatrix);
    Out.vWorldPos = Out.vPosition;
    
    Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
    Out.vPosition = mul(Out.vPosition, g_ProjMatrix);
    Out.vTexcoord = In.vTexcoord;
    Out.vProjPos = Out.vPosition;
    
    return Out;
}

struct PS_IN_EFFECT
{
    float4 vPosition : SV_Position;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
};

PS_OUT PS_MAIN_EFFECT(PS_IN_EFFECT In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.r < 0.1f || Out.vColor.g < 0.1f || Out.vColor.b < 0.1f)
        discard;
    
    //// **���� ������ �Ͼ�� ���� ����**
    //if (0.2f >= Out.vColor.a)
    //    discard;
    
    float2 vTexcoord;
    
    // ( -1 , 1 ) ~ ( 1, -1 ) �� ���� ������ x y �� ���Ӱ� ������. ( 0, 0 ) ~ ( 1 , 1 )
    // w������ ���� �����ؾ� ����� �� ��.
    vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;
    
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, vTexcoord);
    
    // In.vProjPos.w �� �佺���̽� ���� z�� �ǹ���. 
    //Out.vColor.a = Out.vColor.a * (vDepthDesc.y - In.vProjPos.w);
    Out.vColor.a = Out.vColor.a * saturate(vDepthDesc.y - In.vProjPos.w);
    
    return Out;
    
    
    
    
    
    
    //PS_OUT Out = (PS_OUT) 0;

    //// 2x2 �ؽ�ó ��Ʋ�� ����
    //float2 frameSize = float2(0.5, 0.5); // �� ������ ũ��
    //int totalFrames = 4; // �� 4�� ������
    //float animationSpeed = 20.0; // �ʴ� ������ ���� �ӵ�

    //// ���� ������ �ε��� ��� (�ִϸ��̼� �ð� ���)
    //int frameIndex = (int) (g_fAnimationTime) % totalFrames;

    //// 2x2 �ؽ�ó���� UV ������ ���
    //float2 uvOffset = float2((frameIndex % 2) * frameSize.x, (frameIndex / 2) * frameSize.y);
    //float2 animatedUV = In.vTexCoord * frameSize + uvOffset;

    //// �ִϸ��̼ǵ� UV ��ǥ�� �ؽ�ó ���ø�
    //float4 fireColor = g_Texture.Sample(LinearSampler, animatedUV);

    ////  �ұ�� ���� (Laser Boss)
    ////float3 LarvaColor = lerp(float3(1.0f, 0.3f, 0.0f), float3(1.0f, 0.8f, 0.0f), fireColor.r);

    //if (fireColor.a < 0.2f)
    //    discard;

    //float fCorrectValue = (In.vLifeTime.x - In.vLifeTime.y) * 4.0f;
    //fCorrectValue = saturate(fCorrectValue);

    ////Out.vColor = float4(/*LarvaColor * */fCorrectValue, fCorrectValue * fireColor.a);
    ////Out.vColor = float4(fireColor.r *  fCorrectValue, fCorrectValue * fireColor.a);
    //Out.vColor = float4(fireColor.rgb * fCorrectValue, fireColor.a * fCorrectValue);

    //// **���� �ð��� ���� ��ƼŬ ����**
    //if (In.vLifeTime.y >= In.vLifeTime.x)
    //    discard;

    //return Out;
    
    
}

PS_OUT PS_MAIN_ATLAS(PS_IN_EFFECT In)
{
    //PS_OUT Out = (PS_OUT) 0;
    
    //int frameColumns = 7; // ���� ������ ����
    //int frameRows = 1; // ���� ������ ����
    //int totalFrames = frameColumns * frameRows;

    //// ���� �������� ���ε��� ������ ������
    //int currentFrame = g_FrameIndex;

    //int column = currentFrame % frameColumns;
    //int row = currentFrame / frameColumns;

    //float2 frameSize = float2(1.0 / frameColumns, 1.0 / frameRows);
    //float2 uvOffset = float2(column * frameSize.x, row * frameSize.y);

    //// ���� UV ��ǥ�� ������ ����
    //float2 newUV = In.vTexcoord * frameSize + uvOffset;
    
    //Out.vColor = g_Texture.Sample(LinearSampler, newUV);

    //if (Out.vColor.r < 0.1f && Out.vColor.g < 0.1f && Out.vColor.b < 0.1f)
    //    discard;

    //return Out;
    
    
    //PS_OUT Out = (PS_OUT) 0;
    
    //float2 fFrameSize = float2(1.0f/ 7, 1.0f);
    
    //int iTotalFrames = 7;
    
    ////float fAnimationSpeed = 20.0f;
    
    //int iFrameIndex = (int) g_fAnimationTime % iTotalFrames;
    
    ////float2 uvOffset = float2((iFrameIndex % 2) * fFrameSize.x, (iFrameIndex / 2) * fFrameSize.y);
    //float2 uvOffset = float2(iFrameIndex * fFrameSize.x, 0);
    
    //float2 animationUV = In.vTexcoord * fFrameSize + uvOffset;
    
    ////float4 fireColor = g_Texture.Sample(LinearSampler, animationUV);
    //Out.vColor = g_Texture.Sample(LinearSampler, animationUV);
    
    //if (0.05f >= Out.vColor.a)
    //    discard;
    
    //if (fireColor.a < 0.2f)
    //    discard;
    
    //float fCorrectValue = (In.vLifeTime.x - In.vLifeTime.y) * 4.0f;
    //fCorrectValue = saturate(fCorrectValue);
    //
    //Out.vColor = float4(fireColor.rgb * fCorrectValue, fireColor.a * fCorrectValue);
    
    //if (In.vLifeTime.y >= In.vLifeTime.x)
    //    discard;
    
    //Out.vColor = fireColor;
    
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    float2 vTexcoord;
    
    // ( -1 , 1 ) ~ ( 1, -1 ) �� ���� ������ x y �� ���Ӱ� ������. ( 0, 0 ) ~ ( 1 , 1 )
    // w������ ���� �����ؾ� ����� �� ��.
    vTexcoord.x = (In.vProjPos.x / In.vProjPos.w) * 0.5f + 0.5f;
    vTexcoord.y = (In.vProjPos.y / In.vProjPos.w) * -0.5f + 0.5f;
    
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, vTexcoord);
    
    // In.vProjPos.w �� �佺���̽� ���� z�� �ǹ���. 
    //Out.vColor.a = Out.vColor.a * (vDepthDesc.y - In.vProjPos.w);
    Out.vColor.a = Out.vColor.a * saturate(vDepthDesc.y - In.vProjPos.w);
    
    return Out;
}

PS_OUT PS_GOLIATH_EXPLOSION(PS_IN_EFFECT In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float2 fFrameSize = float2(1.0f / 7.0f, 1.0f); // ���� 7�� (1/7), ���� 1�� (1/1)
    
    int iTotalFrames = 7; // �� 7�� ������
    int iFrameIndex = (int) g_fAnimationTime % iTotalFrames;
    
    // ���� 7ĭ�̹Ƿ�, UV �̵��� X�� ���⸸ ����
    float2 uvOffset = float2(iFrameIndex * fFrameSize.x, 0.0f);
    
    float2 animationUV = In.vTexcoord * fFrameSize + uvOffset;
    
    float4 fireColor = g_Texture.Sample(LinearSampler, animationUV);
    
    if (fireColor.a < 0.2f)
        discard;
    
    Out.vColor = fireColor;
    
    return Out;
}


//==============================================================================================
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

    pass PS_MAIN_EFFECT
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_EFFECT();
    }

    pass PS_MAIN_ATLAS
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_ATLAS();
    }

    pass EXPLOSIONPASS
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_EFFECT();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_GOLIATH_EXPLOSION();
    }
}