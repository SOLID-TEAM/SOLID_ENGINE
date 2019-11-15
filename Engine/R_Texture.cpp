#include "R_Texture.h"
#include "Application.h"

#include <GL/GL.h>


R_Texture::R_Texture(UID uid) : Resource(uid, Resource::Type::TEXTURE) {}

R_Texture::~R_Texture() {}

bool R_Texture::LoadInMemory()
{
	// TODO: update moduletextures to accept resources
	//return App->textures->Load(this);

	return true;
}

// ---------------------------------------------------------
void R_Texture::ReleaseFromMemory()
{
	if (buffer_id != 0)
	{
		glDeleteTextures(1, &buffer_id);
		buffer_id = 0;
	}
}

void R_Texture::Save(Config& config) const
{
	Resource::Save(config);

	config.AddInt("Format", format);
	config.AddBool("Mipmaps", has_mips);
	config.AddBool("Linear", linear);
}

void R_Texture::Load(const Config& config) 
{
	Resource::Load(config);

	format = (Format)config.GetInt("Format", unknown);
	has_mips = config.GetBool("Mipmaps", false);
	linear = config.GetBool("Linear", true);

}