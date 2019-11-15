#ifndef __D_MATERIAL_H__
#define __D_MATERIAL_H__

#include "Globals.h"
#include "Resource.h"
#include "R_Texture.h"
#include "external/MathGeoLib/include/MathGeoLib.h"

class R_Material : public Resource
{
public:

	R_Material() : Resource(ResourceType::MATERIAL) 
	{
		for (int i = 0; i < MAX; ++i)
			textures[i] = nullptr;
	}

	R_Material(const char* name, float4 color) : Resource(ResourceType::MATERIAL)
	{
		for (int i = 0; i < MAX; ++i)
			textures[i] = nullptr;

		this->name.assign(name);
		diffuse_color = color;
	}

	~R_Material() {

	};

	void GenerateFileTexture();


	bool SaveToFile(const char* name);
	bool LoadFromFile(const char* name);

public:

	enum TextureType
	{
		DIFFUSE,
		METALIC,
		NORMAL_MAP,
		HEIGHT_MAP,
		MAX
	};

public:

	// TODO: instead of D_Texture, store a UID from future resource texture
	D_Texture*  textures[MAX];
	float4	    diffuse_color;
	uint tex_gl_id;
};

#endif //__R_MATERIAL_H__
