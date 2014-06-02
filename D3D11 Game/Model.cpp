#include "stdafx.h"

Model::Model(
	const std::string& filename,
	InitInfo& info, 
	bool Use32bitFormat, 
	bool NormalMapped, 
	bool HasAOMap,
	bool HasSpecMap,
	bool FillIndices)
{

	Lights[0].Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	Lights[0].Diffuse  = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	Lights[0].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	Lights[0].Direction = XMFLOAT3(-1.0f, -1.0f, 0.0f);

	XMFLOAT3 pos[3];

	for (int i = 0; i < 3; ++i)
	{
		pos[i].x = float(rand() % 100);
		pos[i].y = float(rand() % 100);
		pos[i].z = float(rand() % 100);
	}

	PointLights[0].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	PointLights[0].Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	PointLights[0].Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	PointLights[0].Range = 25.0f;
	PointLights[0].Att   = XMFLOAT3(0.0f, 0.1f, 0.0f);
	PointLights[0].Position = pos[0];

	PointLights[1].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	PointLights[1].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	PointLights[1].Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	PointLights[1].Range = 40.0f;
	PointLights[1].Att   = XMFLOAT3(0.0f, 0.1f, 0.0f);
	PointLights[1].Position = pos[1];

	PointLights[2].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	PointLights[2].Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	PointLights[2].Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	PointLights[2].Range = 60.0f;
	PointLights[2].Att   = XMFLOAT3(0.0f, 0.1f, 0.0f);
	PointLights[2].Position = pos[2];

	mSSAO = false;

	mInfo = info;

	mVisibleObjectCount = 0;
	mDrawCalls = 0;

	Indices = nullptr;
	mInstancedBuffer = nullptr;

	mHasNormalMaps = NormalMapped;
	mHasOcclusionMaps = HasAOMap;
	mHasSpecularMaps = HasSpecMap;


	if (Use32bitFormat)
	{
		if (NormalMapped)
			LoadNormalMapModel32(filename, HasAOMap, HasSpecMap, FillIndices);
		else 
			LoadBasicModel32(filename, HasAOMap, HasSpecMap, FillIndices);
	}
	else
	{
		if (NormalMapped)
			LoadNormalMapModel16(filename, HasAOMap, HasSpecMap, FillIndices);
		else
			LoadBasicModel16(filename, HasAOMap, HasSpecMap, FillIndices);
	}

}

Model::~Model()
{
	ReleaseCOM(mInstancedBuffer);

	SafeDelete(Indices);
}

void Model::UpdateInstanceData(XNA::AxisAlignedBox& box)
{
	mVisibleObjectCount = 0;

	if(mInfo.InstanceFrustumCulling)
	{
		D3D11_MAPPED_SUBRESOURCE mappedData; 
		pDeviceContext->Map(mInstancedBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

		InstancedData* dataView = reinterpret_cast<InstancedData*>(mappedData.pData);

		for(UINT i = 0; i < mInstancedData.size(); ++i)
		{
			XMMATRIX W = XMLoadFloat4x4(&mInstancedData[i].World);
	
			// Perform the box/frustum intersection test in local space.
		    XMVECTOR detView = XMMatrixDeterminant(d3d->m_Cam.View());
	        XMMATRIX invView = XMMatrixInverse(&detView, d3d->m_Cam.View());

	        XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

	        XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);
	 
	        XMVECTOR scale;
	        XMVECTOR rotQuat;
	        XMVECTOR translation;
	        
			XMMatrixDecompose(&scale, &rotQuat, &translation, toLocal);

	        XNA::Frustum localspaceFrustum;
			XNA::TransformFrustum(&localspaceFrustum, &d3d->m_Frustum, XMVectorGetX(scale), rotQuat, translation);

            if (XNA::IntersectAxisAlignedBoxFrustum(&box, &localspaceFrustum) != 0)
			{
				// Write the instance data to dynamic VB of the visible objects.
				dataView[mVisibleObjectCount++] = mInstancedData[i];
			}
		}

		pDeviceContext->Unmap(mInstancedBuffer, 0);
	}
	else // No culling enabled, draw all objects.
	{
		D3D11_MAPPED_SUBRESOURCE mappedData; 
		pDeviceContext->Map(mInstancedBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

		InstancedData* dataView = reinterpret_cast<InstancedData*>(mappedData.pData);

		for(UINT i = 0; i < mInstancedData.size(); ++i)
		{
			dataView[mVisibleObjectCount++] = mInstancedData[i];
		}

		pDeviceContext->Unmap(mInstancedBuffer, 0);
	}

}	

void Model::EnableSSAO(bool Enable)
{
	mSSAO = Enable;
}

MeshData Model::ExtractMeshData()
{
	//useful for rendering outside model class

	MeshData out;

	out.IndexBuffer = mModel.Mesh.GetIndexBuffer();
	out.VertexBuffer = mModel.Mesh.GetVertexBuffer();
	out.InstancedBuffer = &mInstancedBuffer;
	
	out.DiffuseMapSRV.resize(DiffuseMapSRV.size());
	out.subsetTable.resize(mModel.Mesh.SubsetTable.size());
	out.Materials.resize(Materials.size());

	for (USHORT i = 0; i < 3; ++i)
		out.Lights[i] = &Lights[i];

	for (size_t i = 0; i < DiffuseMapSRV.size(); ++i)
		out.DiffuseMapSRV[i] = &DiffuseMapSRV[i];

	for (size_t i = 0; i < mModel.Mesh.SubsetTable.size(); ++i)
		out.subsetTable[i] = &mModel.Mesh.SubsetTable[i];

	for (size_t i = 0; i < Materials.size(); ++i)
		out.Materials[i] = &Materials[i];

	out.MeshGeometry = &mModel.Mesh;
	out.SubsetCount = mModel.Mesh.SubsetTable.size();
	out.NumFaces = mModel.mNumFaces;
	out.NumVertices = mModel.mNumVertices;

	return out;
}

void Model::LoadMaterials(aiMaterial* Mat)
{
	Material tempMat;

    aiColor4D color(0.0f, 0.0f, 0.0f, 0.0f);

    tempMat.Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	tempMat.Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    tempMat.Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
    tempMat.Reflect = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
		
    Mat->Get(AI_MATKEY_COLOR_AMBIENT, color);
	
    tempMat.Ambient = XMFLOAT4(color.r, color.g, color.b, color.a);

    Mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);

    tempMat.Diffuse = XMFLOAT4(color.r, color.g, color.b, color.a);

    Mat->Get(AI_MATKEY_COLOR_SPECULAR, color);

    tempMat.Specular = XMFLOAT4(color.r, color.g, color.b, color.a);

    Mat->Get(AI_MATKEY_COLOR_REFLECTIVE, color);

    tempMat.Reflect = XMFLOAT4(color.r, color.g, color.b, color.a);

	if (tempMat.Ambient.x == 0 && tempMat.Ambient.y == 0 && tempMat.Ambient.z == 0 && tempMat.Ambient.w == 0)
		tempMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);

    if (tempMat.Diffuse.x == 0 && tempMat.Diffuse.y == 0 && tempMat.Diffuse.z == 0 && tempMat.Diffuse.w == 0)
		tempMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

    if (tempMat.Specular.x == 0 && tempMat.Specular.y == 0 && tempMat.Specular.z == 0 && tempMat.Specular.w == 0)
		tempMat.Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);

   Materials.push_back(tempMat);
}

