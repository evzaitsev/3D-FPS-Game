#include "stdafx.h"

Weapon::Weapon(std::string path)
{

	AnimationIndex = 0;

	if (!FileExists("Resources\\gunsettings.new"))
	{
		MessageBox(NULL, L"Error! gunsettings.new not found.\nCreating a new one with default settings. Click OK to continue", 
		L"Settings not found", MB_OK|MB_ICONERROR);

		//create/open file
		std::ofstream file("Resources\\gunsettings.new");

		//save default weapon configurations to file
		char weaponinfo[240];
		sprintf_s(weaponinfo, "%.4f %.4f %.4f\n%.2f", 1.0f, -1.0f, 2.0f, 550.0f);
		file << weaponinfo;

		//close file
		file.close();

	}

	std::ifstream fin("Resources\\gunsettings.new");

	fin >> mWeaponSettings.x >> mWeaponSettings.y >> mWeaponSettings.z;
	fin >> mWeaponSettings.RotationY;

	fin.close();

	Material DefaultMat;

	DefaultMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
	DefaultMat.Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	DefaultMat.Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);

	SkinnedModel::InitInfo info;

	info.Material = DefaultMat;
	info.Mgr = &d3d->m_TextureMgr;
	info.UseDefaultMaterial = false;

	//using only diffuse map will save the memory but will affect the
	//quality
	mWeaponModel = new SkinnedModel(path, info, false);

}

Weapon::~Weapon()
{
	SafeDelete(mWeaponModel);
}

void Weapon::Render()
{
	XMMATRIX ViewMatrix  = d3d->m_Cam.View();
	XMMATRIX CameraWorld = XMMatrixInverse(&XMMatrixDeterminant(ViewMatrix), ViewMatrix);
	XMMATRIX ViewProj = d3d->m_Cam.ViewProj();

	static float degree = mWeaponSettings.RotationY;
	static float x = mWeaponSettings.x;
	static float y = mWeaponSettings.y;
	static float z = mWeaponSettings.z;

#if defined(DEBUG) || defined(_DEBUG)

	if (GetAsyncKeyState('N')&1)
		--degree;
	if (GetAsyncKeyState('M')&1)
		++degree;
	
	if (GetAsyncKeyState(VK_RIGHT)&1)
		x += 0.1f;
	if (GetAsyncKeyState(VK_LEFT)&1)
		x -= 0.1f;

	if (GetAsyncKeyState(VK_UP)&1)
		z += 0.1f;
	if (GetAsyncKeyState(VK_DOWN)&1)
		z -= 0.1f;

	if (GetAsyncKeyState(VK_INSERT)&1)
		y += 0.1f;
	if (GetAsyncKeyState(VK_DELETE)&1)
		y -= 0.1f;

    POINT pos;

    pos.x = 20;
	pos.y = 40;

	wchar_t wchar[200]; 

	wsprintf(wchar, L"\nWeaponInfo:\n\nX: %d Y : %d Z : %d\nRotation : %d Degree", (int)x, (int)y, (int)z, (int)degree);

	if (GetAsyncKeyState('H')&1)
	{
		//create/open file
		std::ofstream file("Resources\\gunsettings.new");

		//save current weapon configurations to file
		char weaponinfo[400];
		sprintf_s(weaponinfo, "%.4f %.4f %.4f\n%.2f", x, y, z, degree);
		file << weaponinfo;

		//close file
		file.close();
	}

	d3d->DrawString(pos, wchar);
#endif

	
	XMMATRIX Rotation = XMMatrixRotationY(XMConvertToRadians(degree));
	XMMATRIX Translation =  XMMatrixTranslation(x, y, z);
	XMMATRIX Scaling = XMMatrixScaling(0.085f, 0.085f, 0.085f);

	XMMATRIX WeaponWorld =  Scaling * Rotation * Translation * CameraWorld;

	//weapon is always infront of camera so we don't need to check for its visibility
	INT status = 2; 

	mWeaponModel->Render(WeaponWorld, ViewProj);

}


void Weapon::Update(float dt)
{
	static bool Reload = false;
	static bool FirstTime = true;

	if (GetAsyncKeyState('R')&1)
	{
		Reload = true;
	}

	if (FirstTime)
	{
		FinalTransforms = mWeaponModel->mSceneAnimator.GetTransforms(0);

		FirstTime = false;
	}

	if (!Reload)
		return;

	TimePos += dt;

	mWeaponModel->mSceneAnimator.SetAnimIndex(AnimationIndex);

    int index = mWeaponModel->mSceneAnimator.GetAnimationIndex();

	mWeaponModel->FinalTransforms = mWeaponModel->mSceneAnimator.GetTransforms(TimePos);

	if (TimePos > mWeaponModel->mSceneAnimator.Animations[index].Duration)
	{
		TimePos = 0.0f;
		Reload = false;
	}
}