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

#include <string>


ModuleTextures::ModuleTextures(bool start_enabled)// : Module(start_enabled)
{
	name.assign("Textures");

	texture_path.assign("Assets/Textures/");
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

	//image_test = LoadTexture("lenna.png");
	//image_test = LoadTexture("Assets/Textures/Baker_house.png");

	return true;
}

bool ModuleTextures::CleanUp()
{
	LOG("[Info] Shutting down DevIL library");
	ilShutDown();

	return true;
}

uint ModuleTextures::LoadTexture(const char* texture_name)
{
	/*ilGenImages(1, &image_test);
	ilBindImage(image_test);*/

	// first find if the texture are previously loaded into buffers
	std::map<const char*, uint>::iterator tex;
	tex = textures.find(texture_name);
	if (tex != textures.end())
	{
		LOG("[Info] Texture with same name %s already loaded into buffers", texture_name);
		
		return (*tex).second;
	}
	
	
	std::string tex_name(texture_name);
	std::string final_path(texture_path + tex_name);

	if (ilLoadImage(final_path.data()))
	{
		LOG("[Info] Image %s correctly loaded into DevIL", final_path.data());

		GLuint texture = 0;
		texture = ilutGLBindTexImage();

		if (texture > 0)
		{
			LOG("[Info] Image correctly uploaded to gl buffer");
			// add to map
			textures.insert({ texture_name, texture });
			// unbind from gpu
			glBindTexture(GL_TEXTURE_2D, 0);
			// delete devil data
			ilDeleteImage(texture);
		}
		
		return texture > 0 ? texture : NULL;
	}
	else
		return 0;
}