void Model::LoadTextures(aiMaterial* Mat, bool& AOMap, bool NormalMap, bool& SpecularMap)
{

	aiString path;
		
	if (Mat->GetTextureCount(aiTextureType_DIFFUSE) > 0 && Mat->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
	{

		std::string file = path.C_Str();
		std::string NoExtension = removeExtension(file);
	    std::string newName = NoExtension + ".dds";
	    newName = "Resources\\Textures\\" + newName;

#if defined (DEBUG) || (_DEBUG)
		OutputDebugStringA(newName.c_str());
		OutputDebugStringA("\n");

		if (!FileExists(newName))
			ShowError(newName.c_str());
#endif

		ID3D11ShaderResourceView* srv = mInfo.Mgr->CreateTexture(newName);

	    DiffuseMapSRV.push_back(srv);

	}
	else
	{
		ID3D11ShaderResourceView* srv = nullptr;
		DiffuseMapSRV.push_back(srv);

		if (NormalMap)
		{
			NormalMapSRV.push_back(srv);
		}
	
		if (AOMap)
		{
			AOMapSRV.push_back(srv);
		}

		if (SpecularMap)
		{
			SpecMapSRV.push_back(srv);
		}

		return;
	}

	if (!AOMap && !NormalMap && !SpecularMap)
		return;

	std::string file = path.C_Str();
	std::string NoExtension = removeExtension(file);

	std::string NormalMapTexturePath = NoExtension + "_NRM"; //add normal map prefix
	NormalMapTexturePath = NormalMapTexturePath + ".dds"; //add extension

    std::string AmbientOcclusionMapTexturePath = NoExtension + "_OCC"; //add occlusion map prefix
    AmbientOcclusionMapTexturePath = AmbientOcclusionMapTexturePath + ".dds"; //add extension

	std::string SpecularMapTexturePath = NoExtension + "_SPEC"; //add specular map prefix
	SpecularMapTexturePath = SpecularMapTexturePath + ".dds"; //add extension

    //finally add full path

    NormalMapTexturePath = "Resources\\Textures\\" + NormalMapTexturePath;
	AmbientOcclusionMapTexturePath = "Resources\\Textures\\" + AmbientOcclusionMapTexturePath;
	SpecularMapTexturePath = "Resources\\Textures\\" + SpecularMapTexturePath;


#if defined(DEBUG) || (_DEBUG)
	OutputDebugStringA(NormalMapTexturePath.c_str());
	OutputDebugStringA("\n");
	OutputDebugStringA(AmbientOcclusionMapTexturePath.c_str());
	OutputDebugStringA("\n");
	OutputDebugStringA(SpecularMapTexturePath.c_str());
	OutputDebugStringA("\n");



	if (SpecularMap && !FileExists(SpecularMapTexturePath))
		ShowError(SpecularMapTexturePath.c_str());

	if (AOMap && !FileExists(AmbientOcclusionMapTexturePath))
		ShowError(AmbientOcclusionMapTexturePath.c_str());

	if (NormalMap && !FileExists(NormalMapTexturePath))
		ShowError(NormalMapTexturePath.c_str());
#endif

	if (NormalMap)
	{
	    ID3D11ShaderResourceView* srv = mInfo.Mgr->CreateTexture(NormalMapTexturePath);
	    NormalMapSRV.push_back(srv);
	}

	if (AOMap)
	{
		ID3D11ShaderResourceView* srv = mInfo.Mgr->CreateTexture(AmbientOcclusionMapTexturePath);
	    AOMapSRV.push_back(srv);
	}

	if (SpecularMap)
	{

		ID3D11ShaderResourceView* srv = mInfo.Mgr->CreateTexture(SpecularMapTexturePath);
		SpecMapSRV.push_back(srv);
	}
}

void Model::LoadNormalMapModel16(const std::string& filename, bool &AOMap, bool &SpecularMap, bool &FillIndices)
{
	Assimp::Importer imp;

	const aiScene* pScene = imp.ReadFile(filename,
	    aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
//		aiProcess_PreTransformVertices |
		aiProcess_RemoveRedundantMaterials |
        aiProcess_ConvertToLeftHanded |
        aiProcess_SortByPType);

	if (pScene == NULL)
		ShowError(imp.GetErrorString());

	if (mInfo.Mgr == nullptr)
		ShowError("Pointer to texture Manager is null in Model::ModelData.");

	std::vector<USHORT> indices;
	std::vector<Vertex::PosNormalTexTan> nmap_vertices;
	std::vector<Subset> subsets;

	for (UINT i = 0; i < pScene->mNumMeshes; ++i)
	{
		aiMesh* mesh = pScene->mMeshes[i];

		Subset subset; 

		subset.VertexCount = mesh->mNumVertices;
		subset.VertexStart = nmap_vertices.size();
		subset.FaceStart = indices.size() / 3;
		subset.FaceCount = mesh->mNumFaces;
		subset.ID = mesh->mMaterialIndex;

		mModel.mNumFaces += mesh->mNumFaces;
		mModel.mNumVertices += mesh->mNumVertices;

		XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	    XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);

        XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	    XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

		for (UINT j = 0; j < subset.VertexCount; ++j)
		{
			Vertex::PosNormalTexTan vertex;

			vertex.Pos.x = mesh->mVertices[j].x;
			vertex.Pos.y = mesh->mVertices[j].y;
			vertex.Pos.z = mesh->mVertices[j].z;

			vertex.TangentU.x = mesh->mTangents[j].x;
			vertex.TangentU.y = mesh->mTangents[j].y;
			vertex.TangentU.z = mesh->mTangents[j].z;
	
			if (mInfo.UseNormalsFromModel)
			{
			    vertex.Normal.x = mesh->mNormals[j].x;
			    vertex.Normal.y = mesh->mNormals[j].y;
			    vertex.Normal.z = mesh->mNormals[j].z;
			}
			else
			{
				vertex.Normal = mInfo.Normal;
			}

			if (mesh->HasTextureCoords(0))
			{
			   vertex.Tex.x = mesh->mTextureCoords[0][j].x;
			   vertex.Tex.y =  mesh->mTextureCoords[0][j].y;
			}

			XMVECTOR Scale = XMLoadFloat3(&mInfo.Scale);
			XMVECTOR P = XMLoadFloat3(&vertex.Pos);

			P *= Scale;
			
		    vMin = XMVectorMin(vMin, P);
		    vMax = XMVectorMax(vMax, P);

			XMStoreFloat3(&vertex.Pos, P);
			nmap_vertices.push_back(vertex);
		}

		for (UINT j = 0; j < subset.FaceCount; ++j)
		{
			for (UINT index = 0; index < mesh->mFaces[j].mNumIndices; ++index)
			{
				indices.push_back(subset.VertexStart + mesh->mFaces[j].mIndices[index]);
			}
		}

		aiMaterial* Mat = pScene->mMaterials[subset.ID];

		if (mInfo.UseDefaultMaterial)
	    {	 
			LoadMaterials(Mat);
		}
		else
		{
			Materials.push_back(mInfo.Material);
		}

		LoadTextures(Mat, AOMap, true, SpecularMap);
	
		XNA::AxisAlignedBox box;

	    XMStoreFloat3(&box.Center, 0.5f * (vMin + vMax)); 
	    XMStoreFloat3(&box.Extents, 0.5f * (vMax - vMin)); 

	    AABB.push_back(box);
		subsets.push_back(subset);
	}


	

	//required for collision detection, picking and computing AABB
	for (size_t i = 0; i < nmap_vertices.size(); ++i)
		vertices.push_back(nmap_vertices[i].Pos);

	if (FillIndices)
	{
	    Indices = new INT[indices.size()];

	    for (size_t i = 0; i < indices.size(); ++i)
		Indices[i] = indices[i];
	}

	ModelVisibleList.resize(subsets.size());

	mModel.mSubsetCount = subsets.size();

	mModel.Mesh.SetSubsetTable(subsets);
	mModel.Mesh.SetIndices(&indices[0], indices.size());
	mModel.Mesh.SetVertices(&nmap_vertices[0], nmap_vertices.size());

}

