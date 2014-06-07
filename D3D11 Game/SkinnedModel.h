#ifndef _SKINNED_MODEL_H_
#define _SKINNED_MODEL_H_

class SkinnedModel
{
public:
	struct InitInfo
	{
		TextureMgr* Mgr;
<<<<<<< HEAD
	
=======
		XMFLOAT3 Scale;

>>>>>>> a9c11a661a28e1282702d16138bb6c56491de5b9
		bool UseDefaultMaterial;
		Material Material;
	};

public:
<<<<<<< HEAD
	SkinnedModel(const std::string& modelpath, InitInfo& info, bool Use32BitIndexFormat);
	~SkinnedModel();

	void Render(CXMMATRIX World, CXMMATRIX ViewProj);

private:
	void LoadSkinnedModel(const std::string& modelpath, bool& Use32BitFormat);

	void LoadTextures(aiMaterial* Mat);
	void LoadMaterials(aiMaterial* Mat);
	
public:
	SceneAnimator mSceneAnimator;

	std::vector<XMFLOAT4X4> FinalTransforms;
=======
	SkinnedModel(const std::string& modelpath, InitInfo& info);
	~SkinnedModel();

	void Render(CXMMATRIX World, CXMMATRIX ViewProj);
	void Update(float dt);

private:
	void LoadSkinnedModel16(const std::string& modelpath);
	void LoadTextures(aiMaterial* Mat);
	void LoadMaterials(aiMaterial* Mat);
>>>>>>> a9c11a661a28e1282702d16138bb6c56491de5b9
private:
	DirectionalLight Lights[3];
	PointLight PointLights[3];

<<<<<<< HEAD
	InitInfo mInfo;
	ModelData mModel;
	
=======
	float TimePos;

	InitInfo mInfo;
	ModelData mModel;
	SceneAnimator mSceneAnimator;

>>>>>>> a9c11a661a28e1282702d16138bb6c56491de5b9
	std::vector<Material> Materials;

	std::vector<ID3D11ShaderResourceView*> DiffuseMapSRV;
	std::vector<ID3D11ShaderResourceView*> NormalMapSRV;

<<<<<<< HEAD
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
	
	std::string Clip;
	XNA::AxisAlignedBox box;
	

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
=======

	std::vector<XMFLOAT4X4> FinalTransforms;
};


>>>>>>> a9c11a661a28e1282702d16138bb6c56491de5b9


#endif