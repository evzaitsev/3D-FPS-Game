#include "stdafx.h"

namespace SceneManagement
{
	Scene::Scene()
	{
		SetupLights();
		SetupShadowMap();

		RenderTargetInfoProvided = false;

		ShadowMapInfo.LightDirection = DirLights[0].Direction;
	}

	Scene::~Scene()
	{
		SafeDelete(ShadowMapInfo.shadowMap);
	}

	void Scene::SetupLights()
	{
		DirLights[0].Ambient  = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
	    DirLights[0].Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	    DirLights[0].Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 0.0f);
	    DirLights[0].Direction = XMFLOAT3(0.0, -1.0f, 0.0f);

	    DirLights[1].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	    DirLights[1].Diffuse  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	    DirLights[1].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	    DirLights[1].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	    DirLights[2].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	    DirLights[2].Diffuse  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	    DirLights[2].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	    DirLights[2].Direction = XMFLOAT3(-0.57735f, -0.57735f, -0.57735f);

		PointLights[0].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	    PointLights[0].Diffuse = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	    PointLights[0].Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	    PointLights[0].Range = 25.0f;
	    PointLights[0].Att   = XMFLOAT3(0.0f, 0.1f, 0.0f);
	    PointLights[0].Position = XMFLOAT3(0.0f, 0.0f, 0.0f);

	    PointLights[1].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	    PointLights[1].Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	    PointLights[1].Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	    PointLights[1].Range = 40.0f;
	    PointLights[1].Att   = XMFLOAT3(0.0f, 0.1f, 0.0f);
	    PointLights[1].Position = XMFLOAT3(0.0f, 10.0f, 0.0f);

