//***************************************************************************************
// Effects.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "stdafx.h"

#pragma region Effect
Effect::Effect(ID3D11Device* device, const std::wstring& filename)
	: mFX(0)
{
	std::ifstream fin(filename.c_str(), std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg); 
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();
	
	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 
		0, device, &mFX));
}

Effect::~Effect()
{
	ReleaseCOM(mFX);
}
#pragma endregion

#pragma region TerrainEffect
TerrainEffect::TerrainEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	Light1Tech    = mFX->GetTechniqueByName("Light1");
	Light2Tech    = mFX->GetTechniqueByName("Light2");
	Light3Tech    = mFX->GetTechniqueByName("Light3");
	Light1FogTech = mFX->GetTechniqueByName("Light1Fog");
	Light2FogTech = mFX->GetTechniqueByName("Light2Fog");
	Light3FogTech = mFX->GetTechniqueByName("Light3Fog");
	TessBuildShadowMapTech = mFX->GetTechniqueByName("TessBuildShadowMapTech");


	ViewProj           = mFX->GetVariableByName("gViewProj")->AsMatrix();
	EyePosW            = mFX->GetVariableByName("gEyePosW")->AsVector();
	FogColor           = mFX->GetVariableByName("gFogColor")->AsVector();
	FogStart           = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange           = mFX->GetVariableByName("gFogRange")->AsScalar();
	DirLights          = mFX->GetVariableByName("gDirLights");
	Mat                = mFX->GetVariableByName("gMaterial");

	MinDist            = mFX->GetVariableByName("gMinDist")->AsScalar();
	MaxDist            = mFX->GetVariableByName("gMaxDist")->AsScalar();
	MinTess            = mFX->GetVariableByName("gMinTess")->AsScalar();
	MaxTess            = mFX->GetVariableByName("gMaxTess")->AsScalar();
	TexelCellSpaceU    = mFX->GetVariableByName("gTexelCellSpaceU")->AsScalar();
	TexelCellSpaceV    = mFX->GetVariableByName("gTexelCellSpaceV")->AsScalar();
	WorldCellSpace     = mFX->GetVariableByName("gWorldCellSpace")->AsScalar();
	ShadowTransform   = mFX->GetVariableByName("gShadowTransform")->AsMatrix();
	WorldFrustumPlanes = mFX->GetVariableByName("gWorldFrustumPlanes")->AsVector();

	LayerMapArray      = mFX->GetVariableByName("gLayerMapArray")->AsShaderResource();
	BlendMap           = mFX->GetVariableByName("gBlendMap")->AsShaderResource();
	HeightMap          = mFX->GetVariableByName("gHeightMap")->AsShaderResource();
	ShadowMap         = mFX->GetVariableByName("gShadowMap")->AsShaderResource();
}

TerrainEffect::~TerrainEffect()
{
}
#pragma endregion


