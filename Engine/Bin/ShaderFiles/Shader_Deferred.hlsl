#include "Engine_Shader_Defines.hlsli"

float4x4		g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
float4x4		g_ViewMatrixInverse, g_ProjMatrixInverse;
Texture2D		g_Texture;

float4x4		g_LightViewMatrix, g_LightProjMatrix;


float4			g_vLightDir;
float4			g_vLightPos;
float			g_fRange;


float4			g_vLightDiffuse;
float4			g_vLightAmbient;
float4			g_vLightSpecular;


Texture2D		g_DiffuseTexture;
float4			g_vMtrlAmbient = vector(1.0f, 1.0f, 1.0f, 1.0f);
float4			g_vMtrlSpecular = vector(1.0f, 1.0f, 1.0f, 1.0f);



Texture2D		g_SpecularTexture;
Texture2D		g_NormalTexture;
Texture2D		g_ShadeTexture;
Texture2D		g_DepthTexture;
Texture2D		g_ShadowTexture;
Texture2D		g_FinalTexture;
Texture2D		g_BlurXTexture;
Texture2D		g_BlurYTexture;

Texture2D       g_GlowTexture;
Texture2D       g_GlowXTexture;
Texture2D       g_GlowYTexture;

float4			g_vCameraPosition;


// �� ���̴��� �簢�� ���۸� �޾ƿͼ� ���� ���̱� ������, VS_IN �� position �� texcoord ���� �ʿ�� �ϸ� �ǰڴ�.
struct VS_IN
{
	float3			vPosition : POSITION;
	float2			vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
	float4			vPosition : SV_POSITION;
	float2			vTexcoord : TEXCOORD0;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT			Out = (VS_OUT)0;

	matrix			matWV, matWVP;

	// �� ��ȯ
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	// ���� ��ȯ
	matWVP = mul(matWV, g_ProjMatrix);

	// ���� Position ��, ��&���� ��� ���ؼ� ����.
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;
}


struct PS_IN
{
	float4			vPosition : SV_POSITION;
	float2			vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	float4			vColor : SV_TARGET0;
};

// ����� ��� �� ���� �����ؼ� �����ְڴٴ� �ǹ̷�, PS_MAIN_DEBUG ��� �������. 
// ���� ��ũ���� �κп�����, �̸��� ���缭 ��������.
PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	return Out;
}

// PS_OUT �� �ΰ���, �����Ϸ��� �̸��� PS_OUT_LIGHT �� ���ڰ�.
// ��� ���� ���� ������ ������ ó���ϹǷ�, �׳� �ȼ� ���̴� ��ü�� ��Ʈ�� �ص��� �� 
struct PS_OUT_LIGHT
{
	float4			vShade : SV_TARGET0;
    float4			vSpecular : SV_TARGET1;
};

// ���⼺ ���� ���� ó��
PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
	
	//   Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f); // ���� NorTex ���̴�����...
	// ������� �� Normal �� ���� ���̴����� ���� Normal �� 0 ~ 1 �� �Ǿ��,
	// �ٽ� ��ġ�� ���ִ°��� -1 ~ 1 �� 
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	// Depth ���� 
    vector vDepthDesc	=		g_DepthTexture.Sample(LinearSampler, In.vTexcoord);
	// Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.0f, 0.0f); ������ In.vProjPos.w �̴�.
    float fViewZ		=		vDepthDesc.y;
	
    vector vShade = saturate(max(dot(normalize(g_vLightDir * -1.f), normalize(vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
	
    Out.vShade = g_vLightDiffuse * vShade;
	
    vector vWorldPos;
	
	// �⺻ ����Ʈ ex 800 600 �̶�� �ϸ� �װ� ���� �������� ( -1, -1 ) ~ ( 1, 1 ) 
	// �� ������, ���� ���� ǥ���ϳ�, Texcoord �̿��ؼ�, �ٵ� Texcoord �� ( 0 , 0 ) ~ ( 1 , 1 ) �̴ϱ�, 
	// ����? �����ִ� ����.
	
	/* ������������ ��ġ��, ������ġ * ���� ���� * ����� * ������� * 1/w */ 
    vWorldPos.x = In.vTexcoord.x * 2.0f - 1.0f;
    vWorldPos.y = In.vTexcoord.y * -2.0f + 1.0f;
	
	// Depth ������ x�� �������ԵǸ�, ��ü�� ������������ ���� ��, z / w �� ���� x �� �������. 
	//  Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.0f, 0.0f);
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.0f;
	
	/* ���� ��ġ * ���� ��� * �� ���  * ������� */
    vWorldPos = vWorldPos.xyzw * fViewZ;
	
	/* ���� ��ġ * ���� ��� * �� ��� */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);
	
	/* ������ġ * ���� ��� ��, ������� */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);

    vector vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));
    vector vLook = vWorldPos - g_vCameraPosition;

    //Out.vSpecular = max(dot(normalize(vReflect * -1.f), normalize(vLook)), 0.f);
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vReflect * -1.0f), normalize(vLook)), 0.0f), 50.0f);
	
	
	return Out;
}

