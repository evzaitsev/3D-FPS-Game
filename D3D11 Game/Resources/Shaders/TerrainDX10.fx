#include "lighthelper.fx"
 
cbuffer cbPerFrame
{
	float4x4 gWorld;
	float4x4 gWVP;
	float4x4 gWorldInvTranspose;
	float4x4 gShadowTransform; 

	Material gMaterial;
	DirectionalLight gDirLights[3];

	float3 gEyePosW;
	float gFogStart;
	float gFogRange;
	float4 gFogColor;
};



cbuffer cbFixed
{
	float gTexScale = 20;

};
 
// Nonnumeric values cannot be added to a cbuffer.
Texture2D gLayer0;
Texture2D gLayer1;
Texture2D gLayer2;
Texture2D gLayer3;
Texture2D gLayer4;
Texture2D gBlendMap;
Texture2D gShadowMap;

SamplerState gTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
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

struct VS_IN
{
	float3 posL    : POSITION;
	float3 normalL : NORMAL;
	float2 texC    : TEXCOORD;
};

struct VS_OUT
{
	float4 posH         : SV_POSITION;
	float3 posW         : POSITION;
	float3 NormalW      : NORMAL;
    float2 tiledUV      : TEXCOORD0;
    float2 stretchedUV  : TEXCOORD1; 
	float4 ShadowPosH   : TEXCOORD2;
};


VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut;
	
	vOut.posH = mul(float4(vIn.posL, 1.0f), gWVP);
	vOut.posW = mul(float4(vIn.posL, 1.0f), gWorld).xyz;
	//vOut.NormalW = mul(vIn.normalL, (float3x3)gWorldInvTranspose);
	
	float4 normalW = mul(float4(vIn.normalL, 0.0f), gWorld);
	vOut.NormalW = normalW.xyz;

	vOut.tiledUV     = gTexScale * vIn.texC;
	vOut.stretchedUV = vIn.texC;

	// Generate projective tex-coords to project shadow map onto scene.
    vOut.ShadowPosH = mul(float4(vIn.posL, 1.0f), gShadowTransform);

	
	return vOut;
}

float4 PS(VS_OUT pIn, uniform bool gFogEnabled, uniform int gLightCount) : SV_Target
{
	float4 c0 = gLayer0.Sample( gTriLinearSam, pIn.tiledUV );
	float4 c1 = gLayer1.Sample( gTriLinearSam, pIn.tiledUV );
	float4 c2 = gLayer2.Sample( gTriLinearSam, pIn.tiledUV );
	float4 c3 = gLayer3.Sample( gTriLinearSam, pIn.tiledUV );
	float4 c4 = gLayer4.Sample( gTriLinearSam, pIn.tiledUV ); 
	
	float4 t = gBlendMap.Sample( gTriLinearSam, pIn.stretchedUV ); 
    
    float4 C = c0;
    C = lerp(C, c1, t.r);
    C = lerp(C, c2, t.g);
    C = lerp(C, c3, t.b);
    C = lerp(C, c4, t.a);
    
	float3 toEye = gEyePosW - pIn.posW;
    float distToEye = length(toEye);

	toEye /= distToEye;
	 

	float4 litColor = C;
	if (gLightCount > 0)
	{
		
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);
		
	
		float3 shadow = float3(1.0f, 1.0f, 1.0f);
		shadow[0] = CalcShadowFactor(samShadow, gShadowMap, pIn.ShadowPosH);

		[unroll]
		for(int i = 0; i < gLightCount; ++i)
		{
			float4 A, D, S;
			ComputeDirectionalLight(gMaterial, gDirLights[i], pIn.NormalW, toEye, 
				A, D, S);

			ambient += A;    
			diffuse += shadow[i]*D;
			spec    += shadow[i]*S;
		}

		litColor = C * (ambient + diffuse) + spec;		

	}

	if (gFogEnabled)
	{

	   pIn.NormalW = normalize(pIn.NormalW);

	   float fogLerp = saturate((distToEye - gFogStart) / gFogRange);
	   litColor = lerp(litColor, gFogColor, fogLerp);

	}
    
	litColor.a = gMaterial.Diffuse.a * C.a;

    return litColor;
}


technique11 TerrainLight1Tech
{
    pass P0

    {
        SetRasterizerState(NULL);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS(false, 1)) );
    }
}

technique11 TerrainLight2Tech
{
    pass P0

    {
        SetRasterizerState(NULL);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS(false, 2)) );
    }
}

technique11 TerrainLight3Tech
{
    pass P0

    {
        SetRasterizerState(NULL);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS(false, 3)) );
    }
}

technique11 TerrainFogLight1Tech
{
	pass P0
	{
		SetRasterizerState(NULL);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS(true, 1)) );
	}

}

technique11 TerrainFogLight2Tech
{
	pass P0
	{
		SetRasterizerState(NULL);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS(true, 2)) );
	}

}

technique11 TerrainFogLight3Tech
{
	pass P0
	{
		SetRasterizerState(NULL);

		SetVertexShader( CompileShader( vs_4_0, VS() ) );
		SetGeometryShader( NULL );
		SetPixelShader( CompileShader( ps_4_0, PS(true, 3)) );
	}

}