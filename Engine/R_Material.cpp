#include "R_Material.h"
#include "GL/glew.h"
#include "SDL\include\SDL_opengl.h"

#include "external/Assimp/include/material.h"

#include "Application.h"

//void R_Material::GenerateFileTexture()
//{
//	uint frame_buffer_id = 0;
//	uint depth_buffer_id = 0;
//
//	// Gen buffers ------------------------------------
//	glGenRenderbuffers(1, &depth_buffer_id);
//	glGenTextures(1, &tex_gl_id);
//	glGenFramebuffers(1, &frame_buffer_id);
//
//	// Settings --------------------------------------
//
//	// Depth storage ---------------
//	glBindRenderbuffer(GL_RENDERBUFFER, depth_buffer_id);
//	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 256, 256);
//	glBindRenderbuffer(GL_RENDERBUFFER, 0);
//
//	// Color texture ---------------
//	glBindTexture(GL_TEXTURE_2D, tex_gl_id);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE); // automatic mipmap
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
//	glBindTexture(GL_TEXTURE_2D, 0);
//
//	// Attach ---------------------
//	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_id);
//	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth_buffer_id);
//	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex_gl_id, 0);
//
//	// Error texture --------------
//	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//	{
//		LOG("[Error] creating screen buffer");
//	}
//
//	glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//	math::float4x4 matrix = math::float4x4::FromTRS(float3::zero, Quat::identity, float3::one);
//
//
//	// Delete buffers ---------------------------------
//	glDeleteRenderbuffers(1, &depth_buffer_id);
//	glDeleteFramebuffers(1, &frame_buffer_id);
//}
//

//bool R_Material::LoadFromFile(const char* name)
//{
//
//	char* buffer = nullptr;
//
//	App->file_sys->Load(name, &buffer);
//
//	if (buffer != nullptr)
//	{
//		LOG("TODO");
//	}
//	else
//	{
//		LOG("[Error] Loading material from %", name);
//	}
//
//	return true;
//}


void R_Material::ReleaseFromMem()
{
	for (uint i = 0; i < MAX; ++i)
	{
		if (textures[i] != 0)
		{
			Resource* r = App->resources->Get(textures[i]);

			if (r)
				r->Release();

			textures[i] = 0;
		}
	}
}


UID R_Material::Import(const aiMaterial* material, const char* material_name, std::string from_path)
{
	UID ret = 0;

	R_Material* m = static_cast<R_Material*>(App->resources->CreateNewResource(Resource::Type::MATERIAL));
	m->GetName().assign(material_name);
	
	// Set albedo color ----------------------------------------------------------
	
	aiColor4D color;
	material->Get(AI_MATKEY_COLOR_DIFFUSE,color);
	m->diffuse_color = { color.r, color.g, color.b, color.a };
	
	// Get assimp path and normalize --------
	
	std::string file;       // file_name + file_ex
	std::string file_name;
	std::string file_ex;
	aiString	ai_path;
	
	material->GetTexture(aiTextureType_DIFFUSE, 0, &ai_path);  
	
	if (ai_path.length > 0)
	{
		App->file_sys->SplitFilePath(ai_path.C_Str(), nullptr, &file_name, &file_ex);
		file = ASSETS_FOLDER + file_name + "." + file_ex;
	
		// Load material textures ------------------------------------------------- 
		
		m->textures[R_Material::DIFFUSE] = App->resources->ImportFile(file.c_str(), Resource::Type::TEXTURE , from_path );
	}
	else
	{
		LOG("[Error] NO TEXTURE ON MODEL");
	}
	

	m->SaveToFile(m->GetNameFromUID().c_str());

	return m->GetUID();
}

bool R_Material::SaveToFile(const char* name)
{
	uint size = sizeof(diffuse_color) + sizeof(textures);
	char* data = new char[size];

	char* cursor = data;

	uint bytes = sizeof(textures);
	memcpy(cursor, &textures, sizeof(textures));

	cursor += bytes;
	bytes = sizeof(diffuse_color);
	memcpy(cursor, &diffuse_color, bytes);

	// TODO, add more data if needed to save
	App->file_sys->Save(std::string(LIBRARY_MATERIAL_FOLDER + std::string(name)).c_str(), data, size);
	
	
	return true;
}

bool R_Material::LoadInMemory()
{
	bool ret = true;

	char* buffer = nullptr;

	App->file_sys->Load(LIBRARY_MATERIAL_FOLDER, GetNameFromUID().c_str(), &buffer);

	if (buffer != nullptr)
	{
		char* cursor = buffer;
		uint bytes = sizeof(textures);

		memcpy(&textures, cursor, bytes);

		cursor += bytes;
		bytes = sizeof(diffuse_color);

		memcpy(&diffuse_color, cursor, bytes);

		for (uint i = 0; i < MAX; ++i)
		{
			if (textures[i] > 0)
			{
				R_Texture* r = (R_Texture*)App->resources->Get(textures[i]);

				if (r != nullptr)
				{
					r->LoadToMemory();
				}
			}
		}

	}
	else ret = false;


	return ret;
}

bool R_Material::LoadDependencies()
{
	return true;
}