#ifndef __MODULETEXTURES_H__
#define __MODULETEXTURES_H__

#include "Globals.h"
#include "Module.h"

#include "external/DevIL-1.8.0/include/IL/il.h"

class ModuleTextures : public Module
{
public:
	ModuleTextures(bool start_enabled = true);
	~ModuleTextures();

	bool Init(Config& config);
	bool Start(Config& config);

	uint LoadTexture(const char* path);

	/*bool GenerateCheckerTexture();*/

public:
	ILuint image_test = 0;
};

#endif // !__MODULETEXTURES_H__