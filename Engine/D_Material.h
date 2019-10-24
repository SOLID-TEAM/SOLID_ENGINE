#ifndef __D_MATERIAL_H__
#define __D_MATERIAL_H__

#include "Globals.h"
#include "Data.h"
#include "Color.h"
#include "D_Texture.h"

class D_Material : public Data
{
public:

	D_Material() : Data(DataType::MATERIAL) 
	{
		for (int i = 0; i < MAX; ++i)
			textures[i] = nullptr;
	}

	D_Material(const char* name, vec4 color) : Data(DataType::MATERIAL)
	{
		for (int i = 0; i < MAX; ++i)
			textures[i] = nullptr;

		GetName().assign(name);
		
		memcpy(&albedo_color, &color, sizeof(float) * 4);
	}

	~D_Material() {};

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

	D_Texture* textures[MAX];
	Color	    albedo_color;
};

#endif //__R_MATERIAL_H__