// ���⼺ ���� ���� ó��
PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	// Depth ���� 
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y;
	
    vector vWorldPos;

	/* ������������ ��ġ */
	/* ������ġ * ������� * ����� * ������� * 1/w */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* ������ġ * ������� * ����� * �������  */
    vWorldPos = vWorldPos.xyzw * fViewZ;

	/* ������ġ * ������� * ����� */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);

	/* ������ġ */
	/* ������ġ * �������  */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);
	
    vector vLightDir = vWorldPos - g_vLightPos;
    float fLength = length(vLightDir);
	// fAtt = �����. �� ������ ����� �ȼ�����, 1�� �����Բ� frange �� ������. ������ ������ ��쿡�� saturate �� �߶����. 
    float fAtt = saturate((g_fRange - fLength) / g_fRange);
	
    vector vShade = saturate(max(dot(normalize(vLightDir * -1.f), normalize(vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
    Out.vShade = g_vLightDiffuse * vShade * fAtt;
	
	
    //vector vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));
    vector vReflect = reflect(normalize(vLightDir), normalize(vNormal));
    vector vLook = vWorldPos - g_vCameraPosition;

    //Out.vSpecular = max(dot(normalize(vReflect * -1.f), normalize(vLook)), 0.f);
    //Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vReflect * -1.0f), normalize(vLook)), 0.0f), 50.0f);
	
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vReflect * -1.f), normalize(vLook)), 0.f), 50.f) * fAtt;
	
    return Out;
}

PS_OUT PS_MAIN_DEFERRED(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (0.f == vDiffuse.a)
        discard;

    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);


    Out.vColor = vDiffuse * vShade + vSpecular;
    //Out.vColor = vDiffuse * vShade;
	
	// �ؿ������� �ϴ°� �׸��� ���۵� ���̵��� ���� �ڵ��� .
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y;

    vector vWorldPos;

	/* ������������ ��ġ */
	/* ������ġ * ������� * ����� * ������� * 1/w */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* ������ġ * ������� * ����� * �������  */
    vWorldPos = vWorldPos.xyzw * fViewZ;

	/* ������ġ * ������� * ����� */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);

	/* ������ġ */
	/* ������ġ * �������  */
	// �ȼ��� ���� ��ġ ���� �����Ծ�.
    vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);

	// �� �ȼ��� ���� ��ġ�� ������ ����� ������. 
    vWorldPos = mul(vWorldPos, g_LightViewMatrix);
	
	// �ȼ��� ���� * ������ ����� * ������ �������
	// ���ϱ⸸ ���� �� ���� ������ �Ϸ��� ���� ������ ���� �ؾ��� 
    vWorldPos = mul(vWorldPos, g_LightProjMatrix);

    float2 vTexcoord;

	// �ȼ��� ���� ��ǥ�� xyz �� ���̰� w �� vWorldPos.w �� ���̰� �Ǵ°Ŷ� �� ���̷� ���� ���� ������ ���� ���� �װ��� .
	// �ٵ� �ؽ� ��� ��ǥ�� ��ȯ�ؼ� �����ٶ�� �ϴ°Ŵϱ��, �ؽ���尡 �»�� (0,0) ���� ���ϴ� (1,1) �̴ϱ�� �� ������ ���ٷ��� ���� ���ó����. 
    vTexcoord.x = (vWorldPos.x / vWorldPos.w) * 0.5f + 0.5f;
    vTexcoord.y = (vWorldPos.y / vWorldPos.w) * -0.5f + 0.5f;

	// ���� ShadowTexture �� ���� ���� ��������. 
    vector vShadowDepthDesc = g_ShadowTexture.Sample(LinearSampler, vTexcoord);

	// �׷��� ������ ���̶�, �ȼ��� ���� ���� ���̶� �񱳸� �ؼ� �ȼ��� ���̰� �� ũ�� ���, 
    if (vShadowDepthDesc.y <= vWorldPos.w)
        Out.vColor = Out.vColor * 0.4f;

	
    return Out;
}

