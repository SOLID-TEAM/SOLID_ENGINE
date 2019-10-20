#ifndef __MODULE_IMPORTER_H__
#define __MODULE_IMPORTER_H__

#include "Globals.h"
#include "Module.h"
#include <vector>
#include <string>
#include "glmath.h"

struct aiScene;
struct aiNode;
struct aiMesh;
struct aiMaterial;

class vec3;
class GameObject;
class D_Mesh;
class D_Material;
class D_Texture;

// TODO: add more types if needed
enum PrimitiveType
{
	CUBE,
	DODECAHEDRON,
	TETRAHEDRON,
	OCTOHEDRON,
	ICOSAHEDRON,
	PLANE,
	SPHERE,
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
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	bool Save(Config& config);
	void Load(Config& config);

	// TODO: think where to fit this better
	//bool ReComputeVertexNormals(float length = 0.4f);
	//bool ReComputeFacesNormals(float length = 0.4f);

	// Create primitives with par_shapes tool
	void  CreatePrimitive(PrimitiveType type, vec3 position = { 0,0,0 }, vec3 size = { 1,1,1 });

	// TODO: implement base class gameObjects (for future components addition)
	std::vector<D_Mesh*>& GetModels();

	void ImportFileFromPath(const char* path);
	void CreateGoFromNodes(const aiScene* scene , aiNode* node, GameObject* parent);
	
	// Import file types -----------------------------
	bool ImportModelFile(const char* path);

	// TODO: ONLY TEST
	void ReloadTextureForAllModels(uint texture_id);

private:

	D_Mesh*			ImportMesh(const aiMesh* mesh, const char* name);
	D_Material*		ImportMaterial(const aiMaterial* material, const char* name);
	D_Texture*		ImportTexture(const char* path);

private:

	// TODO: MAYBE WE NEED a specific module to store all models of the scene and draw etc
	// TODO: for testing here, but we need to search a better place, maybe new module coming
	std::vector<D_Mesh*> meshes;

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

