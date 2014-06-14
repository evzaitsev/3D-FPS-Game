#ifndef _WEAPON_MANAGER_H_
#define _WEAPON_MANAGER_H_

//Use a weapon manager class to avoid duplication of each weapon for each character
//Weapon manager simply stores all weapons and it does not handle loading or rendering weapons

class Weapon;

class WeaponManager
{
private:
	WeaponManager(const WeaponManager& rhs);
	WeaponManager& operator=(const WeaponManager& rhs);
public:
	WeaponManager();
	~WeaponManager();
	
	void pushback_PrimaryWeapon(const std::string& name, Weapon* weapon);
	void pushback_SecondaryWeapon(const std::string& name, Weapon* weapon);

	Weapon* GetPrimaryWeapon(UINT ArrayIndex);
	Weapon* GetPrimaryWeaponByName(const std::string& name);

	Weapon* GetSecondaryWeapon(UINT ArrayIndex);
	Weapon* GetSecondaryWeaponByName(const std::string& name);

	UINT GetNumWeapons(); //returns sum of primary + secondary
	UINT GetNumPrimaryWeapons();  //returns number of primary weapons 
	UINT GetNumSecondaryWeapons(); //returns number of secondary weapons

private:
	std::vector<std::string> mPrimaryWeaponNames;
	std::vector<std::string> mSecondaryWeaponNames;

	std::vector<Weapon*> mWeapons;
	std::vector<Weapon*> mSecondaryWeapons;
};


#endif