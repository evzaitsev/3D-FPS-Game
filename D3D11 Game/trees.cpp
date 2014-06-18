#include "stdafx.h"

/*
Tree::Tree()
{
	mModel = nullptr;
	far_mModel = nullptr;
}

Tree::~Tree()
{
	SafeDelete(mModel);
	SafeDelete(far_mModel);
}

void Tree::Init(InitInfo& info)
{
	mInfo = info;

	if (mInfo.NumInstances > MAX_TREES)
		ShowError("Tree::Info::NumInstances cannot be this much greater.");

	Model::InitInfo modelinfo;

	Material DefaultMat;

	DefaultMat.Ambient = XMFLOAT4(0.6f, 0.6f, 0.6f, 0.6f);
	DefaultMat.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	DefaultMat.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 16.0f);

	modelinfo.AlphaClip = true;
	modelinfo.AlphaToCoverage = true;
	modelinfo.BackfaceCulling = true;
	modelinfo.InstanceFrustumCulling = true;
	modelinfo.Material = DefaultMat;
	modelinfo.UseDefaultMaterial = true;
	modelinfo.NumLights = 1;
	modelinfo.UseNormalsFromModel = true;
	modelinfo.Mgr = &d3d->m_TextureMgr;

	mModel = new Model(mInfo.ModelPath, modelinfo, mInfo.Use32BitIndices, false, false);

	modelinfo.AlphaClip = true;
	modelinfo.AlphaToCoverage = true;
	modelinfo.NumLights = 1;
	modelinfo.BackfaceCulling = false;
	modelinfo.UseNormalsFromModel = false;
	modelinfo.Normal = XMFLOAT3(0.0f, 1.0f, 0.0f); //Normals should directly point up in billboard

	far_mModel = new Model(mInfo.far_ModelPath, modelinfo, false, false, false);

	XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);
	
	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

	for (UINT i = 0; i < mModel->vertices.size(); ++i)
	{
		XMVECTOR P = XMLoadFloat3(&mModel->vertices[i]);
	
		vMin = XMVectorMin(vMin, P);
		vMax = XMVectorMax(vMax, P);
	}

	XMStoreFloat3(&box.Center, 0.5f * (vMin + vMax)); 
	XMStoreFloat3(&box.Extents, 0.5f * (vMax - vMin)); 

	treeData.resize(mInfo.NumInstances);

	for (USHORT i = 0; i < mInfo.NumInstances; ++i)
	{
		RANDOM:
		float x = MathHelper::RandF(-200.0f, 200.0f);
		float z = MathHelper::RandF(-200.0f, 200.0f);

		float y = d3d->m_Land.GetHeight(x, z);

		if (y >= 9.0f)
		{
			__asm JMP RANDOM;
		}

		float scale = 0.1f;

		XMFLOAT3 pos = XMFLOAT3(x, y, z);

		treeData[i].pos = pos;

		XMMATRIX T = XMMatrixTranslation(pos.x, pos.y, pos.z);
		XMMATRIX S = XMMatrixScaling(scale, scale, scale);

		XMMATRIX W = S * T;
		

		AddtreeWorld(W);
		AddtreeWorldToFarModel(W);
	}

	//Build Instance data simply creates InstanceBuffer
	far_mModel->BuildInstanceData();
	mModel->BuildInstanceData();

	//Extract Information for model class which are required for rendering
	mModel_MeshData = mModel->ExtractMeshData();
	far_mModel_MeshData = far_mModel->ExtractMeshData();
}

void Tree::AddtreeWorld(CXMMATRIX World)
{
	XMMATRIX W = World;

	InstancedData data;
	XMStoreFloat4x4(&data.World, W);

	mModel->mInstancedData.push_back(data);
}

void Tree::AddtreeWorldToFarModel(CXMMATRIX World)
{
	XMMATRIX W = World;

	InstancedData data;
	XMStoreFloat4x4(&data.World, W);

	far_mModel->mInstancedData.push_back(data);
}

void Tree::Update()
{
	for (USHORT i = 0; i < mInfo.NumInstances; ++i)
	{
		XMVECTOR treePosXM = XMLoadFloat3(&treeData[i].pos);
		XMVECTOR camposXM = d3d->m_Cam.GetPositionXM();

		XMVECTOR distVec = treePosXM - camposXM;
		XMVECTOR vecLength = XMVector3LengthSq(distVec);

		FLOAT distance;
		XMStoreFloat(&distance, vecLength);

		if (distance > MAX_DISTANCE) //MAX_DISTANCE is 140 (140 * 140 because we use XMVector3LengthSq)
			treeData[i].IsFar = true;
		else
			treeData[i].IsFar = false;
	} 

	NumVisibleObjects = 0;

	D3D11_MAPPED_SUBRESOURCE mappedData; 
	pDeviceContext->Map(*mModel_MeshData.InstancedBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

	InstancedData* dataView = reinterpret_cast<InstancedData*>(mappedData.pData);

	for(UINT i = 0; i < mInfo.NumInstances; ++i)
	{
		XMMATRIX W = XMLoadFloat4x4(&mModel->mInstancedData[i].World);

		treeData[i].Visible = d3d->IntersectAABBFrustum(&box, W);

		if (treeData[i].Visible == false)
			continue;

		if (treeData[i].IsFar == true)
			continue;
		
		dataView[NumVisibleObjects++] = mModel->mInstancedData[i];
	    
	}

	pDeviceContext->Unmap(*mModel_MeshData.InstancedBuffer, 0);

	farModel_NumVisibleObjects = 0;

	D3D11_MAPPED_SUBRESOURCE farModel_mappedData; 
	pDeviceContext->Map(*far_mModel_MeshData.InstancedBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &farModel_mappedData);

	InstancedData* farModel_DataView = reinterpret_cast<InstancedData*>(farModel_mappedData.pData);

	for (UINT i = 0; i < mInfo.NumInstances; ++i)
	{
		
		if (treeData[i].Visible == false)
			continue;

		if (treeData[i].IsFar == false)
			continue;
	
	    farModel_DataView[farModel_NumVisibleObjects++] = far_mModel->mInstancedData[i];
	}

	pDeviceContext->Unmap(*far_mModel_MeshData.InstancedBuffer, 0);
}

void Tree::DrawFarModel(CXMMATRIX W, CXMMATRIX ShadowTransform, CXMMATRIX viewProj, CXMMATRIX worldInvTranspose)
{
	if ((farModel_NumVisibleObjects + 1) == 0)
		return;

	UINT stride[2] = {sizeof(Vertex::Basic32), sizeof(InstancedData)};
    UINT offset[2] = {0,0};

	ID3D11Buffer* vbs[2] = {*far_mModel_MeshData.VertexBuffer, *far_mModel_MeshData.InstancedBuffer};
 

	Effects::InstancedBasicFX->SetDirLights(*far_mModel_MeshData.Lights);
	Effects::InstancedBasicFX->SetEyePosW(d3d->m_Cam.GetPosition());
 
	ID3DX11EffectTechnique* activeTech = activeTech = Effects::InstancedBasicFX->Light1TexAlphaClipTech;

	float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

	pDeviceContext->RSSetState(RenderStates::NoCullRS);

    D3DX11_TECHNIQUE_DESC techDesc;
    activeTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		Effects::InstancedBasicFX->SetWorld(W);
		Effects::InstancedBasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::InstancedBasicFX->SetViewProj(viewProj);
		Effects::InstancedBasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::InstancedBasicFX->SetShadowMap(d3d->GetShadowMap());
		Effects::InstancedBasicFX->SetShadowTransform(ShadowTransform);

		for (UINT i = 0; i < far_mModel_MeshData.SubsetCount; ++i)
	    {   
			pDeviceContext->IASetVertexBuffers(0, 2, vbs, stride, offset);
			pDeviceContext->IASetIndexBuffer(*far_mModel_MeshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		    Effects::InstancedBasicFX->SetMaterial(*far_mModel_MeshData.Materials[i]);
			Effects::InstancedBasicFX->SetDiffuseMap(*far_mModel_MeshData.DiffuseMapSRV[i]);

		    activeTech->GetPassByIndex(p)->Apply(0, pDeviceContext);
			
			pDeviceContext->OMSetBlendState(RenderStates::AlphaToCoverageBS, blendFactor, 0xffffffff);

			pDeviceContext->DrawIndexedInstanced(far_mModel_MeshData.subsetTable[i]->FaceCount * 3, 
				farModel_NumVisibleObjects, far_mModel_MeshData.subsetTable[i]->FaceStart * 3,  0, 0);
		
			pDeviceContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	    }

	  }
	pDeviceContext->RSSetState(0);
}

void Tree::DrawNearModel(CXMMATRIX W, CXMMATRIX ShadowTransform, CXMMATRIX viewProj, CXMMATRIX worldInvTranspose)
{
	if ((NumVisibleObjects + 1) == 0)
		return;

	pDeviceContext->IASetInputLayout(InputLayouts::InstancedBasic32);
    pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
 
	UINT stride[2] = {sizeof(Vertex::Basic32), sizeof(InstancedData)};
    UINT offset[2] = {0,0};

	ID3D11Buffer* vbs[2] = {*mModel_MeshData.VertexBuffer, *mModel_MeshData.InstancedBuffer};
 

	Effects::InstancedBasicFX->SetDirLights(*mModel_MeshData.Lights);
	Effects::InstancedBasicFX->SetEyePosW(d3d->m_Cam.GetPosition());
 
	ID3DX11EffectTechnique* activeTech = activeTech = Effects::InstancedBasicFX->Light1TexAlphaClipTech;
	
	pDeviceContext->RSSetState(0);

	float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    D3DX11_TECHNIQUE_DESC techDesc;
    activeTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		XMMATRIX world = W;
		
		Effects::InstancedBasicFX->SetWorld(world);
		Effects::InstancedBasicFX->SetWorldInvTranspose(worldInvTranspose);
		Effects::InstancedBasicFX->SetViewProj(viewProj);
		Effects::InstancedBasicFX->SetTexTransform(XMMatrixIdentity());
		Effects::InstancedBasicFX->SetShadowMap(d3d->GetShadowMap());
		Effects::InstancedBasicFX->SetShadowTransform(ShadowTransform);

		for (UINT i = 0; i < mModel_MeshData.SubsetCount; ++i)
	    {   
			pDeviceContext->IASetVertexBuffers(0, 2, vbs, stride, offset);
			pDeviceContext->IASetIndexBuffer(*mModel_MeshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		    Effects::InstancedBasicFX->SetMaterial(*mModel_MeshData.Materials[i]);
			Effects::InstancedBasicFX->SetDiffuseMap(*mModel_MeshData.DiffuseMapSRV[i]);

		    activeTech->GetPassByIndex(p)->Apply(0, pDeviceContext);

			pDeviceContext->OMSetBlendState(RenderStates::AlphaToCoverageBS, blendFactor, 0xffffffff);

			pDeviceContext->DrawIndexedInstanced(mModel_MeshData.subsetTable[i]->FaceCount * 3, 
				NumVisibleObjects, mModel_MeshData.subsetTable[i]->FaceStart * 3,  0, 0);

			pDeviceContext->OMSetBlendState(0, blendFactor, 0xffffffff);
	    }

	  }
}

void Tree::DrawNearModelToShadowMap(CXMMATRIX World, CXMMATRIX viewProj, CXMMATRIX worldInvTranspose)
{
	if ((NumVisibleObjects + 1) == 0)
		return;

	pDeviceContext->IASetInputLayout(InputLayouts::InstancedBasic32);
    pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX W = World;

	Effects::BuildShadowMapInstancedFX->SetEyePosW(d3d->m_Cam.GetPosition());

	ID3DX11EffectTechnique* Tech;

	Tech = Effects::BuildShadowMapInstancedFX->BuildShadowMapTech;
	

	UINT stride[2] = {sizeof(Vertex::Basic32), sizeof(InstancedData)};
    UINT offset[2] = {0,0};

	ID3D11Buffer* vbs[2] = {*mModel_MeshData.VertexBuffer, *mModel_MeshData.InstancedBuffer};
 
	
	pDeviceContext->RSSetState(0);

	D3DX11_TECHNIQUE_DESC techDesc;
    Tech->GetDesc(&techDesc);

    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		for (UINT i = 0; i < mModel_MeshData.SubsetCount; ++i)
	    {   
			pDeviceContext->IASetVertexBuffers(0, 2, vbs, stride, offset);
			pDeviceContext->IASetIndexBuffer(*mModel_MeshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		    XMMATRIX TexTransform = XMMatrixIdentity();

		    Effects::BuildShadowMapInstancedFX->SetWorld(W);
			Effects::BuildShadowMapInstancedFX->SetViewProj(viewProj);
		    Effects::BuildShadowMapInstancedFX->SetWorldInvTranspose(worldInvTranspose);
	        Effects::BuildShadowMapInstancedFX->SetTexTransform(TexTransform);

			Effects::InstancedBasicFX->SetDiffuseMap(*mModel_MeshData.DiffuseMapSRV[i]);

			Tech->GetPassByIndex(p)->Apply(0, pDeviceContext);

			pDeviceContext->DrawIndexedInstanced(mModel_MeshData.subsetTable[i]->FaceCount * 3, 
				NumVisibleObjects, mModel_MeshData.subsetTable[i]->FaceStart * 3,  0, 0);
	    }
	 }
}

void Tree::DrawFarModelToShadowMap(CXMMATRIX World, CXMMATRIX viewProj, CXMMATRIX worldInvTranspose)
{
	if ((farModel_NumVisibleObjects + 1) == 0)
		return;

	pDeviceContext->IASetInputLayout(InputLayouts::InstancedBasic32);
    pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX W = World;

	Effects::BuildShadowMapInstancedFX->SetEyePosW(d3d->m_Cam.GetPosition());

	ID3DX11EffectTechnique* Tech;

	Tech = Effects::BuildShadowMapInstancedFX->BuildShadowMapTech;
	

	pDeviceContext->RSSetState(0);

	UINT stride[2] = {sizeof(Vertex::Basic32), sizeof(InstancedData)};
    UINT offset[2] = {0,0};

	ID3D11Buffer* vbs[2] = {*far_mModel_MeshData.VertexBuffer, *far_mModel_MeshData.InstancedBuffer};
 

	D3DX11_TECHNIQUE_DESC techDesc;
    Tech->GetDesc(&techDesc);

    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		for (UINT i = 0; i < far_mModel_MeshData.SubsetCount; ++i)
	    {   
			pDeviceContext->IASetVertexBuffers(0, 2, vbs, stride, offset);
			pDeviceContext->IASetIndexBuffer(*far_mModel_MeshData.IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		    XMMATRIX TexTransform = XMMatrixIdentity();

		    Effects::BuildShadowMapInstancedFX->SetWorld(W);
			Effects::BuildShadowMapInstancedFX->SetViewProj(viewProj);
		    Effects::BuildShadowMapInstancedFX->SetWorldInvTranspose(worldInvTranspose);
	        Effects::BuildShadowMapInstancedFX->SetTexTransform(TexTransform);

			Effects::InstancedBasicFX->SetDiffuseMap(*far_mModel_MeshData.DiffuseMapSRV[i]);

			Tech->GetPassByIndex(p)->Apply(0, pDeviceContext);

			pDeviceContext->DrawIndexedInstanced(far_mModel_MeshData.subsetTable[i]->FaceCount * 3, 
				farModel_NumVisibleObjects, far_mModel_MeshData.subsetTable[i]->FaceStart * 3,  0, 0);
	    }
	 }
}

void Tree::Draw(CXMMATRIX W)
{
	XMMATRIX ShadowTransform = XMLoadFloat4x4(&mShadowTransform);
	XMMATRIX WorldInvTranspose = MathHelper::InverseTranspose(W);

	XMMATRIX view     = d3d->m_Cam.View();
	XMMATRIX proj     = d3d->m_Cam.Proj();
	XMMATRIX viewProj = d3d->m_Cam.ViewProj();

	DrawNearModel(W, ShadowTransform, viewProj, WorldInvTranspose);
	DrawFarModel(W, ShadowTransform, viewProj, WorldInvTranspose);
}

void Tree::DrawToShadowMap(CXMMATRIX W)
{

	XMMATRIX WorldInvTranspose = MathHelper::InverseTranspose(W);
	
	XMMATRIX view = XMLoadFloat4x4(&d3d->m_LightView);
	XMMATRIX proj = XMLoadFloat4x4(&d3d->m_LightProj);
	XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	DrawNearModelToShadowMap(W, viewProj, WorldInvTranspose);
	DrawFarModelToShadowMap(W, viewProj, WorldInvTranspose);
}
*/