#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"

// DevIL image library
#include "external/DevIL-1.8.0/include/IL/il.h"
#include "external/DevIL-1.8.0/include/IL/ilu.h"
#include "external/DevIL-1.8.0/include/IL/ilut.h"

#pragma comment(lib, "external/DevIL-1.8.0/libx86/DevIL.lib")
#pragma comment(lib, "external/DevIL-1.8.0/libx86/ILU.lib")
#pragma comment(lib, "external/DevIL-1.8.0/libx86/ILUT.lib")


ModuleTextures::ModuleTextures(bool start_enabled)// : Module(start_enabled)
{
	name.assign("Textures");
}

ModuleTextures::~ModuleTextures() {}

bool ModuleTextures::Init(Config& config)
{
	// Needed to initialize DevIL.
	ilInit();
	iluInit();
	// GL cannot use palettes anyway, so convert early.
	ilEnable(IL_CONV_PAL);
	// Gets rid of dithering on some nVidia-based cards.
	ilutEnable(ILUT_OPENGL_CONV);
	// Utility Toolkit Functions
	ilutInit();
	// Lets ILUT know to use its OpenGL functions.
	ilutRenderer(ILUT_OPENGL);

	LOG("[Info] Using %s", ilGetString(IL_VERSION_NUM));

	return true;
}

bool ModuleTextures::Start(Config& config)
{

	image_test = LoadTexture("Assets/Textures/lenna.png");

	return true;
}

uint ModuleTextures::LoadTexture(const char* path)
{
	/*ilGenImages(1, &image_test);
	ilBindImage(image_test);*/
	if (ilLoadImage(path))
	{
		LOG("Image %s correctly loaded into DevIL", path);

		GLuint texture = 0;
		texture = ilutGLBindTexImage();

		if (texture > 0)
		{
			glBindTexture(GL_TEXTURE_2D, 0);
			ilDeleteImage(texture);
		}
		
		return texture > 0 ? texture : NULL;
	}
	else
		return 0;
}