#pragma region WavesEffect
WavesEffect::WavesEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	Light1Tech    = mFX->GetTechniqueByName("Light1");
	Light2Tech    = mFX->GetTechniqueByName("Light2");
	Light3Tech    = mFX->GetTechniqueByName("Light3");

	Light0TexTech = mFX->GetTechniqueByName("Light0Tex");
	Light1TexTech = mFX->GetTechniqueByName("Light1Tex");
	Light2TexTech = mFX->GetTechniqueByName("Light2Tex");
	Light3TexTech = mFX->GetTechniqueByName("Light3Tex");

	Light0TexAlphaClipTech = mFX->GetTechniqueByName("Light0TexAlphaClip");
	Light1TexAlphaClipTech = mFX->GetTechniqueByName("Light1TexAlphaClip");
	Light2TexAlphaClipTech = mFX->GetTechniqueByName("Light2TexAlphaClip");
	Light3TexAlphaClipTech = mFX->GetTechniqueByName("Light3TexAlphaClip");

	Light1FogTech    = mFX->GetTechniqueByName("Light1Fog");
	Light2FogTech    = mFX->GetTechniqueByName("Light2Fog");
	Light3FogTech    = mFX->GetTechniqueByName("Light3Fog");

	Light0TexFogTech = mFX->GetTechniqueByName("Light0TexFog");
	Light1TexFogTech = mFX->GetTechniqueByName("Light1TexFog");
	Light2TexFogTech = mFX->GetTechniqueByName("Light2TexFog");
	Light3TexFogTech = mFX->GetTechniqueByName("Light3TexFog");

	Light0TexAlphaClipFogTech = mFX->GetTechniqueByName("Light0TexAlphaClipFog");
	Light1TexAlphaClipFogTech = mFX->GetTechniqueByName("Light1TexAlphaClipFog");
	Light2TexAlphaClipFogTech = mFX->GetTechniqueByName("Light2TexAlphaClipFog");
	Light3TexAlphaClipFogTech = mFX->GetTechniqueByName("Light3TexAlphaClipFog");

	Light1ReflectTech    = mFX->GetTechniqueByName("Light1Reflect");
	Light2ReflectTech    = mFX->GetTechniqueByName("Light2Reflect");
	Light3ReflectTech    = mFX->GetTechniqueByName("Light3Reflect");

	Light0TexReflectTech = mFX->GetTechniqueByName("Light0TexReflect");
	Light1TexReflectTech = mFX->GetTechniqueByName("Light1TexReflect");
	Light2TexReflectTech = mFX->GetTechniqueByName("Light2TexReflect");
	Light3TexReflectTech = mFX->GetTechniqueByName("Light3TexReflect");

	Light0TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light0TexAlphaClipReflect");
	Light1TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light1TexAlphaClipReflect");
	Light2TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light2TexAlphaClipReflect");
	Light3TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light3TexAlphaClipReflect");

	Light1FogReflectTech    = mFX->GetTechniqueByName("Light1FogReflect");
	Light2FogReflectTech    = mFX->GetTechniqueByName("Light2FogReflect");
	Light3FogReflectTech    = mFX->GetTechniqueByName("Light3FogReflect");

	Light0TexFogReflectTech = mFX->GetTechniqueByName("Light0TexFogReflect");
	Light1TexFogReflectTech = mFX->GetTechniqueByName("Light1TexFogReflect");
	Light2TexFogReflectTech = mFX->GetTechniqueByName("Light2TexFogReflect");
	Light3TexFogReflectTech = mFX->GetTechniqueByName("Light3TexFogReflect");

	Light0TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light0TexAlphaClipFogReflect");
	Light1TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light1TexAlphaClipFogReflect");
	Light2TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light2TexAlphaClipFogReflect");
	Light3TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light3TexAlphaClipFogReflect");

	ViewProj                = mFX->GetVariableByName("gViewProj")->AsMatrix();
	WorldViewProj           = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World                   = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose       = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform            = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	WaveDispTexTransform0   = mFX->GetVariableByName("gWaveDispTexTransform0")->AsMatrix();
	WaveDispTexTransform1   = mFX->GetVariableByName("gWaveDispTexTransform1")->AsMatrix();
	WaveNormalTexTransform0 = mFX->GetVariableByName("gWaveNormalTexTransform0")->AsMatrix();
	WaveNormalTexTransform1 = mFX->GetVariableByName("gWaveNormalTexTransform1")->AsMatrix();
	EyePosW                 = mFX->GetVariableByName("gEyePosW")->AsVector();
	FogColor                = mFX->GetVariableByName("gFogColor")->AsVector();
	FogStart                = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange                = mFX->GetVariableByName("gFogRange")->AsScalar();
	DirLights               = mFX->GetVariableByName("gDirLights");
	Mat                     = mFX->GetVariableByName("gMaterial");
	HeightScale0            = mFX->GetVariableByName("gHeightScale0")->AsScalar();
	HeightScale1            = mFX->GetVariableByName("gHeightScale1")->AsScalar();
	MaxTessDistance         = mFX->GetVariableByName("gMaxTessDistance")->AsScalar();
	MinTessDistance         = mFX->GetVariableByName("gMinTessDistance")->AsScalar();
	MinTessFactor           = mFX->GetVariableByName("gMinTessFactor")->AsScalar();
	MaxTessFactor           = mFX->GetVariableByName("gMaxTessFactor")->AsScalar();
	DiffuseMap              = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	CubeMap                 = mFX->GetVariableByName("gCubeMap")->AsShaderResource();
	NormalMap0              = mFX->GetVariableByName("gNormalMap0")->AsShaderResource();
	NormalMap1              = mFX->GetVariableByName("gNormalMap1")->AsShaderResource();
}

WavesEffect::~WavesEffect()
{
}
#pragma endregion


