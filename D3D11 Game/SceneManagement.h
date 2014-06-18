#ifndef _SCENE_MANAGEMENT_H_
#define _SCENE_MANAGEMENT_H_

#define MAX_POINT_LIGHTS 3
#define MAX_DIRECTIONAL_LIGHTS 3

//Goal: Making flexible shadow map system

namespace SceneManagement
{
	enum SHADOWMAP_TYPE
	{
		STATIC_SHADOW_MAP,
		DYNAMIC_SHADOW_MAP
	};
	
	struct ShadowInfo
	{

		ShadowInfo()
		{
			shadowMap = nullptr;
			ShadowType = STATIC_SHADOW_MAP;

			XMMATRIX I = XMMatrixIdentity();

			XMStoreFloat4x4(&LightView, I);
			XMStoreFloat4x4(&LightProj, I);
			XMStoreFloat4x4(&ShadowTransform, I);

			ShadowVolume.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
			ShadowVolume.Radius = 0.0f;

			LightDirection = XMFLOAT3(0.0f, -1.0f, 0.0f);
		}

		ShadowMap* shadowMap;

		SHADOWMAP_TYPE ShadowType;

		static const int ShadowMapResolution = 1024;
		
		XMFLOAT4X4 LightView;
		XMFLOAT4X4 LightProj;
		XMFLOAT4X4 ShadowTransform;

		XNA::Sphere ShadowVolume;

		//direction of light which is going to cast shadows
		XMFLOAT3 LightDirection;

		void BuildShadowTransforms();

		const XMMATRIX& GetShadowTransformXM();
	};

	class Scene
	{
	private:
		void SetupLights();
		void SetupShadowMap();
		void BindShadowMap();
		void UnBindShadowMap();
		void SetShadowMapForAllModels();
	public:
		Scene();
		~Scene();

		void AddStaticModel(ModelInstance& model);
		void AddSkinnedModel(SkinnedModelInstance& model);

		void ComputeAABBAndSphereForAllModels();

		void AddDirectionLight(DirectionalLight& light);
		void AddPointLight(PointLight& light);

		ID3D11ShaderResourceView* GetShadowMap();

		void Update(float& dt);

		void UpdateModelWorldMatrix(CXMMATRIX WorldMatrix, UINT ModelIndex);
		void UpdateSkinnedModelWorldMatrix(CXMMATRIX WorldMatrix, UINT ModelIndex);

		void DrawShadows(); 
		void DrawScene();

		void BuildShadowMapTransforms();

		void SetShadowMapType(SHADOWMAP_TYPE type);
		void SetShadowVolume(XNA::Sphere& SphereVolume);

		UINT GetNumDrawCalls()const;

		//for static models
		XNA::AxisAlignedBox& GetAABB(UINT ModelIndex);
		XNA::Sphere& GetSphere(UINT ModelIndex);

		//for skinned/dynamic models
		XNA::AxisAlignedBox& GetSkinnedModelAABB(UINT ModelIndex);
		XNA::Sphere& GetSkinnedModelSphere(UINT ModelIndex);

		XMMATRIX GetShadowTransform()const;

		void SetRenderTargetInfo(D3D11_VIEWPORT& viewport, ID3D11RenderTargetView* view, ID3D11DepthStencilView* depthstencilview);

	private:
		std::vector<ModelInstance> ModelInstances;
		std::vector<SkinnedModelInstance> SkinnedModelInstances;

		ShadowInfo ShadowMapInfo;

		DirectionalLight DirLights[MAX_POINT_LIGHTS];
		PointLight PointLights[MAX_DIRECTIONAL_LIGHTS];

		D3D11_VIEWPORT ViewPort;
		ID3D11RenderTargetView* RenderTargetView;
		ID3D11DepthStencilView* DepthStencilView;

		bool RenderTargetInfoProvided;
	};



}

#endif