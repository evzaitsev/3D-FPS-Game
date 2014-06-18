#include "stdafx.h"


//==================================================
//A free 3D game under construction by newtechnology
//Current Status: SSAO delayed until deferred shading is implemented
//Credits: Huge thanks to Frank D. Luna for his awesome book and gamedev.net
//Current goal: physics and collision
//==================================================

ID3D11Device* pDevice = 0;
ID3D11DeviceContext* pDeviceContext = 0;
Direct3D* d3d = 0;


LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return d3d->MsgProc(hWnd, msg, wParam, lParam);
}

Direct3D::Direct3D()
{
	
	m_Width = DEFAULT_WIDTH;
	m_Height = DEFAULT_HEIGHT;
	m_FullScreen = false;
	m_MultiSample = false;
	m_MultiSampleQuality = 0;
	m_AppPaused = false;
	m_RenderTargetView = nullptr;
	m_DepthStencilView = nullptr;
#if defined(DEBUG) || defined(_DEBUG)
	mScreenQuadIB = nullptr;
	mScreenQuadVB = nullptr;
#endif
	m_DepthStencilBuffer = nullptr;
	m_Maximize = false;
	m_Minimize = false;
	m_Resizing = false;
	m_freeCamera = false;
	m_fogEnabled = false;

	m_Cam.SetPosition(0.0f, 2.0f, -15.0f);
	ZeroMemory(&m_ViewPort, sizeof(D3D11_VIEWPORT));
}

Direct3D::Direct3D(int Width, int Height, bool FullScreen, bool Multisample)
{
	m_Width = Width;
	m_Height = Height;
	m_FullScreen = FullScreen;
	m_MultiSample = Multisample;
	m_MultiSampleQuality = 0;
	m_AppPaused = false;
	m_RenderTargetView = nullptr;
	m_DepthStencilView = nullptr;
#if defined(DEBUG) || defined(_DEBUG)
	mScreenQuadIB = nullptr;
	mScreenQuadVB = nullptr;
#endif
	m_DepthStencilBuffer = nullptr;
	m_Sky = nullptr;
	m_Smap = nullptr;
	//m_Ssao = nullptr;
	m_Maximize = false;
	m_Minimize = false;
	m_Resizing = false;
	m_freeCamera = false;
	m_fogEnabled = false;
	
	m_Cam.SetPosition(0.0f, 2.0f, -15.0f);

	ZeroMemory(&m_ViewPort, sizeof(D3D11_VIEWPORT));
}

Direct3D::Direct3D(int Width, int Height)
{
	m_Width = Width;
	m_Height = Height;
	m_FullScreen = false;
	m_MultiSample = false;
	m_MultiSampleQuality = 0;
	m_AppPaused = false;
	m_RenderTargetView = nullptr;
	m_DepthStencilView = nullptr;
	m_DepthStencilBuffer = nullptr;
#if defined(DEBUG) || defined(_DEBUG)
	mScreenQuadIB = nullptr;
	mScreenQuadVB = nullptr;
#endif
	m_Character = nullptr;
	m_Sky = nullptr;
	m_Smap = nullptr;
	//m_Ssao = nullptr;
	m_Maximize = false;
	m_Minimize = false;
	m_Resizing = false;
	m_freeCamera = false;
	m_fogEnabled = false;

	m_Cam.SetPosition(0.0f, 0.0f, -15.0f);

	ZeroMemory(&m_ViewPort, sizeof(D3D11_VIEWPORT));
}

Direct3D::~Direct3D()
{
	if (pDeviceContext)
		pDeviceContext->ClearState();

	ReleaseCOM(pDevice);
	ReleaseCOM(pDeviceContext);
	ReleaseCOM(m_RenderTargetView);
	ReleaseCOM(m_DepthStencilView);
	ReleaseCOM(m_SwapChain);
	ReleaseCOM(m_DepthStencilBuffer);

#if defined(DEBUG) || defined(_DEBUG)
	ReleaseCOM(mScreenQuadIB);
	ReleaseCOM(mScreenQuadVB);
#endif

	SafeDelete(m_Sky);
	//SafeDelete(m_Smap);
	SafeDelete(m_Character);
	SafeDelete(SponzaScene);
	SafeDelete(m_Tiny);

#ifdef _USE_DEFERRED_SHADING_
	SafeDelete(m_GBuffers);
#endif
    //SafeDelete(m_Ssao);

	SafeDelete(m_Sponza);

	SafeDelete(m_indexVertexArrays);

    m_dynamicsWorld->removeRigidBody(m_groundRigidBody);

    delete m_groundRigidBody->getMotionState();


	SafeDelete(m_groundRigidBody);
	SafeDelete(m_groundShape);
	SafeDelete(m_dynamicsWorld);
	SafeDelete(m_solver);
	SafeDelete(m_collisionConfiguration);
	SafeDelete(m_dispatcher);
	SafeDelete(m_broadphase); 


	Effects::DestroyAll();
	InputLayouts::DestroyAll();
	RenderStates::DestroyAll();
	Weapons::DestroyAll();

}