#pragma region BasicEffect
BasicEffect::BasicEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	Light1Tech    = mFX->GetTechniqueByName("Light1");
	Light2Tech    = mFX->GetTechniqueByName("Light2");
	Light3Tech    = mFX->GetTechniqueByName("Light3");

	Light0TexTech = mFX->GetTechniqueByName("Light0Tex");
	Light1TexTech = mFX->GetTechniqueByName("Light1Tex");
	Light1TexInstancedTech = mFX->GetTechniqueByName("Light1InstancedTex");
	Light2TexTech = mFX->GetTechniqueByName("Light2Tex");
	Light3TexTech = mFX->GetTechniqueByName("Light3Tex");

	Light0TexAlphaClipTech = mFX->GetTechniqueByName("Light0TexAlphaClip");
	Light1TexAlphaClipTech = mFX->GetTechniqueByName("Light1TexAlphaClip");
	Light2TexAlphaClipTech = mFX->GetTechniqueByName("Light2TexAlphaClip");
	Light3TexAlphaClipTech = mFX->GetTechniqueByName("Light3TexAlphaClip");

	Light1FogTech    = mFX->GetTechniqueByName("Light1Fog");
	Light2FogTech    = mFX->GetTechniqueByName("Light2Fog");
	Light3FogTech    = mFX->GetTechniqueByName("Light3Fog");

	Light0TexFogTech = mFX->GetTechniqueByName("Light0TexFog");
	Light1TexFogTech = mFX->GetTechniqueByName("Light1TexFog");
	Light2TexFogTech = mFX->GetTechniqueByName("Light2TexFog");
	Light3TexFogTech = mFX->GetTechniqueByName("Light3TexFog");

	Light0TexAlphaClipFogTech = mFX->GetTechniqueByName("Light0TexAlphaClipFog");
	Light1TexAlphaClipFogTech = mFX->GetTechniqueByName("Light1TexAlphaClipFog");
	Light2TexAlphaClipFogTech = mFX->GetTechniqueByName("Light2TexAlphaClipFog");
	Light3TexAlphaClipFogTech = mFX->GetTechniqueByName("Light3TexAlphaClipFog");

	Light1ReflectTech    = mFX->GetTechniqueByName("Light1Reflect");
	Light2ReflectTech    = mFX->GetTechniqueByName("Light2Reflect");
	Light3ReflectTech    = mFX->GetTechniqueByName("Light3Reflect");

	Light0TexReflectTech = mFX->GetTechniqueByName("Light0TexReflect");
	Light1TexReflectTech = mFX->GetTechniqueByName("Light1TexReflect");
	Light2TexReflectTech = mFX->GetTechniqueByName("Light2TexReflect");
	Light3TexReflectTech = mFX->GetTechniqueByName("Light3TexReflect");

	Light0TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light0TexAlphaClipReflect");
	Light1TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light1TexAlphaClipReflect");
	Light2TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light2TexAlphaClipReflect");
	Light3TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light3TexAlphaClipReflect");

	Light1FogReflectTech    = mFX->GetTechniqueByName("Light1FogReflect");
	Light2FogReflectTech    = mFX->GetTechniqueByName("Light2FogReflect");
	Light3FogReflectTech    = mFX->GetTechniqueByName("Light3FogReflect");

	Light0TexFogReflectTech = mFX->GetTechniqueByName("Light0TexFogReflect");
	Light1TexFogReflectTech = mFX->GetTechniqueByName("Light1TexFogReflect");
	Light2TexFogReflectTech = mFX->GetTechniqueByName("Light2TexFogReflect");
	Light3TexFogReflectTech = mFX->GetTechniqueByName("Light3TexFogReflect");

	Light0TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light0TexAlphaClipFogReflect");
	Light1TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light1TexAlphaClipFogReflect");
	Light2TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light2TexAlphaClipFogReflect");
	Light3TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light3TexAlphaClipFogReflect");

	Light1TexAlphaClipAOTech = mFX->GetTechniqueByName("Light1TexAlphaClipAOMap");
	Light1TexAOTech = mFX->GetTechniqueByName("Light1TexAOMap");
	Light1TexAlphaClipSsao = mFX->GetTechniqueByName("Light1TexAlphaClipSsao");
	Light1TexSsao = mFX->GetTechniqueByName("Light1TexSsao");

	Light1TexAlphaClipAOSpecMapTech = mFX->GetTechniqueByName("Light1TexAlphaClipAOSpecMap");
	Light1TexAOSpecMapTech = mFX->GetTechniqueByName("Light1TexAOSpecMap");
	Light1TexAlphaClipSpecMapTech = mFX->GetTechniqueByName("Light1TexAlphaClipSpecMap");
	Light1TexSpecMapTech = mFX->GetTechniqueByName("Light1TexSpecMap");

	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	ViewProj          = mFX->GetVariableByName("gViewProj")->AsMatrix();
	WorldViewProjTex  = mFX->GetVariableByName("gWorldViewProjTex")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	ShadowTransform   = mFX->GetVariableByName("gShadowTransform")->AsMatrix();
	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
	FogColor          = mFX->GetVariableByName("gFogColor")->AsVector();
	FogStart          = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange          = mFX->GetVariableByName("gFogRange")->AsScalar();
	DirLights         = mFX->GetVariableByName("gDirLights");
	Mat               = mFX->GetVariableByName("gMaterial");
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	CubeMap           = mFX->GetVariableByName("gCubeMap")->AsShaderResource();
	ShadowMap         = mFX->GetVariableByName("gShadowMap")->AsShaderResource();
	SsaoMap           = mFX->GetVariableByName("gSsaoMap")->AsShaderResource();
	SpecularMap       = mFX->GetVariableByName("gSpecularMap")->AsShaderResource();
}

BasicEffect::~BasicEffect()
{
}
#pragma endregion

