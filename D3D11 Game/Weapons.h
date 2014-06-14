#ifndef _WEAPONS_H_
#define _WEAPONS_H_

class Weapons
{
public:
	static void InitAll();
	static void DestroyAll();

	static WeaponManager WeaponManager;
private:
	static Weapon* AK47;
};


#endif