void Model::LoadNormalMapModel32(const std::string& filename, bool &AOMap, bool &SpecularMap, bool &FillIndices)
{
	Assimp::Importer imp;

	const aiScene* pScene = imp.ReadFile(filename,
	    aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
	//	aiProcess_PreTransformVertices |
		aiProcess_RemoveRedundantMaterials |
        aiProcess_ConvertToLeftHanded |
        aiProcess_SortByPType);

	if (pScene == NULL)
		ShowError(imp.GetErrorString());

	if (mInfo.Mgr == nullptr)
		ShowError("Pointer to texture Manager is null in Model::ModelData.");

	if (pScene->HasLights())
		MessageBoxA(0, "HAS LIGHTS!!!", 0, 0);

	std::vector<UINT> indices;
	std::vector<Vertex::PosNormalTexTan> nmap_vertices;
	std::vector<Subset> subsets;

	for (UINT i = 0; i < pScene->mNumMeshes; ++i)
	{
		aiMesh* mesh = pScene->mMeshes[i];

		Subset subset;

		subset.VertexCount = mesh->mNumVertices;
		subset.VertexStart = nmap_vertices.size();
		subset.FaceStart = indices.size() / 3;
		subset.FaceCount = mesh->mNumFaces;
		subset.ID = mesh->mMaterialIndex;

		mModel.mNumFaces += mesh->mNumFaces;
		mModel.mNumVertices += mesh->mNumVertices;


		XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	    XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);

        XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	    XMVECTOR vMax = XMLoadFloat3(&vMaxf3);


		for (UINT j = 0; j < subset.VertexCount; ++j)
		{
			Vertex::PosNormalTexTan vertex;

			vertex.Pos.x = mesh->mVertices[j].x;
			vertex.Pos.y = mesh->mVertices[j].y;
			vertex.Pos.z = mesh->mVertices[j].z;

			vertex.TangentU.x = mesh->mTangents[j].x;
			vertex.TangentU.y = mesh->mTangents[j].y;
			vertex.TangentU.z = mesh->mTangents[j].z;
	
			if (mInfo.UseNormalsFromModel)
			{
			    vertex.Normal.x = mesh->mNormals[j].x;
			    vertex.Normal.y = mesh->mNormals[j].y;
			    vertex.Normal.z = mesh->mNormals[j].z;
			}
			else
			{
				vertex.Normal = mInfo.Normal;
			}

			if (mesh->HasTextureCoords(0))
			{
			   vertex.Tex.x = mesh->mTextureCoords[0][j].x;
			   vertex.Tex.y =  mesh->mTextureCoords[0][j].y;
			}

			XMVECTOR Scale = XMLoadFloat3(&mInfo.Scale);
			XMVECTOR P = XMLoadFloat3(&vertex.Pos);

			P *= Scale;
			
		    vMin = XMVectorMin(vMin, P);
		    vMax = XMVectorMax(vMax, P);

			XMStoreFloat3(&vertex.Pos, P);
			nmap_vertices.push_back(vertex);
		}

		for (UINT j = 0; j < subset.FaceCount; ++j)
		{
			for (UINT index = 0; index < mesh->mFaces[j].mNumIndices; ++index)
			{
				indices.push_back(subset.VertexStart + mesh->mFaces[j].mIndices[index]);
			}
		}

		aiMaterial* Mat = pScene->mMaterials[subset.ID];

		if (mInfo.UseDefaultMaterial)
		{		 
			LoadMaterials(Mat);
		}
		else
		{
			//user wants to set material manually
			Materials.push_back(mInfo.Material);
		}

		LoadTextures(Mat, AOMap, true, SpecularMap);

		XNA::AxisAlignedBox box;

	    XMStoreFloat3(&box.Center, 0.5f * (vMin + vMax)); 
	    XMStoreFloat3(&box.Extents, 0.5f * (vMax - vMin)); 

		//XNA::Sphere sphere;
		//XNA::ComputeBoundingSphereFromPoints(&sphere, subset.VertexCount, &nmap_vertices[0].Pos, sizeof(Vertex::PosNormalTexTan));

	    AABB.push_back(box);
		subsets.push_back(subset);
	}

	
	//required for collision detection, picking and computing AABB
	for (size_t i = 0; i < nmap_vertices.size(); ++i)
		vertices.push_back(nmap_vertices[i].Pos);

	if (FillIndices)
	{
	    Indices = new INT[indices.size()];

	    for (size_t i = 0; i < indices.size(); ++i)
		Indices[i] = indices[i];
	}

	ModelVisibleList.resize(subsets.size());

	mModel.mSubsetCount = subsets.size();

	mModel.Mesh.SetSubsetTable(subsets);
	mModel.Mesh.SetIndices(&indices[0], indices.size());
	mModel.Mesh.SetVertices(&nmap_vertices[0], nmap_vertices.size());
	mModel.Mesh.SetFormat(DXGI_FORMAT_R32_UINT);

}