#pragma region NormalMapEffect
NormalMapEffect::NormalMapEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
  
	Light0PointLight0TexTech = mFX->GetTechniqueByName("Light0PointLight0Tex");
    Light0PointLight0TexSpecTech = mFX->GetTechniqueByName("Light0PointLight0TexSpec");
    Light0PointLight0TexAOTech = mFX->GetTechniqueByName("Light0PointLight0TexAO");
    Light0PointLight0TexAOSpecTech = mFX->GetTechniqueByName("Light0PointLight0TexAOSpec");
    Light0PointLight1TexTech = mFX->GetTechniqueByName("Light0PointLight1Tex");
    Light0PointLight1TexSpecTech = mFX->GetTechniqueByName("Light0PointLight1TexSpec");
    Light0PointLight1TexAOTech = mFX->GetTechniqueByName("Light0PointLight1TexAO");
    Light0PointLight1TexAOSpecTech = mFX->GetTechniqueByName("Light0PointLight1TexAOSpec");
    Light0PointLight2TexTech = mFX->GetTechniqueByName("Light0PointLight2Tex");
    Light0PointLight2TexSpecTech = mFX->GetTechniqueByName("Light0PointLight2TexSpec");
    Light0PointLight2TexAOTech = mFX->GetTechniqueByName("Light0PointLight2TexAO");
    Light0PointLight2TexAOSpecTech = mFX->GetTechniqueByName("Light0PointLight2TexAOSpec");
    Light0PointLight3TexTech = mFX->GetTechniqueByName("Light0PointLight3Tex");
    Light0PointLight3TexSpecTech = mFX->GetTechniqueByName("Light0PointLight3TexSpec");
    Light0PointLight3TexAOTech = mFX->GetTechniqueByName("Light0PointLight3TexAO");
    Light0PointLight3TexAOSpecTech = mFX->GetTechniqueByName("Light0PointLight3TexAOSpec");
    Light1PointLight0TexTech = mFX->GetTechniqueByName("Light1PointLight0Tex");
    Light1PointLight0TexSpecTech = mFX->GetTechniqueByName("Light1PointLight0TexSpec");
    Light1PointLight0TexAOTech = mFX->GetTechniqueByName("Light1PointLight0TexAO");
    Light1PointLight0TexAOSpecTech = mFX->GetTechniqueByName("Light1PointLight0TexAOSpec");
    Light1PointLight1TexTech = mFX->GetTechniqueByName("Light1PointLight1Tex");
    Light1PointLight1TexSpecTech = mFX->GetTechniqueByName("Light1PointLight1TexSpec");
    Light1PointLight1TexAOTech = mFX->GetTechniqueByName("Light1PointLight1TexAO");
    Light1PointLight1TexAOSpecTech = mFX->GetTechniqueByName("Light1PointLight1TexAOSpec");
    Light1PointLight2TexTech = mFX->GetTechniqueByName("Light1PointLight2Tex");
    Light1PointLight2TexSpecTech = mFX->GetTechniqueByName("Light1PointLight2TexSpec");
    Light1PointLight2TexAOTech = mFX->GetTechniqueByName("Light1PointLight2TexAO");
    Light1PointLight2TexAOSpecTech = mFX->GetTechniqueByName("Light1PointLight2TexAOSpec");
    Light1PointLight3TexTech = mFX->GetTechniqueByName("Light1PointLight3Tex");
    Light1PointLight3TexSpecTech = mFX->GetTechniqueByName("Light1PointLight3TexSpec");
    Light1PointLight3TexAOTech = mFX->GetTechniqueByName("Light1PointLight3TexAO");
    Light1PointLight3TexAOSpecTech = mFX->GetTechniqueByName("Light1PointLight3TexAOSpec");
    Light2PointLight0TexTech = mFX->GetTechniqueByName("Light2PointLight0Tex");
    Light2PointLight0TexSpecTech = mFX->GetTechniqueByName("Light2PointLight0TexSpec");
    Light2PointLight0TexAOTech = mFX->GetTechniqueByName("Light2PointLight0TexAO");
    Light2PointLight0TexAOSpecTech = mFX->GetTechniqueByName("Light2PointLight0TexAOSpec");
    Light2PointLight1TexTech = mFX->GetTechniqueByName("Light2PointLight1Tex");
    Light2PointLight1TexSpecTech = mFX->GetTechniqueByName("Light2PointLight1TexSpec");
    Light2PointLight1TexAOTech = mFX->GetTechniqueByName("Light2PointLight1TexAO");
    Light2PointLight1TexAOSpecTech = mFX->GetTechniqueByName("Light2PointLight1TexAOSpec");
    Light2PointLight2TexTech = mFX->GetTechniqueByName("Light2PointLight2Tex");
    Light2PointLight2TexSpecTech = mFX->GetTechniqueByName("Light2PointLight2TexSpec");
    Light2PointLight2TexAOTech = mFX->GetTechniqueByName("Light2PointLight2TexAO");
    Light2PointLight2TexAOSpecTech = mFX->GetTechniqueByName("Light2PointLight2TexAOSpec");
    Light2PointLight3TexTech = mFX->GetTechniqueByName("Light2PointLight3Tex");
    Light2PointLight3TexSpecTech = mFX->GetTechniqueByName("Light2PointLight3TexSpec");
    Light2PointLight3TexAOTech = mFX->GetTechniqueByName("Light2PointLight3TexAO");
    Light2PointLight3TexAOSpecTech = mFX->GetTechniqueByName("Light2PointLight3TexAOSpec");
    Light3PointLight0TexTech = mFX->GetTechniqueByName("Light3PointLight0Tex");
    Light3PointLight0TexSpecTech = mFX->GetTechniqueByName("Light3PointLight0TexSpec");
    Light3PointLight0TexAOTech = mFX->GetTechniqueByName("Light3PointLight0TexAO");
    Light3PointLight0TexAOSpecTech = mFX->GetTechniqueByName("Light3PointLight0TexAOSpec");
    Light3PointLight1TexTech = mFX->GetTechniqueByName("Light3PointLight1Tex");
    Light3PointLight1TexSpecTech = mFX->GetTechniqueByName("Light3PointLight1TexSpec");
    Light3PointLight1TexAOTech = mFX->GetTechniqueByName("Light3PointLight1TexAO");
    Light3PointLight1TexAOSpecTech = mFX->GetTechniqueByName("Light3PointLight1TexAOSpec");
    Light3PointLight2TexTech = mFX->GetTechniqueByName("Light3PointLight2Tex");
    Light3PointLight2TexSpecTech = mFX->GetTechniqueByName("Light3PointLight2TexSpec");
    Light3PointLight2TexAOTech = mFX->GetTechniqueByName("Light3PointLight2TexAO");
    Light3PointLight2TexAOSpecTech = mFX->GetTechniqueByName("Light3PointLight2TexAOSpec");
    Light3PointLight3TexTech = mFX->GetTechniqueByName("Light3PointLight3Tex");
    Light3PointLight3TexSpecTech = mFX->GetTechniqueByName("Light3PointLight3TexSpec");
    Light3PointLight3TexAOTech = mFX->GetTechniqueByName("Light3PointLight3TexAO");
    Light3PointLight3TexAOSpecTech = mFX->GetTechniqueByName("Light3PointLight3TexAOSpec");

    Light0PointLight0InstancedTech = mFX->GetTechniqueByName("Light0PointLight0Instanced");
    Light1TexSkinnedTech = mFX->GetTechniqueByName("Light1TexSkinned");

	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	ViewProj          = mFX->GetVariableByName("gViewProj")->AsMatrix();
	BoneTransforms    = mFX->GetVariableByName("gBoneTransforms")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	ShadowTransform   = mFX->GetVariableByName("gShadowTransform")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
	FogColor          = mFX->GetVariableByName("gFogColor")->AsVector();
	FogStart          = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange          = mFX->GetVariableByName("gFogRange")->AsScalar();
	DirLights         = mFX->GetVariableByName("gDirLights");
	PointLights       = mFX->GetVariableByName("gPointLights");
	Mat               = mFX->GetVariableByName("gMaterial");
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	CubeMap           = mFX->GetVariableByName("gCubeMap")->AsShaderResource();
	NormalMap         = mFX->GetVariableByName("gNormalMap")->AsShaderResource();
	AOMap             = mFX->GetVariableByName("gAOMap")->AsShaderResource();
	ShadowMap         = mFX->GetVariableByName("gShadowMap")->AsShaderResource();
	SpecularMap       = mFX->GetVariableByName("gSpecularMap")->AsShaderResource();
	NumLights    = mFX->GetVariableByName("NumLights");


}