void Direct3D::InitPhysics()
{
	
      m_broadphase = new btDbvtBroadphase();

      m_collisionConfiguration = new btDefaultCollisionConfiguration();
      m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

      m_solver = new btSequentialImpulseConstraintSolver();

      m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver,
		  m_collisionConfiguration);

      m_dynamicsWorld->setGravity(btVector3(0, -10.0f, 0));

	  int numtri = m_Sponza->GetNumTriangles();

	  m_indexVertexArrays = new btTriangleIndexVertexArray(numtri,
		  m_Sponza->Indices,
		  3 * sizeof(int),
		  m_Sponza->vertices.size(), (btScalar*) &m_Sponza->vertices[0].x, sizeof(XMFLOAT3));

	  m_groundShape =  new btBvhTriangleMeshShape(m_indexVertexArrays, true, true);


      m_groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0)));

      btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, m_groundMotionState, m_groundShape, btVector3(0, 0, 0));
     
	  m_groundRigidBody = new btRigidBody(groundRigidBodyCI);
        
	  m_dynamicsWorld->addRigidBody(m_groundRigidBody);

	
}


void Direct3D::InitAllModels()
{
	mDirLights[0].Ambient  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mDirLights[0].Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(0.0, -1.0f, 0.0f);

	mDirLights[1].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	mDirLights[2].Ambient  = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse  = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(-0.57735f, -0.57735f, -0.57735f);

	ModelInstance m_SponzaInstance;
	SkinnedModelInstance m_TinyInstance;

	
	Material DefaultMat;

	DefaultMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
	DefaultMat.Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	DefaultMat.Specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);

	Model::InitInfo info;

	info.Material = DefaultMat;
	info.Mgr = &m_TextureMgr;
	info.UseDefaultMaterial = true;
	info.AlphaClip = false;
	info.BackfaceCulling = true;
	info.AlphaToCoverage = false;
	info.InstanceFrustumCulling = true;
	info.UseNormalsFromModel = true;
	info.Scale = XMFLOAT3(0.1f, 0.1f, 0.1f);
	info.technique = Effects::NormalMapFX->Light1PointLight0TexAOSpecTech;


	//using only diffuse map will save the memory but will affect the
	//quality
	m_Sponza = new Model("Resources\\Models\\sponza.obj", info, true, true, true, true, true);

	
	XMMATRIX World = XMMatrixIdentity();

	m_SponzaInstance.UseInstancing = false;
	m_SponzaInstance.ComputeSSAO = false;
	m_SponzaInstance.CastShadows = true;
	m_SponzaInstance.Model = m_Sponza;

	SkinnedModel::InitInfo SkinnedInfo;

	SkinnedInfo.Mgr = &m_TextureMgr;
	SkinnedInfo.Material = DefaultMat;
	SkinnedInfo.Scale = XMFLOAT3(0.01f, 0.01f, 0.01f);
	SkinnedInfo.UseDefaultMaterial = false;

	m_Tiny = new SkinnedModel("Resources\\Models\\tiny.x", SkinnedInfo, false, false);

	m_TinyInstance.SetWorld(XMMatrixScaling(0.01f, 0.01f, 0.01f));
	m_TinyInstance.Clip = "Walk";
	m_TinyInstance.SkinnedModel = m_Tiny;
	
	for (int i = 0; i < 5; ++i)
		m_TinyInstance.DoubleSpeed();

	SponzaScene->AddStaticModel(m_SponzaInstance);
	SponzaScene->AddSkinnedModel(m_TinyInstance);

	SponzaScene->ComputeAABBAndSphereForAllModels();
}

void Direct3D::InitCharacter()
{
	//Init weapons before player
	Weapons::InitAll();

	m_Character = new Character(&Weapons::WeaponManager, m_broadphase, m_dynamicsWorld);

	m_Character->SetPrimaryWeaponModel("AK47");
}

