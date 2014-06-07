#ifndef TEXTUREMGR_H
#define TEXTUREMGR_H

///<summary>
/// Simple texture manager to avoid loading duplicate textures from file.  That can
/// happen, for example, if multiple meshes reference the same texture filename. 
///</summary>
class TextureMgr
{
public:
	TextureMgr();
	~TextureMgr();

	ID3D11ShaderResourceView* CreateTexture(std::string filename);

private:
	TextureMgr(const TextureMgr& rhs);
	TextureMgr& operator=(const TextureMgr& rhs);
	
private:

	std::map<std::string, ID3D11ShaderResourceView*> mTextureSRV;
};

#endif // TEXTUREMGR_H