NormalMapEffect::~NormalMapEffect()
{
}
#pragma endregion

#pragma region BuildShadowMapEffect
BuildShadowMapEffect::BuildShadowMapEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	BuildShadowMapTech           = mFX->GetTechniqueByName("BuildShadowMapTech");
	BuildShadowMapAlphaClipTech  = mFX->GetTechniqueByName("BuildShadowMapAlphaClipTech");

	TessBuildShadowMapTech           = mFX->GetTechniqueByName("TessBuildShadowMapTech");
	TessBuildShadowMapAlphaClipTech  = mFX->GetTechniqueByName("TessBuildShadowMapAlphaClipTech");
	
	ViewProj          = mFX->GetVariableByName("gViewProj")->AsMatrix();
	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
	HeightScale       = mFX->GetVariableByName("gHeightScale")->AsScalar();
	MaxTessDistance   = mFX->GetVariableByName("gMaxTessDistance")->AsScalar();
	MinTessDistance   = mFX->GetVariableByName("gMinTessDistance")->AsScalar();
	MinTessFactor     = mFX->GetVariableByName("gMinTessFactor")->AsScalar();
	MaxTessFactor     = mFX->GetVariableByName("gMaxTessFactor")->AsScalar();
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	NormalMap         = mFX->GetVariableByName("gNormalMap")->AsShaderResource();
}

BuildShadowMapEffect::~BuildShadowMapEffect()
{
}
#pragma endregion

#pragma region BuildShadowMapInstancedEffect
BuildShadowMapInstancedEffect::BuildShadowMapInstancedEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	BuildShadowMapTech           = mFX->GetTechniqueByName("BuildShadowMapTech");
	BuildShadowMapAlphaClipTech  = mFX->GetTechniqueByName("BuildShadowMapAlphaClipTech");

	TessBuildShadowMapTech           = mFX->GetTechniqueByName("TessBuildShadowMapTech");
	TessBuildShadowMapAlphaClipTech  = mFX->GetTechniqueByName("TessBuildShadowMapAlphaClipTech");
	
	ViewProj          = mFX->GetVariableByName("gViewProj")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
	HeightScale       = mFX->GetVariableByName("gHeightScale")->AsScalar();
	MaxTessDistance   = mFX->GetVariableByName("gMaxTessDistance")->AsScalar();
	MinTessDistance   = mFX->GetVariableByName("gMinTessDistance")->AsScalar();
	MinTessFactor     = mFX->GetVariableByName("gMinTessFactor")->AsScalar();
	MaxTessFactor     = mFX->GetVariableByName("gMaxTessFactor")->AsScalar();
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	NormalMap         = mFX->GetVariableByName("gNormalMap")->AsShaderResource();
}