void Direct3D::Init()
{
		UINT createDeviceFlags = 0;
		static bool isDebug = false;

#if defined(DEBUG) || defined(_DEBUG)  
		isDebug = true;
#endif
		if (isDebug)
		   createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;

		createDeviceFlags |= D3D11_CREATE_DEVICE_SINGLETHREADED;
	

	HRESULT hr = D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		createDeviceFlags,
		NULL, NULL,
		D3D11_SDK_VERSION,
		&pDevice,
		&m_featureLevel,
		&pDeviceContext);

	if (FAILED(hr))
		ShowError("D3D11CreateDevice()\nFAILED");

	switch (m_featureLevel)
	{
	case D3D_FEATURE_LEVEL_9_1:
	case D3D_FEATURE_LEVEL_9_2:
	case D3D_FEATURE_LEVEL_9_3:
		ShowError("Sorry, your video card is too old to run this.");
		break;
	case D3D_FEATURE_LEVEL_10_0:
	case D3D_FEATURE_LEVEL_10_1:
		if (!isDebug)
		MessageBox(NULL, L"Your video card doesn't support DirectX 11 but the application can run DirectX 11 on older hardware with limited features\n"
			L"(you won't be able to use DirectX 11 features such as compute shader & tessellation).\nClick OK to continue.", 0, 0);
		break;

	case D3D_FEATURE_LEVEL_11_0:
		//nothing to do here
	break;

	default:
		ShowError("Your video card is uselss, buy a newone.");
		break;
	}

	HR(pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m_MultiSampleQuality));
	
	assert(m_MultiSampleQuality > 0);

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width  = m_Width;
	sd.BufferDesc.Height = m_Height;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (m_MultiSample)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m_MultiSampleQuality - 1;
	}

	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage  = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount  = 1;
	sd.OutputWindow = hWnd;
	sd.Windowed     = TRUE;
	sd.SwapEffect   = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags        = 0;

	
	IDXGIDevice* dxgiDevice = 0;
	HR(pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice));
	      
	IDXGIAdapter* dxgiAdapter = 0;
	HR(dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter));

	IDXGIFactory* dxgiFactory = 0;
	HR(dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory));

	HR(dxgiFactory->CreateSwapChain(pDevice, &sd, &m_SwapChain));

	
	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);

	OnResize();

	Effects::InitAll(pDevice);
	InputLayouts::InitAll(pDevice);
	RenderStates::InitAll(pDevice);

	if (!m_fogEnabled)
	m_Sky = new Sky(pDevice, L"Resources\\Textures\\sky.dds", 50000.0f);


#ifdef _USE_DEFERRED_SHADING_
	m_GBuffers = new gBuffers(pDevice, pDeviceContext, m_ViewPort, m_Width, m_Height);
#endif


	SponzaScene = new SceneManagement::Scene();
	
	InitAllModels();
	InitPhysics();
	InitCharacter();
	InitFont();

	XNA::Sphere vol;

	vol.Center = SponzaScene->GetSphere(0).Center;
	vol.Radius = SponzaScene->GetSphere(0).Radius;

	SponzaScene->SetShadowMapType(SceneManagement::STATIC_SHADOW_MAP);
	SponzaScene->SetShadowVolume(vol);
	SponzaScene->SetRenderTargetInfo(m_ViewPort, m_RenderTargetView, m_DepthStencilView);
	SponzaScene->BuildShadowMapTransforms();


	//m_Ssao = new Ssao(pDevice, pDeviceContext, m_Width, m_Height, m_Cam.GetFovY(), m_Cam.GetFarZ());

	BuildShadowTransform();

#if defined(DEBUG) || defined(_DEBUG)
	BuildScreenQuadGeometryBuffers();
#endif
}

