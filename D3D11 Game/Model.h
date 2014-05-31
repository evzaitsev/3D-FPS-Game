#ifndef _MODEL_H_
#define _MODEL_H_

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>



struct InstancedData
{
   XMFLOAT4X4 World;
};


struct Subset
{
	Subset()
	{
		VertexCount = 0;
		FaceCount = 0;

		VertexStart = 0;
		FaceStart = 0;
	}

	UINT ID;
	UINT VertexStart;
	UINT VertexCount;
	UINT FaceStart;
	UINT FaceCount;

};

class MeshGeometry
{
public:
	MeshGeometry();
	~MeshGeometry();

	template <typename VertexType>
	void SetVertices(const VertexType* vertices, UINT count);
	void SetIndices(const USHORT* indices, UINT count);
	void SetIndices(const UINT* indices, UINT count);
	void SetIndices(const DWORD* indices, UINT count);
	void SetSubsetTable(std::vector<Subset>& subsetTable);

	ID3D11Buffer** GetVertexBuffer();
	ID3D11Buffer** GetIndexBuffer();

	DXGI_FORMAT GetIndexBufferFormat();

	void SetFormat(DXGI_FORMAT format);

	void Draw(UINT subsetID);

private:
	MeshGeometry(const MeshGeometry& rhs);
	MeshGeometry& operator=(const MeshGeometry& rhs);

private:
	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;

	DXGI_FORMAT IndexBufferFormat;
	UINT VertexStride;

public:
	std::vector<Subset> SubsetTable;
};

struct MeshData
{
    ID3D11Buffer** VertexBuffer;
	ID3D11Buffer** IndexBuffer;
	ID3D11Buffer** InstancedBuffer;
	DirectionalLight* Lights[3];

	MeshGeometry* MeshGeometry;
	USHORT SubsetCount;
	UINT NumFaces;
	UINT NumVertices;

	std::vector<Subset*> subsetTable; 
	std::vector<Material*> Materials;
	std::vector<ID3D11ShaderResourceView**> DiffuseMapSRV;
};



struct ModelData
{ 
	ModelData()
	{
	   mNumVertices = 0;
	   mNumFaces = 0;
	   mSubsetCount = 0;
	}

	MeshGeometry Mesh;

	UINT mNumVertices;
	UINT mNumFaces;
	UINT mSubsetCount;

};


class Model
{
public:
	struct InitInfo
	{
		//pointer to texture manager so that a same texture
		//doesn't get loaded multiple times
		TextureMgr* Mgr;
		
		//optional
		//only set this if you want to manually set material
		Material Material; 

		//clip pixels which alpha value near to 0 ?
		bool AlphaClip;

		//num of dir lights
		UINT NumLights;

		//num of point lights
		UINT NumPointLights;

		//set this to true if you want to use materials loaded from model
		//else set this to false if you want to manually specify materials
		bool UseDefaultMaterial; 

		//enable or disable backface culling
		bool BackfaceCulling;

		//AlphaToCoverage On or off
		bool AlphaToCoverage;

		//Instanced objects frustum culling On or Off
		bool InstanceFrustumCulling;

		//If you set this to false, then you'll have to manually specify normals 
		//default is true
		bool UseNormalsFromModel;

		//Only works if UseNormalsFromModel is set to false
		//currently this only support specifying normals at global level i.e this normal will be used to specify all
		//vertex normals (uniform all over.)
		XMFLOAT3 Normal;

		//pass 1.0f if you do not wish to scale model
		XMFLOAT3 Scale;

		//effect technique by which model should be rendered. Only a temporary solution.
		//This would be removed soon so that you do not have to stress your mind to select a technique by yourself
		ID3DX11EffectTechnique* technique;
	};

	std::vector<InstancedData> mInstancedData;
	std::vector<XMFLOAT3> vertices;
	std::vector<UINT> Indices;
	std::vector<XNA::AxisAlignedBox> AABB;
public:
	Model(const std::string& filename, 
		InitInfo& info, bool Use32bitFormat, bool NormalMapped, bool HasAOMap, bool HasSpecularMap);
	~Model();

	void Render(CXMMATRIX World, CXMMATRIX ViewProj);
	void RenderInstanced(CXMMATRIX World, CXMMATRIX ViewProj);

	UINT GetNumTriangles();

#ifdef _USE_DEFERRED_SHADING
	void RenderDeferred(CXMMATRIX World, CXMMATRIX ViewProj);
#endif

	void RenderShadowMap(CXMMATRIX World, CXMMATRIX ViewProj);
	void RenderInstancedShadowMap(CXMMATRIX World, CXMMATRIX ViewProj);

#ifdef USE_SSAO
	void RenderNormalDepthMap(CXMMATRIX World, CXMMATRIX ViewProj);
#endif

	UINT GetNumDrawCalls();

	//used for instancing
	void UpdateInstanceData(XNA::AxisAlignedBox& box);
	void BuildInstanceData();

	void Update(CXMMATRIX World);

	void SetModelVisibleStatus(INT &status);

	void SetDirLight(DirectionalLight DirLight[3]);

	void EnableSSAO(bool Enable);

	MeshData ExtractMeshData();
private:
	Model(const Model& rhs);
	Model& operator=(const Model& rhs);

    void LoadBasicModel16(const std::string& filename, bool AOMap, bool SpecularMap);
	void LoadBasicModel32(const std::string& filename, bool AOMap, bool SpecularMap);
	void LoadNormalMapModel16(const std::string& filename, bool AOMap, bool SpecularMap);
	void LoadNormalMapModel32(const std::string& filename, bool AOMap, bool SpecularMap);

	void LoadTextures(aiMaterial* mat, bool& AOMap, bool NormalMaps, bool& SpecularMap);
	void LoadMaterials(aiMaterial* mat);

	void Render(CXMMATRIX World, CXMMATRIX ViewProj, bool AOMap, bool SpecularMap);
	void RenderNormalMapped(CXMMATRIX World, CXMMATRIX ViewProj, bool AOMap, bool SpecularMap); 

	void RenderInstanced(CXMMATRIX World, CXMMATRIX ViewProj, bool& AOMap, bool& SpecularMap);
	void RenderInstancedNormalMapped(CXMMATRIX World, CXMMATRIX ViewProj, bool& AOMap, bool& SpecularMap);

	void RenderSubset(UINT ID, UINT pass,
		CXMMATRIX World, CXMMATRIX WorldViewProj, 
		CXMMATRIX WorldInvTranspose, CXMMATRIX ShadowTransform, 
		CXMMATRIX TexTransform);

	ID3DX11EffectTechnique* GetTechnique();

private:
	DirectionalLight Lights[3];
	PointLight PointLights[3];

	//for instancing
	UINT mVisibleObjectCount;

	//keep track of number of draw calls
	UINT mDrawCalls;

	// 0 = fully outside frustum
	// 1 = intersect
	// 2 = fully inside frustum
	INT mIsModelVisible;

	bool mSSAO;

	bool mHasSpecularMaps;
	bool mHasNormalMaps;
	bool mHasOcclusionMaps;

	std::vector<ID3D11ShaderResourceView*> DiffuseMapSRV;
	std::vector<ID3D11ShaderResourceView*> NormalMapSRV;
	std::vector<ID3D11ShaderResourceView*> AOMapSRV;
	std::vector<ID3D11ShaderResourceView*> SpecMapSRV;


	std::vector<Material> Materials;
	std::vector<bool> ModelVisibleList;
	

	ModelData mModel;
	InitInfo mInfo;
 
	ID3D11Buffer* mInstancedBuffer;
};






#endif