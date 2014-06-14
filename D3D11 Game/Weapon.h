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
	SkinnedModel* WeaponModel;
	WeaponSettings WeaponSettings;


	float TimePos;
	int AnimationIndex;

	std::vector<XMFLOAT4X4> FinalTransforms;
	std::string AnimationClip;
	std::string WeaponName; 
public:
	Weapon(const std::string& WeaponName, const std::string& WeaponModelPath);
	~Weapon();


	const std::string& GetWeaponName();

	virtual void Render();
	virtual void Update(float dt);

};

#endif