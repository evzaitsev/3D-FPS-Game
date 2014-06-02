// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#define BT_NO_SIMD_OPERATOR_OVERLOADS



// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <d3dx11.h>
#include "d3dx11Effect.h"
#include <xnamath.h>
#include <dxerr.h>
#include <cassert>
#include <ctime>
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <windowsx.h>
#include <list>
#include <stdio.h>
#include <Gdiplus.h>
#include <map>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// TODO: reference additional headers your program requires here

#include "Lighting.h"
#include "btBulletDynamicsCommon.h"
#include "D3D11 Game.h"
#include "d3dUtil.h"
#include "SpriteBatch.h"
#include "FontSheet.h"
#include "Timer.h"
#include "GeometryGenerator.h"
#include "XNACollision.h"
#include "Math.h"
#include "Camera.h"
#include "Effects.h"
#include "RenderStates.h"
#include "Vertex.h"
#include "Sky.h"
#include "ShadowMap.h"
#include "TerrainDX10.h"
#include "Terrain.h"
#include "TextureManager.h"
#include "Waves.h"
#include "Ssao.h"
#include "Model.h"
#include "trees.h"
#include "Grass.h"
#include "Map.h"
#include "gBuffers.h"
#include "WeaponRenderer.h"
#include "SkinnedData.h"
#include "SkinnedModel.h"
#include "Direct3D.h"
