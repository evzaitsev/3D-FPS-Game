// techniqueGen.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

std::string GetTech(int NumLights,
			  int NumPointLights,
			  int gUseTexure, 
		      int gAlphaClip, 
		      int gFogEnabled, 
		      int gReflectionEnabled,
		      int gUseOcclusionMap,
		      int gUseSpecularMap)
{
	std::string str;

	//allocate 1 byte
	char tech[1024];

	sprintf_s(tech, "\ntechnique11 Light%dPointLight%d%s%s%s%s%s%s\n"
"{\n  "
    "pass P0\n"
"    {\n"
    "      SetVertexShader( CompileShader( vs_4_0, VS() ) );\n"
    "      SetGeometryShader( NULL );\n"
    "      SetPixelShader( CompileShader( ps_4_0, PS(%d, %d, %s, %s, %s, %s, %s, %s))  );\n"
"    }\n"
"}\n  ", NumLights, NumPointLights, gUseTexure ? "Tex" : "",
gAlphaClip ? "AlphaClip" : "", gFogEnabled ? "Fog" : "", gReflectionEnabled ? "Reflect" : "",
gUseOcclusionMap ? "AO" : "", gUseSpecularMap ? "Spec" : "",
NumLights, NumPointLights,
gUseTexure ? "true" : "false",
gAlphaClip ? "true" : "false", gFogEnabled ? "true" : "false",  gReflectionEnabled ? "true" : "false",
gUseOcclusionMap ? "true" : "false", gUseSpecularMap ? "true" : "false"); 
               
	str = tech;

	return str;
}

std::string GetTechInstanced(int NumLights,
			  int NumPointLights,
			  int gUseTexure, 
		      int gAlphaClip, 
		      int gFogEnabled, 
		      int gReflectionEnabled,
		      int gUseOcclusionMap,
		      int gUseSpecularMap)
{
	std::string str;

	char tech[1024];

	sprintf_s(tech, "\ntechnique11 Light%dPointLight%d%s%s%s%s%s%sInstanced\n"
"{\n  "
    "pass P0\n"
"    {\n"
    "      SetVertexShader( CompileShader( vs_4_0, VS_INSTANCED() ) );\n"
    "      SetGeometryShader( NULL );\n"
    "      SetPixelShader( CompileShader( ps_4_0, PS(%d, %d, %s, %s, %s, %s, %s, %s))  );\n"
"    }\n"
"}\n  ", NumLights, NumPointLights, gUseTexure ? "Tex" : "",
gAlphaClip ? "AlphaClip" : "", gFogEnabled ? "Fog" : "", gReflectionEnabled ? "Reflect" : "",
gUseOcclusionMap ? "AO" : "", gUseSpecularMap ? "Spec" : "",
NumLights, NumPointLights,
gUseTexure ? "true" : "false",
gAlphaClip ? "true" : "false", gFogEnabled ? "true" : "false",  gReflectionEnabled ? "true" : "false",
gUseOcclusionMap ? "true" : "false", gUseSpecularMap ? "true" : "false"); 
               
	str = tech;

	return str;
}

std::string GetEffectTechniqueVariable(int NumLights,
			  int NumPointLights,
			  int gUseTexure, 
		      int gAlphaClip, 
		      int gFogEnabled, 
		      int gReflectionEnabled,
		      int gUseOcclusionMap,
		      int gUseSpecularMap)
{

	std::string str;

	char tech[1024];

	sprintf_s(tech, "    ID3DX11EffectTechnique* Light%dPointLight%d%s%s%s%s%s%sTech;\n", NumLights, NumPointLights, gUseTexure ? "Tex" : "",
gAlphaClip ? "AlphaClip" : "", gFogEnabled ? "Fog" : "", gReflectionEnabled ? "Reflect" : "",
gUseOcclusionMap ? "AO" : "", gUseSpecularMap ? "Spec" : "");

	str = tech;
	
	return str;
}

std::string GetEffectTechniqueVariableInstanced(int NumLights,
			  int NumPointLights,
			  int gUseTexure, 
		      int gAlphaClip, 
		      int gFogEnabled, 
		      int gReflectionEnabled,
		      int gUseOcclusionMap,
		      int gUseSpecularMap)
{

	std::string str;

	char tech[1024];

	sprintf_s(tech, "    ID3DX11EffectTechnique* Light%dPointLight%d%s%s%s%s%s%sInstancedTech;\n", NumLights, NumPointLights, gUseTexure ? "Tex" : "",
gAlphaClip ? "AlphaClip" : "", gFogEnabled ? "Fog" : "", gReflectionEnabled ? "Reflect" : "",
gUseOcclusionMap ? "AO" : "", gUseSpecularMap ? "Spec" : "");

	str = tech;
	
	return str;
}


