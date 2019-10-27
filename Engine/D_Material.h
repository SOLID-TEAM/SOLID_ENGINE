#ifndef __D_MATERIAL_H__
#define __D_MATERIAL_H__

#include "Globals.h"
#include "Data.h"
#include "D_Texture.h"
#include "external/MathGeoLib/include/MathGeoLib.h"

class D_Material : public Data
{
public:

	D_Material() : Data(DataType::MATERIAL) 
	{
		for (int i = 0; i < MAX; ++i)
			textures[i] = nullptr;
	}

	D_Material(const char* name, float4 color) : Data(DataType::MATERIAL)
	{
		for (int i = 0; i < MAX; ++i)
			textures[i] = nullptr;

		this->name.assign(name);
		diffuse_color = color;
	}

	~D_Material() {

	};

	void GenerateFileTexture();

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

	D_Texture*  textures[MAX];
	float4	    diffuse_color; 
};

#endif //__R_MATERIAL_H__