BuildShadowMapInstancedEffect::~BuildShadowMapInstancedEffect()
{
}
#pragma endregion

#pragma region SkyEffect
SkyEffect::SkyEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	SkyTech       = mFX->GetTechniqueByName("SkyTech");
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	CubeMap       = mFX->GetVariableByName("gCubeMap")->AsShaderResource();
}

SkyEffect::~SkyEffect()
{
}
#pragma endregion

#if defined(DEBUG) || (_DEBUG)
#pragma region DebugTexEffect
DebugTexEffect::DebugTexEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	ViewArgbTech  = mFX->GetTechniqueByName("ViewArgbTech");
	ViewRedTech   = mFX->GetTechniqueByName("ViewRedTech");
	ViewGreenTech = mFX->GetTechniqueByName("ViewGreenTech");
	ViewBlueTech  = mFX->GetTechniqueByName("ViewBlueTech");
	ViewAlphaTech = mFX->GetTechniqueByName("ViewAlphaTech");

	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	Texture       = mFX->GetVariableByName("gTexture")->AsShaderResource();
}

DebugTexEffect::~DebugTexEffect()
{

}


#pragma endregion
#endif

#ifdef _USE_DEFERRED_SHADING_

#pragma region DeferredLighting

DeferredLightingEffect::DeferredLightingEffect(ID3D11Device* device, const std::wstring& file) 
	: Effect(device, file)
{
	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();

	DirLight          = mFX->GetVariableByName("gLight");
	Mat               = mFX->GetVariableByName("gMaterial");

	ColorMap          = mFX->GetVariableByName("gColorMap")->AsShaderResource();
	NormalMap         = mFX->GetVariableByName("gNormalsMap")->AsShaderResource();
	PositionMap       = mFX->GetVariableByName("gPositionMap")->AsShaderResource();

	DeferredLightingTech = mFX->GetTechniqueByName("DeferredLighting");
}

DeferredLightingEffect::~DeferredLightingEffect()
{
}


#pragma endregion



#pragma region DeferredRenderer

DeferredRendererEffect::DeferredRendererEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	ShadowTransform   = mFX->GetVariableByName("gShadowTransform")->AsMatrix();

	Mat               = mFX->GetVariableByName("gMaterial");

	BuildGBuffers     = mFX->GetTechniqueByName("BuildGBuffers");

	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	NormalMap        = mFX->GetVariableByName("gNormalMap")->AsShaderResource();
	ShadowMap        = mFX->GetVariableByName("gShadowMap")->AsShaderResource();
}

#pragma endregion

#endif

