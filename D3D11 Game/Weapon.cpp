#include "stdafx.h"

Weapon::Weapon(const std::string& name, const std::string& path)
	:
	AnimationIndex(0),
	WeaponName(name)
{

	if (!FileExists("Resources\\" + WeaponName + ".abhi"))
	{
		ShowError("Error! weapon settings for a particular weapon not found.\nCreating a new one with default settings.\nClick OK to continue");

		//create/open file
		std::ofstream file("Resources\\" + WeaponName + ".abhi");

		//save default weapon configurations to file
		char weaponinfo[240];
		sprintf_s(weaponinfo, "%.4f %.4f %.4f\n%.2f", 1.0f, -1.0f, 2.0f, 550.0f);
		file << weaponinfo;

		//close file
		file.close();

	}

	std::ifstream fin("Resources\\" + WeaponName + ".abhi");

	fin >> WeaponSettings.x >> WeaponSettings.y >> WeaponSettings.z;
	fin >> WeaponSettings.RotationY;

	fin.close();

	Material DefaultMat;

	DefaultMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
	DefaultMat.Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	DefaultMat.Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);

	SkinnedModel::InitInfo info;

	info.Material = DefaultMat;
	info.Mgr = &d3d->m_TextureMgr;
	info.Scale = XMFLOAT3(0.085f, 0.085f, 0.085f);
	info.UseDefaultMaterial = false;

	//using only diffuse map will save the memory but will affect the
	//quality
	WeaponModel = new SkinnedModel(path, info, false, false);


	//Gun is always infront of camera
	WeaponModel->SetModelVisibleStatus(INSIDE);
}

Weapon::~Weapon()
{
	SafeDelete(WeaponModel);
}

void Weapon::Render()
{
	XMMATRIX ViewMatrix  = d3d->m_Cam.View();
	XMMATRIX CameraWorld = XMMatrixInverse(&XMMatrixDeterminant(ViewMatrix), ViewMatrix);
	XMMATRIX ViewProj = d3d->m_Cam.ViewProj();

	static float degree = WeaponSettings.RotationY;
	static float x = WeaponSettings.x;
	static float y = WeaponSettings.y;
	static float z = WeaponSettings.z;

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

	wchar_t wchar[300]; 

	wsprintf(wchar, L"\nCurrent Weapon Info:\n\nX: %d Y : %d Z : %d\nRotation : %d Degree", (int)x, (int)y, (int)z, (int)degree);

	if (GetAsyncKeyState('H')&1)
	{
		//create/open file for saving settings
		std::ofstream file("Resources\\" + WeaponName + ".abhi");

		//write into file

		//allocate half byte
		char temp[512];

		sprintf_s(temp, "%.10f %.10f %.10f\n%.10f", x, y, z, degree);

		//write into file
		file << temp;

		//close file
		file.close();
	}

	d3d->DrawString(pos, wchar);
#endif

	
	XMMATRIX Rotation = XMMatrixRotationY(XMConvertToRadians(degree));
	XMMATRIX Translation =  XMMatrixTranslation(x, y, z);
	XMMATRIX Scaling = XMMatrixScaling(0.085f, 0.085f, 0.085f);

	XMMATRIX WeaponWorld =  Scaling * Rotation * Translation * CameraWorld;



	WeaponModel->SetWorld(WeaponWorld);
	WeaponModel->Render(ViewProj);

}


void Weapon::Update(float dt)
{
	static bool Reload = false;
	static bool FirstFrame = true;

	if (GetAsyncKeyState('R')&1)
	{
		Reload = true;
	}

	if (FirstFrame)
	{
		TimePos += dt;

		WeaponModel->mSceneAnimator.SetAnimIndex(AnimationIndex);

        int index = WeaponModel->mSceneAnimator.GetAnimationIndex();

	    WeaponModel->FinalTransforms = WeaponModel->mSceneAnimator.GetTransforms(TimePos);

		FirstFrame = false;
	}

	if (!Reload)
		return;

	TimePos += dt;

	WeaponModel->mSceneAnimator.SetAnimIndex(AnimationIndex);

    int index = WeaponModel->mSceneAnimator.GetAnimationIndex();

	WeaponModel->FinalTransforms = WeaponModel->mSceneAnimator.GetTransforms(TimePos);

	if (TimePos > WeaponModel->mSceneAnimator.Animations[index].Duration)
	{
		TimePos = 0.0f;
		Reload = false;
	}
}

const std::string& Weapon::GetWeaponName()
{
	return WeaponName;
}
