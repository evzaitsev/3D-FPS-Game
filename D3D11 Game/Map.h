#ifndef _MAP_RENDERER_H_
#define _MAP_RENDERER_H_


struct ModelInstance
{
	ModelInstance()
	{
		Model = nullptr;

		ComputeSSAO = false;
		UseInstancing = false;
		Visible = true; 

		XMMATRIX world = XMMatrixIdentity();
		XMStoreFloat4x4(&World, world);
	}

	XMMATRIX GetWorldXM()const
	{
		return XMLoadFloat4x4(&World);
	}

	void StoreWorld(CXMMATRIX W)
	{
		XMStoreFloat4x4(&World, W);
	}

	void push_backWorld(CXMMATRIX W)
	{
		InstancedData data;
		XMStoreFloat4x4(&data.World, W);

		Model->mInstancedData.push_back(data);
	}

	void BuildInstanceData()
	{
		Model->BuildInstanceData();
	}

	void UpdateInstanceData()
	{
		Model->UpdateInstanceData(box);
	}
		
	void RenderInstanced(CXMMATRIX ViewProj)
	{
		Model->RenderInstanced(GetWorldXM(), ViewProj);
	}


	XNA::AxisAlignedBox box;
	XNA::Sphere sphere;

	XMFLOAT4X4 World;

	Model* Model;

	bool Visible;
	bool UseInstancing;
	bool ComputeSSAO;
};

struct TerrainTextures
{
	std::wstring HeightMapFilename;
	std::wstring LayerMapFilename0;
	std::wstring LayerMapFilename1;
	std::wstring LayerMapFilename2;
	std::wstring LayerMapFilename3;
	std::wstring LayerMapFilename4;
	std::wstring BlendMapFilename;
};

struct StaticMesh
{
	Model* mModel;
	XMFLOAT4X4 mWorld;
	XNA::AxisAlignedBox Box;
	XNA::Sphere Sphere;
};


class Map
{
private:
	DirectionalLight mDirLight;
	Sky* mSky;

	std::vector<ModelInstance> ModelInstances;
public:
	Map();
	~Map();

	void InitSky(std::wstring& cubeMapFileName, float SkySphereRadius);

	void Render();
	void RendertoShadowMap();

	void AddModel(ModelInstance& instance);

	void ComputeAABB();

	void Update(float& dt);
private:
	void DrawTerrain();
	void DrawTerrainToShadowMap();
	void DrawModels();
	void DrawModelsToShadowMap();
	void DrawInstancedModels();
	void DrawInstancedModelsToShadowMap();
	void UpdateInstancedModels();

	void DrawSky();
};



#endif