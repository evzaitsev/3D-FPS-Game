// D3D11 Game.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "D3D11 Game.h"


BOOL WINAPI WinMain(HINSTANCE hInst,
					HINSTANCE hPrevInstance,
					LPSTR lpCmdline,
					int nCmdShow)
{
	Direct3D app(800, 600);

	d3d = &app;

	return d3d->Run(hInst);
}