void Model::LoadBasicModel32(const std::string& filename, bool &AOMap, bool &SpecularMap, bool &FillIndices)
{
	Assimp::Importer imp;

	const aiScene* pScene = imp.ReadFile(filename,
	  //  aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
	//	aiProcess_PreTransformVertices |
		aiProcess_RemoveRedundantMaterials |
        aiProcess_ConvertToLeftHanded |
        aiProcess_SortByPType);

	if (pScene == NULL)
		ShowError(imp.GetErrorString());

	if (mInfo.Mgr == nullptr)
		ShowError("Pointer to texture Manager is null in Model::ModelData.");

	std::vector<UINT> indices;
	std::vector<Vertex::Basic32> temp_vertices;
	std::vector<Subset> subsets;


	for (UINT i = 0; i < pScene->mNumMeshes; ++i)
	{
		aiMesh* mesh = pScene->mMeshes[i];

		Subset subset;

		subset.VertexCount = mesh->mNumVertices;
		subset.VertexStart = temp_vertices.size();
		subset.FaceStart = indices.size() / 3;
		subset.FaceCount = mesh->mNumFaces;
		subset.ID = mesh->mMaterialIndex;

		mModel.mNumFaces += mesh->mNumFaces;
		mModel.mNumVertices += mesh->mNumVertices;

		XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	    XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);

        XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	    XMVECTOR vMax = XMLoadFloat3(&vMaxf3);


		for (UINT j = 0; j < subset.VertexCount; ++j)
		{
			Vertex::Basic32 vertex;

			vertex.Pos.x = mesh->mVertices[j].x;
			vertex.Pos.y = mesh->mVertices[j].y;
			vertex.Pos.z = mesh->mVertices[j].z;
	
			if (mInfo.UseNormalsFromModel)
			{
			    vertex.Normal.x = mesh->mNormals[j].x;
			    vertex.Normal.y = mesh->mNormals[j].y;
			    vertex.Normal.z = mesh->mNormals[j].z;
			}
			else
			{
				vertex.Normal = mInfo.Normal;
			}

			if (mesh->HasTextureCoords(0))
			{
			   vertex.Tex.x = mesh->mTextureCoords[0][j].x;
			   vertex.Tex.y =  mesh->mTextureCoords[0][j].y;
			}

			XMVECTOR Scale = XMLoadFloat3(&mInfo.Scale);
			XMVECTOR P = XMLoadFloat3(&vertex.Pos);

			P *= Scale;
			
		    vMin = XMVectorMin(vMin, P);
		    vMax = XMVectorMax(vMax, P);

			XMStoreFloat3(&vertex.Pos, P);
			temp_vertices.push_back(vertex);
		}

		
		for (UINT j = 0; j < subset.FaceCount; ++j)
		{
			for (UINT index = 0; index < mesh->mFaces[j].mNumIndices; ++index)
			{
				indices.push_back(subset.VertexStart + mesh->mFaces[j].mIndices[index]);
			}
		}

		aiMaterial* Mat = pScene->mMaterials[subset.ID];

		if (mInfo.UseDefaultMaterial)
		{		 
			LoadMaterials(Mat);
		}
		else
		{
			//user wants to set material manually
			Materials.push_back(mInfo.Material);
		}

		LoadTextures(Mat, AOMap, false, SpecularMap);


		XNA::AxisAlignedBox box;

	    XMStoreFloat3(&box.Center, 0.5f * (vMin + vMax)); 
	    XMStoreFloat3(&box.Extents, 0.5f * (vMax - vMin)); 

	    AABB.push_back(box);
		subsets.push_back(subset);

	}
	
	for (size_t i = 0; i < temp_vertices.size(); ++i)
	{
		vertices.push_back(temp_vertices[i].Pos);
	}

	
	if (FillIndices)
	{
	    Indices = new INT[indices.size()];

	    for (size_t i = 0; i < indices.size(); ++i)
		Indices[i] = indices[i];
	}

	ModelVisibleList.resize(subsets.size());

	mModel.mSubsetCount = subsets.size();

	mModel.Mesh.SetFormat(DXGI_FORMAT_R32_UINT);
	mModel.Mesh.SetSubsetTable(subsets);
	mModel.Mesh.SetIndices(&indices[0], indices.size());
	mModel.Mesh.SetVertices(&temp_vertices[0], temp_vertices.size());
	

}


void Model::LoadBasicModel16(const std::string& filename, bool &AOMap, bool &SpecularMap, bool &FillIndices)
{

	Assimp::Importer imp;

	const aiScene* pScene = imp.ReadFile(filename,
	  //  aiProcess_CalcTangentSpace |
		aiProcess_JoinIdenticalVertices |
        aiProcess_Triangulate |
        aiProcess_GenSmoothNormals |
		aiProcess_OptimizeMeshes |
		aiProcess_OptimizeGraph |
	//	aiProcess_PreTransformVertices |
		aiProcess_RemoveRedundantMaterials |
        aiProcess_ConvertToLeftHanded |
        aiProcess_SortByPType);


	if (pScene == NULL)
		ShowError(imp.GetErrorString());

	if (mInfo.Mgr == nullptr)
		ShowError("Pointer to texture Manager is null in Model::ModelData.");

	std::vector<USHORT> indices;
	std::vector<Vertex::Basic32> temp_vertices;
	std::vector<Subset> subsets;


	for (UINT i = 0; i < pScene->mNumMeshes; ++i)
	{
		aiMesh* mesh = pScene->mMeshes[i];

		Subset subset;

		subset.VertexCount = mesh->mNumVertices;
		subset.VertexStart = temp_vertices.size();
		subset.FaceStart = indices.size() / 3;
		subset.FaceCount = mesh->mNumFaces;
		subset.ID = mesh->mMaterialIndex;

		mModel.mNumFaces += mesh->mNumFaces;
		mModel.mNumVertices += mesh->mNumVertices;


		XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	    XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);

        XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	    XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

		for (UINT j = 0; j < subset.VertexCount; ++j)
		{
			Vertex::Basic32 vertex;

			vertex.Pos.x = mesh->mVertices[j].x;
			vertex.Pos.y = mesh->mVertices[j].y;
			vertex.Pos.z = mesh->mVertices[j].z;
	
			if (mInfo.UseNormalsFromModel)
			{
			    vertex.Normal.x = mesh->mNormals[j].x;
			    vertex.Normal.y = mesh->mNormals[j].y;
			    vertex.Normal.z = mesh->mNormals[j].z;
			}
			else
			{
				vertex.Normal = mInfo.Normal;
			}

			if (mesh->HasTextureCoords(0))
			{
			   vertex.Tex.x = mesh->mTextureCoords[0][j].x;
			   vertex.Tex.y =  mesh->mTextureCoords[0][j].y;
			}


			XMVECTOR Scale = XMLoadFloat3(&mInfo.Scale);
			XMVECTOR P = XMLoadFloat3(&vertex.Pos);

			P *= Scale;
			
		    vMin = XMVectorMin(vMin, P);
		    vMax = XMVectorMax(vMax, P);

			XMStoreFloat3(&vertex.Pos, P);

			temp_vertices.push_back(vertex);
		}

		for (UINT j = 0; j < subset.FaceCount; ++j)
		{
			for (UINT index = 0; index < mesh->mFaces[j].mNumIndices; ++index)
			{
				indices.push_back(subset.VertexStart + mesh->mFaces[j].mIndices[index]);
			}
		}

		aiMaterial* Mat = pScene->mMaterials[subset.ID];

		if (mInfo.UseDefaultMaterial)
		{		 
			LoadMaterials(Mat);
		}
		else
		{
			Materials.push_back(mInfo.Material);
		}

		LoadTextures(Mat, AOMap, false, SpecularMap);

		XNA::AxisAlignedBox box;

	    XMStoreFloat3(&box.Center, 0.5f * (vMin + vMax)); 
	    XMStoreFloat3(&box.Extents, 0.5f * (vMax - vMin)); 


		AABB.push_back(box);
		subsets.push_back(subset);
	}

	for (size_t i = 0; i < temp_vertices.size(); ++i)
		vertices.push_back(temp_vertices[i].Pos);

	
	if (FillIndices)
	{
	    Indices = new INT[indices.size()];

	    for (size_t i = 0; i < indices.size(); ++i)
		Indices[i] = indices[i];
	}

	ModelVisibleList.resize(subsets.size());

	mModel.mSubsetCount = subsets.size();

	mModel.Mesh.SetSubsetTable(subsets);
	mModel.Mesh.SetIndices(&indices[0], indices.size());
	mModel.Mesh.SetVertices(&temp_vertices[0], temp_vertices.size());

}

