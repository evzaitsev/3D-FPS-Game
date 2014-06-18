#include "stdafx.h"
#include "D3D11 Game.h"


BOOL WINAPI WinMain(HINSTANCE hInst,
					HINSTANCE hPrevInstance,
					LPSTR lpCmdline,
					int nCmdShow)
{
	Direct3D app;

	d3d = &app;

	return d3d->Run(hInst);
}
