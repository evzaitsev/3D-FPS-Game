#include "stdafx.h"

TextureMgr::TextureMgr() 
{

}

TextureMgr::~TextureMgr()
{
	for(auto it = mTextureSRV.begin(); it != mTextureSRV.end(); ++it)
    {
		ReleaseCOM(it->second);
    }

	mTextureSRV.clear();
}


ID3D11ShaderResourceView* TextureMgr::CreateTexture(std::string filename)
{
	ID3D11ShaderResourceView* srv = 0;

	// Does it already exist?
	if( mTextureSRV.find(filename) != mTextureSRV.end() )
	{
		srv = mTextureSRV[filename];
	}
	else
	{
		HR(D3DX11CreateShaderResourceViewFromFileA(pDevice, filename.c_str(), 0, 0, &srv, 0 ));

		mTextureSRV[filename] = srv;
	}

	return srv;
}