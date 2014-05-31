#ifndef _TREE_H_
#define _TREE_H_

/*
class Tree
{
public:
	struct InitInfo
	{
		bool Use32BitIndices;
		USHORT NumInstances;
		
		std::string ModelPath;
		std::string far_ModelPath; 
	};

	struct TreeData
	{
		XMFLOAT3 pos;
		bool IsFar;
		bool Visible;
	};

private:
	XNA::AxisAlignedBox box;
	Model* mModel;
	Model* far_mModel;
	InitInfo mInfo;

	MeshData mModel_MeshData;
	MeshData far_mModel_MeshData;
	
	USHORT NumVisibleObjects;
	USHORT farModel_NumVisibleObjects;

	std::vector<TreeData> treeData;

	static const unsigned short MAX_TREES = 100;
	static const unsigned int MAX_DISTANCE = 140 * 140;
private:
	void AddtreeWorld(CXMMATRIX World);
	void AddtreeWorldToFarModel(CXMMATRIX World);

	void DrawNearModel(CXMMATRIX W, CXMMATRIX ShadowTransform, CXMMATRIX ViewProj, CXMMATRIX WorldInvTranspose);
	void DrawFarModel(CXMMATRIX W, CXMMATRIX ShadowTransform, CXMMATRIX ViewProj, CXMMATRIX WorldInvTranspose);

	void DrawNearModelToShadowMap(CXMMATRIX W, CXMMATRIX ViewProj, CXMMATRIX WorldInvTranspose);
	void DrawFarModelToShadowMap(CXMMATRIX W, CXMMATRIX ViewProj, CXMMATRIX WorldInvTranspose);
public:
	Tree();
	~Tree();

	void Init(InitInfo& info);


	void Update();
	void Draw(CXMMATRIX W);
	void DrawToShadowMap(CXMMATRIX W);
}; */

#endif