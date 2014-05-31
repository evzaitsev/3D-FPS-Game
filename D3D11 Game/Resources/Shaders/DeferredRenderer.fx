#include "LightHelper.fx"

//================================
//DeferredRenderer: Renders lighting info to gbuffers
//DefferedRenderer.fx by newtechnology
//My coding style is inspired from Frank D. Luna (author of: Introduction to 3D game programming using DirectX 11)
//================================

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldViewProj;
	float4x4 gWorldInvTranspose;
	float4x4 gTexTransform;
    float4x4 gShadowTransform; 

	Material gMaterial;
};


cbuffer cbFixed
{
	const float AOFactor = 0.3f;
	const float SpecIntensity = 0.1f;
	const float specPower = 0.02f;
};

struct VertexIn
{
	float3 PosL : POSITION;
	float3 NormalL : NORMAL;
	float2 Tex : TEXCOORD;
	float3 TangentL : TANGENT;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosW : POSITION;
	float3 NormalW : NORMAL;
	float2 Tex : TEXCOORD;
	float3 TangentW : TANGENT;
	float4 ShadowPosH : TEXCOORD1;
};

SamplerComparisonState samShadow
{
	Filter   = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU = BORDER;
	AddressV = BORDER;
	AddressW = BORDER;
	BorderColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

    ComparisonFunc = LESS;
};
 

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

Texture2D gDiffuseMap;
Texture2D gNormalMap;
Texture2D gShadowMap;

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosW     = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW  = mul(vin.NormalL, (float3x3)gWorldInvTranspose);
	vout.TangentW = mul(vin.TangentL, (float3x3)gWorld);
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;
	vout.ShadowPosH = mul(float4(vin.PosL, 1.0f), gShadowTransform);

	return vout;
}


struct PixelOut
{
	float4 Color : SV_Target0;
	float4 Normal : SV_Target1;
	float4 Position : SV_Target2;
};

PixelOut PS(VertexOut pin) : SV_Target
{
	PixelOut Out;


	pin.NormalW = 0.5f * (normalize(pin.NormalW) + 1.0f);

	float3 color = gDiffuseMap.Sample(samLinear, pin.Tex).rgb;

	float ShadowFactor = CalcShadowFactor(samShadow, gShadowMap, pin.ShadowPosH);

	//output lighting info to gbuffer
	Out.Color = float4(color, specPower);
	Out.Normal = float4(pin.NormalW.xyz, SpecIntensity);
	Out.Position = float4(pin.PosW.xy, ShadowFactor, AOFactor);
  
	return Out;
}

technique11 BuildGBuffers
{
	pass P0
	{
		SetVertexShader( CompileShader(vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader(ps_4_0, PS() ) );
	}
}