void Direct3D::OnResize()
{
	assert(pDeviceContext);
	assert(pDevice);
	assert(m_SwapChain);

	ReleaseCOM(m_RenderTargetView);
	ReleaseCOM(m_DepthStencilView);
	ReleaseCOM(m_DepthStencilBuffer);

	HR(m_SwapChain->ResizeBuffers(1, m_Width, m_Height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
	ID3D11Texture2D* backbuffer;

	HR(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backbuffer)));
	HR(pDevice->CreateRenderTargetView(backbuffer, 0, &m_RenderTargetView));

	ReleaseCOM(backbuffer);

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = m_Width;
	depthStencilDesc.Height = m_Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;


	if (m_MultiSample)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m_MultiSampleQuality - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage          = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	HR(pDevice->CreateTexture2D(&depthStencilDesc, 0, &m_DepthStencilBuffer));
	HR(pDevice->CreateDepthStencilView(m_DepthStencilBuffer, 0, &m_DepthStencilView));

	pDeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

	m_ViewPort.TopLeftX = 0; 
	m_ViewPort.TopLeftY = 0;
	m_ViewPort.Width = static_cast<float>(m_Width);
	m_ViewPort.Height = static_cast<float>(m_Height);
	m_ViewPort.MinDepth = 0.0f;
	m_ViewPort.MaxDepth = 1.0f;

	pDeviceContext->RSSetViewports(1, &m_ViewPort);

	m_Cam.SetLens(0.25f * MathHelper::Pi, (FLOAT)m_Width/(FLOAT)m_Height, 0.1f, 30000.0f);

	XNA::ComputeFrustumFromProjection(&m_Frustum, &m_Cam.Proj());

	//if (m_Ssao)
		//m_Ssao->OnSize(m_Width, m_Height, m_Cam.GetFovY(), m_Cam.GetFarZ());

}

XNA::FrustumIntersection Direct3D::IntersectSphereFrustum(XNA::Sphere* sphere, CXMMATRIX world)
{
	XMVECTOR detView = XMMatrixDeterminant(m_Cam.View());
	XMMATRIX invView = XMMatrixInverse(&detView, m_Cam.View());

	XMMATRIX W = world;
	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

	XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);
	 
	XMVECTOR scale;
	XMVECTOR rotQuat;
	XMVECTOR translation;
	XMMatrixDecompose(&scale, &rotQuat, &translation, toLocal);

	XNA::Frustum localspaceFrustum;
	XNA::TransformFrustum(&localspaceFrustum, &m_Frustum, XMVectorGetX(scale), rotQuat, translation);

	INT intersect = XNA::IntersectSphereFrustum(sphere, &localspaceFrustum);

	if (intersect == 0)
		return OUTSIDE;
	else if (intersect == 1)
		return INTERSECT;
	else
		return INSIDE;
}

XNA::FrustumIntersection Direct3D::IntersectAABBFrustum(XNA::AxisAlignedBox* box, CXMMATRIX world)
{
	XMVECTOR detView = XMMatrixDeterminant(m_Cam.View());
	XMMATRIX invView = XMMatrixInverse(&detView, m_Cam.View());

	XMMATRIX W = world;
	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

	XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);
	 
	XMVECTOR scale;
	XMVECTOR rotQuat;
	XMVECTOR translation;
	XMMatrixDecompose(&scale, &rotQuat, &translation, toLocal);

	XNA::Frustum localspaceFrustum;
	XNA::TransformFrustum(&localspaceFrustum, &m_Frustum, XMVectorGetX(scale), rotQuat, translation);

    INT intersect = XNA::IntersectAxisAlignedBoxFrustum(box, &localspaceFrustum);

	if (intersect == 0)
		return OUTSIDE;
	else if (intersect == 1)
		return INTERSECT;
	else 
		return INSIDE;
}

void Direct3D::RestoreRenderTarget()
{
	ID3D11RenderTargetView* renderTargets[1] = {m_RenderTargetView};
	pDeviceContext->OMSetRenderTargets(1, renderTargets, m_DepthStencilView);
	pDeviceContext->RSSetViewports(1, &m_ViewPort);
}

