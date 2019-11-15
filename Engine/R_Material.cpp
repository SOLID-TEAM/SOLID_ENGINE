#include "R_Material.h"
#include "GL/glew.h"
#include "SDL\include\SDL_opengl.h"

#include "Application.h"

void R_Material::GenerateFileTexture()
{
	uint frame_buffer_id = 0;
	uint depth_buffer_id = 0;

	// Gen buffers ------------------------------------
	glGenRenderbuffers(1, &depth_buffer_id);
	glGenTextures(1, &tex_gl_id);
	glGenFramebuffers(1, &frame_buffer_id);

	// Settings --------------------------------------

	// Depth storage ---------------
	glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 256, 256);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Color texture ---------------
	glBindTexture(GL_TEXTURE_2D, tex_gl_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Attach ---------------------
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_buffer_id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_gl_id, 0);

	// Error texture --------------
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG("[Error] creating screen buffer");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	math::float4x4 matrix = math::float4x4::FromTRS(float3::zero, Quat::identity, float3::one);


	// Delete buffers ---------------------------------
	glDeleteRenderbuffers(1, &depth_buffer_id);
	glDeleteFramebuffers(1, &frame_buffer_id);
}

bool R_Material::SaveToFile(const char* name)
{
	uint size = sizeof(diffuse_color);

	char* data = new char[size];

	char* cursor = data;

	memcpy(cursor, &diffuse_color, size);

	// TODO, add more data if needed to save
	App->file_sys->Save(name, data, size);
	
	
	return true;
}

bool R_Material::LoadFromFile(const char* name)
{

	char* buffer = nullptr;

	App->file_sys->Load(name, &buffer);

	if (buffer != nullptr)
	{
		LOG("TODO");
	}
	else
	{
		LOG("[Error] Loading material from %", name);
	}

	return true;
}