#ifndef _DIRECT3D_H_
#define _DIRECT3D_H_



class Direct3D 
{
private:
	POINT m_LastMousePos;
	D3D_FEATURE_LEVEL m_featureLevel;
	XMFLOAT4X4 m_treeWorld;
	UINT m_Width;
	UINT m_Height;
	bool m_FullScreen;
	bool m_Minimize;
	bool m_Maximize;
	bool m_AppPaused;
	bool m_MultiSample;
	bool m_Resizing;
	bool m_freeCamera;
	bool m_fogEnabled;
	UINT m_MultiSampleQuality;
	IDXGISwapChain* m_SwapChain;
	ID3D11Texture2D* m_DepthStencilBuffer;
	ID3D11RenderTargetView* m_RenderTargetView;
	ID3D11DepthStencilView* m_DepthStencilView;
	DirectionalLight mDirLights[3];
	D3D11_VIEWPORT m_ViewPort;
	HWND hWnd;
	BoundingSphere m_SceneBounds;

	FontSheet m_Font;
	SpriteBatch m_SpriteBatch;

	Model* m_Sponza;
	SkinnedModel* m_Tiny;
	
	Weapon* m_AK47;

	ID3D11Buffer* mScreenQuadVB;
	ID3D11Buffer* mScreenQuadIB;

	static const int SMapSize = 1024;

#ifdef _USE_DEFERRED_SHADING_
	gBuffers* m_GBuffers;
#endif

	std::vector<ModelInstance> ModelInstances;  
	std::vector<SkinnedModelInstance> SkinnedModelInstances;

	btTriangleIndexVertexArray*  m_indexVertexArrays;
	btBroadphaseInterface* m_broadphase;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btSequentialImpulseConstraintSolver* m_solver;
    btDiscreteDynamicsWorld* m_dynamicsWorld;
	btCollisionShape* m_groundShape;	
	btCollisionShape* m_fallShape;
	btDefaultMotionState* m_groundMotionState;
    btRigidBody* m_groundRigidBody;
    btDefaultMotionState* m_fallMotionState;
    btRigidBody* m_fallRigidBody;
public:
	XNA::Frustum m_Frustum;
	Camera m_Cam;
	GameTimer m_Timer;
	Sky* m_Sky;
	XMFLOAT4X4 m_LightView;
	XMFLOAT4X4 m_LightProj;
	XMFLOAT4X4 m_ShadowTransform;
	ShadowMap* m_Smap;
	//Ssao* m_Ssao;
	TextureMgr m_TextureMgr;


private:
	void UpdateInstancedModelsData();
	void DrawModelsToShadowMap();
	void DrawModelsToSsaoMap();
	void Init();
	void InitPhysics();
	void InitTerrainResources();
	void InitAllModels();
	void InitFont();
	void ComputeAABB();
	void DrawModels(bool ComputeSSAO);
	void DrawInstancedModels();
	void DrawInstancedModelsToShadowMap();
	void BuildShadowTransform();
	void RestoreRenderTarget();

	void Do2DPostProcessingOnScene();

	void RenderToShadowMap();

#if defined(DEBUG) || defined(_DEBUG)
	void DrawScreenQuad();
#endif

	void BuildScreenQuadGeometryBuffers();
public:
	LRESULT CALLBACK MsgProc(HWND& hWnd, UINT& msg, WPARAM& wParam, LPARAM& lParam);
	BOOL WINAPI Run(HINSTANCE& hInstance);

	Direct3D(int Width, int Height, bool FullScreen, bool EnableMultiSample);
	Direct3D(int Width, int Height);
	Direct3D(); 
	~Direct3D();

	ID3D11ShaderResourceView* GetShadowMap() { return m_Smap->DepthMapSRV(); }

	bool FogEnabled()const { return m_fogEnabled; }

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM state, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

	void DrawString(POINT pos, std::wstring string);

	XNA::FrustumIntersection IntersectAABBFrustum(XNA::AxisAlignedBox* box, CXMMATRIX world);
	XNA::FrustumIntersection IntersectSphereFrustum(XNA::Sphere* box, CXMMATRIX world);

	AxisAlignedBox MergeAABBs(AxisAlignedBox& box0, AxisAlignedBox& box1);

	void SetPauseStatus(bool Enabled);
	void OnResize();
	void CalcFPS();
	void UpdateScene(float dt);
	void DrawScene();
};

extern ID3D11Device* pDevice;
extern ID3D11DeviceContext* pDeviceContext;
extern Direct3D* d3d;


#endif