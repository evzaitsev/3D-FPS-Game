#include "stdafx.h"

Weapon* Weapons::AK47 = 0;
WeaponManager Weapons::WeaponManager;

void Weapons::InitAll()
{
	AK47 = new Weapon("AK47", "Resources\\Models\\AK47.dae");


	WeaponManager.pushback_PrimaryWeapon(AK47->GetWeaponName(), AK47);
}

void Weapons::DestroyAll()
{
	//release if needed
}