void Direct3D::UpdateScene(float dt)
{
	m_dynamicsWorld->stepSimulation(1.0f/60.0f, 10.0f);

   
#ifdef USE_FREE_CAMERA_KEY
	if( GetAsyncKeyState('P') & 1 )
		m_freeCamera = !m_freeCamera;
#endif

#ifdef OUTPUT_POSITION_INFO
	if ( GetAsyncKeyState('I') & 1 )
	{
		char pos[170];
		XMFLOAT3 campos = m_Cam.GetPosition();

	    sprintf_s(pos, "X: %.2f Y: %.2f Z: %.2f\n", campos.x, campos.y, campos.z);
   		
		OutputDebugStringA(pos);
	}
#endif

	m_Cam.UpdateViewMatrix();

	SponzaScene->Update(dt);

	static float x = 0.0f, y = 0.0f, z = 0.0f;

	if (GetAsyncKeyState('G')&1)
		++x;
	if (GetAsyncKeyState('T')&1)
		++y;
	if (GetAsyncKeyState('Y')&1)
		++z;

	XMMATRIX TinyWorld = XMMatrixScaling(0.01f, 0.01f, 0.01f) * XMMatrixTranslation(x, y, z);

	SponzaScene->UpdateSkinnedModelWorldMatrix(TinyWorld, 0);

	m_Character->Update(dt);

#ifdef USE_FREE_CAMERA_KEY
	if (m_freeCamera)
		return;
#endif


	//XMFLOAT3 pos = m_Cam.GetPosition();
	//float y = m_Land.GetHeight(pos.x, pos.z) + 7.0f;
	//m_Cam.SetPosition(pos.x, y, pos.z);
}


void Direct3D::RenderToShadowMap()
{
	SponzaScene->DrawShadows();
}

#ifdef _USE_DEFERRED_SHADING_

//part of deferred shading
void Direct3D::Do2DPostProcessingOnScene()
{
	UINT stride = sizeof(Vertex::PosTex);
    UINT offset = 0;

	pDeviceContext->RSSetState(0);
	pDeviceContext->IASetInputLayout(InputLayouts::PosTex);
    pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pDeviceContext->IASetVertexBuffers(0, 1, &mScreenQuadVB, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(mScreenQuadIB, DXGI_FORMAT_R16_UINT, 0);
 
	XMMATRIX world = XMMatrixIdentity();

	ID3DX11EffectTechnique* tech = Effects::DeferredLightingFX->DeferredLightingTech;

	Effects::DeferredLightingFX->SetEyePosW(m_Cam.GetPosition());
	Effects::DeferredLightingFX->SetDirLight(mDirLights[0]);

	D3DX11_TECHNIQUE_DESC techDesc;

	tech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		Effects::DeferredLightingFX->SetWorldViewProj(world);
		Effects::DeferredLightingFX->SetColorMap(m_GBuffers->GetGBuffersSRV(0));
		Effects::DeferredLightingFX->SetNormalMap(m_GBuffers->GetGBuffersSRV(1));
		Effects::DeferredLightingFX->SetPositionMap(m_GBuffers->GetGBuffersSRV(2));

		tech->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(6, 0, 0);
    }
}

#endif

void Direct3D::DrawScene()
{

	RenderToShadowMap();


#ifdef _USE_DEFERRED_SHADING_
	m_GBuffers->SetRenderTargetView(m_DepthStencilView);

	DrawModels(false);


	RestoreRenderTarget();
#endif

	pDeviceContext->ClearRenderTargetView(m_RenderTargetView, reinterpret_cast<const float*>(&Colors::Silver));
	pDeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

#if defined(DEBUG) || defined(_DEBUG)
	if (GetAsyncKeyState('1') & 0x8000)
		pDeviceContext->RSSetState(RenderStates::WireframeRS);
#endif

	SponzaScene->DrawScene();

	m_Character->Draw();

#ifdef _USE_DEFERRED_SHADING_
	//Do lighting as a post processing step
	pDeviceContext->OMSetDepthStencilState(RenderStates::DisableDepthDDS, 0);
	
	Do2DPostProcessingOnScene();

	pDeviceContext->OMSetDepthStencilState(0, 0);
#endif

	m_Sky->Draw(pDeviceContext, m_Cam);

	pDeviceContext->RSSetState(0);
	pDeviceContext->OMSetDepthStencilState(0, 0);
	
#if defined(DEBUG) || defined(_DEBUG)
	wchar_t s[100];

	UINT TotalDrawCalls = SponzaScene->GetNumDrawCalls();

	wsprintf(s, L"DrawCalls:%d", TotalDrawCalls);
	
	POINT pos;
	pos.x = 20;
	pos.y = 20;

	d3d->DrawString(pos, s);

	if (GetAsyncKeyState('Z'))
	{
		DrawScreenQuad();
	}

#endif

	pDeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_Character->DrawWeapon();

	ID3D11ShaderResourceView* nullSRV[16] = { 0 };
	pDeviceContext->PSSetShaderResources(0, 16, nullSRV);

	HR(m_SwapChain->Present(0, 0));
}

