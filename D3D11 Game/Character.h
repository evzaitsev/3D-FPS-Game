#ifndef _CHARACTER_H_
#define _CHARACTER_H_

class Character
{
public:
	Character(WeaponManager* WeaponManager, btBroadphaseInterface* broadphase, btDynamicsWorld* dynamicsWorld);
	~Character();

	void SetPrimaryWeaponModel(const std::string& name);
	void SetPrimaryWeaponModel(UINT WeaponArrayIndex);

	void SetSecondaryWeaponModel(const std::string& name);
	void SetSecondaryWeaponModel(UINT WeaponArrayIndex);
	

	void Draw();
	void DrawWeapon(); 
	void Update(float dt);
	
private:
	void UpdateWeapon(float& dt);
	void UpdateCharacter(float& dt);

private:
	btDynamicsWorld* mDynamicsWorld;
	btPairCachingGhostObject* mGhostObject;
	btKinematicCharacterController* mCharacter;
	btConvexShape* mCharacterShape;

	//to be provided
	WeaponManager* mWeaponManager;

	Weapon* mWeapon;
	Weapon* mSecondaryWeapon;

	float CharacterHeight;
};

#endif