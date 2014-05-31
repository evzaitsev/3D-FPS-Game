#include "stdafx.h"
#include <list>

/*

TerrainDX10::TerrainDX10()
{
	mFX = nullptr;
	mLayer0 = nullptr;
	mLayer1 = nullptr;
	mLayer2 = nullptr;
	mLayer3 = nullptr;
	mLayer4 = nullptr;
	mBlendMap = nullptr;

	mLights[0].Ambient  = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mLights[0].Diffuse  = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	mLights[0].Specular = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mLights[0].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	mMaterial.Ambient = XMFLOAT4(0.45f, 0.45f, 0.45f, 1.0f);
	mMaterial.Diffuse = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	mMaterial.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

}

TerrainDX10::~TerrainDX10()
{
	ReleaseCOM(mFX);

	ReleaseCOM(mBlendMap);
	ReleaseCOM(mLayer0);
	ReleaseCOM(mLayer1);
	ReleaseCOM(mLayer2);
	ReleaseCOM(mLayer3);
	ReleaseCOM(mLayer4);
	
	for (USHORT i = 0; i < mSubGrid.size(); ++i)
	{
		ReleaseCOM(mSubGrid[i].VertexBuffer);
		ReleaseCOM(mSubGrid[i].IndexBuffer);
	}


}

void TerrainDX10::TranslateLight(FLOAT X, FLOAT Y, FLOAT Z)
{
	mLights[0].Direction.x += X;
	mLights[0].Direction.y += Y;
	mLights[0].Direction.z += Z;
}

UINT TerrainDX10::Triangles()const
{
	return mNumFaces;
}

UINT TerrainDX10::GetNumVertices()const 
{ 
	return mNumVertices;
}

float TerrainDX10::width()const
{
	return (mInfo.NumCols - 1) * mInfo.CellSpacing;
}

float TerrainDX10::depth()const
{
	return (mInfo.NumRows - 1) * mInfo.CellSpacing;
}

bool TerrainDX10::SubGrid::operator<(const SubGrid& rhs) const
{
	XMVECTOR boxCenter = XMLoadFloat3(&box.Center);
	XMVECTOR rhsCenter = XMLoadFloat3(&rhs.box.Center);

	XMVECTOR d1 = boxCenter - d3d->m_Cam.GetPositionXM();
	XMVECTOR d2 = rhsCenter - d3d->m_Cam.GetPositionXM();

	XMVECTOR f = XMVector3LengthSq(d1);
	XMVECTOR _f = XMVector3LengthSq(d2);

	FLOAT o, _o;

	XMStoreFloat(&o, f);
	XMStoreFloat(&_o, _f);

	return o < _o;
}

float TerrainDX10::getHeight(float x, float z)const
{
	// Transform from terrain local space to "cell" space.
	float c = (x + 0.5f*width()) /  mInfo.CellSpacing;
	float d = (z - 0.5f*depth()) / -mInfo.CellSpacing;

	// Get the row and column we are in.
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	// Grab the heights of the cell we are in.
	// A*--*B
	//  | /|
	//  |/ |
	// C*--*D
	float A = mHeightmap[row*mInfo.NumCols + col];
	float B = mHeightmap[row*mInfo.NumCols + col + 1];
	float C = mHeightmap[(row+1)*mInfo.NumCols + col];
	float D = mHeightmap[(row+1)*mInfo.NumCols + col + 1];

	// Where we are relative to the cell.
	float s = c - (float)col;
	float t = d - (float)row;

	// If upper triangle ABC.
	if( s + t <= 1.0f)
	{
		float uy = B - A;
		float vy = C - A;
		return A + s*uy + t*vy;
	}
	else // lower triangle DCB.
	{
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f-s)*uy + (1.0f-t)*vy;
	}

	return 0;
}

void TerrainDX10::BuildFX()
{
	std::ifstream fin("Resources\\Shaders\\TerrainDX10.fxo", std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();
	
	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 
		0, pDevice, &mFX));
}

void TerrainDX10::Init(const InitInfo& initInfo)
{
	BuildFX();

	if (d3d->FogEnabled())
	    mTech = mFX->GetTechniqueByName("TerrainFogLight1Tech");
	else
		mTech = mFX->GetTechniqueByName("TerrainLight1Tech");
	
	mfxWVPVar      = mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxWorldVar    = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxLayer0Var   = mFX->GetVariableByName("gLayer0")->AsShaderResource();
	mfxLayer1Var   = mFX->GetVariableByName("gLayer1")->AsShaderResource();
	mfxLayer2Var   = mFX->GetVariableByName("gLayer2")->AsShaderResource();
	mfxLayer3Var   = mFX->GetVariableByName("gLayer3")->AsShaderResource();
	mfxLayer4Var   = mFX->GetVariableByName("gLayer4")->AsShaderResource();
	mfxBlendMapVar = mFX->GetVariableByName("gBlendMap")->AsShaderResource();
	mfxWorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	mfxEyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	mfxFogColor = mFX->GetVariableByName("gFogColor")->AsVector();
	mfxFogStart = mFX->GetVariableByName("gFogStart")->AsScalar();
	mfxFogRange = mFX->GetVariableByName("gFogRange")->AsScalar();
	mfxLights = mFX->GetVariableByName("gDirLights");
	mfxMaterial = mFX->GetVariableByName("gMaterial");
	mfxShadowTransform   = mFX->GetVariableByName("gShadowTransform")->AsMatrix();
	mfxShadowMap         = mFX->GetVariableByName("gShadowMap")->AsShaderResource();

	mInfo = initInfo;

	mNumVertices = mInfo.NumRows * mInfo.NumCols;
	mNumFaces    = ((mInfo.NumRows - 1) * (mInfo.NumCols - 1) * 2);

	loadHeightMap();
	Smooth();

	BuildGeometry();

	HR(D3DX11CreateShaderResourceViewFromFileW(pDevice, initInfo.LayerMapFileName0.c_str(), 0, 0, &mLayer0, 0));
	HR(D3DX11CreateShaderResourceViewFromFileW(pDevice, initInfo.LayerMapFileName1.c_str(), 0, 0, &mLayer1, 0));
	HR(D3DX11CreateShaderResourceViewFromFileW(pDevice, initInfo.LayerMapFileName2.c_str(), 0, 0, &mLayer2, 0));
	HR(D3DX11CreateShaderResourceViewFromFileW(pDevice, initInfo.LayerMapFileName3.c_str(), 0, 0, &mLayer3, 0));
	HR(D3DX11CreateShaderResourceViewFromFileW(pDevice, initInfo.LayerMapFileName4.c_str(), 0, 0, &mLayer4, 0));
	HR(D3DX11CreateShaderResourceViewFromFileW(pDevice, initInfo.BlendMapFileName.c_str(), 0, 0, &mBlendMap, 0));

	
	mfxLayer0Var->SetResource(mLayer0);
	mfxLayer1Var->SetResource(mLayer1);
	mfxLayer2Var->SetResource(mLayer2);
	mfxLayer3Var->SetResource(mLayer3);
	mfxLayer4Var->SetResource(mLayer4);
	mfxBlendMapVar->SetResource(mBlendMap); 
}

void TerrainDX10::RenderSubGrid(SubGrid* grid)
{
	UINT stride = sizeof(Vertex::Basic32);
    UINT offset = 0;

	pDeviceContext->IASetVertexBuffers(0, 1, &grid->VertexBuffer, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(grid->IndexBuffer, DXGI_FORMAT_R16_UINT, 0); 

	D3DX11_TECHNIQUE_DESC techDesc;
    mTech->GetDesc(&techDesc);

    for(USHORT i = 0; i < techDesc.Passes; ++i)
    {
        ID3DX11EffectPass* pass = mTech->GetPassByIndex(i);
		pass->Apply(0, pDeviceContext);

		pDeviceContext->DrawIndexed(grid->NUM_INDICES, 0, 0);
	}	

}

void TerrainDX10::DrawShadowMap(CXMMATRIX world, Camera &cam)
{
	XMMATRIX viewProj = XMMatrixMultiply(XMLoadFloat4x4(&d3d->m_LightView), XMLoadFloat4x4(&d3d->m_LightProj));
	XMMATRIX W = world;
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(W);
	XMMATRIX WorldViewProj = W * viewProj;
	XMMATRIX TexTransform = XMMatrixIdentity();

	ID3DX11EffectTechnique* Tech = Effects::BuildShadowMapFX->BuildShadowMapTech;

	UINT stride = sizeof(Vertex::Basic32);
    UINT offset = 0;

	Effects::BuildShadowMapFX->SetEyePosW(cam.GetPosition());
	Effects::BuildShadowMapFX->SetViewProj(viewProj);

	D3DX11_TECHNIQUE_DESC techDesc;
	Tech->GetDesc(&techDesc);

		// Frustum cull sub-grids.
	std::list<SubGrid> visibleSubGrids;
	for(UINT i = 0; i < mSubGrid.size(); ++i)
	{
		if( d3d->IntersectAABBFrustum(&mSubGrid[i].box, W))
		{
			mSubGrid[i].Visible = true;
			visibleSubGrids.push_back(mSubGrid[i]);
		}
		else
			mSubGrid[i].Visible = false;
	}

	// Sort front-to-back from camera.
	visibleSubGrids.sort();

    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
		    
		for(std::list<SubGrid>::iterator iter = visibleSubGrids.begin(); iter != visibleSubGrids.end(); ++iter)
		{

		   	Effects::BuildShadowMapFX->SetWorld(W);
		    Effects::BuildShadowMapFX->SetWorldInvTranspose(worldInvTranspose);
			Effects::BuildShadowMapFX->SetWorldViewProj(WorldViewProj);
			Effects::BuildShadowMapFX->SetTexTransform(TexTransform);

			pDeviceContext->IASetVertexBuffers(0, 1, &iter->VertexBuffer, &stride, &offset);
			pDeviceContext->IASetIndexBuffer(iter->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

			ID3DX11EffectPass* pass = Tech->GetPassByIndex(p);
		    pass->Apply(0, pDeviceContext);

			pDeviceContext->DrawIndexed(mSubGrid[0].NUM_INDICES, 0, 0);
		}

	}
	 
	 
}

void TerrainDX10::draw(CXMMATRIX world, Camera& cam)
{
	XMMATRIX WorldInvTranspose = MathHelper::InverseTranspose(world);
	XMMATRIX WorldViewProj = world * cam.ViewProj();
	XMMATRIX ShadowTransform = world * XMLoadFloat4x4(&d3d->m_ShadowTransform);

	UINT stride = sizeof(Vertex::Basic32);
    UINT offset = 0;

	SetShadowMap(d3d->GetShadowMap());
	SetShadowTransform(ShadowTransform);

	if (d3d->FogEnabled())
	{
	    SetEyePosW(cam.GetPosition());
		SetFogColor(Colors::Silver);
	    SetFogStart(15.0f);
 	    SetFogRange(175.0f);

	    SetWorldInvTranspose(WorldInvTranspose);
	}
	

	mfxWVPVar->SetMatrix((float*)&WorldViewProj);
	mfxWorldVar->SetMatrix((float*)&world);
	mfxMaterial->SetRawValue(&mMaterial, 0, sizeof(Material));

	//we are not going to have more than one light
	mfxLights->SetRawValue(mLights, 0, 1 * sizeof(DirectionalLight));

	std::list<SubGrid> visibleSubGrids;
	for (USHORT i = 0; i < mSubGrid.size(); ++i)
	{
		if (d3d->IntersectAABBFrustum(&mSubGrid[i].box, world))
		{
			visibleSubGrids.push_back(mSubGrid[i]);
		}
	}

	visibleSubGrids.sort();

	for(std::list<SubGrid>::iterator grid = visibleSubGrids.begin(); grid != visibleSubGrids.end(); ++grid)
	{
	    pDeviceContext->IASetVertexBuffers(0, 1, &grid->VertexBuffer, &stride, &offset);
	    pDeviceContext->IASetIndexBuffer(grid->IndexBuffer, DXGI_FORMAT_R16_UINT, 0); 

	    D3DX11_TECHNIQUE_DESC techDesc;
        mTech->GetDesc(&techDesc);

        for(USHORT i = 0; i < techDesc.Passes; ++i)
            {
                ID3DX11EffectPass* pass = mTech->GetPassByIndex(i);
		        pass->Apply(0, pDeviceContext);

		        pDeviceContext->DrawIndexed(grid->NUM_INDICES, 0, 0);
	        }	


	}
		
}

void TerrainDX10::loadHeightMap()
{
	// A height for each vertex
	std::vector<unsigned char> in( mInfo.NumRows * mInfo.NumCols );

	// Open the file.
	std::ifstream inFile;
	inFile.open(mInfo.HeightmapFileName.c_str(), std::ios_base::binary);

	if(inFile)
	{
		// Read the RAW bytes.
		inFile.read((char*)&in[0], (std::streamsize)in.size());

		// Done with file.
		inFile.close();
	}

	// Copy the array data into a float array, and scale and offset the heights.
	mHeightmap.resize(mInfo.NumRows * mInfo.NumCols, 0);
	for(UINT i = 0; i < mInfo.NumRows * mInfo.NumCols; ++i)
	{
		mHeightmap[i] = (float)in[i] * mInfo.HeightScale + mInfo.HeightOffset;
	}
}


void TerrainDX10::Smooth()
{
	std::vector<float> dest( mHeightmap.size() );

	for(UINT i = 0; i < mInfo.NumRows; ++i)
	{
		for(UINT j = 0; j < mInfo.NumCols; ++j)
		{
			dest[i*mInfo.NumCols+j] = average(i,j);
		}
	}

	// Replace the old heightmap with the filtered one.
	mHeightmap = dest;
}



bool TerrainDX10::inBounds(UINT i, UINT j)
{
	// True if ij are valid indices; false otherwise.
	return 
		i >= 0 && i < mInfo.NumRows && 
		j >= 0 && j < mInfo.NumCols;
}

float TerrainDX10::average(UINT i, UINT j)
{
	// Function computes the average height of the ij element.
	// It averages itself with its eight neighbor pixels.  Note
	// that if a pixel is missing neighbor, we just don't include it
	// in the average--that is, edge pixels don't have a neighbor pixel.
	//
	// ----------
	// | 1| 2| 3|
	// ----------
	// |4 |ij| 6|
	// ----------
	// | 7| 8| 9|
	// ----------

	float avg = 0.0f;
	float num = 0.0f;

	for(UINT m = i-1; m <= i+1; ++m)
	{
		for(UINT n = j-1; n <= j+1; ++n)
		{
			if( inBounds(m,n) )
			{
				avg += mHeightmap[m*mInfo.NumCols + n];
				num += 1.0f;
			}
		}
	}

	return avg / num;
}

	
void TerrainDX10::ComputeSubGridIndices(std::vector<USHORT>& meshData)
{

	static const int& n = SubGrid::NUM_COLS;

	meshData.resize(SubGrid::NUM_INDICES); 

	UINT k = 0;
	for(UINT i = 0; i < SubGrid::NUM_ROWS-1; ++i)
	{
		for(UINT j = 0; j < SubGrid::NUM_COLS-1; ++j)
		{
			meshData[k]   = i*n+j;
			meshData[k+1] = i*n+j+1;
			meshData[k+2] = (i+1)*n+j;

			meshData[k+3] = (i+1)*n+j;
			meshData[k+4] = i*n+j+1;
			meshData[k+5] = (i+1)*n+j+1;

			k += 6; 
		}
	}

}



void TerrainDX10::BuildGeometry()
{
	//=========================================
	//               Build Vertices
	//=========================================

	std::vector<Vertex::Basic32> vertices(mNumVertices);
	std::vector<Vertex::Basic32> SubGridVertices(SubGrid::NUM_VERTEX);
	std::vector<USHORT> SubGridIndices(SubGrid::NUM_INDICES);


	float halfWidth = (mInfo.NumCols-1)*mInfo.CellSpacing*0.5f;
	float halfDepth = (mInfo.NumRows-1)*mInfo.CellSpacing*0.5f;

	float du = 1.0f / (mInfo.NumCols-1);
	float dv = 1.0f / (mInfo.NumRows-1);
	for(UINT i = 0; i < mInfo.NumRows; ++i)
	{
		float z = halfDepth - i*mInfo.CellSpacing;
		for(UINT j = 0; j < mInfo.NumCols; ++j)
		{
			float x = -halfWidth + j*mInfo.CellSpacing;

			float y = mHeightmap[i*mInfo.NumCols+j];
			vertices[i*mInfo.NumCols+j].Pos    = XMFLOAT3(x, y, z);
			vertices[i*mInfo.NumCols+j].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);

			// Stretch texture over grid.
			vertices[i*mInfo.NumCols+j].Tex.x = j*du;
			vertices[i*mInfo.NumCols+j].Tex.y = i*dv;
		}
	}
 
	// Estimate normals for interior nodes using central difference.
	float invTwoDX = 1.0f / (2.0f*mInfo.CellSpacing);
	float invTwoDZ = 1.0f / (2.0f*mInfo.CellSpacing);
	for(UINT i = 2; i < mInfo.NumRows-1; ++i)
	{
		for(UINT j = 2; j < mInfo.NumCols-1; ++j)
		{
			float t = mHeightmap[(i-1)*mInfo.NumCols + j];
			float b = mHeightmap[(i+1)*mInfo.NumCols + j];
			float l = mHeightmap[i*mInfo.NumCols + j - 1];
			float r = mHeightmap[i*mInfo.NumCols + j + 1];

			XMFLOAT3 tanZ(0.0f, (t-b)*invTwoDZ, 1.0f);
			XMFLOAT3 tanX(1.0f, (r-l)*invTwoDX, 0.0f);

			XMVECTOR N;
			N = XMVector3Cross(XMLoadFloat3(&tanZ), XMLoadFloat3(&tanX));
			N = XMVector3Normalize(N);
	
			XMStoreFloat3(&vertices[i*mInfo.NumCols+j].Normal, N);

		}
	}


	int subGridRows = (mInfo.NumRows-1) / (SubGrid::NUM_ROWS-1);
	int subGridCols = (mInfo.NumCols-1) / (SubGrid::NUM_COLS-1);

	for(int r = 0; r < subGridRows; ++r)
	{
		for(int c = 0; c < subGridCols; ++c)
		{
			// Rectangle that indicates (via matrix indices ij) the
			// portion of grid vertices to use for this subgrid.
			RECT R = 
			{
					c * (SubGrid::NUM_COLS - 1),
					r * (SubGrid::NUM_ROWS - 1),
				(c+1) * (SubGrid::NUM_COLS - 1),
				(r+1) * (SubGrid::NUM_ROWS - 1)
			};

			
			UINT k = 0;
			for (UINT i = R.top; i <= R.bottom; ++i)
			{
				for (UINT j = R.left; j <= R.right; ++j)
				{
					SubGridVertices[k++] = vertices[i*mInfo.NumCols+j];
				}
			}
    
			SubGrid g;

	        XMFLOAT3 vMinf3(+MathHelper::Infinity, +MathHelper::Infinity, +MathHelper::Infinity);
	        XMFLOAT3 vMaxf3(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);
	
	        XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	        XMVECTOR vMax = XMLoadFloat3(&vMaxf3);
	     
			for(UINT i = 0; i < SubGrid::NUM_VERTEX; ++i)
			{
		        XMVECTOR P = XMLoadFloat3(&SubGridVertices[i].Pos);
	
		        vMin = XMVectorMin(vMin, P);
		        vMax = XMVectorMax(vMax, P);
	        }
	

	        XMStoreFloat3(&g.box.Center, 0.5f * (vMin + vMax)); 
	        XMStoreFloat3(&g.box.Extents, 0.5f * (vMax - vMin)); 

			ComputeSubGridIndices(SubGridIndices);
		   

			D3D11_BUFFER_DESC vbd;
			vbd.Usage = D3D11_USAGE_IMMUTABLE;
			vbd.ByteWidth = sizeof(Vertex::Basic32) * SubGrid::NUM_VERTEX;
            vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            vbd.CPUAccessFlags = 0;
            vbd.MiscFlags = 0;

            D3D11_SUBRESOURCE_DATA vinitData;
	        vinitData.pSysMem = &SubGridVertices[0];

	        HR(pDevice->CreateBuffer(&vbd, &vinitData, &g.VertexBuffer));



	        D3D11_BUFFER_DESC ibd;
            ibd.Usage = D3D11_USAGE_IMMUTABLE; 
         	ibd.ByteWidth = sizeof(USHORT) * SubGrid::NUM_INDICES;
            ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
            ibd.CPUAccessFlags = 0;
            ibd.MiscFlags = 0;

	        D3D11_SUBRESOURCE_DATA iinitData;
	        iinitData.pSysMem = &SubGridIndices[0];

	        HR(pDevice->CreateBuffer(&ibd, &iinitData, &g.IndexBuffer)); 
		 
	        mSubGrid.push_back(g);

		}
	} 

/*
	//=======================================================
	//                     Build Indices
	//=======================================================

	std::vector<DWORD> indices(mNumFaces*3);

	int k = 0;
	for(UINT i = 0; i < mInfo.NumRows-1; ++i)
	{
		for(UINT j = 0; j < mInfo.NumCols-1; ++j)
		{
			indices[k]   = i*mInfo.NumCols+j;
			indices[k+1] = i*mInfo.NumCols+j+1;
			indices[k+2] = (i+1)*mInfo.NumCols+j;

			indices[k+3] = (i+1)*mInfo.NumCols+j;
			indices[k+4] = i*mInfo.NumCols+j+1;
			indices[k+5] = (i+1)*mInfo.NumCols+j+1;

			k += 6; 
		}
	} 


    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(Vertex::Basic32) * mNumVertices;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = &vertices[0];

	HR(pDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE; 
	ibd.ByteWidth = sizeof(DWORD) * mNumFaces * 3;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = &indices[0];

	HR(pDevice->CreateBuffer(&ibd, &iinitData, &mIB)); 
	*/
//}


