#ifndef _TERRAIN_DX10_H
#define _TERRAIN_DX10_H

/*
using namespace std;


class TerrainDX10
{
private:
	struct SubGrid
	{
		SubGrid()
		{
			VertexBuffer = 0;
			IndexBuffer = 0;

			Visible = false;
		}

		ID3D11Buffer* VertexBuffer;
		ID3D11Buffer* IndexBuffer;

		XNA::AxisAlignedBox box;


		static const int NUM_ROWS = 64;
		static const int NUM_COLS = 64;
		static const int NUM_TRIANGLES = (NUM_ROWS - 1) * (NUM_COLS - 1) * 2;
		static const int NUM_VERTEX = NUM_ROWS * NUM_COLS;
		static const int NUM_INDICES = NUM_TRIANGLES * 3;

		bool operator<(const SubGrid& rhs)const;


		bool Visible;
	};
public:
	struct InitInfo
	{
		wstring HeightmapFileName,
			LayerMapFileName0,
			LayerMapFileName1,
			LayerMapFileName2,
			LayerMapFileName3,
			LayerMapFileName4,
			BlendMapFileName;

		float HeightScale;
		float HeightOffset;
		UINT NumRows;
		UINT NumCols;
		float CellSpacing;

	};
public:
	TerrainDX10();
	~TerrainDX10();

	void Init(const InitInfo& info);

	float width()const;
	float depth()const;
	UINT GetNumVertices()const;
	UINT Triangles()const;

	float getHeight(float x, float z)const;
	void draw(CXMMATRIX world, Camera& cam);
	void DrawShadowMap(CXMMATRIX world, Camera& cam);
	void TranslateLight(FLOAT X, FLOAT Y, FLOAT Z);
private:
	void BuildFX();
	void loadHeightMap();
	void Smooth();
	bool inBounds(UINT i, UINT j);
	float average(UINT i, UINT j);
	void ComputeSubGridIndices(std::vector<USHORT>& IndicesOut);
	void BuildGeometry();
	void RenderSubGrid(SubGrid* grid);

    void SetWorldInvTranspose(CXMMATRIX M)              { mfxWorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
	void SetEyePosW(const XMFLOAT3& v)                  { mfxEyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
	void SetFogColor(const FXMVECTOR v)                 { mfxFogColor->SetFloatVector(reinterpret_cast<const float*>(&v)); }
	void SetFogStart(float f)                           { mfxFogStart->SetFloat(f); }
	void SetFogRange(float f)                           { mfxFogRange->SetFloat(f); }
	void SetShadowMap(ID3D11ShaderResourceView* tex)    { mfxShadowMap->SetResource(tex); }
	void SetShadowTransform(CXMMATRIX M)                { mfxShadowTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
private:
	InitInfo mInfo;
	UINT mNumVertices;
	UINT mNumFaces;

	DirectionalLight mLights[3];
	Material mMaterial;


	vector<float> mHeightmap;
	vector<SubGrid> mSubGrid;

	ID3DX11Effect* mFX;

	ID3D11ShaderResourceView* mLayer0;
	ID3D11ShaderResourceView* mLayer1;
	ID3D11ShaderResourceView* mLayer2;
	ID3D11ShaderResourceView* mLayer3;
	ID3D11ShaderResourceView* mLayer4;
	ID3D11ShaderResourceView* mBlendMap;

	ID3DX11EffectTechnique* mTech;
	ID3DX11EffectVariable* mfxLights;
	ID3DX11EffectVariable* mfxMaterial;
	ID3DX11EffectMatrixVariable* mfxWVPVar;
	ID3DX11EffectMatrixVariable* mfxWorldVar;
	ID3DX11EffectShaderResourceVariable* mfxLayer0Var;
	ID3DX11EffectShaderResourceVariable* mfxLayer1Var;
	ID3DX11EffectShaderResourceVariable* mfxLayer2Var;
	ID3DX11EffectShaderResourceVariable* mfxLayer3Var;
	ID3DX11EffectShaderResourceVariable* mfxLayer4Var;
	ID3DX11EffectShaderResourceVariable* mfxBlendMapVar;

	ID3DX11EffectVectorVariable* mfxEyePosW;
	ID3DX11EffectVectorVariable* mfxFogColor;
	ID3DX11EffectScalarVariable* mfxFogStart;
	ID3DX11EffectScalarVariable* mfxFogRange;
	ID3DX11EffectMatrixVariable* mfxWorldInvTranspose;
	ID3DX11EffectMatrixVariable* mfxShadowTransform;
	ID3DX11EffectShaderResourceVariable* mfxShadowMap;

}; */

#endif