#pragma region DisplacementMapEffect
DisplacementMapEffect::DisplacementMapEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	Light1Tech    = mFX->GetTechniqueByName("Light1");
	Light2Tech    = mFX->GetTechniqueByName("Light2");
	Light3Tech    = mFX->GetTechniqueByName("Light3");

	Light0TexTech = mFX->GetTechniqueByName("Light0Tex");
	Light1TexTech = mFX->GetTechniqueByName("Light1Tex");
	Light2TexTech = mFX->GetTechniqueByName("Light2Tex");
	Light3TexTech = mFX->GetTechniqueByName("Light3Tex");

	Light0TexAlphaClipTech = mFX->GetTechniqueByName("Light0TexAlphaClip");
	Light1TexAlphaClipTech = mFX->GetTechniqueByName("Light1TexAlphaClip");
	Light2TexAlphaClipTech = mFX->GetTechniqueByName("Light2TexAlphaClip");
	Light3TexAlphaClipTech = mFX->GetTechniqueByName("Light3TexAlphaClip");

	Light1FogTech    = mFX->GetTechniqueByName("Light1Fog");
	Light2FogTech    = mFX->GetTechniqueByName("Light2Fog");
	Light3FogTech    = mFX->GetTechniqueByName("Light3Fog");

	Light0TexFogTech = mFX->GetTechniqueByName("Light0TexFog");
	Light1TexFogTech = mFX->GetTechniqueByName("Light1TexFog");
	Light2TexFogTech = mFX->GetTechniqueByName("Light2TexFog");
	Light3TexFogTech = mFX->GetTechniqueByName("Light3TexFog");

	Light0TexAlphaClipFogTech = mFX->GetTechniqueByName("Light0TexAlphaClipFog");
	Light1TexAlphaClipFogTech = mFX->GetTechniqueByName("Light1TexAlphaClipFog");
	Light2TexAlphaClipFogTech = mFX->GetTechniqueByName("Light2TexAlphaClipFog");
	Light3TexAlphaClipFogTech = mFX->GetTechniqueByName("Light3TexAlphaClipFog");

	Light1ReflectTech    = mFX->GetTechniqueByName("Light1Reflect");
	Light2ReflectTech    = mFX->GetTechniqueByName("Light2Reflect");
	Light3ReflectTech    = mFX->GetTechniqueByName("Light3Reflect");

	Light0TexReflectTech = mFX->GetTechniqueByName("Light0TexReflect");
	Light1TexReflectTech = mFX->GetTechniqueByName("Light1TexReflect");
	Light2TexReflectTech = mFX->GetTechniqueByName("Light2TexReflect");
	Light3TexReflectTech = mFX->GetTechniqueByName("Light3TexReflect");

	Light0TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light0TexAlphaClipReflect");
	Light1TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light1TexAlphaClipReflect");
	Light2TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light2TexAlphaClipReflect");
	Light3TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light3TexAlphaClipReflect");

	Light1FogReflectTech    = mFX->GetTechniqueByName("Light1FogReflect");
	Light2FogReflectTech    = mFX->GetTechniqueByName("Light2FogReflect");
	Light3FogReflectTech    = mFX->GetTechniqueByName("Light3FogReflect");

	Light0TexFogReflectTech = mFX->GetTechniqueByName("Light0TexFogReflect");
	Light1TexFogReflectTech = mFX->GetTechniqueByName("Light1TexFogReflect");
	Light2TexFogReflectTech = mFX->GetTechniqueByName("Light2TexFogReflect");
	Light3TexFogReflectTech = mFX->GetTechniqueByName("Light3TexFogReflect");

	Light0TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light0TexAlphaClipFogReflect");
	Light1TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light1TexAlphaClipFogReflect");
	Light2TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light2TexAlphaClipFogReflect");
	Light3TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light3TexAlphaClipFogReflect");

	ViewProj          = mFX->GetVariableByName("gViewProj")->AsMatrix();
	WorldViewProj     = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World             = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform      = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW           = mFX->GetVariableByName("gEyePosW")->AsVector();
	FogColor          = mFX->GetVariableByName("gFogColor")->AsVector();
	FogStart          = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange          = mFX->GetVariableByName("gFogRange")->AsScalar();
	DirLights         = mFX->GetVariableByName("gDirLights");
	Mat               = mFX->GetVariableByName("gMaterial");
	HeightScale       = mFX->GetVariableByName("gHeightScale")->AsScalar();
	MaxTessDistance   = mFX->GetVariableByName("gMaxTessDistance")->AsScalar();
	MinTessDistance   = mFX->GetVariableByName("gMinTessDistance")->AsScalar();
	MinTessFactor     = mFX->GetVariableByName("gMinTessFactor")->AsScalar();
	MaxTessFactor     = mFX->GetVariableByName("gMaxTessFactor")->AsScalar();
	DiffuseMap        = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	CubeMap           = mFX->GetVariableByName("gCubeMap")->AsShaderResource();
	NormalMap         = mFX->GetVariableByName("gNormalMap")->AsShaderResource();
}

DisplacementMapEffect::~DisplacementMapEffect()
{
}
#pragma endregion



#ifdef USE_SSAO

#pragma region SsaoNormalDepthEffect
SsaoNormalDepthEffect::SsaoNormalDepthEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	NormalDepthTech          = mFX->GetTechniqueByName("NormalDepth");
	NormalDepthAlphaClipTech = mFX->GetTechniqueByName("NormalDepthAlphaClip");

	WorldView             = mFX->GetVariableByName("gWorldView")->AsMatrix();
	WorldInvTransposeView = mFX->GetVariableByName("gWorldInvTransposeView")->AsMatrix();
	WorldViewProj         = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	TexTransform          = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	DiffuseMap            = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
}

SsaoNormalDepthEffect::~SsaoNormalDepthEffect()
{
}
#pragma endregion

#pragma region SsaoEffect
SsaoEffect::SsaoEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	SsaoTech           = mFX->GetTechniqueByName("Ssao");

	ViewToTexSpace     = mFX->GetVariableByName("gViewToTexSpace")->AsMatrix();
	OffsetVectors      = mFX->GetVariableByName("gOffsetVectors")->AsVector();
	FrustumCorners     = mFX->GetVariableByName("gFrustumCorners")->AsVector();
	OcclusionRadius    = mFX->GetVariableByName("gOcclusionRadius")->AsScalar();
	OcclusionFadeStart = mFX->GetVariableByName("gOcclusionFadeStart")->AsScalar();
	OcclusionFadeEnd   = mFX->GetVariableByName("gOcclusionFadeEnd")->AsScalar();
	SurfaceEpsilon     = mFX->GetVariableByName("gSurfaceEpsilon")->AsScalar();

	NormalDepthMap     = mFX->GetVariableByName("gNormalDepthMap")->AsShaderResource();
	RandomVecMap       = mFX->GetVariableByName("gRandomVecMap")->AsShaderResource();
}

SsaoEffect::~SsaoEffect()
{
}
#pragma endregion