// ����ġ Blur �뵵. ��� 1�� �������� �º� ��ġ�� <-> �캯 ��ġ�� ���ϴ� ���� 
float g_fWeights[13] =
{
    0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1.f, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};

float g_fWeights_Glow[13] =
{
    0.0561, 0.1353, 0.278, 0.4868, 0.7261, 0.9231, 1.f, 0.9231, 0.7261, 0.4868, 0.278, 0.1353, 0.0561
};


PS_OUT PS_MAIN_BLUR_X(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vTexcoord = 0.f;

    for (int i = -6; i < 7; i++)
    {
        // �� ȭ�� ����Ʈ ���ΰ� 1280 �̴ϱ�� �װ� �ؽ���� ���� ������ �������� ��
        vTexcoord = float2(In.vTexcoord.x + (1.f / 1280.f) * i, In.vTexcoord.y);

        // FinalTexture �� ���� ���� ������ �ϰ����ϴ�~
        // LinearSampler_Clamp : �׳� LinearSampler �� �ϸ� ���� ���� Wrap ���� ���õǾ��־ texcoord ���ݻ�
        // ������ �� �����̶�� �ϸ� �� �ݴ��� ���������� ó���Ǳ� ������, �װ� �����ϰ��� wrap ��忡�� clamp ���� �Ϸ��� �̷�����.
        Out.vColor += g_fWeights[i + 6] * g_FinalTexture.Sample(LinearSampler_Clamp, vTexcoord);
    }

    // ������ 6���� ���� �������Ǵµ�, 3���� ������ �� ����� ��
    Out.vColor /= 6.f;

    return Out;
}

PS_OUT PS_MAIN_BLUR_Y(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vTexcoord = 0.f;

    for (int i = -6; i < 7; i++)
    {
        // �� ȭ�� ���� 720 �̴ϱ�� ���� ������ �����ż���
        vTexcoord = float2(In.vTexcoord.x, In.vTexcoord.y + (1.f / 720.f) * i);

        // BlurX���� ������ �س��� �װ� ������ �������ϴ�~ 
        Out.vColor += g_fWeights[i + 6] * g_GlowTexture.Sample(LinearSampler_Clamp, vTexcoord);
    }

    Out.vColor /= 6.f;

    return Out;
}

PS_OUT PS_MAIN_GLOW_X(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vTexcoord = 0.f;

    for (int i = -6; i < 7; i++)
    {
        vTexcoord = float2(In.vTexcoord.x + (1.f / 1280.f) * i, In.vTexcoord.y);

        // GLOW_X ������ �׳� g_GlowTexture 
        Out.vColor += g_fWeights_Glow[i + 6] * g_GlowTexture.Sample(LinearSampler_Clamp, vTexcoord);
    }

    Out.vColor /= 0.5f;

    return Out;
}

PS_OUT PS_MAIN_GLOW_Y(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vTexcoord = 0.f;

    for (int i = -6; i < 7; i++)
    {
        vTexcoord = float2(In.vTexcoord.x, In.vTexcoord.y + (1.f / 720.f) * i);

        // GLOW_Y ������ g_Glow X Texture ���ٰ� 
        Out.vColor += g_fWeights_Glow[i + 6] * g_GlowXTexture.Sample(LinearSampler_Clamp, vTexcoord);
    }

    Out.vColor /= 0.5f;

    return Out;
}

PS_OUT PS_MAIN_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vBlur = g_BlurYTexture.Sample(LinearSampler, In.vTexcoord);
    //vector vGlow = g_GlowXTexture.Sample(LinearSampler, In.vTexcoord);
    
    // ���̳ο����� g_Glow Y Texture ���� ���ø��Ѱ� (������) 
    vector vGlow = g_GlowYTexture.Sample(LinearSampler, In.vTexcoord);
    vector vFinal = g_FinalTexture.Sample(LinearSampler, In.vTexcoord);
	
    Out.vColor = vBlur + vFinal + vGlow;

    return Out;
}


technique11 DefaultTechnique
{
	pass Debug
	{
		//SetRasterizerState(RS_Default);
		SetDepthStencilState(DSS_Default, 0);
		SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DEBUG();
	}

	pass Directional
	{
		//SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff); // BS_Blend �� �ؼ� ���� �� �� ����. 
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	// ���߿� ������;
    pass Point
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        //SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Deferred
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DEFERRED();
    }

// 4
    pass BlurX
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_X();
    }

// 5
    pass BlurY
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_BLUR_Y();
    }

// 6
    pass Final
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }

// 6
    pass GlowX
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GLOW_X();
    }

// 
    pass GlowY
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Skip_Z, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_GLOW_Y();
    }
}