void Model::BuildInstanceData()
{
	D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.ByteWidth = sizeof(InstancedData) * mInstancedData.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	
    HR(pDevice->CreateBuffer(&vbd, 0, &mInstancedBuffer));

}

void Model::SetDirLight(DirectionalLight Light[3])
{
	for (USHORT i = 0; i < 3; ++i)
		Lights[i] = Light[i];
}

void Model::Render(CXMMATRIX World, CXMMATRIX ViewProj)
{
	if (mHasNormalMaps)
		RenderNormalMapped(World, ViewProj, mHasOcclusionMaps, mHasSpecularMaps);
	else
		Render(World, ViewProj, mHasOcclusionMaps, mHasSpecularMaps);
}




void Model::RenderSubset(UINT Id, UINT pass, CXMMATRIX World, CXMMATRIX WorldViewProj, 
						 CXMMATRIX WorldInvTranspose, CXMMATRIX ShadowTransform, 
						 CXMMATRIX TexTransform)
{
	Effects::NormalMapFX->SetEyePosW(d3d->m_Cam.GetPosition());
	Effects::NormalMapFX->SetDirLights(Lights);
	Effects::NormalMapFX->SetPointLights(PointLights);
	Effects::NormalMapFX->SetShadowMap(d3d->GetShadowMap());
	
	Effects::NormalMapFX->SetWorld(World);
	Effects::NormalMapFX->SetWorldInvTranspose(WorldInvTranspose);
	Effects::NormalMapFX->SetWorldViewProj(WorldViewProj);
	Effects::NormalMapFX->SetTexTransform(TexTransform);
	Effects::NormalMapFX->SetShadowTransform(ShadowTransform);

	Effects::NormalMapFX->SetMaterial(Materials[Id]);

	Effects::NormalMapFX->SetDiffuseMap(DiffuseMapSRV[Id]);
	Effects::NormalMapFX->SetNormalMap(NormalMapSRV[Id]);
	Effects::NormalMapFX->SetAmbientOcclusionMap(AOMapSRV[Id]);
	Effects::NormalMapFX->SetSpecularMap(SpecMapSRV[Id]);
			
    //activeTech->GetPassByIndex(pass)->Apply(0, pDeviceContext);

	mModel.Mesh.Draw(Id);

}

void Model::RenderShadowMap(CXMMATRIX World, CXMMATRIX ViewProj)
{
	pDeviceContext->IASetInputLayout(InputLayouts::Basic32);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	

	XMMATRIX W = World;

	Effects::BuildShadowMapFX->SetEyePosW(d3d->m_Cam.GetPosition());
	Effects::BuildShadowMapFX->SetViewProj(ViewProj);

	
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(W);
	XMMATRIX WorldViewProj = W * ViewProj;
	XMMATRIX TexTransform = XMMatrixIdentity();

	ID3DX11EffectTechnique* Tech;

	if (mInfo.AlphaClip)
		Tech = Effects::BuildShadowMapFX->BuildShadowMapAlphaClipTech;
	else
		Tech = Effects::BuildShadowMapFX->BuildShadowMapTech;

	UINT stride = sizeof(Vertex::Basic32);
    UINT offset = 0;

	if (!mInfo.BackfaceCulling)
	{
		pDeviceContext->RSSetState(RenderStates::NoCullRS);
	}

	D3DX11_TECHNIQUE_DESC techDesc;
    Tech->GetDesc(&techDesc);

    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		for (UINT i = 0; i < mModel.mSubsetCount; ++i)
	    {   
		//	if (ModelVisibleList[i] == false)
		//		continue;

			//since its not going to be rendered anyways 
			if (DiffuseMapSRV[i] == nullptr)
				continue;

		   	Effects::BuildShadowMapFX->SetWorld(W);
		    Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BuildShadowMapFX->SetWorldViewProj(WorldViewProj);
			Effects::BuildShadowMapFX->SetTexTransform(TexTransform);

			Tech->GetPassByIndex(p)->Apply(0, pDeviceContext);

		    mModel.Mesh.Draw(i);
	    }
	 }

	if (!mInfo.BackfaceCulling)
	    pDeviceContext->RSSetState(0);

}

void Model::RenderInstanced(CXMMATRIX World, CXMMATRIX ViewProj)
{
	if (mHasNormalMaps)
		RenderInstancedNormalMapped(World, ViewProj, mHasOcclusionMaps, mHasSpecularMaps);
	else
		RenderInstanced(World, ViewProj, mHasOcclusionMaps, mHasSpecularMaps);

}

