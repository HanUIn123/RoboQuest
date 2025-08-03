#include "../../../EngineSDK/Hlsl/Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

/* 이 메시(정점들에)에 영향을 주는 뼈 행렬들 */
// 뼈 행렬, (메쉬 기준) 
float4x4 g_BoneMatrices[256];

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;

//float g_HitEffectValue;

//Texture2D g_DissolveTexture; // 디졸브 노이즈 텍스처
//float g_DissolveValue; // 디졸브 진행도 (0.0 ~ 1.0)
//float4 g_DissolveColor; // 디졸브 색상



struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
    
    uint4 vBlendIndex : BLENDINDEX;
    float4 vBlendWeight : BLENDWEIGHT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float4 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
    float4 vProjPos : TEXCOORD2;
    
        
    float4 vTangent : TANGENT;
    float4 vBinormal : BINORMAL;
};


VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    //Out.vPosition = mul(float4(In.vPosition, 1.0f), g_WorldMatrix);
    //Out.vPosition = mul(Out.vPosition, g_ViewMatrix);
    //Out.vPosition = mul(Out.vPosition, g_ProjMatrix);
    
    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    // 로컬 위치를 뼈의 상태로 바꿔준다. Anim 모델은 정점이 움직여야 하니까, (뼈를 따라 움직여서) 
    // 이 정점이 뼈를 따라 움직인다는 건, 로컬에서 뼈 자체의 움직임을 가져감. 
    // 로컬에서 움직인 정점들에다가 월드 뷰 투형 변화를 곱해서 그려낸다.
    // Bone Matrix 를 만들어야 한다. 위에  g_BoneMatrices 는 최대 256개 인데, 이 정점은 최대
    // 4개의 뼈를 가져올 수 있다. 4개 뼈의 상태를 가져와서 그걸 합쳐서 하나에 적용하기 위한 행렬로 
    // 만들어 줘야한다. 
    // 진짜 이 정점에 적용되어야 할, 뼈 행렬을 만든다!!
    // vBlendWeight.x 가 뼈의 인덱스, 메쉬에게 영향을 주는 뼈의 인덱스. 
    // weight 값에 의해 적용된 행렬들을 다 더해서 하나의 상태 행렬을 만든다. 
    
    
    // 숙제는, 칼 이랑 그림자 (애니메이션이 없는 애들) 을 띄워 오는 것.
    // numbone 갯수가 0개 겠지. 메쉬를 로드할 때 보면, 갯수가 0개 니까 정점의 블렌드 인덱스를 
    // 채울수가 없으니, 인덱스 0 0 0 (배열 첫번째 주소)  x y z w 가 0 곱해서 0이 되서 날라가버리는거임
    // Ai Process ~ 이 플래그 키워드를 여기다 적용할꺼임. 칼메쉬는 원래 떠야할 위치가 있었다.
    // 칼메쉬 로드할 때, 뼈 갯수 0 개인데, 강제로 하나 늘릴꺼임. 
    // 그리고 이 뼈를 이용하지 않는 칼메쉬에 뼈를 하나 늘렸다 -> 이 메쉬랑 이름이 똑같은 뼈를 찾아.
    // 그 뼈를 이 메쉬가 이용해야할 뼈로 저장을 해둘꺼임. 
    
    // 칼 Mesh 랑 이름이 똑같은 뼈를 모델 전체 모아놓은 뼈에서 찾을 꺼임. 
    // 이 뼈가 칼 Mesh를 띄워놓기 위한 상태를 지정하기 위한 뼈 였다.
    // 내가 직접 모델에서 뼈를 검색해서, 메쉬랑 같은 이름의 뼈를 찾아서, 그 뼈를 이 칼 mesh가 이용해야할
    // 뼈 갯수 하나로 늘려버릴 꺼임. 
    // numBones 가 1개로 늘었고, 
    // 이 Weitght w 를 괜히 다시 한 번 구한다. 
    // float w = 1 - ( x + y + z )  로 구할 거고,
    // 이것도 0 0 0 0 번 째 행렬의 값을 곱해서 하나를 살려올꺼임.
    // 그리고 곱하면 칼 메쉬가 뜰 것이다. 
    
    In.vBlendWeight.w = (1.0f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z));
    
    matrix BoneMatrix =
     g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
     g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
     g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
     g_BoneMatrices[In.vBlendIndex.w] * In.vBlendWeight.w;
    
    // 위에 행렬을 로컬 위치에다가 곱한게 벡터의 포지션이다. 
    // 이제 여기다가 월드 뷰 투영 을 곱함.
    vector vPosition = mul(vector(In.vPosition, 1.0f), BoneMatrix);
    vector vNormal = mul(vector(In.vNormal, 0.0f), BoneMatrix);
    
    Out.vPosition = mul(vPosition, matWVP);
    Out.vNormal = normalize(mul(vNormal, g_WorldMatrix));
    Out.vTexcoord = In.vTexcoord;
    Out.vWorldPos = mul(vPosition, g_WorldMatrix);
    Out.vProjPos = Out.vPosition;

    
    Out.vTangent = normalize(mul(float4(In.vTangent, 0.0f), g_WorldMatrix));
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
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);

    if (vMtrlDiffuse.a < 0.3f)
        discard;

    //  // 디졸브 노이즈 샘플링
    //float dissolveNoise = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord).r;

    //if (dissolveNoise < g_DissolveValue)
    //{
    //    // 디졸브 경계선 부분을 강조하는 효과
    //    float edgeFactor = smoothstep(g_DissolveValue - 0.05f, g_DissolveValue, dissolveNoise);
    //    vMtrlDiffuse.rgb = lerp(g_DissolveColor.rgb, vMtrlDiffuse.rgb, edgeFactor);
    //}

    //if (dissolveNoise < g_DissolveValue - 0.02f)
    //    discard;
    
    
    //vector vHitEffectColor = vMtrlDiffuse + float4(g_HitEffectValue, g_HitEffectValue, g_HitEffectValue, 0.0f);
    //vHitEffectColor = saturate(vHitEffectColor);
    
    vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, In.vTexcoord);
    float3 vNormal = vNormalDesc.xyz * 2.0f - 1.0f;
    
    float3x3 WorldMatrix = float3x3(In.vTangent.xyz, In.vBinormal.xyz, In.vNormal.xyz);
    vNormal = normalize(mul(vNormal, WorldMatrix));
    
    //Out.vDiffuse = vMtrlDiffuse;
    ////Out.vNormal = vector(In.vNormal.xyz * 0.5f + 0.5f, 0.f);
    //Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.0f);
    //Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.0f, 0.0f);
    
    
    //Out.vDiffuse = vHitEffectColor;
    Out.vDiffuse = vMtrlDiffuse;
    Out.vNormal = vector(vNormal * 0.5f + 0.5f, 0.0f);
    Out.vDepth = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.0f, 0.0f);
    
    return Out;
}

