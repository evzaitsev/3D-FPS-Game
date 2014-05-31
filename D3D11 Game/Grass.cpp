#include "stdafx.h"

/*
Grass::Grass(std::string path, USHORT NumInstances, GrassArea area)
{
	XMMATRIX I = XMMatrixIdentity();
	XMStoreFloat4x4(&mWorld, I);

	Init(path, NumInstances, area);
}


Grass::~Grass()
{
	SafeDelete(mGrass);
}

void Grass::Init(std::string& path, USHORT& numInstances, GrassArea& area)
{
	Model::InitInfo info;

	Material DefaultMat;

	DefaultMat.Ambient = XMFLOAT4(0.6f, 0.6f, 0.6f, 0.6f);
	DefaultMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	DefaultMat.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 16.0f);

	info.AlphaClip = true;
	info.AlphaToCoverage = true;
	info.BackfaceCulling = false;
	info.InstanceFrustumCulling = true;
	info.Mgr = &d3d->m_TextureMgr;

	info.UseDefaultMaterial = false;
	info.Material = DefaultMat;
	info.NumLights = 1;


	info.UseNormalsFromModel = false;
	info.Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

	mGrass = new Model(path, info, false, false, false);

	for (USHORT i = 0; i < numInstances; ++i)
	{
RANDOM:
		float x = MathHelper::RandF(area.x, area.z);
		float z = MathHelper::RandF(area.x, area.z);

		x = (int)x;
		z = (int)z;

		float y = d3d->m_Land.GetHeight(x, z) - 1.75f;

		float scale = 0.07f;

		if (y >= 7.0f)
			__asm JMP RANDOM;

		XMMATRIX T = XMMatrixTranslation(x, y, z);
		XMMATRIX S = XMMatrixScaling(scale, scale, scale);

		XMMATRIX W = S * T;

		InstancedData data;
	    XMStoreFloat4x4(&data.World, W);

		mGrass->mInstancedData.push_back(data);
	}

	mGrass->BuildInstanceData();


	XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);
	
	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);


	for (UINT i = 0; i < mGrass->vertices.size(); ++i)
	{
		XMVECTOR P = XMLoadFloat3(&mGrass->vertices[i]);
	
		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);

	}

	XMStoreFloat3(&mBox.Center, 0.5f * (vMin + vMax)); 
	XMStoreFloat3(&mBox.Extents, 0.5f * (vMax - vMin)); 

	//compute the area around which grass would be generated
	//if area is not visible then its useless to even test for aabb of each grass instance

	vMinf3 = XMFLOAT3(area.x, area.x, area.x);
	vMaxf3 = XMFLOAT3(area.z, area.z, area.z);

	XMFLOAT3 points[2] = { vMinf3, vMaxf3 };

	XNA::ComputeBoundingAxisAlignedBoxFromPoints(&Parent, 2, points, sizeof(XMFLOAT3));

}


void Grass::Update()
{
	ParentVisible = d3d->IntersectAABBFrustum(&Parent, XMLoadFloat4x4(&mWorld));

	if (ParentVisible)
	    mGrass->UpdateInstanceData(mBox);
}

void Grass::Draw()
{
	if (ParentVisible)
		mGrass->RenderInstanced(XMLoadFloat4x4(&mWorld), d3d->m_Cam.ViewProj());
}*/