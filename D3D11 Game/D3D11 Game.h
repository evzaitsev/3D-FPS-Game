#pragma once

#include "resource.h"

#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768

#define DEBUG


#if defined(DEBUG) || defined(_DEBUG)

#define USE_FREE_CAMERA_KEY
#define ALLOW_FRAME_PAUSE_KEY
#define OUTPUT_POSITION_INFO

#endif 


static bool FileExists(const std::string& file)
{
    std::ifstream infile(file);
    return infile.good();
}


static void ShowError(const char* e, ...)
{
	MessageBoxA(NULL, e, "Error", MB_OK|MB_ICONERROR);
	PostQuitMessage(0);
}

static std::string removeExtension(std::string filename) {
    size_t lastdot = filename.find_last_of(".");
    if (lastdot == std::string::npos) return filename;
    return filename.substr(0, lastdot); 
}