void Model::RenderInstancedShadowMap(CXMMATRIX World, CXMMATRIX ViewProj)
{
	if (mVisibleObjectCount == 0)
		return;

	pDeviceContext->IASetInputLayout(InputLayouts::InstancedBasic32);
    pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX W = World;

	Effects::BuildShadowMapInstancedFX->SetEyePosW(d3d->m_Cam.GetPosition());

	ID3DX11EffectTechnique* Tech;

	if (mInfo.AlphaClip)
		Tech = Effects::BuildShadowMapInstancedFX->BuildShadowMapAlphaClipTech;
	else
		Tech = Effects::BuildShadowMapInstancedFX->BuildShadowMapTech;

	UINT stride[2] = {sizeof(Vertex::Basic32), sizeof(InstancedData)};
    UINT offset[2] = {0,0};

	ID3D11Buffer* vbs[2] = {*mModel.Mesh.GetVertexBuffer(), mInstancedBuffer};
 
	if (!mInfo.BackfaceCulling)
		pDeviceContext->RSSetState(RenderStates::NoCullRS);

	D3DX11_TECHNIQUE_DESC techDesc;
    Tech->GetDesc(&techDesc);

    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		for (UINT i = 0; i < mModel.mSubsetCount; ++i)
	    {   
			pDeviceContext->IASetVertexBuffers(0, 2, vbs, stride, offset);
			pDeviceContext->IASetIndexBuffer(*mModel.Mesh.GetIndexBuffer(), mModel.Mesh.GetIndexBufferFormat(), 0);

			XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(W);
		    XMMATRIX TexTransform = XMMatrixIdentity();

		    Effects::BuildShadowMapInstancedFX->SetWorld(W);
			Effects::BuildShadowMapInstancedFX->SetViewProj(ViewProj);
		    Effects::BuildShadowMapInstancedFX->SetWorldInvTranspose(worldInvTranspose);
	        Effects::BuildShadowMapInstancedFX->SetTexTransform(TexTransform);

			Effects::BuildShadowMapInstancedFX->SetDiffuseMap(DiffuseMapSRV[i]);

			Tech->GetPassByIndex(p)->Apply(0, pDeviceContext);

			pDeviceContext->DrawIndexedInstanced(mModel.Mesh.SubsetTable[i].FaceCount * 3, 
				mVisibleObjectCount, mModel.Mesh.SubsetTable[i].FaceStart * 3,  0, 0);
	    }
	 }

  if (!mInfo.BackfaceCulling)
      pDeviceContext->RSSetState(0);
}

void Model::RenderInstancedNormalMapped(CXMMATRIX World, CXMMATRIX ViewProj, bool& AOMap, bool& SpecularMap)
{
	if (mVisibleObjectCount == 0)
		return;

	pDeviceContext->IASetInputLayout(InputLayouts::InstancedPosNormalTexTan);
    pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
 
	UINT stride[2] = {sizeof(Vertex::PosNormalTexTan), sizeof(InstancedData)};
    UINT offset[2] = {0,0};

	ID3D11Buffer* vbs[2] = {*mModel.Mesh.GetVertexBuffer(), mInstancedBuffer};
 
	XMMATRIX W = World;
	XMMATRIX ShadowTransform = W * XMLoadFloat4x4(&d3d->m_ShadowTransform);
	
	Effects::NormalMapFX->SetDirLights(Lights);
	Effects::NormalMapFX->SetPointLights(PointLights);
	Effects::NormalMapFX->SetEyePosW(d3d->m_Cam.GetPosition());
 
	ID3DX11EffectTechnique* activeTech = mInfo.technique;

	if (!mInfo.BackfaceCulling)
		pDeviceContext->RSSetState(RenderStates::NoCullRS);
	
	float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    D3DX11_TECHNIQUE_DESC techDesc;
    activeTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(W);

		Effects::NormalMapFX->SetWorld(W);
		Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::NormalMapFX->SetViewProj(ViewProj);
		Effects::NormalMapFX->SetTexTransform(XMMatrixIdentity());
		Effects::NormalMapFX->SetShadowMap(d3d->GetShadowMap());
		Effects::NormalMapFX->SetShadowTransform(ShadowTransform);

		for (UINT i = 0; i < mModel.mSubsetCount; ++i)
	    {   
			pDeviceContext->IASetVertexBuffers(0, 2, vbs, stride, offset);
			pDeviceContext->IASetIndexBuffer(*mModel.Mesh.GetIndexBuffer(), mModel.Mesh.GetIndexBufferFormat(), 0);

		    Effects::NormalMapFX->SetMaterial(Materials[i]);
			Effects::NormalMapFX->SetDiffuseMap(DiffuseMapSRV[i]);
			Effects::NormalMapFX->SetNormalMap(NormalMapSRV[i]);

			if (AOMap)
				Effects::NormalMapFX->SetAmbientOcclusionMap(AOMapSRV[i]);
			if (SpecularMap)
				Effects::NormalMapFX->SetSpecularMap(SpecMapSRV[i]);

		    activeTech->GetPassByIndex(p)->Apply(0, pDeviceContext);

			if (mInfo.AlphaToCoverage)
				pDeviceContext->OMSetBlendState(RenderStates::AlphaToCoverageBS, blendFactor, 0xffffffff);

			pDeviceContext->DrawIndexedInstanced(mModel.Mesh.SubsetTable[i].FaceCount * 3, 
				mVisibleObjectCount, mModel.Mesh.SubsetTable[i].FaceStart * 3,  0, 0);

			++mDrawCalls;

			if (mInfo.AlphaToCoverage)
			    pDeviceContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	    }

	  }

	if (!mInfo.BackfaceCulling)
	    pDeviceContext->RSSetState(0);

}

void Model::RenderInstanced(CXMMATRIX World, CXMMATRIX ViewProj, bool& AOMap, bool& SpecularMap)
{
	if (mVisibleObjectCount == 0)
		return;

	pDeviceContext->IASetInputLayout(InputLayouts::InstancedBasic32);
    pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
 
	UINT stride[2] = {sizeof(Vertex::Basic32), sizeof(InstancedData)};
    UINT offset[2] = {0,0};

	ID3D11Buffer* vbs[2] = {*mModel.Mesh.GetVertexBuffer(), mInstancedBuffer};
 
	XMMATRIX W = World;
	XMMATRIX ShadowTransform = W * XMLoadFloat4x4(&d3d->m_ShadowTransform);

	Effects::BasicFX->SetDirLights(Lights);
	Effects::BasicFX->SetEyePosW(d3d->m_Cam.GetPosition());
 
	ID3DX11EffectTechnique* activeTech = mInfo.technique;

	if (!mInfo.BackfaceCulling)
		pDeviceContext->RSSetState(RenderStates::NoCullRS);
	

	float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    D3DX11_TECHNIQUE_DESC techDesc;
    activeTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(W);

		Effects::BasicFX->SetWorld(W);
		Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::BasicFX->SetViewProj(ViewProj);
		Effects::BasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::BasicFX->SetShadowMap(d3d->GetShadowMap());
		Effects::BasicFX->SetShadowTransform(ShadowTransform);

		for (UINT i = 0; i < mModel.mSubsetCount; ++i)
	    {   

			pDeviceContext->IASetVertexBuffers(0, 2, vbs, stride, offset);
			pDeviceContext->IASetIndexBuffer(*mModel.Mesh.GetIndexBuffer(), mModel.Mesh.GetIndexBufferFormat(), 0);

		    Effects::BasicFX->SetMaterial(Materials[i]);
			Effects::BasicFX->SetDiffuseMap(DiffuseMapSRV[i]);

		    activeTech->GetPassByIndex(p)->Apply(0, pDeviceContext);

			if (mInfo.AlphaToCoverage)
				pDeviceContext->OMSetBlendState(RenderStates::AlphaToCoverageBS, blendFactor, 0xffffffff);

			pDeviceContext->DrawIndexedInstanced(mModel.Mesh.SubsetTable[i].FaceCount * 3, 
				mVisibleObjectCount, mModel.Mesh.SubsetTable[i].FaceStart * 3,  0, 0);

			if (mInfo.AlphaToCoverage)
			    pDeviceContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	    }

	  }

	if (!mInfo.BackfaceCulling)
	    pDeviceContext->RSSetState(0);
}

