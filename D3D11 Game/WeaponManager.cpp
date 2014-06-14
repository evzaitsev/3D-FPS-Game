#include "stdafx.h"


WeaponManager::WeaponManager()
{

}

WeaponManager::~WeaponManager()
{
	for (USHORT i = 0; i < mWeapons.size(); ++i)
	{
		SafeDelete(mWeapons[i]);
	}

	for (USHORT i = 0; i < mSecondaryWeapons.size(); ++i)
	{
		SafeDelete(mSecondaryWeapons[i]);
	}
}

Weapon* WeaponManager::GetPrimaryWeapon(UINT ArrayIndex)
{
	if (ArrayIndex >= mWeapons.size()) //invalid input
		return nullptr;

	return mWeapons[ArrayIndex];
}


Weapon* WeaponManager::GetSecondaryWeapon(UINT ArrayIndex)
{
	if (ArrayIndex >= mSecondaryWeapons.size()) //invalid input
		return nullptr;

	return mWeapons[ArrayIndex];
}


Weapon* WeaponManager::GetPrimaryWeaponByName(const std::string& name)
{
	for (USHORT i = 0; i < mPrimaryWeaponNames.size(); ++i)
	{
		if (mPrimaryWeaponNames[i] == name)
		{
			return mWeapons[i];
		}
	}

	return nullptr;
}

Weapon* WeaponManager::GetSecondaryWeaponByName(const std::string& name)
{
	for (USHORT i = 0; i < mSecondaryWeaponNames.size(); ++i)
	{
		if (mSecondaryWeaponNames[i] == name)
		{
			return mSecondaryWeapons[i];
		}
	}

	return nullptr;
}

void WeaponManager::pushback_PrimaryWeapon(const std::string& name, Weapon* weapon)
{
	mPrimaryWeaponNames.push_back(name);
	mWeapons.push_back(weapon);
}

void WeaponManager::pushback_SecondaryWeapon(const std::string& name, Weapon* weapon)
{
	mSecondaryWeaponNames.push_back(name);
	mSecondaryWeapons.push_back(weapon);
}

UINT WeaponManager::GetNumWeapons()
{
	return mWeapons.size() + mSecondaryWeapons.size();
}

UINT WeaponManager::GetNumPrimaryWeapons()
{
	return mWeapons.size();
}

UINT WeaponManager::GetNumSecondaryWeapons()
{
	return mSecondaryWeapons.size();
}