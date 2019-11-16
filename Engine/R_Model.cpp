#include "R_Model.h"
#include "R_Mesh.h"
#include "R_Material.h"
#include "Application.h"

// ASSIMP ----------------------------
#include "external/Assimp/include/cimport.h"
#include "external/Assimp/include/scene.h"
#include "external/Assimp/include/postprocess.h"
#include "external/Assimp/include/cfileio.h"
#include "external/Assimp/include/version.h"

#pragma comment (lib, "external/Assimp/libx86/assimp.lib")
// ------------------------------------

R_Model::R_Model(UID uid) : Resource(uid, Resource::Type::MODEL) {}


R_Model::~R_Model() {}

bool R_Model::LoadInMemory()
{
	return true;
}

bool R_Model::Import(const char* path, UID& output_uid)
{
	bool ret = true;
	
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_GenBoundingBoxes);

	if(scene != nullptr)
	{
		
		// Create separate gameobjects from nodes
		//CreateGoFromNodes(scene, scene->mRootNode, root);
		R_Model new_model(0);

		std::vector<UID> meshes, materials;
		new_model.GenerateMeshes(scene, path, meshes);

		// Free the imported scene
		aiReleaseImport(scene);
	}
	else
		LOG("[Error] Loading scene %s ", path);

	return ret;
}

void R_Model::GenerateMeshes(const aiScene* scene, const char* file, std::vector <UID>& meshes)
{
	meshes.reserve(scene->mNumMeshes);

	for (uint i = 0; i < scene->mNumMeshes; ++i)
	{
		meshes.push_back(R_Mesh::Import(scene->mMeshes[i], file));
	}
}