std::string GetEffectTechniqueVariableDefinition(int NumLights,
			  int NumPointLights,
			  int gUseTexure, 
		      int gAlphaClip, 
		      int gFogEnabled, 
		      int gReflectionEnabled,
		      int gUseOcclusionMap,
		      int gUseSpecularMap)
{
	std::string str;
	std::string str2;

	std::string commaString= "\"";

	char tech[1024];

	sprintf_s(tech, "Light%dPointLight%d%s%s%s%s%s%s", NumLights, NumPointLights, gUseTexure ? "Tex" : "",
gAlphaClip ? "AlphaClip" : "", gFogEnabled ? "Fog" : "", gReflectionEnabled ? "Reflect" : "",
gUseOcclusionMap ? "AO" : "", gUseSpecularMap ? "Spec" : "");

	str = tech;

	str2 = "    " + str + "Tech" + " = mFX->GetTechniqueByName(" + commaString + str + commaString + ");\n";
	
	return str2;

}


std::string GetEffectTechniqueVariableDefinitionInstanced(int NumLights,
			  int NumPointLights,
			  int gUseTexure, 
		      int gAlphaClip, 
		      int gFogEnabled, 
		      int gReflectionEnabled,
		      int gUseOcclusionMap,
		      int gUseSpecularMap)
{
	std::string str;
	std::string str2;

	std::string commaString= "\"";

	char tech[1024];

	sprintf_s(tech, "Light%dPointLight%d%s%s%s%s%s%sInstanced", NumLights, NumPointLights, gUseTexure ? "Tex" : "",
gAlphaClip ? "AlphaClip" : "", gFogEnabled ? "Fog" : "", gReflectionEnabled ? "Reflect" : "",
gUseOcclusionMap ? "AO" : "", gUseSpecularMap ? "Spec" : "");

	str = tech;

	str2 = "    " + str + "Tech" + " = mFX->GetTechniqueByName(" + commaString + str + commaString + ");\n";
	
	return str2;

}

int _tmain(int argc, _TCHAR* argv[])
{
	const int NumLights = 3;
	const int NumPointLights = 3;

	ofstream file;
	ofstream file2;
	ofstream file3;

	//i = instanced
	ofstream ifile;
	ofstream ifile2;
	ofstream ifile3;

	file.open("techniques.txt");
	file2.open("techniques_variables.txt");
	file3.open("techniques_definition.txt");

	ifile.open("itechniques.txt");
	ifile2.open("itechniques_variables.txt");
	ifile3.open("itechniques_definition.txt");

	printf_s("Generating techniques....\n");


	int numtech = 0;

	//things rarely used
	int fog = 0;
	int reflect = 0;
	int alphaclip = 0; 

	//most commonly used
	int tex = 1; 
	

	for (int i = 0; i <= NumLights; ++i)
	{
		for (int j = 0; j <= NumPointLights; ++j)
		{
			for (int aomap = 0; aomap <= 1; ++aomap)
			    {
					for (int specmap = 0; specmap <= 1; ++specmap)
					{
						++numtech;

						string t = GetTech(i, j, tex, alphaclip, fog, reflect, aomap, specmap);
						string q = GetEffectTechniqueVariable(i, j, tex, alphaclip, fog, reflect, aomap, specmap);
						string r = GetEffectTechniqueVariableDefinition(i, j, tex, alphaclip, fog, reflect, aomap, specmap);

						++numtech;

						string it = GetTechInstanced(i, j, tex, alphaclip, fog, reflect, aomap, specmap);
						string iq = GetEffectTechniqueVariableInstanced(i, j, tex, alphaclip, fog, reflect, aomap, specmap);
						string ir = GetEffectTechniqueVariableDefinitionInstanced(i, j, tex, alphaclip, fog, reflect, aomap, specmap);

						file << t;
						file2 << q;
						file3 << r;

						ifile << it;
						ifile2 << iq;
						ifile3 << ir;
						}
					}
		}

	}

	printf_s("Number of techniques generated: %d\n", numtech);

	file.close();
	file2.close();
	file3.close();

	ifile.close();
	ifile2.close();
	ifile3.close();

	system("PAUSE\n");



	return 0;
}

