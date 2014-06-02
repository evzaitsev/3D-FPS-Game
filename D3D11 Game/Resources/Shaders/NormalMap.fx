//=============================================================================
// NormalMap.fx by Frank Luna (C) 2011 All Rights Reserved.
//=============================================================================

#include "LightHelper.fx"
 
cbuffer cbPerFrame
{ 
	DirectionalLight gDirLights[3];
	PointLight gPointLights[3];
	float3 gEyePosW;

	float  gFogStart;
	float  gFogRange;
	float4 gFogColor; 

};

cbuffer cbSkinned
{
	float4x4 gBoneTransforms[96];
};

cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gViewProj; // used for instancing
	float4x4 gTexTransform;
	float4x4 gShadowTransform; 
	Material gMaterial;

}; 

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;
Texture2D gNormalMap;
Texture2D gShadowMap;
Texture2D gAOMap;
Texture2D gSpecularMap;
TextureCube gCubeMap;

SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
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
 
struct VertexIn
{
	float3 PosL     : POSITION;
	float3 NormalL  : NORMAL;
	float2 Tex      : TEXCOORD;
	float3 TangentL : TANGENT;
};

struct SkinnedVertexIn
{
	float3 PosL       : POSITION;
	float3 NormalL    : NORMAL;
	float2 Tex        : TEXCOORD;
	float4 TangentL   : TANGENT;
	float3 Weights    : WEIGHTS;
	uint4 BoneIndices : BONEINDICES;
};

struct InstancedVertexIn
{
	float3 PosL     : POSITION;
	float3 NormalL  : NORMAL;
	float2 Tex      : TEXCOORD;
	float3 TangentL : TANGENT;
	row_major float4x4 World  : WORLD;
	uint InstanceId : SV_InstanceID;
};

struct VertexOut
{
	float4 PosH     : SV_POSITION;
    float3 PosW     : POSITION;
    float3 NormalW  : NORMAL;
	float3 TangentW : TANGENT;
	float2 Tex      : TEXCOORD0;
	float4 ShadowPosH : TEXCOORD1;
};

VertexOut VS_INSTANCED(InstancedVertexIn vin)
{
	VertexOut vout;
	
	// Transform to world space space.
	vout.PosW    = mul(float4(vin.PosL, 1.0f), vin.World).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3)vin.World);
	vout.TangentW = mul(vin.TangentL, (float3x3)vin.World);
		
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vout.PosW, 1.0f), gViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	vout.Tex   = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	vout.ShadowPosH = mul(float4(vin.PosL, 1.0f), gShadowTransform);

	return vout;
}

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to world space space.
	vout.PosW     = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW  = mul(vin.NormalL, (float3x3)gWorldInvTranspose);
	vout.TangentW = mul(vin.TangentL, (float3x3)gWorld);

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	// Generate projective tex-coords to project shadow map onto scene.
	vout.ShadowPosH = mul(float4(vin.PosL, 1.0f), gShadowTransform);


	return vout;
}

VertexOut SKINNED_VS(SkinnedVertexIn vin)
{
    VertexOut vout;

	// Init array or else we get strange warnings about SV_POSITION.
	float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	weights[0] = vin.Weights.x;
	weights[1] = vin.Weights.y;
	weights[2] = vin.Weights.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

	float3 posL     = float3(0.0f, 0.0f, 0.0f);
	float3 normalL  = float3(0.0f, 0.0f, 0.0f);
	float3 tangentL = float3(0.0f, 0.0f, 0.0f);
	for(int i = 0; i < 4; ++i)
	{
	    // Assume no nonuniform scaling when transforming normals, so 
		// that we do not have to use the inverse-transpose.

	    posL     += weights[i]*mul(float4(vin.PosL, 1.0f), gBoneTransforms[vin.BoneIndices[i]]).xyz;
		normalL  += weights[i]*mul(vin.NormalL,  (float3x3)gBoneTransforms[vin.BoneIndices[i]]);
		tangentL += weights[i]*mul(vin.TangentL.xyz, (float3x3)gBoneTransforms[vin.BoneIndices[i]]);
	}
 
	// Transform to world space space.
	vout.PosW     = mul(float4(posL, 1.0f), gWorld).xyz;
	vout.NormalW  = mul(normalL, (float3x3)gWorldInvTranspose);
	vout.TangentW = float4(mul(tangentL, (float3x3)gWorld), vin.TangentL.w);

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(posL, 1.0f), gWorldViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	// Generate projective tex-coords to project shadow map onto scene.
	vout.ShadowPosH = mul(float4(posL, 1.0f), gShadowTransform);


	return vout;
}
 
