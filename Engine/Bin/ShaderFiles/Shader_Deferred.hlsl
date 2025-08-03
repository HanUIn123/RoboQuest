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


// 이 쉐이더는 사각형 버퍼를 받아와서 쓰일 것이기 때문에, VS_IN 은 position 과 texcoord 만을 필요로 하면 되겠다.
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

	// 뷰 변환
	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	// 투영 변환
	matWVP = mul(matWV, g_ProjMatrix);

	// 로컬 Position 에, 뷰&투영 행렬 곱해서 지정.
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

// 디버그 모드 일 때만 구동해서 보여주겠다는 의미로, PS_MAIN_DEBUG 라고 명명했음. 
// 밑의 테크니컬 부분에서도, 이름을 맞춰서 변경하자.
PS_OUT PS_MAIN_DEBUG(PS_IN In)
{
	PS_OUT		Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

	return Out;
}

// PS_OUT 이 두갠데, 구별하려고 이름을 PS_OUT_LIGHT 로 하자고.
// 얘는 같은 정점 정보를 가지고 처리하므로, 그냥 픽셸 셰이더 자체로 컨트롤 해도됨 ㅋ 
struct PS_OUT_LIGHT
{
	float4			vShade : SV_TARGET0;
    float4			vSpecular : SV_TARGET1;
};

// 방향성 빛에 대한 처리
PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
	PS_OUT_LIGHT		Out = (PS_OUT_LIGHT)0;

	vector		vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
	
	//   Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f); // 지형 NorTex 쉐이더에서...
	// 여기까지 온 Normal 은 지형 쉐이더에서 쓰던 Normal 이 0 ~ 1 로 되었어서,
	// 다시 역치를 해주는거임 -1 ~ 1 로 
	vector		vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	// Depth ㄱㄱ 
    vector vDepthDesc	=		g_DepthTexture.Sample(LinearSampler, In.vTexcoord);
	// Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.0f, 0.0f); 에서의 In.vProjPos.w 이다.
    float fViewZ		=		vDepthDesc.y;
	
    vector vShade = saturate(max(dot(normalize(g_vLightDir * -1.f), normalize(vNormal)), 0.f) + (g_vLightAmbient * g_vMtrlAmbient));
	
    Out.vShade = g_vLightDiffuse * vShade;
	
    vector vWorldPos;
	
	// 기본 뷰포트 ex 800 600 이라고 하면 그걸 투영 공간으로 ( -1, -1 ) ~ ( 1, 1 ) 
	// 이 새끼를, 이제 어케 표현하냐, Texcoord 이용해서, 근데 Texcoord 가 ( 0 , 0 ) ~ ( 1 , 1 ) 이니까, 
	// 비율? 맞춰주는 거임.
	
	/* 투영공간상의 위치는, 로컬위치 * 월드 행혈 * 뷰행렬 * 투영행렬 * 1/w */ 
    vWorldPos.x = In.vTexcoord.x * 2.0f - 1.0f;
    vWorldPos.y = In.vTexcoord.y * -2.0f + 1.0f;
	
	// Depth 에서의 x를 가져오게되면, 객체를 투영공간에서 봤을 때, z / w 한 값이 x 에 담겨있음. 
	//  Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.0f, 0.0f);
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.0f;
	
	/* 로컬 위치 * 월드 행렬 * 뷰 행렬  * 투형행렬 */
    vWorldPos = vWorldPos.xyzw * fViewZ;
	
	/* 로컬 위치 * 월드 행렬 * 뷰 행렬 */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);
	
	/* 로컬위치 * 월드 행렬 즉, 월드행렬 */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);

    vector vReflect = reflect(normalize(g_vLightDir), normalize(vNormal));
    vector vLook = vWorldPos - g_vCameraPosition;

    //Out.vSpecular = max(dot(normalize(vReflect * -1.f), normalize(vLook)), 0.f);
    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * pow(max(dot(normalize(vReflect * -1.0f), normalize(vLook)), 0.0f), 50.0f);
	
	
	return Out;
}

