#ifndef __MODULE_IMPORTER_H__
#define __MODULE_IMPORTER_H__

#include "Globals.h"
#include "Module.h"

#include "external/Assimp/include/cimport.h"
#include "external/Assimp/include/scene.h"
#include "external/Assimp/include/postprocess.h"
#include "external/Assimp/include/cfileio.h"

#pragma comment (lib, "external/Assimp/libx86/assimp.lib")

#include "ModelData.h"
#include "external/par_shapes.h"

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
	// TODO: add more types if needed
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

	bool LoadFileMesh(const char* path);

	// TODO: think where to fit this better
	bool ReComputeVertexNormals(float length = 0.4f);
	bool ReComputeFacesNormals(float length = 0.4f);

	// Create primitives with par_shapes tool
	ModelData* CreatePrimitive(PrimitiveType type, vec3 position = { 0,0,0 }, vec3 size = { 1,1,1 });

	// TODO: implement base class gameObjects (for future components addition)
	std::vector<ModelData*>& GetModels();

	void ImportFileFromPath(const char* path);

private:
	// TODO: for testing here, but we need to search a better place, maybe new module coming
	std::vector<ModelData*> meshes;
	//std::vector<ModelData*> startup_meshes;

// TODO: MAYBE WE NEED a specific module to store all models of the scene and draw etc
public:
	// TODO: maybe this vars fits better on the model itself and each model has its own properties of visualization
	// implement when we already finish module filesystem
	bool wireframe = true;
	bool fill_faces = true;
	bool debug_vertex_normals = true;
	bool debug_face_normals = true;
	ImVec4 fill_color = ImVec4(1.0f, 0.35f, 1.0f , 1.0f);
	ImVec4 wire_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);;
	ImVec4 d_vertex_p_color = ImVec4(0.59f, 0.22f, 1.0f, 1.0f);
	ImVec4 d_vertex_l_color = ImVec4(0.2f, 1.0f, 0.0f, 1.0f);
	ImVec4 d_vertex_face_color = ImVec4(1.0f, 0.5f, 0.2f,1.0f);
	ImVec4 d_vertex_face_n_color = ImVec4(0.0f, 0.5f, 1.0f, 1.0f);

	float wire_line_width = 1.0f;
	float v_point_size   = 5.0f;
	float f_v_point_size = 5.0f;
	float v_n_line_width = 1.0f;
	float f_n_line_width = 1.0f;

	float v_n_line_length = 0.4f;
	float f_n_line_length = 0.4f;
};

#endif // !__MODULE_IMPORTER_H__
