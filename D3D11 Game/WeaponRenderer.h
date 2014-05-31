#ifndef _WEAPON_RENDERER_H_
#define _WEAPON_RENDERER_H_


class Weapon
{
private:
	Model* mWeaponModel;
public:
	Weapon(std::string WeaponModelPath);
	~Weapon();

	void Render();
};




#endif