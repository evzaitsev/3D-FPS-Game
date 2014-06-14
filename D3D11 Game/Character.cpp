#include "stdafx.h"


Character::Character(WeaponManager* WeaponManager, btBroadphaseInterface* broadphase, btDynamicsWorld* dynamicsWorld)
	: mDynamicsWorld(dynamicsWorld),
	  mWeapon(nullptr),
	  mWeaponManager(WeaponManager),
	  CharacterHeight(31.5f)
{

	btTransform startTransform;
	startTransform.setIdentity();
	startTransform.setOrigin(btVector3(0, 1.0f, 0));

	mGhostObject = new btPairCachingGhostObject();
	mGhostObject->setWorldTransform(startTransform);

	broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

    btScalar characterWidth  = 1.75f;

	mCharacterShape = new btCapsuleShape(characterWidth, CharacterHeight);

	mGhostObject->setCollisionShape(mCharacterShape);
	mGhostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);

	btScalar stepHeight = btScalar(0.35f);
	
	mCharacter = new btKinematicCharacterController(mGhostObject, mCharacterShape, stepHeight);

	dynamicsWorld->addCollisionObject(mGhostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter|btBroadphaseProxy::DefaultFilter);

	dynamicsWorld->addAction(mCharacter);
	 
	mCharacter->setJumpSpeed(15.0f);
}

Character::~Character()
{
	mDynamicsWorld->removeCollisionObject(mGhostObject);

	SafeDelete(mCharacter);
	SafeDelete(mCharacterShape);
	SafeDelete(mGhostObject);
}

void Character::Draw()
{
	//we only need to show a gun in FPS games
	//can add extra things here such as bullets fired
}

void Character::Update(float dt)
{
	UpdateWeapon(dt);
	UpdateCharacter(dt);
}

void Character::DrawWeapon()
{
	mWeapon->Render();
}

void Character::UpdateWeapon(float& dt)
{
	mWeapon->Update(dt);
}

void Character::UpdateCharacter(float& dt)
{
	const XMFLOAT3& Look = d3d->m_Cam.GetLook();
	const XMFLOAT3& Right = d3d->m_Cam.GetRight();

	const btVector3& forwardDir = btVector3(Look.x, Look.y, Look.z);
	const btVector3& RightVec = btVector3(Right.x, Right.y, Right.z);

	btVector3 walkDirection = btVector3(0.0f, 0.0f, 0.0f);
	const btScalar walkVelocity = 6.1f * 4.0f; 
	const btScalar walkSpeed = walkVelocity * dt;

	if( GetAsyncKeyState('W') & 0x8000 )
	{
		walkDirection += forwardDir;
	}
	if( GetAsyncKeyState('S') & 0x8000 )
	{
		walkDirection -= forwardDir;	
	}

	if (GetAsyncKeyState('A') & 0x8000 )
	{
		walkDirection -= RightVec;
	}

	if (GetAsyncKeyState('D') & 0x8000 )
	{
		walkDirection += RightVec;
	}
			
	mCharacter->setWalkDirection(walkDirection * walkSpeed);

	static bool Jump = false;

	if (GetAsyncKeyState(VK_SPACE)&1)
	{
		Jump = true;
	}

	if (Jump)
	{
		if (mCharacter->canJump())
		{
			mCharacter->jump();
			Jump = false;
		}
	}

	btTransform trans = mGhostObject->getWorldTransform();

	CharacterHeight = trans.getOrigin().getY();

	d3d->m_Cam.SetPosition(trans.getOrigin().getX(), CharacterHeight, trans.getOrigin().getZ());

}

void Character::SetPrimaryWeaponModel(const std::string& name)
{
	mWeapon = mWeaponManager->GetPrimaryWeaponByName(name);

	if (mWeapon == nullptr)
		ShowError("Invalid weapon name");
}

void Character::SetPrimaryWeaponModel(UINT ArrayIndex)
{
	mWeapon = mWeaponManager->GetPrimaryWeapon(ArrayIndex);

	if (mWeapon == nullptr)
		ShowError("Invalid weapon index");
}

void Character::SetSecondaryWeaponModel(const std::string& name)
{
	mSecondaryWeapon = mWeaponManager->GetSecondaryWeaponByName(name);

	if (mSecondaryWeapon == nullptr)
		ShowError("Invalid weapon name");
}

void Character::SetSecondaryWeaponModel(UINT ArrayIndex)
{
	mSecondaryWeapon = mWeaponManager->GetSecondaryWeapon(ArrayIndex);

	if (mSecondaryWeapon == nullptr)
		ShowError("Invalid weapon index");
}