float4 PS(VertexOut pin, 
          uniform int gLightCount, 
		  uniform int gPointLightCount,
		  uniform bool gUseTexure, 
		  uniform bool gAlphaClip, 
		  uniform bool gFogEnabled, 
		  uniform bool gReflectionEnabled,
		  uniform bool gUseOcclusionMap,
		  uniform bool gUseSpecularMap) : SV_Target
{
	// Interpolating normal can unnormalize it, so normalize it.
	pin.NormalW = normalize(pin.NormalW);

	// The toEye vector is used in lighting.
	float3 toEye = gEyePosW - pin.PosW;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye);

	// Normalize.
	toEye /= distToEye;
	
    // Default to multiplicative identity.
    float4 texColor = float4(1, 1, 1, 1);
	float4 AOColor = float4(1, 1, 1, 1);
	float4 SpecMapColor = float4(1, 1, 1, 1);
    if(gUseTexure)
	{
		// Sample texture.
		texColor = gDiffuseMap.Sample( samLinear, pin.Tex );

		if(gAlphaClip)
		{
			// Discard pixel if texture alpha < 0.1.  Note that we do this
			// test as soon as possible so that we can potentially exit the shader 
			// early, thereby skipping the rest of the shader code.
			clip(texColor.a - 0.1f);
		}
		 
		// Sample AO Map texture
		if (gUseOcclusionMap)
		{
		    AOColor = gAOMap.Sample( samLinear, pin.Tex );
		}

		//sample Specular Map texture
		if (gUseSpecularMap)
		{
			SpecMapColor = gSpecularMap.Sample( samLinear, pin.Tex );
		}
	}

	//
	// Normal mapping  
	// 
	 
	float3 normalMapSample = gNormalMap.Sample(samLinear, pin.Tex).rgb;
	float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample, pin.NormalW, pin.TangentW);
	 
	//
	// Lighting.
	//

	float4 litColor = texColor;
	if(gLightCount > 0)
	{  
		// Start with a sum of zero. 
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// Only the first light casts a shadow.
		float3 shadow = float3(1.0f, 1.0f, 1.0f);
		shadow[0] = CalcShadowFactor(samShadow, gShadowMap, pin.ShadowPosH);

		// Sum the light contribution from each light source.  
		[unroll]
		for(int i = 0; i < gLightCount; ++i)
		{
			float4 A, D, S;

			ComputeDirectionalLight(gMaterial, gDirLights[i], bumpedNormalW, toEye, 
			 A, D, S);

			 ambient += A*AOColor;
			 diffuse += D*shadow[i];
			 spec    += S*shadow[i]*SpecMapColor;
			
		}

		if (gPointLightCount)
		{
			[unroll]
			for (int i = 0; i < gPointLightCount; ++i)
			{
				float4 A, D, S;

				ComputePointLight(gMaterial, gPointLights[i], pin.PosW, bumpedNormalW, toEye,
					A, D, S);

				ambient += A*AOColor;
				diffuse += D;
				spec    += S*SpecMapColor;

			}

		}

		litColor = texColor*(ambient + diffuse) + spec;

		if( gReflectionEnabled )
		{
			float3 incident = -toEye;
			float3 reflectionVector = reflect(incident, bumpedNormalW);
			float4 reflectionColor  = gCubeMap.Sample(samLinear, reflectionVector);

			litColor += gMaterial.Reflect*reflectionColor;
		}
	}
 
	//
	// Fogging
	//

	if( gFogEnabled )
	{
		float fogLerp = saturate( (distToEye - gFogStart) / gFogRange ); 

		// Blend the fog color and the lit color.
		litColor = lerp(litColor, gFogColor, fogLerp);
	}

	// Common to take alpha from diffuse material and texture.
	litColor.a = gMaterial.Diffuse.a * texColor.a;

    return litColor;
}


