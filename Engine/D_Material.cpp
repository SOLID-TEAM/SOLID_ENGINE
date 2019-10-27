#include "D_Material.h"
#include "GL/glew.h"
#include "SDL\include\SDL_opengl.h"



void D_Material::GenerateFileTexture()
{
	uint frame_buffer_id = 0;
	uint depth_buffer_id = 0;

	// Gen buffers ------------------------------------
	glGenRenderbuffers(1, &depth_buffer_id);
	glGenTextures(1, &file_texture);
	glGenFramebuffers(1, &frame_buffer_id);

	// Settings --------------------------------------

	// Depth storage ---------------
	glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer_id);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 256, 256);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	// Color texture ---------------
	glBindTexture(GL_TEXTURE_2D, file_texture);
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
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, file_texture, 0);

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