void Direct3D::DrawString(POINT pos, const std::wstring text)
{
	float blendFactor[4] = {1.0f};
	pDeviceContext->OMSetBlendState(RenderStates::TransparentBS, blendFactor, 0xffffffff);

	// Calculate the text width.
	int textWidth = 0;
	for(UINT i = 0; i < text.size(); ++i)
	{
		WCHAR character = text[i];
		if(character == ' ') 
		{
			textWidth += m_Font.GetSpaceWidth();
		}
		else
		{
			const CD3D11_RECT& r = m_Font.GetCharRect(text[i]);
			textWidth += (r.right - r.left + 1);
		}
	}

	m_SpriteBatch.DrawString(pDeviceContext, m_Font, text, pos, XMCOLOR(0xffffffff));

	// restore default
	pDeviceContext->OMSetBlendState(0, blendFactor, 0xffffffff);

}

#if defined(DEBUG) || defined(_DEBUG)

void Direct3D::DrawScreenQuad()
{
	UINT stride = sizeof(Vertex::PosTex);
    UINT offset = 0;

	ID3D11DeviceContext* md3dImmediateContext = pDeviceContext;

	md3dImmediateContext->RSSetState(0);
	md3dImmediateContext->IASetInputLayout(InputLayouts::PosTex);
    md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dImmediateContext->IASetVertexBuffers(0, 1, &mScreenQuadVB, &stride, &offset);
	md3dImmediateContext->IASetIndexBuffer(mScreenQuadIB, DXGI_FORMAT_R16_UINT, 0);
 
	// Scale and shift quad to lower-right corner.
	XMMATRIX world(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.0f, 1.0f);

	ID3DX11EffectTechnique* tech = Effects::DebugTexFX->ViewRedTech;

	D3DX11_TECHNIQUE_DESC techDesc;

	tech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		Effects::DebugTexFX->SetWorldViewProj(world);

		Effects::DebugTexFX->SetTexture(d3d->GetShadowMap());

		tech->GetPassByIndex(p)->Apply(0, md3dImmediateContext);
		md3dImmediateContext->DrawIndexed(6, 0, 0);
    }
}



void Direct3D::BuildScreenQuadGeometryBuffers()
{
	GeometryGenerator::MeshData quad;

	GeometryGenerator geoGen;
	geoGen.CreateFullscreenQuad(quad);

	//
	// Extract the vertex elements we are interested in and pack the
	// vertices of all the meshes into one vertex buffer.
	//

	std::vector<Vertex::PosTex> vertices(quad.Vertices.size());
	std::vector<USHORT> indices(quad.Indices.size());


	for(UINT i = 0; i < quad.Vertices.size(); ++i)
	{
		vertices[i].Pos   = quad.Vertices[i].Position;
		vertices[i].Tex    = quad.Vertices[i].TexC;
	}

	for (UINT i = 0; i < quad.Indices.size(); ++i)
	{
		indices[i] = (USHORT)quad.Indices[i];
	}

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::PosTex) * vertices.size();
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = &vertices[0];
    HR(pDevice->CreateBuffer(&vbd, &vinitData, &mScreenQuadVB));

	//
	// Pack the indices of all the meshes into one index buffer.
	//

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(USHORT) * indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];
    HR(pDevice->CreateBuffer(&ibd, &iinitData, &mScreenQuadIB));
}

#endif


void Direct3D::BuildShadowTransform()
{
	// Only the first "main" light casts a shadow.
	XMFLOAT3 dir = XMFLOAT3(0.0, -1.0f, 0.0f);
	XMVECTOR lightDir = XMLoadFloat3(&dir);
	XMVECTOR lightPos = -2.0f * m_SceneBounds.Radius * lightDir;
	XMVECTOR targetPos = XMLoadFloat3(&m_SceneBounds.Center);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtLH(lightPos, targetPos, up);

	// Transform bounding sphere to light space.
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, V));

	// Ortho frustum in light space encloses scene.
	float l = sphereCenterLS.x - m_SceneBounds.Radius;
	float b = sphereCenterLS.y - m_SceneBounds.Radius;
	float n = sphereCenterLS.z - m_SceneBounds.Radius;
	float r = sphereCenterLS.x + m_SceneBounds.Radius;
	float t = sphereCenterLS.y + m_SceneBounds.Radius;
	float f = sphereCenterLS.z + m_SceneBounds.Radius;
	XMMATRIX P = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX S = V*P*T;

	XMStoreFloat4x4(&m_LightView, V);
	XMStoreFloat4x4(&m_LightProj, P);
	XMStoreFloat4x4(&m_ShadowTransform, S);
}