#pragma region SsaoBlurEffect
SsaoBlurEffect::SsaoBlurEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	HorzBlurTech    = mFX->GetTechniqueByName("HorzBlur");
	VertBlurTech    = mFX->GetTechniqueByName("VertBlur");

	TexelWidth      = mFX->GetVariableByName("gTexelWidth")->AsScalar();
	TexelHeight     = mFX->GetVariableByName("gTexelHeight")->AsScalar();

	NormalDepthMap  = mFX->GetVariableByName("gNormalDepthMap")->AsShaderResource();
	InputImage      = mFX->GetVariableByName("gInputImage")->AsShaderResource();
}

SsaoBlurEffect::~SsaoBlurEffect()
{
}
#pragma endregion

#endif

#pragma region SpriteEffect
SpriteEffect::SpriteEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	SpriteTech = mFX->GetTechniqueByName("SpriteTech");
	SpriteMap  = mFX->GetVariableByName("gSpriteTex")->AsShaderResource();
}

SpriteEffect::~SpriteEffect()
{
}
#pragma endregion

#pragma region Effects

BasicEffect*           Effects::BasicFX           = 0;
NormalMapEffect*       Effects::NormalMapFX       = 0;
BuildShadowMapEffect*  Effects::BuildShadowMapFX  = 0;
BuildShadowMapInstancedEffect* Effects::BuildShadowMapInstancedFX = 0;
SkyEffect*             Effects::SkyFX             = 0;
#if defined(DEBUG)||(_DEBUG)
DebugTexEffect*        Effects::DebugTexFX        = 0;
#endif
TerrainEffect* Effects::TerrainFX = 0;
DisplacementMapEffect* Effects::DisplacementMapFX = 0;
#if USE_SSAO
SsaoNormalDepthEffect* Effects::SsaoNormalDepthFX = 0;
SsaoEffect*            Effects::SsaoFX            = 0;
SsaoBlurEffect*        Effects::SsaoBlurFX        = 0;
#endif
SpriteEffect* Effects::SpriteFX = 0;
WavesEffect* Effects::WavesFX = 0;
#ifdef _USE_DEFERRED_SHADING_
DeferredRendererEffect* Effects::DeferredFX = 0;
DeferredLightingEffect* Effects::DeferredLightingFX = 0;
#endif

void Effects::InitAll(ID3D11Device* device)
{
	BasicFX           = new BasicEffect(device, L"Resources/Shaders/Basic.fxo");
	NormalMapFX       = new NormalMapEffect(device, L"Resources/Shaders/NormalMap.fxo");
	BuildShadowMapFX  = new BuildShadowMapEffect(device, L"Resources/Shaders/BuildShadowMap.fxo");
	BuildShadowMapInstancedFX = new BuildShadowMapInstancedEffect(device, L"Resources/Shaders/BuildShadowMapInstanced.fxo");
	SkyFX             = new SkyEffect(device, L"Resources/Shaders/Sky.fxo");
#if defined(DEBUG)||(_DEBUG)
	DebugTexFX        = new DebugTexEffect(device, L"Resources/Shaders/DebugTexture.fxo");
#endif
	TerrainFX = new TerrainEffect(device, L"Resources/Shaders/Terrain.fxo");
	DisplacementMapFX= new DisplacementMapEffect(device, L"Resources/Shaders/DisplacementMap.fxo");
#if USE_SSAO
	SsaoNormalDepthFX = new SsaoNormalDepthEffect(device, L"Resources/Shaders/SsaoNormalDepth.fxo");
	SsaoFX            = new SsaoEffect(device, L"Resources/Shaders/Ssao.fxo");
	SsaoBlurFX        = new SsaoBlurEffect(device, L"Resources/Shaders/SsaoBlur.fxo");
#endif
	WavesFX           = new WavesEffect(device, L"Resources/Shaders/Waves.fxo");
	SpriteFX = new SpriteEffect(device, L"Resources/Shaders/Sprite.fxo"); 
#ifdef _USE_DEFERRED_SHADING_
	DeferredFX = new DeferredRendererEffect(device, L"Resources/Shaders/DeferredRenderer.fxo");
	DeferredLightingFX = new DeferredLightingEffect(device, L"Resources/Shaders/DeferredLighting.fxo");
#endif
}

void Effects::DestroyAll()
{
	SafeDelete(BasicFX);
	SafeDelete(NormalMapFX);
	SafeDelete(BuildShadowMapFX);
	SafeDelete(BuildShadowMapInstancedFX);
	SafeDelete(SkyFX);
#if defined(DEBUG)||(_DEBUG)
	SafeDelete(DebugTexFX);
#endif
	SafeDelete(TerrainFX);
	SafeDelete(DisplacementMapFX);
#ifdef USE_SSAO
	SafeDelete(SsaoBlurFX);
	SafeDelete(SsaoFX);
	SafeDelete(SsaoNormalDepthFX);
#endif
	SafeDelete(WavesFX);
	SafeDelete(SpriteFX);
#ifdef _USE_DEFERRED_SHADING_
	SafeDelete(DeferredFX);
	SafeDelete(DeferredLightingFX);
#endif
}

#pragma endregion