//***************************************************************************************
// Effects.h by Frank Luna (C) 2011 All Rights Reserved.
//
// Defines lightweight effect wrappers to group an effect and its variables.
// Also defines a static Effects class from which we can access all of our effects.
//***************************************************************************************

#ifndef EFFECTS_H
#define EFFECTS_H

#pragma region Effect
class Effect
{
public:
	Effect(ID3D11Device* device, const std::wstring& filename);
	virtual ~Effect();

private:
	Effect(const Effect& rhs);
	Effect& operator=(const Effect& rhs);

protected:
	ID3DX11Effect* mFX;
};
#pragma endregion

#pragma region TerrainEffect
class TerrainEffect : public Effect
{
public:
	TerrainEffect(ID3D11Device* device, const std::wstring& filename);
	~TerrainEffect();

	void SetViewProj(CXMMATRIX M)                       { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetFogColor(const FXMVECTOR v)                 { FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetFogStart(float f)                           { FogStart->SetFloat(f); }
	void SetFogRange(float f)                           { FogRange->SetFloat(f); }
	void SetShadowTransform(CXMMATRIX M)                { ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetDirLights(const DirectionalLight* lights)   { DirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }

	void SetMinDist(float f)                            { MinDist->SetFloat(f); }
	void SetMaxDist(float f)                            { MaxDist->SetFloat(f); }
	void SetMinTess(float f)                            { MinTess->SetFloat(f); }
	void SetMaxTess(float f)                            { MaxTess->SetFloat(f); }
	void SetTexelCellSpaceU(float f)                    { TexelCellSpaceU->SetFloat(f); }
	void SetTexelCellSpaceV(float f)                    { TexelCellSpaceV->SetFloat(f); }
	void SetWorldCellSpace(float f)                     { WorldCellSpace->SetFloat(f); }
	void SetWorldFrustumPlanes(XMFLOAT4 planes[6])      { WorldFrustumPlanes->SetFloatVectorArray(reinterpret_cast<float*>(planes), 0, 6); }

	void SetLayerMapArray(ID3D11ShaderResourceView* tex)   { LayerMapArray->SetResource(tex); }
	void SetBlendMap(ID3D11ShaderResourceView* tex)        { BlendMap->SetResource(tex); }
	void SetHeightMap(ID3D11ShaderResourceView* tex)       { HeightMap->SetResource(tex); }
	void SetShadowMap(ID3D11ShaderResourceView* tex)    { ShadowMap->SetResource(tex); }

	ID3DX11EffectTechnique* Light1Tech;
	ID3DX11EffectTechnique* Light2Tech;
	ID3DX11EffectTechnique* Light3Tech;
	ID3DX11EffectTechnique* Light1FogTech;
	ID3DX11EffectTechnique* Light2FogTech;
	ID3DX11EffectTechnique* Light3FogTech;
	ID3DX11EffectTechnique* NormalDepthTech;
	ID3DX11EffectTechnique* TessBuildShadowMapTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectMatrixVariable* World;

	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;
	ID3DX11EffectScalarVariable* ScreenWidth;
	ID3DX11EffectScalarVariable* ScreenHeight;
	ID3DX11EffectScalarVariable* MinDist;
	ID3DX11EffectScalarVariable* MaxDist;
	ID3DX11EffectScalarVariable* MinTess;
	ID3DX11EffectScalarVariable* MaxTess;
	ID3DX11EffectScalarVariable* TexelCellSpaceU;
	ID3DX11EffectScalarVariable* TexelCellSpaceV;
	ID3DX11EffectScalarVariable* WorldCellSpace;
	ID3DX11EffectMatrixVariable* ShadowTransform;
	ID3DX11EffectVectorVariable* WorldFrustumPlanes;

	ID3DX11EffectShaderResourceVariable* LayerMapArray;
	ID3DX11EffectShaderResourceVariable* BlendMap;
	ID3DX11EffectShaderResourceVariable* HeightMap;
	ID3DX11EffectShaderResourceVariable* ShadowMap;
};
#pragma endregion


#pragma region WavesEffect
class WavesEffect : public Effect
{
public:
	WavesEffect(ID3D11Device* device, const std::wstring& filename);
	~WavesEffect();

	void SetViewProj(CXMMATRIX M)                       { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWaveDispTexTransform0(CXMMATRIX M)          { WaveDispTexTransform0->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWaveDispTexTransform1(CXMMATRIX M)          { WaveDispTexTransform1->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWaveNormalTexTransform0(CXMMATRIX M)        { WaveNormalTexTransform0->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWaveNormalTexTransform1(CXMMATRIX M)        { WaveNormalTexTransform1->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetFogColor(const FXMVECTOR v)                 { FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetFogStart(float f)                           { FogStart->SetFloat(f); }
	void SetFogRange(float f)                           { FogRange->SetFloat(f); }
	void SetDirLights(const DirectionalLight* lights)   { DirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetHeightScale0(float f)                       { HeightScale0->SetFloat(f); }
	void SetHeightScale1(float f)                       { HeightScale1->SetFloat(f); }
	void SetMaxTessDistance(float f)                    { MaxTessDistance->SetFloat(f); }
	void SetMinTessDistance(float f)                    { MinTessDistance->SetFloat(f); }
	void SetMinTessFactor(float f)                      { MinTessFactor->SetFloat(f); }
	void SetMaxTessFactor(float f)                      { MaxTessFactor->SetFloat(f); }

	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetCubeMap(ID3D11ShaderResourceView* tex)      { CubeMap->SetResource(tex); }
	void SetNormalMap0(ID3D11ShaderResourceView* tex)   { NormalMap0->SetResource(tex); }
	void SetNormalMap1(ID3D11ShaderResourceView* tex)   { NormalMap1->SetResource(tex); }

	ID3DX11EffectTechnique* Light1Tech;
	ID3DX11EffectTechnique* Light2Tech;
	ID3DX11EffectTechnique* Light3Tech;

	ID3DX11EffectTechnique* Light0TexTech;
	ID3DX11EffectTechnique* Light1TexTech;
	ID3DX11EffectTechnique* Light2TexTech;
	ID3DX11EffectTechnique* Light3TexTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipTech;

	ID3DX11EffectTechnique* Light1FogTech;
	ID3DX11EffectTechnique* Light2FogTech;
	ID3DX11EffectTechnique* Light3FogTech;

	ID3DX11EffectTechnique* Light0TexFogTech;
	ID3DX11EffectTechnique* Light1TexFogTech;
	ID3DX11EffectTechnique* Light2TexFogTech;
	ID3DX11EffectTechnique* Light3TexFogTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogTech;

	ID3DX11EffectTechnique* Light1ReflectTech;
	ID3DX11EffectTechnique* Light2ReflectTech;
	ID3DX11EffectTechnique* Light3ReflectTech;

	ID3DX11EffectTechnique* Light0TexReflectTech;
	ID3DX11EffectTechnique* Light1TexReflectTech;
	ID3DX11EffectTechnique* Light2TexReflectTech;
	ID3DX11EffectTechnique* Light3TexReflectTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipReflectTech;

	ID3DX11EffectTechnique* Light1FogReflectTech;
	ID3DX11EffectTechnique* Light2FogReflectTech;
	ID3DX11EffectTechnique* Light3FogReflectTech;

	ID3DX11EffectTechnique* Light0TexFogReflectTech;
	ID3DX11EffectTechnique* Light1TexFogReflectTech;
	ID3DX11EffectTechnique* Light2TexFogReflectTech;
	ID3DX11EffectTechnique* Light3TexFogReflectTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogReflectTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectMatrixVariable* WaveDispTexTransform0;
	ID3DX11EffectMatrixVariable* WaveDispTexTransform1;
	ID3DX11EffectMatrixVariable* WaveNormalTexTransform0;
	ID3DX11EffectMatrixVariable* WaveNormalTexTransform1;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;
	ID3DX11EffectScalarVariable* HeightScale0;
	ID3DX11EffectScalarVariable* HeightScale1;
	ID3DX11EffectScalarVariable* MaxTessDistance;
	ID3DX11EffectScalarVariable* MinTessDistance;
	ID3DX11EffectScalarVariable* MinTessFactor;
	ID3DX11EffectScalarVariable* MaxTessFactor;
 
	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* CubeMap;
	ID3DX11EffectShaderResourceVariable* NormalMap0;
	ID3DX11EffectShaderResourceVariable* NormalMap1;
};
#pragma endregion


#pragma region BasicEffect
class BasicEffect : public Effect
{
public:
	BasicEffect(ID3D11Device* device, const std::wstring& filename);
	~BasicEffect();

	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetViewProj(CXMMATRIX M)                       { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProjTex(CXMMATRIX M)               { WorldViewProjTex->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetShadowTransform(CXMMATRIX M)                { ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetFogColor(const FXMVECTOR v)                 { FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetFogStart(float f)                           { FogStart->SetFloat(f); }
	void SetFogRange(float f)                           { FogRange->SetFloat(f); }
	void SetDirLights(const DirectionalLight* lights)   { DirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetShadowMap(ID3D11ShaderResourceView* tex)    { ShadowMap->SetResource(tex); }
	void SetSsaoMap(ID3D11ShaderResourceView* tex)      { SsaoMap->SetResource(tex); }
	void SetCubeMap(ID3D11ShaderResourceView* tex)      { CubeMap->SetResource(tex); }
	void SetAmbientOcclusionMap(ID3D11ShaderResourceView* tex) { AOMap->SetResource(tex); }
	void SetSpecularMap(ID3D11ShaderResourceView* tex) { SpecularMap->SetResource(tex); }

	ID3DX11EffectTechnique* Light1Tech;
	ID3DX11EffectTechnique* Light2Tech;
	ID3DX11EffectTechnique* Light3Tech;

	ID3DX11EffectTechnique* Light0TexTech;
	ID3DX11EffectTechnique* Light1TexTech;
	ID3DX11EffectTechnique* Light1TexInstancedTech;
	ID3DX11EffectTechnique* Light2TexTech;
	ID3DX11EffectTechnique* Light3TexTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipTech;

	ID3DX11EffectTechnique* Light1FogTech;
	ID3DX11EffectTechnique* Light2FogTech;
	ID3DX11EffectTechnique* Light3FogTech;

	ID3DX11EffectTechnique* Light0TexFogTech;
	ID3DX11EffectTechnique* Light1TexFogTech;
	ID3DX11EffectTechnique* Light2TexFogTech;
	ID3DX11EffectTechnique* Light3TexFogTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogTech;

	ID3DX11EffectTechnique* Light1ReflectTech;
	ID3DX11EffectTechnique* Light2ReflectTech;
	ID3DX11EffectTechnique* Light3ReflectTech;

	ID3DX11EffectTechnique* Light0TexReflectTech;
	ID3DX11EffectTechnique* Light1TexReflectTech;
	ID3DX11EffectTechnique* Light2TexReflectTech;
	ID3DX11EffectTechnique* Light3TexReflectTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipReflectTech;

	ID3DX11EffectTechnique* Light1FogReflectTech;
	ID3DX11EffectTechnique* Light2FogReflectTech;
	ID3DX11EffectTechnique* Light3FogReflectTech;

	ID3DX11EffectTechnique* Light0TexFogReflectTech;
	ID3DX11EffectTechnique* Light1TexFogReflectTech;
	ID3DX11EffectTechnique* Light2TexFogReflectTech;
	ID3DX11EffectTechnique* Light3TexFogReflectTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipAOTech;
	ID3DX11EffectTechnique* Light1TexAOTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipSsao;
	ID3DX11EffectTechnique* Light1TexSsao;

	ID3DX11EffectTechnique* Light1TexAlphaClipAOSpecMapTech;
	ID3DX11EffectTechnique* Light1TexAOSpecMapTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipSpecMapTech;
	ID3DX11EffectTechnique* Light1TexSpecMapTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectMatrixVariable* WorldViewProjTex;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* ShadowTransform;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* ShadowMap;
	ID3DX11EffectShaderResourceVariable* SsaoMap;
	ID3DX11EffectShaderResourceVariable* AOMap;
	ID3DX11EffectShaderResourceVariable* CubeMap;
	ID3DX11EffectShaderResourceVariable* SpecularMap;
};
#pragma endregion

#ifdef USE_SSAO

#pragma region SsaoNormalDepthEffect
class SsaoNormalDepthEffect : public Effect
{
public:
	SsaoNormalDepthEffect(ID3D11Device* device, const std::wstring& filename);
	~SsaoNormalDepthEffect();

	void SetWorldView(CXMMATRIX M)                      { WorldView->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTransposeView(CXMMATRIX M)          { WorldInvTransposeView->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }

	ID3DX11EffectTechnique* NormalDepthTech;
	ID3DX11EffectTechnique* NormalDepthAlphaClipTech;

	ID3DX11EffectMatrixVariable* WorldView;
	ID3DX11EffectMatrixVariable* WorldInvTransposeView;
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* TexTransform;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
};
#pragma endregion

#pragma region SsaoEffect
class SsaoEffect : public Effect
{
public:
	SsaoEffect(ID3D11Device* device, const std::wstring& filename);
	~SsaoEffect();

	void SetViewToTexSpace(CXMMATRIX M)                    { ViewToTexSpace->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetOffsetVectors(const XMFLOAT4 v[14])            { OffsetVectors->SetFloatVectorArray(reinterpret_cast<const float*>(v), 0, 14); }
	void SetFrustumCorners(const XMFLOAT4 v[4])            { FrustumCorners->SetFloatVectorArray(reinterpret_cast<const float*>(v), 0, 4); }
	void SetOcclusionRadius(float f)                       { OcclusionRadius->SetFloat(f); }
	void SetOcclusionFadeStart(float f)                    { OcclusionFadeStart->SetFloat(f); }
	void SetOcclusionFadeEnd(float f)                      { OcclusionFadeEnd->SetFloat(f); }
	void SetSurfaceEpsilon(float f)                        { SurfaceEpsilon->SetFloat(f); }

	void SetNormalDepthMap(ID3D11ShaderResourceView* srv)  { NormalDepthMap->SetResource(srv); }
	void SetRandomVecMap(ID3D11ShaderResourceView* srv)    { RandomVecMap->SetResource(srv); }

	ID3DX11EffectTechnique* SsaoTech;

	ID3DX11EffectMatrixVariable* ViewToTexSpace;
	ID3DX11EffectVectorVariable* OffsetVectors;
	ID3DX11EffectVectorVariable* FrustumCorners;
	ID3DX11EffectScalarVariable* OcclusionRadius;
	ID3DX11EffectScalarVariable* OcclusionFadeStart;
	ID3DX11EffectScalarVariable* OcclusionFadeEnd;
	ID3DX11EffectScalarVariable* SurfaceEpsilon;

	ID3DX11EffectShaderResourceVariable* NormalDepthMap;
	ID3DX11EffectShaderResourceVariable* RandomVecMap;
};
#pragma endregion

#pragma region SsaoBlurEffect
class SsaoBlurEffect : public Effect
{
public:
	SsaoBlurEffect(ID3D11Device* device, const std::wstring& filename);
	~SsaoBlurEffect();

	void SetTexelWidth(float f)                            { TexelWidth->SetFloat(f); }
	void SetTexelHeight(float f)                           { TexelHeight->SetFloat(f); }

	void SetNormalDepthMap(ID3D11ShaderResourceView* srv)  { NormalDepthMap->SetResource(srv); }
	void SetInputImage(ID3D11ShaderResourceView* srv)      { InputImage->SetResource(srv); }

	ID3DX11EffectTechnique* HorzBlurTech;
	ID3DX11EffectTechnique* VertBlurTech;

	ID3DX11EffectScalarVariable* TexelWidth;
	ID3DX11EffectScalarVariable* TexelHeight;

	ID3DX11EffectShaderResourceVariable* NormalDepthMap;
	ID3DX11EffectShaderResourceVariable* InputImage;
};
#pragma endregion
#endif

#pragma region NormalMapEffect
class NormalMapEffect : public Effect
{
public:
	NormalMapEffect(ID3D11Device* device, const std::wstring& filename);
	~NormalMapEffect();

	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetViewProj(CXMMATRIX M)                       { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetBoneTransforms(const XMFLOAT4X4* M, int cnt){ BoneTransforms->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetShadowTransform(CXMMATRIX M)                { ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetFogColor(const FXMVECTOR v)                 { FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetFogStart(float f)                           { FogStart->SetFloat(f); }
	void SetFogRange(float f)                           { FogRange->SetFloat(f); }
	void SetDirLights(const DirectionalLight* lights)   { DirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
	void SetPointLights(const PointLight* lights)       { PointLights->SetRawValue(lights, 0, 3*sizeof(PointLight)); }
	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetCubeMap(ID3D11ShaderResourceView* tex)      { CubeMap->SetResource(tex); }
	void SetNormalMap(ID3D11ShaderResourceView* tex)    { NormalMap->SetResource(tex); }
	void SetSpecularMap(ID3D11ShaderResourceView* tex)  { SpecularMap->SetResource(tex); }
	void SetShadowMap(ID3D11ShaderResourceView* tex)    { ShadowMap->SetResource(tex); }
	void SetAmbientOcclusionMap(ID3D11ShaderResourceView* tex) { AOMap->SetResource(tex); }
	void SetNumLights(UINT& Num)                   { NumLights->SetRawValue(&Num, 0, sizeof(UINT)); }


	ID3DX11EffectTechnique* Light0PointLight0TexTech;
    ID3DX11EffectTechnique* Light0PointLight0TexSpecTech;
    ID3DX11EffectTechnique* Light0PointLight0TexAOTech;
    ID3DX11EffectTechnique* Light0PointLight0TexAOSpecTech;
    ID3DX11EffectTechnique* Light0PointLight1TexTech;
    ID3DX11EffectTechnique* Light0PointLight1TexSpecTech;
    ID3DX11EffectTechnique* Light0PointLight1TexAOTech;
    ID3DX11EffectTechnique* Light0PointLight1TexAOSpecTech;
    ID3DX11EffectTechnique* Light0PointLight2TexTech;
    ID3DX11EffectTechnique* Light0PointLight2TexSpecTech;
    ID3DX11EffectTechnique* Light0PointLight2TexAOTech;
    ID3DX11EffectTechnique* Light0PointLight2TexAOSpecTech;
    ID3DX11EffectTechnique* Light0PointLight3TexTech;
    ID3DX11EffectTechnique* Light0PointLight3TexSpecTech;
    ID3DX11EffectTechnique* Light0PointLight3TexAOTech;
    ID3DX11EffectTechnique* Light0PointLight3TexAOSpecTech;
    ID3DX11EffectTechnique* Light1PointLight0TexTech;
    ID3DX11EffectTechnique* Light1PointLight0TexSpecTech;
    ID3DX11EffectTechnique* Light1PointLight0TexAOTech;
    ID3DX11EffectTechnique* Light1PointLight0TexAOSpecTech;
    ID3DX11EffectTechnique* Light1PointLight1TexTech;
    ID3DX11EffectTechnique* Light1PointLight1TexSpecTech;
    ID3DX11EffectTechnique* Light1PointLight1TexAOTech;
    ID3DX11EffectTechnique* Light1PointLight1TexAOSpecTech;
    ID3DX11EffectTechnique* Light1PointLight2TexTech;
    ID3DX11EffectTechnique* Light1PointLight2TexSpecTech;
    ID3DX11EffectTechnique* Light1PointLight2TexAOTech;
    ID3DX11EffectTechnique* Light1PointLight2TexAOSpecTech;
    ID3DX11EffectTechnique* Light1PointLight3TexTech;
    ID3DX11EffectTechnique* Light1PointLight3TexSpecTech;
    ID3DX11EffectTechnique* Light1PointLight3TexAOTech;
    ID3DX11EffectTechnique* Light1PointLight3TexAOSpecTech;
    ID3DX11EffectTechnique* Light2PointLight0TexTech;
    ID3DX11EffectTechnique* Light2PointLight0TexSpecTech;
    ID3DX11EffectTechnique* Light2PointLight0TexAOTech;
    ID3DX11EffectTechnique* Light2PointLight0TexAOSpecTech;
    ID3DX11EffectTechnique* Light2PointLight1TexTech;
    ID3DX11EffectTechnique* Light2PointLight1TexSpecTech;
    ID3DX11EffectTechnique* Light2PointLight1TexAOTech;
    ID3DX11EffectTechnique* Light2PointLight1TexAOSpecTech;
    ID3DX11EffectTechnique* Light2PointLight2TexTech;
    ID3DX11EffectTechnique* Light2PointLight2TexSpecTech;
    ID3DX11EffectTechnique* Light2PointLight2TexAOTech;
    ID3DX11EffectTechnique* Light2PointLight2TexAOSpecTech;
    ID3DX11EffectTechnique* Light2PointLight3TexTech;
    ID3DX11EffectTechnique* Light2PointLight3TexSpecTech;
    ID3DX11EffectTechnique* Light2PointLight3TexAOTech;
    ID3DX11EffectTechnique* Light2PointLight3TexAOSpecTech;
    ID3DX11EffectTechnique* Light3PointLight0TexTech;
    ID3DX11EffectTechnique* Light3PointLight0TexSpecTech;
    ID3DX11EffectTechnique* Light3PointLight0TexAOTech;
    ID3DX11EffectTechnique* Light3PointLight0TexAOSpecTech;
    ID3DX11EffectTechnique* Light3PointLight1TexTech;
    ID3DX11EffectTechnique* Light3PointLight1TexSpecTech;
    ID3DX11EffectTechnique* Light3PointLight1TexAOTech;
    ID3DX11EffectTechnique* Light3PointLight1TexAOSpecTech;
    ID3DX11EffectTechnique* Light3PointLight2TexTech;
    ID3DX11EffectTechnique* Light3PointLight2TexSpecTech;
    ID3DX11EffectTechnique* Light3PointLight2TexAOTech;
    ID3DX11EffectTechnique* Light3PointLight2TexAOSpecTech;
    ID3DX11EffectTechnique* Light3PointLight3TexTech;
    ID3DX11EffectTechnique* Light3PointLight3TexSpecTech;
    ID3DX11EffectTechnique* Light3PointLight3TexAOTech;
    ID3DX11EffectTechnique* Light3PointLight3TexAOSpecTech;
	ID3DX11EffectTechnique* Light1TexSkinnedTech;


	ID3DX11EffectTechnique* Light0PointLight0InstancedTech;



    ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* ShadowTransform;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* PointLights;
	ID3DX11EffectMatrixVariable* BoneTransforms;
	ID3DX11EffectVariable* NumLights;
	ID3DX11EffectVariable* Mat;


	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* CubeMap;
	ID3DX11EffectShaderResourceVariable* NormalMap;
	ID3DX11EffectShaderResourceVariable* ShadowMap;
	ID3DX11EffectShaderResourceVariable* AOMap;
	ID3DX11EffectShaderResourceVariable* SpecularMap;
};
#pragma endregion

#ifdef _USE_DEFERRED_SHADING_

#pragma region DeferredRenderer

class DeferredRendererEffect : public Effect
{
public:
	DeferredRendererEffect(ID3D11Device* device, const std::wstring& filename);
	DeferredRendererEffect();

	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetShadowTransform(CXMMATRIX M)                { ShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }

	void SetNormalMap(ID3D11ShaderResourceView* tex)    { NormalMap->SetResource(tex); }
	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetShadowMap(ID3D11ShaderResourceView* tex)    { ShadowMap->SetResource(tex); }

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* ShadowTransform;
	ID3DX11EffectMatrixVariable* TexTransform;

	ID3DX11EffectVariable* Mat;
	
	ID3DX11EffectTechnique* BuildGBuffers;

	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* NormalMap;
	ID3DX11EffectShaderResourceVariable* ShadowMap;
};

#pragma endregion

#pragma region DeferredLighting

class DeferredLightingEffect : public Effect
{
public:
	DeferredLightingEffect(ID3D11Device* device, const std::wstring& filename);
	~DeferredLightingEffect();

	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }

	void SetColorMap(ID3D11ShaderResourceView* tex)   { ColorMap->SetResource(tex); }
	void SetNormalMap(ID3D11ShaderResourceView* tex)   { NormalMap->SetResource(tex); }
	void SetDirLight(const DirectionalLight light)   { DirLight->SetRawValue(&light, 0, sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetPositionMap(ID3D11ShaderResourceView* tex)   { PositionMap->SetResource(tex); }

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectVectorVariable* EyePosW;

	//Geometry buffers
	ID3DX11EffectShaderResourceVariable* ColorMap;
	ID3DX11EffectShaderResourceVariable* NormalMap;
	ID3DX11EffectShaderResourceVariable* PositionMap;

	ID3DX11EffectVariable* Mat;

	ID3DX11EffectVariable* DirLight;

	ID3DX11EffectTechnique* DeferredLightingTech;
};

#pragma endregion

#endif


#pragma region BuildShadowMapEffect
class BuildShadowMapEffect : public Effect
{
public:
	BuildShadowMapEffect(ID3D11Device* device, const std::wstring& filename);
	~BuildShadowMapEffect();

	void SetViewProj(CXMMATRIX M)                       { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	
	void SetHeightScale(float f)                        { HeightScale->SetFloat(f); }
	void SetMaxTessDistance(float f)                    { MaxTessDistance->SetFloat(f); }
	void SetMinTessDistance(float f)                    { MinTessDistance->SetFloat(f); }
	void SetMinTessFactor(float f)                      { MinTessFactor->SetFloat(f); }
	void SetMaxTessFactor(float f)                      { MaxTessFactor->SetFloat(f); }

	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetNormalMap(ID3D11ShaderResourceView* tex)    { NormalMap->SetResource(tex); }

	ID3DX11EffectTechnique* BuildShadowMapTech;
	ID3DX11EffectTechnique* BuildShadowMapAlphaClipTech;
	ID3DX11EffectTechnique* TessBuildShadowMapTech;
	ID3DX11EffectTechnique* TessBuildShadowMapAlphaClipTech;

	ID3DX11EffectTechnique* BuildShadowMapInstancedTech;
	ID3DX11EffectTechnique* BuildShadowMapAlphaClipInstancedTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectScalarVariable* HeightScale;
	ID3DX11EffectScalarVariable* MaxTessDistance;
	ID3DX11EffectScalarVariable* MinTessDistance;
	ID3DX11EffectScalarVariable* MinTessFactor;
	ID3DX11EffectScalarVariable* MaxTessFactor;


 
	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* NormalMap;
};
#pragma endregion


#pragma region DisplacementMapEffect
class DisplacementMapEffect : public Effect
{
public:
	DisplacementMapEffect(ID3D11Device* device, const std::wstring& filename);
	~DisplacementMapEffect();

	void SetViewProj(CXMMATRIX M)                       { ViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorld(CXMMATRIX M)                          { World->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetWorldInvTranspose(CXMMATRIX M)              { WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexTransform(CXMMATRIX M)                   { TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                  { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetFogColor(const FXMVECTOR v)                 { FogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetFogStart(float f)                           { FogStart->SetFloat(f); }
	void SetFogRange(float f)                           { FogRange->SetFloat(f); }
	void SetDirLights(const DirectionalLight* lights)   { DirLights->SetRawValue(lights, 0, 3*sizeof(DirectionalLight)); }
	void SetMaterial(const Material& mat)               { Mat->SetRawValue(&mat, 0, sizeof(Material)); }
	void SetHeightScale(float f)                        { HeightScale->SetFloat(f); }
	void SetMaxTessDistance(float f)                    { MaxTessDistance->SetFloat(f); }
	void SetMinTessDistance(float f)                    { MinTessDistance->SetFloat(f); }
	void SetMinTessFactor(float f)                      { MinTessFactor->SetFloat(f); }
	void SetMaxTessFactor(float f)                      { MaxTessFactor->SetFloat(f); }

	void SetDiffuseMap(ID3D11ShaderResourceView* tex)   { DiffuseMap->SetResource(tex); }
	void SetCubeMap(ID3D11ShaderResourceView* tex)      { CubeMap->SetResource(tex); }
	void SetNormalMap(ID3D11ShaderResourceView* tex)    { NormalMap->SetResource(tex); }

	ID3DX11EffectTechnique* Light1Tech;
	ID3DX11EffectTechnique* Light2Tech;
	ID3DX11EffectTechnique* Light3Tech;

	ID3DX11EffectTechnique* Light0TexTech;
	ID3DX11EffectTechnique* Light1TexTech;
	ID3DX11EffectTechnique* Light2TexTech;
	ID3DX11EffectTechnique* Light3TexTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipTech;

	ID3DX11EffectTechnique* Light1FogTech;
	ID3DX11EffectTechnique* Light2FogTech;
	ID3DX11EffectTechnique* Light3FogTech;

	ID3DX11EffectTechnique* Light0TexFogTech;
	ID3DX11EffectTechnique* Light1TexFogTech;
	ID3DX11EffectTechnique* Light2TexFogTech;
	ID3DX11EffectTechnique* Light3TexFogTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogTech;

	ID3DX11EffectTechnique* Light1ReflectTech;
	ID3DX11EffectTechnique* Light2ReflectTech;
	ID3DX11EffectTechnique* Light3ReflectTech;

	ID3DX11EffectTechnique* Light0TexReflectTech;
	ID3DX11EffectTechnique* Light1TexReflectTech;
	ID3DX11EffectTechnique* Light2TexReflectTech;
	ID3DX11EffectTechnique* Light3TexReflectTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipReflectTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipReflectTech;

	ID3DX11EffectTechnique* Light1FogReflectTech;
	ID3DX11EffectTechnique* Light2FogReflectTech;
	ID3DX11EffectTechnique* Light3FogReflectTech;

	ID3DX11EffectTechnique* Light0TexFogReflectTech;
	ID3DX11EffectTechnique* Light1TexFogReflectTech;
	ID3DX11EffectTechnique* Light2TexFogReflectTech;
	ID3DX11EffectTechnique* Light3TexFogReflectTech;

	ID3DX11EffectTechnique* Light0TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light1TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light2TexAlphaClipFogReflectTech;
	ID3DX11EffectTechnique* Light3TexAlphaClipFogReflectTech;

	ID3DX11EffectMatrixVariable* ViewProj;
	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectMatrixVariable* World;
	ID3DX11EffectMatrixVariable* WorldInvTranspose;
	ID3DX11EffectMatrixVariable* TexTransform;
	ID3DX11EffectVectorVariable* EyePosW;
	ID3DX11EffectVectorVariable* FogColor;
	ID3DX11EffectScalarVariable* FogStart;
	ID3DX11EffectScalarVariable* FogRange;
	ID3DX11EffectVariable* DirLights;
	ID3DX11EffectVariable* Mat;
	ID3DX11EffectScalarVariable* HeightScale;
	ID3DX11EffectScalarVariable* MaxTessDistance;
	ID3DX11EffectScalarVariable* MinTessDistance;
	ID3DX11EffectScalarVariable* MinTessFactor;
	ID3DX11EffectScalarVariable* MaxTessFactor;
 
	ID3DX11EffectShaderResourceVariable* DiffuseMap;
	ID3DX11EffectShaderResourceVariable* CubeMap;
	ID3DX11EffectShaderResourceVariable* NormalMap;
};
#pragma endregion

#pragma region SkyEffect
class SkyEffect : public Effect
{
public:
	SkyEffect(ID3D11Device* device, const std::wstring& filename);
	~SkyEffect();

	void SetWorldViewProj(CXMMATRIX M)                  { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetCubeMap(ID3D11ShaderResourceView* cubemap)  { CubeMap->SetResource(cubemap); }

	ID3DX11EffectTechnique* SkyTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;

	ID3DX11EffectShaderResourceVariable* CubeMap;
};
#pragma endregion

#if defined(DEBUG)||defined(_DEBUG)
#pragma region DebugTexEffect
class DebugTexEffect : public Effect
{
public:
	DebugTexEffect(ID3D11Device* device, const std::wstring& filename);
	~DebugTexEffect();

	void SetWorldViewProj(CXMMATRIX M)              { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetTexture(ID3D11ShaderResourceView* tex)  { Texture->SetResource(tex); }

	ID3DX11EffectTechnique* ViewArgbTech;
	ID3DX11EffectTechnique* ViewRedTech;
	ID3DX11EffectTechnique* ViewGreenTech;
	ID3DX11EffectTechnique* ViewBlueTech;
	ID3DX11EffectTechnique* ViewAlphaTech;

	ID3DX11EffectMatrixVariable* WorldViewProj;
	ID3DX11EffectShaderResourceVariable* Texture;
};
#pragma endregion
#endif

#pragma region SpriteEffect
class SpriteEffect : public Effect
{
public:
	SpriteEffect(ID3D11Device* device, const std::wstring& filename);
	~SpriteEffect();

	void SetSpriteMap(ID3D11ShaderResourceView* tex)   { SpriteMap->SetResource(tex); }

	ID3DX11EffectTechnique* SpriteTech;
	ID3DX11EffectShaderResourceVariable* SpriteMap;
};
#pragma endregion

#pragma region Effects
class Effects
{
public:
	static void InitAll(ID3D11Device* device);
	static void DestroyAll();

	static BasicEffect* BasicFX;
	static NormalMapEffect* NormalMapFX;
	static BuildShadowMapEffect* BuildShadowMapFX;
	static SkyEffect* SkyFX;
	static TerrainEffect* TerrainFX;
#ifdef _USE_DEFERRED_SHADING_
	static DeferredRendererEffect* DeferredFX;
	static DeferredLightingEffect* DeferredLightingFX;
#endif
#if defined(DEBUG)||(_DEBUG)
	static DebugTexEffect* DebugTexFX;
#endif
	static DisplacementMapEffect* DisplacementMapFX;
	static WavesEffect* WavesFX;

#ifdef USE_SSAO
	static SsaoNormalDepthEffect* SsaoNormalDepthFX;
	static SsaoEffect* SsaoFX;
	static SsaoBlurEffect* SsaoBlurFX;
#endif
	static SpriteEffect* SpriteFX;
};
#pragma endregion

#endif // EFFECTS_H