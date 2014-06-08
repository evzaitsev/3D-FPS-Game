#ifndef _WEAPON_RENDERER_H_
#define _WEAPON_RENDERER_H_


class Weapon
{
private:
	struct WeaponSettings
	{
		float x;
		float y;
		float z;
		float RotationY;
	};
private:
	//3D Model stuff
	SkinnedModel* mWeaponModel;
	WeaponSettings mWeaponSettings;

	//Animation stuff
	std::vector<XMFLOAT4X4> FinalTransforms;
	float TimePos;
	std::string AnimationClip;
	int AnimationIndex;
public:
	Weapon(std::string WeaponModelPath);
	~Weapon();

	virtual void Render();
	virtual void Update(float dt);
};

class AK47 : Weapon
{

public:
	AK47();
	~AK47();

	void Render();
};



#endif