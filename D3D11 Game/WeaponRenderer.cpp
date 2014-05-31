#include "stdafx.h"

Weapon::Weapon(std::string path)
{

	Material DefaultMat;

	DefaultMat.Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 0.5f);
	DefaultMat.Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	DefaultMat.Specular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);

	Model::InitInfo info;

	info.Material = DefaultMat;
	info.Mgr = &d3d->m_TextureMgr;
	info.UseDefaultMaterial = false;
	info.AlphaClip = false;
	info.BackfaceCulling = true;
	info.AlphaToCoverage = false;
	info.InstanceFrustumCulling = true;
	info.UseNormalsFromModel = true;
	info.Scale = XMFLOAT3(0.085f, 0.085f, 0.085f);
	info.technique = Effects::BasicFX->Light1TexTech;


	//using only diffuse map will save the memory but will affect the
	//quality
	mWeaponModel = new Model(path, info, false, false, false, false);

}

Weapon::~Weapon()
{
	SafeDelete(mWeaponModel);
}

void Weapon::Render()
{
	XMMATRIX WeaponWorld = XMMatrixIdentity();
	XMMATRIX ViewMatrix  = d3d->m_Cam.View();
	XMMATRIX CameraWorld = XMMatrixInverse(&XMMatrixDeterminant(ViewMatrix), ViewMatrix);
	XMMATRIX ViewProj = d3d->m_Cam.ViewProj();

	XMMATRIX Rotation = XMMatrixRotationY(XMConvertToRadians(180));
	XMMATRIX Translation =  XMMatrixTranslation(3.0f, -1.0f, 4.0f);

	WeaponWorld =  Rotation*Translation*CameraWorld;

	INT status = 2;

	mWeaponModel->SetModelVisibleStatus(status);
	mWeaponModel->Update(WeaponWorld);
	mWeaponModel->Render(WeaponWorld, ViewProj);

}