void Direct3D::InitFont()
{

	HR(m_Font.Initialize(pDevice, L"Times New Roman", 24.0f, FontSheet::FontStyleItalic, true));
	HR(m_SpriteBatch.Initialize(pDevice));

}

void Direct3D::CalcFPS()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	
	if( (m_Timer.TotalTime() - timeElapsed) >= 1.0f )
	{
		float fps = (float)frameCnt; 
		float mspf = 1000.0f / fps;

		std::wostringstream outs;   
		outs.precision(6);
		outs << "My FPS Game" << L"    "
			 << L"FPS: " << fps << L"    " 
			 << L"Frame Time: " << mspf << L" (ms)";
	
		SetWindowText(hWnd, outs.str().c_str());
		
		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void Direct3D::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_LastMousePos.x = x;
	m_LastMousePos.y = y;

	SetCapture(hWnd);
}


void Direct3D::OnMouseMove(WPARAM btnState, int x, int y)
{
	if( (btnState & MK_LBUTTON) != 0 )
	{
		// Make each pixel correspond to a quarter (1/4th i.e 0.25) of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - m_LastMousePos.y));

		m_Cam.Pitch(dy);
		m_Cam.RotateY(dx);
	}

	m_LastMousePos.x = x;
	m_LastMousePos.y = y;
}

void Direct3D::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}


BOOL WINAPI Direct3D::Run(HINSTANCE& hInstance)
{
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));

	wc.lpszClassName = L"WindowClass1";
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.style = CS_HREDRAW|CS_VREDRAW;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WindowProc;
	

	RegisterClassEx(&wc);

	hWnd = CreateWindowEx(
		NULL,
		L"WindowClass1",
		L"My FPS Game",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		DEFAULT_WIDTH,
		DEFAULT_HEIGHT,
		0,
		0,
		hInstance,
		0);

	ShowWindow(hWnd, SW_SHOW);

	MSG msg = {0};

	Init();

	m_Timer.Reset();

	while (msg.message != WM_QUIT)
	{ 

#ifdef ALLOW_FRAME_PAUSE_KEY
		if (GetAsyncKeyState('K') &1)
			m_AppPaused = !m_AppPaused;
#endif

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_Timer.Tick();

			if (!m_AppPaused)
			{
				UpdateScene(m_Timer.DeltaTime());
				DrawScene();
				CalcFPS();
			}
			else 
			{
				Sleep(350); 
			} 
		}
	}

	return (int)msg.wParam;
}


LRESULT CALLBACK Direct3D::MsgProc(HWND& hWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
			return 0;

	case WM_ACTIVATE:
		if( LOWORD(wParam) == WA_INACTIVE)
		{
			m_AppPaused = true;
			m_Timer.Stop();
		}
		else
		{
			m_AppPaused = false;
			m_Timer.Start();
		}
		return 0;


		case WM_SIZE:
		// Save the new client area dimensions.
			m_Width  = LOWORD(lParam);
			m_Height = HIWORD(lParam);
			if(pDevice)
		{
			if( wParam == SIZE_MINIMIZED )
			{
				m_AppPaused = true;
				m_Minimize = true;
				m_Maximize = false;
			}
			else if( wParam == SIZE_MAXIMIZED )
			{
				m_AppPaused = false;
				m_Minimize = false;
				m_Maximize = true;
				OnResize();
			}
			else if( wParam == SIZE_RESTORED )
			{
				
				// Restoring from minimized state?
				if(m_Minimize)
				{
					m_AppPaused = false;
					m_Minimize = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if(m_Maximize)
				{
					m_AppPaused = false;
					m_Maximize = false;
					OnResize();
				}
				else if(m_Resizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

		case WM_ENTERSIZEMOVE:
		m_AppPaused = true;
		m_Resizing  = true;
		m_Timer.Stop();
		return 0;

		case WM_EXITSIZEMOVE:
		m_AppPaused = false;
		m_Resizing  = false;
		m_Timer.Start();
		OnResize();
		return 0;

		case WM_MENUCHAR:
        // Don't beep when we alt-enter.
        return MAKELRESULT(0, MNC_CLOSE);

	// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200; 
		return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	
	return 0;
}

void Direct3D::GetScreenResolution(UINT& Width, UINT& Height)
{
	Width = m_Width;
	Height = m_Height;
}