void Model::RenderNormalMapped(CXMMATRIX World, CXMMATRIX ViewProj, bool AOMap, bool SpecularMap)
{
	if (mIsModelVisible == OUTSIDE)
		return;

	ID3DX11EffectTechnique* activeTech = mInfo.technique;

	pDeviceContext->IASetInputLayout(InputLayouts::PosNormalTexTan);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	

	XMMATRIX W = World;
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(W);
	XMMATRIX WorldViewProj = W * ViewProj;
	XMMATRIX TexTransform = XMMatrixIdentity();
	XMMATRIX ShadowTransform = W * XMLoadFloat4x4(&d3d->m_ShadowTransform);


	Effects::NormalMapFX->SetEyePosW(d3d->m_Cam.GetPosition());
	Effects::NormalMapFX->SetDirLights(Lights);
	Effects::NormalMapFX->SetPointLights(PointLights);
	Effects::NormalMapFX->SetShadowMap(d3d->GetShadowMap());
	
	Effects::NormalMapFX->SetWorld(W);
	Effects::NormalMapFX->SetWorldInvTranspose(worldInvTranspose);
	Effects::NormalMapFX->SetWorldViewProj(WorldViewProj);
	Effects::NormalMapFX->SetTexTransform(TexTransform);
	Effects::NormalMapFX->SetShadowTransform(ShadowTransform);
	

	if (!mInfo.BackfaceCulling)
		pDeviceContext->RSSetState(RenderStates::NoCullRS);
	

	D3DX11_TECHNIQUE_DESC techDesc;
    activeTech->GetDesc(&techDesc);

	float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		for (UINT i = 0; i < mModel.mSubsetCount; ++i)
	    {   
			if (ModelVisibleList[i] == false)
				continue;

			if (DiffuseMapSRV[i] == nullptr)
				continue;

			++mDrawCalls;

		    Effects::NormalMapFX->SetMaterial(Materials[i]);
			Effects::NormalMapFX->SetDiffuseMap(DiffuseMapSRV[i]);
			Effects::NormalMapFX->SetNormalMap(NormalMapSRV[i]);

			if (mHasOcclusionMaps)
			{
				Effects::NormalMapFX->SetAmbientOcclusionMap(AOMapSRV[i]);
			}

			if (mHasSpecularMaps)
			{
			    Effects::NormalMapFX->SetSpecularMap(SpecMapSRV[i]);
			}

			if (mInfo.AlphaToCoverage)
				pDeviceContext->OMSetBlendState(RenderStates::AlphaToCoverageBS, blendFactor, 0xffffffff);
		
		    activeTech->GetPassByIndex(p)->Apply(0, pDeviceContext);

		    mModel.Mesh.Draw(i);

			if (mInfo.AlphaToCoverage)
			    pDeviceContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	    }
	 }

	if (!mInfo.BackfaceCulling)
	    pDeviceContext->RSSetState(0);

}

#ifdef _USE_DEFERRED_SHADING_
void Model::RenderDeferred(CXMMATRIX World, CXMMATRIX ViewProj)
{
	if (mIsModelVisible == OUTSIDE)
		return;

	ID3DX11EffectTechnique* activeTech = Effects::DeferredFX->BuildGBuffers;

	pDeviceContext->IASetInputLayout(InputLayouts::PosNormalTexTan);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	

	XMMATRIX W = World;
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(W);
	XMMATRIX WorldViewProj = W * ViewProj;
	XMMATRIX TexTransform = XMMatrixIdentity();
	XMMATRIX ShadowTransform = W * XMLoadFloat4x4(&d3d->m_ShadowTransform);

	Effects::DeferredFX->SetWorld(W);
	Effects::DeferredFX->SetWorldInvTranspose(worldInvTranspose);
	Effects::DeferredFX->SetWorldViewProj(WorldViewProj);
	Effects::DeferredFX->SetTexTransform(TexTransform);
	Effects::DeferredFX->SetShadowMap(d3d->GetShadowMap());
	Effects::DeferredFX->SetShadowTransform(ShadowTransform);

	if (!mInfo.BackfaceCulling)
		pDeviceContext->RSSetState(RenderStates::NoCullRS);
	

	D3DX11_TECHNIQUE_DESC techDesc;
    activeTech->GetDesc(&techDesc);

	float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		for (UINT i = 0; i < mModel.mSubsetCount; ++i)
	    {   
			if (ModelVisibleList[i] == false)
				continue;

			if (DiffuseMapSRV[i] == nullptr)
				continue;

			++mDrawCalls;

			Effects::DeferredFX->SetDiffuseMap(DiffuseMapSRV[i]);
			Effects::DeferredFX->SetNormalMap(NormalMapSRV[i]);
			Effects::DeferredFX->SetMaterial(Materials[i]);
			
			if (mInfo.AlphaToCoverage)
				pDeviceContext->OMSetBlendState(RenderStates::AlphaToCoverageBS, blendFactor, 0xffffffff);
		
		    activeTech->GetPassByIndex(p)->Apply(0, pDeviceContext);

		    mModel.Mesh.Draw(i);

			if (mInfo.AlphaToCoverage)
			    pDeviceContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	    }
	 }

	if (!mInfo.BackfaceCulling)
	    pDeviceContext->RSSetState(0);

}
#endif

#ifdef USE_SSAO

void Model::RenderNormalDepthMap(CXMMATRIX World, CXMMATRIX ViewProj)
{
	if (mIsModelVisible == 0)
		return;

	ID3DX11EffectTechnique* activeTech = Effects::SsaoNormalDepthFX->NormalDepthTech;;

	pDeviceContext->IASetInputLayout(InputLayouts::Basic32);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	

	XMMATRIX view = d3d->m_Cam.View();

	XMMATRIX world = World;
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
	XMMATRIX worldView   = world* view;
	XMMATRIX worldInvTransposeView = worldInvTranspose*view;
	XMMATRIX worldViewProj = world * ViewProj;


	D3DX11_TECHNIQUE_DESC techDesc;
    activeTech->GetDesc(&techDesc);

	float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		for (UINT i = 0; i < mModel.mSubsetCount; ++i)
	    {   	

			if (ModelVisibleList[i] == false)
				continue;

			++mDrawCalls;

			Effects::SsaoNormalDepthFX->SetWorldView(worldView);
	        Effects::SsaoNormalDepthFX->SetWorldInvTransposeView(worldInvTransposeView);
	        Effects::SsaoNormalDepthFX->SetWorldViewProj(worldViewProj);
	        Effects::SsaoNormalDepthFX->SetTexTransform(XMMatrixIdentity());

		    activeTech->GetPassByIndex(p)->Apply(0, pDeviceContext);

		    mModel.Mesh.Draw(i);

	    }
	 }
}