technique11 Light0PointLight0Tex
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(0, 0, true, false, false, false, false, false))  );
    }
}
  
technique11 Light0PointLight0TexSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(0, 0, true, false, false, false, false, true))  );
    }
}
  
technique11 Light0PointLight0TexAO
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(0, 0, true, false, false, false, true, false))  );
    }
}
  
technique11 Light0PointLight0TexAOSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(0, 0, true, false, false, false, true, true))  );
    }
}
  
technique11 Light0PointLight1Tex
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(0, 1, true, false, false, false, false, false))  );
    }
}
  
technique11 Light0PointLight1TexSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(0, 1, true, false, false, false, false, true))  );
    }
}
  
technique11 Light0PointLight1TexAO
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(0, 1, true, false, false, false, true, false))  );
    }
}
  
technique11 Light0PointLight1TexAOSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(0, 1, true, false, false, false, true, true))  );
    }
}
  
technique11 Light0PointLight2Tex
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(0, 2, true, false, false, false, false, false))  );
    }
}
  
technique11 Light0PointLight2TexSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(0, 2, true, false, false, false, false, true))  );
    }
}
  
technique11 Light0PointLight2TexAO
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(0, 2, true, false, false, false, true, false))  );
    }
}
  
technique11 Light0PointLight2TexAOSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(0, 2, true, false, false, false, true, true))  );
    }
}
  
technique11 Light0PointLight3Tex
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(0, 3, true, false, false, false, false, false))  );
    }
}
  
technique11 Light0PointLight3TexSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(0, 3, true, false, false, false, false, true))  );
    }
}
  
technique11 Light0PointLight3TexAO
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(0, 3, true, false, false, false, true, false))  );
    }
}
  
technique11 Light0PointLight3TexAOSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(0, 3, true, false, false, false, true, true))  );
    }
}
  
technique11 Light1PointLight0Tex
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(1, 0, true, false, false, false, false, false))  );
    }
}
  
technique11 Light1PointLight0TexSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(1, 0, true, false, false, false, false, true))  );
    }
}
  
technique11 Light1PointLight0TexAO
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(1, 0, true, false, false, false, true, false))  );
    }
}
  
technique11 Light1PointLight0TexAOSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(1, 0, true, false, false, false, true, true))  );
    }
}
  
technique11 Light1PointLight1Tex
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(1, 1, true, false, false, false, false, false))  );
    }
}
  
technique11 Light1PointLight1TexSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(1, 1, true, false, false, false, false, true))  );
    }
}
  
technique11 Light1PointLight1TexAO
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(1, 1, true, false, false, false, true, false))  );
    }
}
  
technique11 Light1PointLight1TexAOSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(1, 1, true, false, false, false, true, true))  );
    }
}
  
technique11 Light1PointLight2Tex
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(1, 2, true, false, false, false, false, false))  );
    }
}
  
technique11 Light1PointLight2TexSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(1, 2, true, false, false, false, false, true))  );
    }
}
  
technique11 Light1PointLight2TexAO
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(1, 2, true, false, false, false, true, false))  );
    }
}
  
technique11 Light1PointLight2TexAOSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(1, 2, true, false, false, false, true, true))  );
    }
}
  
technique11 Light1PointLight3Tex
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(1, 3, true, false, false, false, false, false))  );
    }
}
  
technique11 Light1PointLight3TexSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(1, 3, true, false, false, false, false, true))  );
    }
}
  
technique11 Light1PointLight3TexAO
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(1, 3, true, false, false, false, true, false))  );
    }
}
  
technique11 Light1PointLight3TexAOSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(1, 3, true, false, false, false, true, true))  );
    }
}
  
technique11 Light2PointLight0Tex
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(2, 0, true, false, false, false, false, false))  );
    }
}
  
