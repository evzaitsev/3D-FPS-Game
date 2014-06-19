// TextureDDSConverter.cpp : Reads input as texture file paths and converts them to compressed .dds formats
// Reqiures texconv.exe which comes with directx sdk in the folder of .exe 

#include "stdafx.h"
#include <iostream>
#include <istream>
#include <ostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;


int _tmain(int argc, _TCHAR* argv[])
{
	ifstream ifstr("textures.txt");

	unsigned short numTextures;


	bool GenerateMipMaps = false;

	std::string ignore;

	ifstr >> ignore >> numTextures;

	std::vector<std::string> texturepaths;

	printf_s("Generate mip maps?\n");

	char Answer = getchar();

	if (Answer == 'Y' || Answer == 'y')
	{
		GenerateMipMaps = true;
	}
	else if (Answer == 'N' || Answer == 'n')
		GenerateMipMaps = false;

	for (unsigned short i = 0; i < numTextures; ++i)
	{
		std::string path;

		ifstr >> path;

		printf_s("Reading %s\n", path);

		texturepaths.push_back(path);
	}

	ofstream output("Textures.bat");
	
	output << "@echo off\n";
	output << "mkdir CompressedTextures\n";

	for (unsigned short i = 0; i < texturepaths.size(); ++i)
	{

		if (GenerateMipMaps)

            output << "texconv.exe -m 12 -f DXT1 -o CompressedTextures " << texturepaths[i] << "\n";
		else
			output << "texconv.exe -f DXT1 -o CompressedTextures " << texturepaths[i] << "\n";

		if (i == texturepaths.size()-1)
			output << "pause";
	}

	printf_s("\n\nProcess completed. Run Textures.bat and it'll convert textures to DirectX 9 compressed format.");

	return 0;
}

