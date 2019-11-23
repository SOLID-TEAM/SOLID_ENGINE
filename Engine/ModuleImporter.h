#ifndef __MODULE_IMPORTER_H__
#define __MODULE_IMPORTER_H__

#include "Globals.h"
#include "Module.h"
#include <vector>
#include <string>

#include "external/MathGeoLib/include/Math/MathAll.h"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

class vec3;
class GameObject;
class R_Mesh;
class R_Material;
class R_Texture;

// TODO: add more types if needed
enum PrimitiveType
{
	CUBE,
	SPHERE,
	ICOSPHERE,
	PLANE,
	CYLINDER,
	CONE,
	TORUS,
	MAX
};

class ModuleImporter : public Module
{
public:
	ModuleImporter(bool start_enabled = true);
	~ModuleImporter();

	bool Init(Config& config);
	bool Start(Config& config);
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	bool Save(Config& config);
	void Load(Config& config);

	// TODO: think where to fit this better
	//bool ReComputeVertexNormals(float length = 0.4f);
	//bool ReComputeFacesNormals(float length = 0.4f);

	// Create primitives with par_shapes tool
	GameObject* CreatePrimitive(PrimitiveType type, float3 position = { 0,0,0 }, float3 size = { 1,1,1 }, float2 slicesStacks = { 10,10 }, float4 color = { 1.0f,1.0f,1.0f,1.0f });

	// TODO: implement base class gameObjects (for future components addition)
	std::vector<R_Mesh*>& GetModels();

	//void ImportFileFromPath(const char* path);
	
	//void CreateGoFromNodes(const aiScene* scene , aiNode* node, GameObject* parent);
	
	// Import file types -----------------------------
	//bool ImportModelFile(const char* path);

	// TODO: ONLY TEST
	//void ReloadTextureForAllModels(uint texture_id);

private:

	/*R_Mesh*			ImportMesh(const aiMesh* mesh, const char* name);
	R_Material*		ImportMaterial(const aiMaterial* material, const char* name);
	R_Material*		CreateDefaultMaterial(const char* name, float4 color) const;
	R_Texture*		ImportTexture(const char* path);*/

	// sequentiall id for every thing
	UID GetNewUID();
	std::string GetFormattedName(std::string name, UID id);

private:

	UID last_id = 0;

	// TODO: MAYBE WE NEED a specific module to store all models of the scene and draw etc
	// TODO: for testing here, but we need to search a better place, maybe new module coming
	std::vector<R_Mesh*> meshes;

	std::string relative_path_from_model;

public:

	float wire_line_width = 1.0f;
	float v_point_size   = 5.0f;
	float f_v_point_size = 5.0f;
	float v_n_line_width = 1.0f;
	float f_n_line_width = 1.0f;

	float v_n_line_length = 0.4f;
	float f_n_line_length = 0.4f;
};

#endif // !__MODULE_IMPORTER_H__