technique11 Light2PointLight0TexSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(2, 0, true, false, false, false, false, true))  );
    }
}
  
technique11 Light2PointLight0TexAO
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(2, 0, true, false, false, false, true, false))  );
    }
}
  
technique11 Light2PointLight0TexAOSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(2, 0, true, false, false, false, true, true))  );
    }
}
  
technique11 Light2PointLight1Tex
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(2, 1, true, false, false, false, false, false))  );
    }
}
  
technique11 Light2PointLight1TexSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(2, 1, true, false, false, false, false, true))  );
    }
}
  
technique11 Light2PointLight1TexAO
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(2, 1, true, false, false, false, true, false))  );
    }
}
  
technique11 Light2PointLight1TexAOSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(2, 1, true, false, false, false, true, true))  );
    }
}
  
technique11 Light2PointLight2Tex
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(2, 2, true, false, false, false, false, false))  );
    }
}
  
technique11 Light2PointLight2TexSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(2, 2, true, false, false, false, false, true))  );
    }
}
  
technique11 Light2PointLight2TexAO
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(2, 2, true, false, false, false, true, false))  );
    }
}
  
technique11 Light2PointLight2TexAOSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(2, 2, true, false, false, false, true, true))  );
    }
}
  
technique11 Light2PointLight3Tex
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(2, 3, true, false, false, false, false, false))  );
    }
}
  
technique11 Light2PointLight3TexSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(2, 3, true, false, false, false, false, true))  );
    }
}
  
technique11 Light2PointLight3TexAO
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(2, 3, true, false, false, false, true, false))  );
    }
}
  
technique11 Light2PointLight3TexAOSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(2, 3, true, false, false, false, true, true))  );
    }
}
  
technique11 Light3PointLight0Tex
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(3, 0, true, false, false, false, false, false))  );
    }
}
  
technique11 Light3PointLight0TexSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(3, 0, true, false, false, false, false, true))  );
    }
}
  
technique11 Light3PointLight0TexAO
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(3, 0, true, false, false, false, true, false))  );
    }
}
  
technique11 Light3PointLight0TexAOSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(3, 0, true, false, false, false, true, true))  );
    }
}
  
technique11 Light3PointLight1Tex
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(3, 1, true, false, false, false, false, false))  );
    }
}
  
technique11 Light3PointLight1TexSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(3, 1, true, false, false, false, false, true))  );
    }
}
  
technique11 Light3PointLight1TexAO
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(3, 1, true, false, false, false, true, false))  );
    }
}
  
technique11 Light3PointLight1TexAOSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(3, 1, true, false, false, false, true, true))  );
    }
}
  
technique11 Light3PointLight2Tex
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(3, 2, true, false, false, false, false, false))  );
    }
}
  
technique11 Light3PointLight2TexSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(3, 2, true, false, false, false, false, true))  );
    }
}
  
technique11 Light3PointLight2TexAO
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(3, 2, true, false, false, false, true, false))  );
    }
}
  
technique11 Light3PointLight2TexAOSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(3, 2, true, false, false, false, true, true))  );
    }
}
  
technique11 Light3PointLight3Tex
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(3, 3, true, false, false, false, false, false))  );
    }
}
  
technique11 Light3PointLight3TexSpec
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(3, 3, true, false, false, false, false, true))  );
    }
}
  
technique11 Light3PointLight3TexAO
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(3, 3, true, false, false, false, true, false))  );
    }
}
  
technique11 Light3PointLight3TexAOSpec
{
  pass P0
    { 
      SetVertexShader( CompileShader( vs_4_0, VS() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(3, 3, true, false, false, false, true, true))  );
    }
}
  


technique11 Light0PointLight0Instanced
{
  pass P0
    {
      SetVertexShader( CompileShader( vs_4_0, VS_INSTANCED() ) );
      SetGeometryShader( NULL );
      SetPixelShader( CompileShader( ps_4_0, PS(0, 0, false, false, false, false, false, false))  );
    }
}


technique11 Light1TexSkinned
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, SKINNED_VS() ) );
		SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS(1, 0, true, false, false, false, false, false) ) );
    }
}