#endif

void Model::Render(CXMMATRIX World, CXMMATRIX ViewProj, bool AOMap, bool SpecularMap)
{
	if (mIsModelVisible == 0)
		return;

	ID3DX11EffectTechnique* activeTech = mInfo.technique;

	pDeviceContext->IASetInputLayout(InputLayouts::Basic32);
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);	

	XMMATRIX W = World;
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(W);
	XMMATRIX WorldViewProj = W * ViewProj;
	XMMATRIX TexTransform = XMMatrixIdentity();
	XMMATRIX ShadowTransform = W * XMLoadFloat4x4(&d3d->m_ShadowTransform);

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
/*	XMMATRIX toTexSpace(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f); */

	Effects::BasicFX->SetDirLights(Lights);
	Effects::BasicFX->SetEyePosW(d3d->m_Cam.GetPosition());



	if (!mInfo.BackfaceCulling)
		pDeviceContext->RSSetState(RenderStates::NoCullRS);
	

	Effects::BasicFX->SetShadowMap(d3d->GetShadowMap());

//	if (mSSAO)
	   // Effects::BasicFX->SetSsaoMap(d3d->m_Ssao->AmbientSRV());

	Effects::BasicFX->SetWorld(W);
	Effects::BasicFX->SetWorldInvTranspose(worldInvTranspose);
	Effects::BasicFX->SetWorldViewProj(WorldViewProj);
//	Effects::BasicFX->SetWorldViewProjTex(WorldViewProj * toTexSpace);
	Effects::BasicFX->SetTexTransform(TexTransform);
	Effects::BasicFX->SetShadowTransform(ShadowTransform);
	

	D3DX11_TECHNIQUE_DESC techDesc;
    activeTech->GetDesc(&techDesc);

	float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};


    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		for (UINT i = 0; i < mModel.mSubsetCount; ++i)
	    {   
			if (ModelVisibleList[i] == false)
				continue;

			if (DiffuseMapSRV[i] == nullptr)
				continue;

			++mDrawCalls;

		    Effects::BasicFX->SetMaterial(Materials[i]);
			Effects::BasicFX->SetDiffuseMap(DiffuseMapSRV[i]);
			
			if (AOMap)
			{
				Effects::BasicFX->SetAmbientOcclusionMap(AOMapSRV[i]);
			}

			if (SpecularMap)
			{
				Effects::BasicFX->SetSpecularMap(SpecMapSRV[i]);
			}

			if (mInfo.AlphaToCoverage)
				pDeviceContext->OMSetBlendState(RenderStates::AlphaToCoverageBS, blendFactor, 0xffffffff);
		
		    activeTech->GetPassByIndex(p)->Apply(0, pDeviceContext);

		    mModel.Mesh.Draw(i);

			if (mInfo.AlphaToCoverage)
			    pDeviceContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	    }
	 }

	

	if (!mInfo.BackfaceCulling)
	    pDeviceContext->RSSetState(0);
}

void Model::Update(CXMMATRIX World)
{
	mDrawCalls = 0;

	if (mIsModelVisible == INSIDE)
	{
		for (UINT i = 0; i < mModel.mSubsetCount; ++i)
		{
			ModelVisibleList[i] = true;
		}
		
	}

	if (mIsModelVisible == INTERSECT)
	{
	    for (UINT i = 0; i < mModel.mSubsetCount; ++i)
	    {
	        if (d3d->IntersectAABBFrustum(&AABB[i], World) != 0)
		    {
			    ModelVisibleList[i] = true;
		    }
		    else
			    ModelVisibleList[i] = false;
	   }

	}
}


UINT Model::GetNumTriangles()
{
	return mModel.mNumFaces;
}

UINT Model::GetNumDrawCalls()
{
	return mDrawCalls;
}


MeshGeometry::MeshGeometry()
{
	VertexBuffer = nullptr;
	IndexBuffer = nullptr;

	IndexBufferFormat = DXGI_FORMAT_R16_UINT;
	VertexStride = 0;
}

MeshGeometry::~MeshGeometry()
{
	ReleaseCOM(VertexBuffer);
	ReleaseCOM(IndexBuffer);
}

ID3D11Buffer** MeshGeometry::GetVertexBuffer()
{
	return &VertexBuffer;
}

ID3D11Buffer** MeshGeometry::GetIndexBuffer()
{
	return &IndexBuffer;
}

DXGI_FORMAT MeshGeometry::GetIndexBufferFormat()
{
	return IndexBufferFormat;
}

void Model::SetModelVisibleStatus(INT& s)
{
	mIsModelVisible = s;
}

void MeshGeometry::SetFormat(DXGI_FORMAT format)
{
	IndexBufferFormat = format;
}

void MeshGeometry::SetIndices(const USHORT* indices, UINT count)
{
	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(USHORT) * count;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;

	HR(pDevice->CreateBuffer(&ibd, &iinitData, &IndexBuffer));
}


void MeshGeometry::SetIndices(const DWORD* indices, UINT count)
{
	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(DWORD) * count;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;

	HR(pDevice->CreateBuffer(&ibd, &iinitData, &IndexBuffer));
}




void MeshGeometry::SetIndices(const UINT* indices, UINT count)
{
	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * count;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;

	HR(pDevice->CreateBuffer(&ibd, &iinitData, &IndexBuffer));
}

void MeshGeometry::SetSubsetTable(std::vector<Subset>& subsetTable)
{
	SubsetTable = subsetTable;
}

void MeshGeometry::Draw(UINT subsetId)
{
    UINT offset = 0;

	pDeviceContext->IASetVertexBuffers(0, 1, &VertexBuffer, &VertexStride, &offset);
	pDeviceContext->IASetIndexBuffer(IndexBuffer, IndexBufferFormat, 0);

	pDeviceContext->DrawIndexed(SubsetTable[subsetId].FaceCount * 3, SubsetTable[subsetId].FaceStart * 3,  0);
}


template <typename VertexType>
void MeshGeometry::SetVertices(const VertexType* vertices, UINT count)
{
	ReleaseCOM(VertexBuffer);

	VertexStride = sizeof(VertexType);

	D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(VertexType) * count;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices;

	HR(pDevice->CreateBuffer(&vbd, &vinitData, &VertexBuffer));
}
