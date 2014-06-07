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
<<<<<<< HEAD
	//3D Model stuff
	SkinnedModel* mWeaponModel;
	WeaponSettings mWeaponSettings;

	//Animation stuff
	std::vector<XMFLOAT4X4> FinalTransforms;
	float TimePos;
	std::string AnimationClip;
	int AnimationIndex;
=======
	Model* mWeaponModel;
	WeaponSettings mWeaponSettings;
>>>>>>> a9c11a661a28e1282702d16138bb6c56491de5b9
public:
	Weapon(std::string WeaponModelPath);
	~Weapon();

	void Render();
	void Update(float dt);
};




#endif