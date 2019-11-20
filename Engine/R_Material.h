#ifndef __D_MATERIAL_H__
#define __D_MATERIAL_H__

#include "Globals.h"
#include "Resource.h"
#include "R_Texture.h"
#include "external/MathGeoLib/include/MathGeoLib.h"

struct aiMaterial;

class R_Material : public Resource
{
public:

	R_Material(UID uid) : Resource(uid, Type::MATERIAL) {}

	R_Material(UID uid, const char* name, float4 color) : Resource(uid, Type::MATERIAL)
	{
		this->name.assign(name);
		diffuse_color = color;
	}

	~R_Material() {};

	//void GenerateFileTexture();

	bool LoadInMemory();
	void ReleaseFromMem();


	bool SaveToFile(const char* name);
	//bool LoadFromFile(const char* name);

	bool LoadDependencies();

	static UID Import(const aiMaterial* material, const char* material_name, std::string from_path);

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

	UID  textures[MAX]{ 0,0,0,0 };
	float4	    diffuse_color;

	//uint tex_gl_id; // TODO: remove this
};

#endif //__R_MATERIAL_H__
