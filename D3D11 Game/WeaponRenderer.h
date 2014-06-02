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
	Model* mWeaponModel;
	WeaponSettings mWeaponSettings;
public:
	Weapon(std::string WeaponModelPath);
	~Weapon();

	void Render();
};




#endif