struct VS_OUT_SHADOW
{
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};

VS_OUT_SHADOW VS_MAIN_SHADOW(VS_IN In)
{
    VS_OUT_SHADOW Out = (VS_OUT_SHADOW) 0;

    matrix matWV, matWVP;
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    In.vBlendWeight.w = (1.0f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z));

    matrix BoneMatrix =
     g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
     g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
     g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
     g_BoneMatrices[In.vBlendIndex.w] * In.vBlendWeight.w;
    
	/* 로컬스페이스 상에서 변환을 한다. */
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vProjPos = Out.vPosition;

    return Out;
}

struct PS_IN_SHADOW
{
    // 쉐도우는 포지션하고 투영 쪽 좌표만 필요하니, normal 이나 texcoord 다 빼버리셈 
    float4 vPosition : SV_POSITION;
    float4 vProjPos : TEXCOORD0;
};

struct PS_OUT_SHADOW
{
    float4 vShadow : SV_TARGET0;
};


PS_OUT_SHADOW PS_MAIN_SHADOW(PS_IN_SHADOW In)
{
    PS_OUT_SHADOW Out = (PS_OUT_SHADOW) 0;

    Out.vShadow = vector(In.vProjPos.z / In.vProjPos.w, In.vProjPos.w, 0.f, 0.f);

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

    pass Shadow
    {
        //SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_SHADOW();
        GeometryShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SHADOW();
    }
}