// 방향성 빛에 대한 처리
PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);

	// Depth ㄱㄱ 
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y;
	
    vector vWorldPos;

	/* 투영공간상의 위치 */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 * 1/w */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬  */
    vWorldPos = vWorldPos.xyzw * fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);

	/* 월드위치 */
	/* 로컬위치 * 월드행렬  */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);
	
    vector vLightDir = vWorldPos - g_vLightPos;
    float fLength = length(vLightDir);
	// fAtt = 감쇠비. 점 광원과 가까운 픽셀들은, 1이 나오게끔 frange 로 나눠줌. 음수로 나오는 경우에는 saturate 로 잘라버림. 
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
	
	// 밑에서부터 하는건 그림자 디퍼딩 쉐이딩을 위한 코드임 .
    vector vDepthDesc = g_DepthTexture.Sample(LinearSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y;

    vector vWorldPos;

	/* 투영공간상의 위치 */
	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 * 1/w */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬  */
    vWorldPos = vWorldPos.xyzw * fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInverse);

	/* 월드위치 */
	/* 로컬위치 * 월드행렬  */
	// 픽셀의 월드 위치 까지 가져왔어.
    vWorldPos = mul(vWorldPos, g_ViewMatrixInverse);

	// 그 픽셀의 월드 위치에 광원의 뷰행렬 곱햇음. 
    vWorldPos = mul(vWorldPos, g_LightViewMatrix);
	
	// 픽셀의 월드 * 광원의 뷰행렬 * 광원의 투영행렬
	// 곱하기만 했을 뿐 실제 투영을 하려면 깊이 나누기 까지 해야함 
    vWorldPos = mul(vWorldPos, g_LightProjMatrix);

    float2 vTexcoord;

	// 픽셀의 월드 좌표의 xyz 랑 깊이가 w 라서 vWorldPos.w 가 깊이가 되는거라서 이 깊이로 나눈 것이 레알찐 최종 투영 그거임 .
	// 근데 텍스 쿠드 좌표로 변환해서 보여줄라고 하는거니까는, 텍스쿠드가 좌상단 (0,0) 에서 우하단 (1,1) 이니까는 이 비율로 해줄려고 비율 계산처리함. 
    vTexcoord.x = (vWorldPos.x / vWorldPos.w) * 0.5f + 0.5f;
    vTexcoord.y = (vWorldPos.y / vWorldPos.w) * -0.5f + 0.5f;

	// 이제 ShadowTexture 로 부터 깊이 꺼내와줘. 
    vector vShadowDepthDesc = g_ShadowTexture.Sample(LinearSampler, vTexcoord);

	// 그래서 꺼내온 깊이랑, 픽셀의 월드 상의 깊이랑 비교를 해서 픽셀으 깊이가 더 크다 라면, 
    if (vShadowDepthDesc.y <= vWorldPos.w)
        Out.vColor = Out.vColor * 0.4f;

	
    return Out;
}

// 가중치 Blur 용도. 가운데 1을 기준으로 좌변 수치량 <-> 우변 수치량 뜻하는 거임 
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
        // 내 화면 뷰포트 가로가 1280 이니까는 그걸 텍스쿠드 가로 비율로 나눈거임 ㅋ
        vTexcoord = float2(In.vTexcoord.x + (1.f / 1280.f) * i, In.vTexcoord.y);

        // FinalTexture 의 색을 갖다 조작질 하겟읍니다~
        // LinearSampler_Clamp : 그냥 LinearSampler 로 하면 지금 저게 Wrap 모드로 세팅되어있어서 texcoord 성격상
        // 왼쪽의 더 왼쪽이라고 하면 저 반대편 오른쪽으로 처리되기 때문에, 그걸 방지하고자 wrap 모드에서 clamp 모드로 하려고 이런거임.
        Out.vColor += g_fWeights[i + 6] * g_FinalTexture.Sample(LinearSampler_Clamp, vTexcoord);
    }

    // 지금은 6으로 반절 나눠버렷는데, 3으로 나누면 더 밝아짐 ㅋ
    Out.vColor /= 6.f;

    return Out;
}

PS_OUT PS_MAIN_BLUR_Y(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float2 vTexcoord = 0.f;

    for (int i = -6; i < 7; i++)
    {
        // 내 화면 세로 720 이니까는 세로 비율로 나눈거셈ㅋ
        vTexcoord = float2(In.vTexcoord.x, In.vTexcoord.y + (1.f / 720.f) * i);

        // BlurX에서 조작질 해놓은 그걸 가져다 쓰겟읍니다~ 
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

        // GLOW_X 에서는 그냥 g_GlowTexture 
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

        // GLOW_Y 에서는 g_Glow X Texture 에다가 
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
    
    // 파이널에서는 g_Glow Y Texture 에서 샘플링한거 (최종값) 
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
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff); // BS_Blend 로 해서 섞을 수 도 있음. 
		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = NULL;
		PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
	}

	// 나중에 쓸꺼셈;
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
