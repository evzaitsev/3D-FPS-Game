#ifndef _SKINNED_MODEL_H_
#define _SKINNED_MODEL_H_

class SkinnedModel
{
public:
	struct InitInfo
	{
		TextureMgr* Mgr;
		XMFLOAT3 Scale;

		bool UseDefaultMaterial;
		Material Material;
	};

public:
	SkinnedModel(const std::string& modelpath, InitInfo& info);
	~SkinnedModel();

	void Render(CXMMATRIX World, CXMMATRIX ViewProj);
	void Update(float dt);

private:
	void LoadSkinnedModel16(const std::string& modelpath);
	void LoadTextures(aiMaterial* Mat);
	void LoadMaterials(aiMaterial* Mat);
private:
	DirectionalLight Lights[3];
	PointLight PointLights[3];

	float TimePos;

	InitInfo mInfo;
	ModelData mModel;
	SceneAnimator mSceneAnimator;

	std::vector<Material> Materials;

	std::vector<ID3D11ShaderResourceView*> DiffuseMapSRV;
	std::vector<ID3D11ShaderResourceView*> NormalMapSRV;


	std::vector<XMFLOAT4X4> FinalTransforms;
};




#endif