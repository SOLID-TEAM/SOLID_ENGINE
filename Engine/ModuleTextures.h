#ifndef __MODULETEXTURES_H__
#define __MODULETEXTURES_H__

#include "Globals.h"
#include "Module.h"

#include "external/DevIL-1.8.0/include/IL/il.h"

#include <map>

class ModuleTextures : public Module
{
public:
	ModuleTextures(bool start_enabled = true);
	~ModuleTextures();

	bool Init(Config& config);
	bool Start(Config& config);
	bool CleanUp();

	uint LoadTexture(const char* path);

	// test
	/*void LoadTextureFromEvent(const char* path, bool& already, uint& id);*/

	bool FreeTextureBuffer(uint id);

	/*bool GenerateCheckerTexture();*/

public:
	ILuint image_test = 0;
	std::string texture_path;

	std::map<std::string, uint> textures;
};

#endif // !__MODULETEXTURES_H__