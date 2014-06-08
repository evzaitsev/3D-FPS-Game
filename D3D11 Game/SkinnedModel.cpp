#include "stdafx.h"

//=================================================
// Assimp Skinned Model Loader by newtechnology
// This Assimp Skinned Model Loader uses Scot lee's animation code for loading skinned models
// Status : On the stage of improvement
// Updates :
// *Added SkinnedModelInstance class to draw a single model multiple times
// *Added Support for Skinned Models who require 32 Bit Indices
//=================================================

SkinnedModel::SkinnedModel(const std::string& modelpath, InitInfo& info, bool Use32BitIndexFormat)
{
	mInfo = info;
	
	
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

	LoadSkinnedModel(modelpath, Use32BitIndexFormat);
}

SkinnedModel::~SkinnedModel()
{

}

void SkinnedModel::LoadMaterials(aiMaterial* Mat)
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

void SkinnedModel::LoadTextures(aiMaterial* Mat)
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
		NormalMapSRV.push_back(srv);

		return;
	}

	std::string file = path.C_Str();
	std::string NoExtension = removeExtension(file);

	std::string NormalMapTexturePath = NoExtension + "_NRM"; //add normal map prefix
	NormalMapTexturePath = NormalMapTexturePath + ".dds"; //add extension

	NormalMapTexturePath = "Resources\\Textures\\" + NormalMapTexturePath;

#if defined(DEBUG) || defined(_DEBUG)
	if (!FileExists(NormalMapTexturePath))
		ShowError("A Normal map is missing.");
#endif

	ID3D11ShaderResourceView* srv = mInfo.Mgr->CreateTexture(NormalMapTexturePath);
	NormalMapSRV.push_back(srv);
}


