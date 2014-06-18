#ifndef _SKINNED_MODEL_H_
#define _SKINNED_MODEL_H_


class SkinnedModel
{
public:
	struct InitInfo
	{
		InitInfo()
		{
			Mgr = nullptr;
			UseDefaultMaterial = true;

			Scale = XMFLOAT3(1.0f, 1.0f, 1.0f);
		}

		TextureMgr* Mgr;
		XMFLOAT3 Scale;
	
		bool UseDefaultMaterial;
		Material Material;
	};

public:
	//Set FillIndices to false if you do not plan to do picking or collision using bullet physics
	SkinnedModel(const std::string& modelpath, InitInfo& info, bool Use32BitIndexFormat, bool FillIndices);
	~SkinnedModel();

	void SetWorld(CXMMATRIX World);

	void Render(CXMMATRIX ViewProj);

	INT Pick(int sx, int sy, XNA::AxisAlignedBox& box);

	void SetModelVisibleStatus(XNA::FrustumIntersection status);
	void SetShadowMap(ID3D11ShaderResourceView* srv);
	void SetShadowTransform(CXMMATRIX Transform);


#if defined(DEBUG) || defined(_DEBUG)
	UINT GetNumDrawCalls();
#endif

private:
	void LoadSkinnedModel(const std::string& modelpath, bool& Use32BitFormat, bool& FillIndices);

	void LoadTextures(aiMaterial* Mat);
	void LoadMaterials(aiMaterial* Mat);
	
public:
	SceneAnimator mSceneAnimator;

	std::vector<XMFLOAT3> vertices;
	std::vector<XMFLOAT4X4> FinalTransforms;

	INT* Indices;
private:
	DirectionalLight Lights[3];
	PointLight PointLights[3];

	XMFLOAT4X4 mWorld;

#if defined(DEBUG) || defined(_DEBUG)
	UINT mDrawCalls;
#endif

	XNA::FrustumIntersection mModelVisibleStatus;

	InitInfo mInfo;
	ModelData mModel;

	XMFLOAT4X4 mShadowTransform;
	
	std::vector<Material> Materials;

	std::vector<ID3D11ShaderResourceView*> DiffuseMapSRV;
	std::vector<ID3D11ShaderResourceView*> NormalMapSRV;


	ID3D11ShaderResourceView* mShadowMap;

};


struct SkinnedModelInstance
{
	SkinnedModelInstance()
	{
		SkinnedModel = nullptr;
		AnimationIndex = 0;

		XMMATRIX I = XMMatrixIdentity();

		XMStoreFloat4x4(&World, I);
	}
	
	XMMATRIX GetWorldXM()const
	{
		return XMLoadFloat4x4(&World);
	}

	FLOAT GetCurrentAnimationDuration()const
	{
		return SkinnedModel->mSceneAnimator.Animations[AnimationIndex].Duration;
	}

	UINT GetNumAnimations()const
	{
		return SkinnedModel->mSceneAnimator.Animations.size();
	}

	VOID SetWorld(CXMMATRIX W)
	{
		XMStoreFloat4x4(&World, W);
	}

	void AdjustAnimationSpeedBy(float percentage)
	{
		SkinnedModel->mSceneAnimator.AdjustAnimationSpeedBy(percentage);
	}

	void AdjustAnimationSpeedTo(float TicksPerSecond)
	{
		SkinnedModel->mSceneAnimator.AdjustAnimationSpeedTo(TicksPerSecond);
	}


	void DoubleSpeed()
	{
		SkinnedModel->mSceneAnimator.AdjustAnimationSpeedTo(SkinnedModel->mSceneAnimator.GetAnimationSpeed() * 2);
	}

	SkinnedModel* SkinnedModel;
	XMFLOAT4X4 World;

	float TimePos;
	int AnimationIndex;

	//sometimes there may be some animated model which do not move but stay static. For e.g trees
	//So we do not require to update AABB/Sphere of such models
	bool UpdateShape;
	
	std::string Clip;

	XNA::AxisAlignedBox box;
	XNA::Sphere sphere;

	void Update(float dt)
	{
		TimePos += dt;

		if (!Clip.empty())
		    SkinnedModel->mSceneAnimator.SetAnimation(Clip);
		else
			SkinnedModel->mSceneAnimator.SetAnimIndex(AnimationIndex);
	
		int index = SkinnedModel->mSceneAnimator.GetAnimationIndex();

		SkinnedModel->FinalTransforms = SkinnedModel->mSceneAnimator.GetTransforms(TimePos);

		if (TimePos > SkinnedModel->mSceneAnimator.Animations[index].Duration)
			TimePos = 0.0f;
	
	}

};


#endif