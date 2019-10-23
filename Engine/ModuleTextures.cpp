#include "Globals.h"
#include "Application.h"
#include "ModuleTextures.h"

#include "GL/glew.h"

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

uint ModuleTextures::LoadTexture(const char* texture_path)
{
	/*ilGenImages(1, &image_test);
	ilBindImage(image_test);*/

	std::string texture_name;
	std::string extension;
	std::string path;
	App->file_sys->SplitFilePath(texture_path, &path, &texture_name, &extension);

	texture_name = texture_name + '.' + extension;

	// first find if the texture are previously loaded into buffers
	std::map<std::string, uint>::iterator tex;
	tex = textures.find(texture_name);

	if (tex != textures.end())
	{
		LOG("// ---------------------------------------------------");
		LOG("[Info] Texture with same name %s already loaded into buffers", texture_name.data());
		LOG("[Info] returning stored texture id");
		LOG("// ---------------------------------------------------");
		return (*tex).second;
	}
	
	// TODO: maybe we want to modify how this path are passed (applicable to other loading methods)
	// First: check filetype and decide what to call
	// Sec: duplicate the file on desired directory for the extension file
	// 3: pass the path of the "internal copied file"

	if (ilLoadImage(texture_path))
	{
		LOG("// ---------------------------------------------------");
		LOG("[Info] Image %s correctly loaded into DevIL", texture_name.data());
		LOG("[Info] from filepath %s", path.data());

		GLuint texture = 0;

		texture = ilutGLBindTexImage();

		if (texture > 0)
		{
			LOG("[Info] Image correctly uploaded to gl buffer");
			// add to map
			textures.insert({ texture_name, texture });
			LOG("[Info] Added image %s to texture map", texture_name.data());
			// unbind from gpu
			glBindTexture(GL_TEXTURE_2D, 0);
			// delete devil data
			ilDeleteImage(texture);
			LOG("[Info] Image deleted from DevIL");
		}

		LOG("// ---------------------------------------------------");
		
		return texture > 0 ? texture : NULL;
	}
	else
	{
		LOG("[Error] Loading texture: %s", iluErrorString(ilGetError()));
		LOG("[Error] name %s from path %s", texture_name.data(), path.data());
		return 0;
	}
}

bool ModuleTextures::FreeTextureBuffer(uint id)
{
	bool ret = false;

	// TODO: improve this, with map to not load textures more than 1 time, to delete from map too
	// we need to iterate map to delete from it too before free gl buffers
	std::map<std::string, uint>::iterator allTextures = textures.begin();
	for (; allTextures != textures.end(); ++allTextures)
	{
		uint test = (*allTextures).second;
		if ((*allTextures).second == id)
		{
			glDeleteTextures(1, &(*allTextures).second);
			LOG("Freed texture buffer %s from vram", (*allTextures).first.data());
			LOG("Deleted texture %s from map", (*allTextures).first.data());
			(*allTextures).second = 0;
			textures.erase(allTextures);
			ret = true;
			
			break;
		}
	}

	return ret;
}

uint ModuleTextures::GenerateCheckerTexture(uint w, uint h)
{
	uint ret_id = 0;

	GLubyte* checker = nullptr;
	checker = new GLubyte[w * h * 4];

	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checker[i * h * 4 + j * 4 + 0] = (GLubyte)c;
			checker[i * h * 4 + j * 4 + 1] = (GLubyte)c;
			checker[i * h * 4 + j * 4 + 2] = (GLubyte)c;
			checker[i * h * 4 + j * 4 + 3] = (GLubyte)255;
		}
	}

	// FOR SAVE THE PROCEDURALLY GENERATED CHECKER INTO A IMAGE FILE WITH DEVIL

	// If you pass a value for Image that was not generated by ilGenImages, 
	//ilBindImage automatically creates an image specified by the image name passed.  
	//An image must always be bound before you call any functions that operate on images and their data

	uint ilImage = 0;
	ilBindImage(ilImage);

	/*When DevIL creates a new image, the image has the default properties of 1x1x1 with a bit depth of 8.  
	DevIL creates a new image when you call ilBindImage with an image name that has not been generated by ilGenImages or
	when you call ilGenImagesspecifically*/

	if (ilTexImage(w, h, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, checker))
	{
		// custom implementation on ilut lib
		// by default ilutGLBindTexImage has GL_LINEAR, wich we dont want to for checker texture
		ret_id = ilutGLBindTexImageEx(GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0); // unlink

		// SAVING IMAGE ------------------------------------------------------------------------------
		// TODO: FILESYSTEM
		std::string temporary_path("Assets/Textures/temp/");
		std::string imgname("checker");
		std::string id(std::to_string(ret_id));
		std::string extension(".png");
		std::string final_path(temporary_path + imgname + id + extension);
		// to save the image into disk (for show the image into inspector)
		if (ilSave(IL_PNG, final_path.data()))
		{
			LOG("[Info] checker texture %s correctly saved into temporary directory", (imgname + id + extension).data());
		}
		else
		{
			LOG("[Error] Saving checker texture: %s", iluErrorString(ilGetError()));
		}
		// --------------------------------------------------------------------------------------------
		
		// add to textures map
		textures.insert({ (imgname + id + extension).data(), ret_id });

		LOG("[Info] Correctly generated and binded procedurally checker texture with %ix%i pixels size", w, h);
	}
	else
	{
		LOG("[Error] creating DevIL image from data %s", iluErrorString(ilGetError()));
	}

	ilDeleteImage(ilImage);

	delete[] checker;

	return ret_id > 0 ? ret_id : 0;
}

void ModuleTextures::GetTextureSize(uint& w, uint& h) const
{
	//
}