	    PointLights[2].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	    PointLights[2].Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	    PointLights[2].Specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	    PointLights[2].Range = 60.0f;
	    PointLights[2].Att   = XMFLOAT3(0.0f, 0.1f, 0.0f);
	    PointLights[2].Position = XMFLOAT3(0.0f, 10.0f, 10.0f);
	}

	void Scene::SetupShadowMap()
	{
		ShadowMapInfo.shadowMap = new ShadowMap(pDevice, ShadowMapInfo.ShadowMapResolution, ShadowMapInfo.ShadowMapResolution);
	}

	void Scene::BuildShadowMapTransforms()
	{
		ShadowMapInfo.BuildShadowTransforms();
	}

	void Scene::AddStaticModel(ModelInstance& model)
	{
		ModelInstances.push_back(model);
	}

	void Scene::AddSkinnedModel(SkinnedModelInstance& model)
	{
		SkinnedModelInstances.push_back(model);
	}

	void Scene::AddDirectionLight(DirectionalLight& light)
	{
		static int index = 0;

		if (index >= MAX_DIRECTIONAL_LIGHTS)
			ShowError("Scene::AddDirectionLight exceeded limit of number of directional lights supported.");

		DirLights[index++] = light;

		if (index == 0)
			ShadowMapInfo.LightDirection = DirLights[index].Direction;
	}

	void Scene::AddPointLight(PointLight& light)
	{
		static int index = 0;

		if (index >= MAX_POINT_LIGHTS)
			ShowError("Scene::AddPointLight exceeded limit of number of point lights supported.");

		PointLights[index++] = light;
	}

	ID3D11ShaderResourceView* Scene::GetShadowMap()
	{
		return ShadowMapInfo.shadowMap->DepthMapSRV();
	}
	
	void Scene::UnBindShadowMap()
	{
		if (!RenderTargetInfoProvided)
			ShowError("Cannot unbind shadow map. Scene::SetRenderTargetInfo needs to be called before Scene::UnBindShadowMap is called.");

		ID3D11RenderTargetView* renderTargets[1] = {RenderTargetView};
	    pDeviceContext->OMSetRenderTargets(1, renderTargets, DepthStencilView);
	    pDeviceContext->RSSetViewports(1, &ViewPort);
	}

	void Scene::BindShadowMap()
	{
		ShadowMapInfo.shadowMap->BindDsvAndSetNullRenderTarget(pDeviceContext);
	}

	void Scene::SetRenderTargetInfo(D3D11_VIEWPORT& viewport, ID3D11RenderTargetView* view, ID3D11DepthStencilView* depthstencilview)
	{
		ViewPort = viewport;
		RenderTargetView = view;
		DepthStencilView = depthstencilview;

		RenderTargetInfoProvided = true;
	}

	void Scene::DrawShadows()
	{
		static bool FirstFrame = false;

		if (ShadowMapInfo.ShadowType == STATIC_SHADOW_MAP)
		{
			if (FirstFrame)
				return;
		}

		//make sure not to render more than once into static shadow map!
		if (FirstFrame && ShadowMapInfo.ShadowType == STATIC_SHADOW_MAP)
			ShowError("Error: Rendering more than once into static shadow map.");

		BindShadowMap();

		XMMATRIX view = XMLoadFloat4x4(&ShadowMapInfo.LightView);
	    XMMATRIX proj = XMLoadFloat4x4(&ShadowMapInfo.LightProj);
        XMMATRIX viewProj = XMMatrixMultiply(view, proj);

	    for (USHORT i = 0; i < ModelInstances.size(); ++i)
	    {
			XMMATRIX& W = ModelInstances[i].GetWorldXM();

		   if (ModelInstances[i].Model == nullptr)
			    continue;

		   if (ModelInstances[i].CastShadows == false)
			   continue;

		    if (ModelInstances[i].UseInstancing)
			    ModelInstances[i].Model->RenderInstancedShadowMap(W, viewProj);
			else
				ModelInstances[i].Model->RenderShadowMap(W, viewProj, (ShadowMapInfo.ShadowType == STATIC_SHADOW_MAP) ? true : false);
		}

		pDeviceContext->RSSetState(0);

		UnBindShadowMap();

		SetShadowMapForAllModels();

		FirstFrame = true;
	}

	void Scene::DrawScene()
	{
		XMMATRIX ViewProj = d3d->m_Cam.ViewProj();

		const XMMATRIX& ShadowTransform = ShadowMapInfo.GetShadowTransformXM();
		
		for (USHORT i = 0; i < ModelInstances.size(); ++i)
		{
			if (ModelInstances[i].Model == nullptr)
				continue;

			XMMATRIX& W = ModelInstances[i].GetWorldXM();

			XMMATRIX shadowTransform = W * ShadowTransform;

			ModelInstances[i].Model->SetShadowTransform(shadowTransform);
			ModelInstances[i].Model->SetDirLights(DirLights);
			ModelInstances[i].Model->SetPointLights(PointLights);

			if (ModelInstances[i].UseInstancing)
			{
				ModelInstances[i].UpdateInstanceData();
				ModelInstances[i].Model->RenderInstanced(W, ViewProj);
			}
			else
#ifndef _USE_DEFERRED_SHADING_
			    ModelInstances[i].Model->Render(W, ViewProj);
#else
			    ModelInstances[i].Model->RenderDeferred(W, ViewProj);
#endif
		}


		for (USHORT i = 0; i < SkinnedModelInstances.size(); ++i)
		{
			if (SkinnedModelInstances[i].SkinnedModel == nullptr)
			    continue;

			XMMATRIX& W = SkinnedModelInstances[i].GetWorldXM();

			XMMATRIX shadowTransform = W * ShadowTransform;

			SkinnedModelInstances[i].SkinnedModel->SetShadowTransform(shadowTransform);
		    SkinnedModelInstances[i].SkinnedModel->Render(ViewProj);
		}
	}

	void Scene::Update(float& dt)
	{
		if (ShadowMapInfo.ShadowType == DYNAMIC_SHADOW_MAP)
			ShadowMapInfo.BuildShadowTransforms();

		for (USHORT i = 0; i < ModelInstances.size(); ++i)
		{

			XMMATRIX& W = ModelInstances[i].GetWorldXM();

		    INT Intersect = d3d->IntersectSphereFrustum(&ModelInstances[i].sphere, W);
	  
		    if (Intersect == INTERSECT)
		    {
			    Intersect = d3d->IntersectAABBFrustum(&ModelInstances[i].box, W);
			    ModelInstances[i].Model->SetModelVisibleStatus(Intersect);
		    }
		    else
				ModelInstances[i].Model->SetModelVisibleStatus(Intersect);

		    ModelInstances[i].Model->Update(W);
		}

		for (USHORT i = 0; i < SkinnedModelInstances.size(); ++i)
		{
			if (SkinnedModelInstances[i].SkinnedModel == 0)
			    continue;

		    XMMATRIX& W = SkinnedModelInstances[i].GetWorldXM();

	
		    XNA::FrustumIntersection ModelVisibleStatus = d3d->IntersectAABBFrustum(&SkinnedModelInstances[i].box, W);

		    SkinnedModelInstances[i].SkinnedModel->SetWorld(W);
		    SkinnedModelInstances[i].SkinnedModel->SetModelVisibleStatus(ModelVisibleStatus);
		    SkinnedModelInstances[i].Update(dt);

		}
	}


	void Scene::ComputeAABBAndSphereForAllModels()
	{
		for (USHORT i = 0; i < ModelInstances.size(); ++i)
		{
			XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	        XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);
	
	        XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	        XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

			for (UINT j = 0; j < ModelInstances[i].Model->vertices.size(); ++j)
			{
				XMVECTOR P = XMLoadFloat3(&ModelInstances[i].Model->vertices[j]);
			
		        vMin = XMVectorMin(vMin, P);
		        vMax = XMVectorMax(vMax, P);
			}

			XMStoreFloat3(&ModelInstances[i].box.Center, 0.5f * (vMin + vMax)); 
		    XMStoreFloat3(&ModelInstances[i].box.Extents, 0.5f * (vMax - vMin)); 

		    XNA::ComputeBoundingSphereFromPoints(&ModelInstances[i].sphere, ModelInstances[i].Model->vertices.size(), 
				&ModelInstances[i].Model->vertices[0], sizeof(XMFLOAT3));

		    if (ModelInstances[i].UseInstancing)
		        ModelInstances[i].BuildInstanceData();
		}


		for (USHORT i = 0; i < SkinnedModelInstances.size(); ++i)
		{
			XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	        XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);
	
	        XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	        XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

			for (UINT j = 0; j < SkinnedModelInstances[i].SkinnedModel->vertices.size(); ++j)
			{
				XMVECTOR P = XMLoadFloat3(&SkinnedModelInstances[i].SkinnedModel->vertices[j]);
			
		        vMin = XMVectorMin(vMin, P);
		        vMax = XMVectorMax(vMax, P);
			}

			XMStoreFloat3(&SkinnedModelInstances[i].box.Center, 0.5f * (vMin + vMax)); 
		    XMStoreFloat3(&SkinnedModelInstances[i].box.Extents, 0.5f * (vMax - vMin)); 

			
		    XNA::ComputeBoundingSphereFromPoints(&SkinnedModelInstances[i].sphere, SkinnedModelInstances[i].SkinnedModel->vertices.size(), 
				&SkinnedModelInstances[i].SkinnedModel->vertices[0], sizeof(XMFLOAT3));
		}
	}


	void ShadowInfo::BuildShadowTransforms()
	{
		XMVECTOR lightDir = XMLoadFloat3(&LightDirection);
		XMVECTOR lightPos = -2.0f * ShadowVolume.Radius * lightDir;
	    XMVECTOR targetPos = XMLoadFloat3(&ShadowVolume.Center);
	    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	    XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

	    XMFLOAT3 sphereCenterLS;
	    XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

	    float l = sphereCenterLS.x - ShadowVolume.Radius;
	    float b = sphereCenterLS.y - ShadowVolume.Radius;
	    float n = sphereCenterLS.z - ShadowVolume.Radius;
	    float r = sphereCenterLS.x + ShadowVolume.Radius;
	    float t = sphereCenterLS.y + ShadowVolume.Radius;
	    float f = sphereCenterLS.z + ShadowVolume.Radius;
	    XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	    XMMATRIX T(
		   0.5f, 0.0f, 0.0f, 0.0f,
		   0.0f, -0.5f, 0.0f, 0.0f,
		   0.0f, 0.0f, 1.0f, 0.0f,
		   0.5f, 0.5f, 0.0f, 1.0f);

	    XMMATRIX S = V*P*T;

	    XMStoreFloat4x4(&LightView, V);
	    XMStoreFloat4x4(&LightProj, P);
	    XMStoreFloat4x4(&ShadowTransform, S);

	}

	void Scene::SetShadowMapType(SHADOWMAP_TYPE type)
	{
		ShadowMapInfo.ShadowType = type;
	}

	const XMMATRIX& ShadowInfo::GetShadowTransformXM()
	{
		return XMLoadFloat4x4(&ShadowTransform);
	}

	void Scene::SetShadowVolume(XNA::Sphere& SphereVolume)
	{
		ShadowMapInfo.ShadowVolume = SphereVolume;
	}

	void Scene::SetShadowMapForAllModels()
	{
		ID3D11ShaderResourceView* shadowMap = ShadowMapInfo.shadowMap->DepthMapSRV();

		for (UINT i = 0; i < ModelInstances.size(); ++i)
		{
			ModelInstances[i].Model->SetShadowMap(shadowMap);
		}

		for (UINT i = 0; i < SkinnedModelInstances.size(); ++i)
		{
			SkinnedModelInstances[i].SkinnedModel->SetShadowMap(shadowMap);
		}

	}

	void Scene::UpdateModelWorldMatrix(CXMMATRIX W, UINT ModelIndex)
	{
		ModelInstances[ModelIndex].StoreWorld(W);
	}

	void Scene::UpdateSkinnedModelWorldMatrix(CXMMATRIX W, UINT ModelIndex)
	{
		SkinnedModelInstances[ModelIndex].SetWorld(W);
	}

	XNA::AxisAlignedBox& Scene::GetAABB(UINT ModelIndex)
	{
		return ModelInstances[ModelIndex].box;
	}

	XNA::Sphere& Scene::GetSphere(UINT ModelIndex)
	{
		return ModelInstances[ModelIndex].sphere;
	}

	XNA::AxisAlignedBox& Scene::GetSkinnedModelAABB(UINT ModelIndex)
	{
		return SkinnedModelInstances[ModelIndex].box;
	}

	XNA::Sphere& Scene::GetSkinnedModelSphere(UINT ModelIndex)
	{
		return SkinnedModelInstances[ModelIndex].sphere;
	}

	XMMATRIX Scene::GetShadowTransform()const
	{
		return XMLoadFloat4x4(&ShadowMapInfo.ShadowTransform);
	}

	UINT Scene::GetNumDrawCalls()const
	{
		UINT DrawCalls = 0;

		for (UINT i = 0; i < ModelInstances.size(); ++i)
			DrawCalls += ModelInstances[i].Model->GetNumDrawCalls();
		
		for (UINT i = 0; i < SkinnedModelInstances.size(); ++i)
			DrawCalls += SkinnedModelInstances[i].SkinnedModel->GetNumDrawCalls();

		return DrawCalls;
	}
};