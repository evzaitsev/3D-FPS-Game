#include "LightHelper.fx"

//lightcolor is not fixed, it is just for debugging/testing purpose
cbuffer cbFixed
{
	const float3 LightColor = float3(1.0f, 1.0f, 1.0f);
};

SamplerState samPoint
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = CLAMP;
	AddressV = CLAMP;
};

cbuffer cbPerObject
{
	float4x4 gWorldViewProj;
};

cbuffer cbPerFrame
{
	float3 gEyePosW;
	Material gMaterial;
	DirectionalLight gLight;
};


//gbuffers
Texture2D gColorMap;
Texture2D gNormalsMap;
Texture2D gPositionMap;

struct VertexIn
{
	float3 PosL : POSITION;
	float2 tex : TEXCOORD;
};

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float2 tex : TEXCOORD;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

	//no transformation required here
	vout.tex = vin.tex;

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	//Sample textures (gbuffers)
	float4 color = gColorMap.Sample(samPoint, pin.tex);
	float4 normal = gNormalsMap.Sample(samPoint, pin.tex);
	float4 position = gPositionMap.Sample(samPoint, pin.tex);

	//extract material properties from alpha channel of all 3 textures
	float specPower = color.a * 255;
	float specIntensity = normal.a;
	float AOFactor = position.a;
	float ShadowFactor = position.z;



    float3 normalW = 2.0f * normal.xyz - 1.0f;
	
	float3 lightVector = -float3(0.0f, -1.0f, 0.0f);

	float NdL = saturate(dot(lightVector, normalW));

	float SpecularLight = 0.0f;

	[flattern]
	if (NdL > 0.0f)
	{
	    float3 DirectionToCamera = gEyePosW - position.xyz;

		float DistToEye = length(DirectionToCamera);

		DirectionToCamera /= DistToEye;

	    float3 reflectionVector = reflect(-lightVector, normalW);

	    SpecularLight = specIntensity *  pow(saturate(dot(reflectionVector, DirectionToCamera)), specPower);
	} 

	
	float3 DiffuseLight = (NdL * LightColor) * ShadowFactor;

	DiffuseLight += AOFactor;

	float3 finalcolor = color.rgb * DiffuseLight;

	return float4(finalcolor.rgb, 1.0f);
}

technique11 DeferredLighting
{
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS() ) );
	}
};