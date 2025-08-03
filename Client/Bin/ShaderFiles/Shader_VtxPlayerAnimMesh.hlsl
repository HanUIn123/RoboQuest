#include "../../../EngineSDK/Hlsl/Engine_Shader_Defines.hlsli"

float4x4 g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

/* �� �޽�(�����鿡)�� ������ �ִ� �� ��ĵ� */
// �� ���, (�޽� ����) 
float4x4 g_BoneMatrices[256];

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;

//float g_HitEffectValue;

//Texture2D g_DissolveTexture; // ������ ������ �ؽ�ó
//float g_DissolveValue; // ������ ���൵ (0.0 ~ 1.0)
//float4 g_DissolveColor; // ������ ����



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
    
    // ���� ��ġ�� ���� ���·� �ٲ��ش�. Anim ���� ������ �������� �ϴϱ�, (���� ���� ��������) 
    // �� ������ ���� ���� �����δٴ� ��, ���ÿ��� �� ��ü�� �������� ������. 
    // ���ÿ��� ������ �����鿡�ٰ� ���� �� ���� ��ȭ�� ���ؼ� �׷�����.
    // Bone Matrix �� ������ �Ѵ�. ����  g_BoneMatrices �� �ִ� 256�� �ε�, �� ������ �ִ�
    // 4���� ���� ������ �� �ִ�. 4�� ���� ���¸� �����ͼ� �װ� ���ļ� �ϳ��� �����ϱ� ���� ��ķ� 
    // ����� ����Ѵ�. 
    // ��¥ �� ������ ����Ǿ�� ��, �� ����� �����!!
    // vBlendWeight.x �� ���� �ε���, �޽����� ������ �ִ� ���� �ε���. 
    // weight ���� ���� ����� ��ĵ��� �� ���ؼ� �ϳ��� ���� ����� �����. 
    
    
    // ������, Į �̶� �׸��� (�ִϸ��̼��� ���� �ֵ�) �� ��� ���� ��.
    // numbone ������ 0�� ����. �޽��� �ε��� �� ����, ������ 0�� �ϱ� ������ ���� �ε����� 
    // ä����� ������, �ε��� 0 0 0 (�迭 ù��° �ּ�)  x y z w �� 0 ���ؼ� 0�� �Ǽ� ���󰡹����°���
    // Ai Process ~ �� �÷��� Ű���带 ����� �����Ҳ���. Į�޽��� ���� ������ ��ġ�� �־���.
    // Į�޽� �ε��� ��, �� ���� 0 ���ε�, ������ �ϳ� �ø�����. 
    // �׸��� �� ���� �̿����� �ʴ� Į�޽��� ���� �ϳ� �÷ȴ� -> �� �޽��� �̸��� �Ȱ��� ���� ã��.
    // �� ���� �� �޽��� �̿��ؾ��� ���� ������ �صѲ���. 
    
    // Į Mesh �� �̸��� �Ȱ��� ���� �� ��ü ��Ƴ��� ������ ã�� ����. 
    // �� ���� Į Mesh�� ������� ���� ���¸� �����ϱ� ���� �� ����.
    // ���� ���� �𵨿��� ���� �˻��ؼ�, �޽��� ���� �̸��� ���� ã�Ƽ�, �� ���� �� Į mesh�� �̿��ؾ���
    // �� ���� �ϳ��� �÷����� ����. 
    // numBones �� 1���� �þ���, 
    // �� Weitght w �� ���� �ٽ� �� �� ���Ѵ�. 
    // float w = 1 - ( x + y + z )  �� ���� �Ű�,
    // �̰͵� 0 0 0 0 �� ° ����� ���� ���ؼ� �ϳ��� ����ò���.
    // �׸��� ���ϸ� Į �޽��� �� ���̴�. 
    
    In.vBlendWeight.w = (1.0f - (In.vBlendWeight.x + In.vBlendWeight.y + In.vBlendWeight.z));
    
    matrix BoneMatrix =
     g_BoneMatrices[In.vBlendIndex.x] * In.vBlendWeight.x +
     g_BoneMatrices[In.vBlendIndex.y] * In.vBlendWeight.y +
     g_BoneMatrices[In.vBlendIndex.z] * In.vBlendWeight.z +
     g_BoneMatrices[In.vBlendIndex.w] * In.vBlendWeight.w;
    
    // ���� ����� ���� ��ġ���ٰ� ���Ѱ� ������ �������̴�. 
    // ���� ����ٰ� ���� �� ���� �� ����.
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

    //  // ������ ������ ���ø�
    //float dissolveNoise = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord).r;

    //if (dissolveNoise < g_DissolveValue)
    //{
    //    // ������ ��輱 �κ��� �����ϴ� ȿ��
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
    
	/* ���ý����̽� �󿡼� ��ȯ�� �Ѵ�. */
    vector vPosition = mul(vector(In.vPosition, 1.f), BoneMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vProjPos = Out.vPosition;

    return Out;
}

struct PS_IN_SHADOW
{
    // ������� �������ϰ� ���� �� ��ǥ�� �ʿ��ϴ�, normal �̳� texcoord �� �������� 
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