void SkinnedModel::LoadSkinnedModel(const std::string& path, bool& Use32BitFormat)
{
	Assimp::Importer imp;

	std::vector<Vertex::PosNormalTexTanSkinned> temp_vertices;
	std::vector<USHORT> Indices_16Bit;
	std::vector<UINT>   Indices_32Bit;
	std::vector<Subset> Subsets;
	
	const aiScene* pScene = imp.ReadFile(path, 
		aiProcessPreset_TargetRealtime_Quality |
		aiProcess_ConvertToLeftHanded );
	
	if (pScene == NULL)
		ShowError(imp.GetErrorString());

	if (mInfo.Mgr == nullptr)
		ShowError("No Address found in pointer mInfo::Mgr");

	if (!pScene->HasAnimations())
		ShowError("Model does have animations.");

	mSceneAnimator.Init(pScene);

	for (UINT i = 0; i < pScene->mNumMeshes; ++i)
	{
		aiMesh* mesh = pScene->mMeshes[i];

		Subset subset;

		subset.VertexCount = mesh->mNumVertices;
		subset.VertexStart = temp_vertices.size();
		subset.FaceCount = mesh->mNumFaces;
		subset.ID = mesh->mMaterialIndex;

		if (Use32BitFormat)
			subset.FaceStart = Indices_32Bit.size() / 3;
		else
			subset.FaceStart = Indices_16Bit.size() / 3;

		mModel.mNumFaces += subset.FaceCount;
		mModel.mNumVertices += subset.VertexCount;

		std::vector<std::vector<aiVertexWeight> > weightsPerVertex(mesh->mNumVertices);

		for (UINT b = 0; b < mesh->mNumBones; ++b)
		{
			const aiBone* bone = mesh->mBones[b];

			int BoneIndex = mSceneAnimator.GetBoneIndex(bone->mName.C_Str());

			for (UINT w = 0; w < bone->mNumWeights; ++w)
			{
				weightsPerVertex[bone->mWeights[w].mVertexId].push_back(aiVertexWeight(BoneIndex, bone->mWeights[w].mWeight));
			}
		}


		for (UINT j = 0; j < subset.VertexCount; ++j)
		{
			Vertex::PosNormalTexTanSkinned vertex;

			vertex.Pos.x = mesh->mVertices[j].x;
			vertex.Pos.y = mesh->mVertices[j].y;
			vertex.Pos.z = mesh->mVertices[j].z;

			vertex.TangentU.x = mesh->mTangents[j].x;
			vertex.TangentU.y = mesh->mTangents[j].y;
			vertex.TangentU.z = mesh->mTangents[j].z;
	
			vertex.Normal.x = mesh->mNormals[j].x;
			vertex.Normal.y = mesh->mNormals[j].y;
			vertex.Normal.z = mesh->mNormals[j].z;

			if (!mesh->HasBones())
				ShowError("Mesh does not have bones.");


			if (mesh->HasTextureCoords(0))
			{
			   vertex.Tex.x = mesh->mTextureCoords[0][j].x;
			   vertex.Tex.y =  mesh->mTextureCoords[0][j].y;
			}

			//Init to zero
			vertex.Weights.x = 0.0f;
			vertex.Weights.y = 0.0f;
			vertex.Weights.z = 0.0f;
			vertex.Weights.w = 0.0f;
		
			vertex.BoneIndices[0] = 0;
			vertex.BoneIndices[1] = 0;
			vertex.BoneIndices[2] = 0;
			vertex.BoneIndices[3] = 0;

			int size = weightsPerVertex[j].size();
			
			float weights[4] = {0.0f, 0.0f, 0.0f, 0.0f};

			for (UINT w = 0; w < size; ++w)
			{
				vertex.BoneIndices[w] = weightsPerVertex[j][w].mVertexId;
				weights[w] = weightsPerVertex[j][w].mWeight;
			}

			vertex.Weights.x = weights[0];
			vertex.Weights.y = weights[1];
			vertex.Weights.z = weights[2];
			vertex.Weights.w = weights[3];

			temp_vertices.push_back(vertex);
		}

		for (UINT j = 0; j < subset.FaceCount; ++j)
		{
			for (UINT index = 0; index < mesh->mFaces[j].mNumIndices; ++index)
			{
				if (Use32BitFormat)
				    Indices_32Bit.push_back(subset.VertexStart + mesh->mFaces[j].mIndices[index]);
				else
				    Indices_16Bit.push_back(subset.VertexStart + mesh->mFaces[j].mIndices[index]);
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

		LoadTextures(Mat);

		Subsets.push_back(subset);
	}

	
	mModel.mSubsetCount = Subsets.size();

	mModel.Mesh.SetSubsetTable(Subsets);
	mModel.Mesh.SetVertices(&temp_vertices[0], temp_vertices.size());
	
	if (Use32BitFormat)
	{
		mModel.Mesh.SetIndices(&Indices_32Bit[0], Indices_32Bit.size());
		mModel.Mesh.SetFormat(DXGI_FORMAT_R32_UINT);
	}
	else
	{
		mModel.Mesh.SetIndices(&Indices_16Bit[0], Indices_16Bit.size());
		mModel.Mesh.SetFormat(DXGI_FORMAT_R16_UINT);
	}

	//Store Scaled vertices or we will get wrong bounding box
	//NOTE: Scale vector should match the x, y, z components with scaling matrix
	XMVECTOR ScaleVector = XMLoadFloat3(&mInfo.Scale);

	for (UINT i = 0; i < temp_vertices.size(); ++i)
	{
		XMVECTOR VertexPos = XMLoadFloat3(&temp_vertices[i].Pos);
		VertexPos *= ScaleVector;
		XMFLOAT3 tempos;

		XMStoreFloat3(&tempos, VertexPos);

		vertices.push_back(tempos);

	} 
}


void SkinnedModel::Render(CXMMATRIX World, CXMMATRIX ViewProj)
{
	//responsibilty of user to pass correct status
	//SkinnedModel class does not handle frustum culling
	if (mModelVisibleStatus == OUTSIDE)
		return;

	ID3DX11EffectTechnique* activeTech = Effects::NormalMapFX->Light1TexSkinnedTech;

	pDeviceContext->IASetInputLayout(InputLayouts::PosNormalTexTanSkinned);
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
	Effects::NormalMapFX->SetBoneTransforms(&FinalTransforms[0], FinalTransforms.size());	
	
	D3DX11_TECHNIQUE_DESC techDesc;
    activeTech->GetDesc(&techDesc);

	float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		for (UINT i = 0; i < mModel.mSubsetCount; ++i)
	    {   

			if (DiffuseMapSRV[i] == nullptr)
				continue;

		    Effects::NormalMapFX->SetMaterial(Materials[i]);
			Effects::NormalMapFX->SetDiffuseMap(DiffuseMapSRV[i]);
			Effects::NormalMapFX->SetNormalMap(NormalMapSRV[i]);

		    activeTech->GetPassByIndex(p)->Apply(0, pDeviceContext);

		    mModel.Mesh.Draw(i);

	    }
	 }

}


void SkinnedModel::SetModelVisibleStatus(XNA::FrustumIntersection status)
{
	mModelVisibleStatus = status;
}

