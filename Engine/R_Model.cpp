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


bool R_Model::Import(const char* path, UID& output_uid, std::string from_path)
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
		new_model.GenerateMaterials(scene, path, materials, from_path);
		new_model.GenerateNodes(scene, scene->mRootNode, 0, meshes, materials);

		// Free the imported scene
		aiReleaseImport(scene);

		// save own model -----
		// generate new guid for meta and file name
		UID new_guid = App->random->Int();
		new_model.SaveToFile(new_guid);

		// generate new meta


		output_uid = new_guid;
	}
	else
		LOG("[Error] Loading scene %s ", path);

	return ret;
}

bool R_Model::SaveToFile(UID uid)
{
	std::string filename(std::to_string(uid));

	/*uint size = 0;

	size += sizeof(int);

	for (int i = 0; i < nodes.size(); ++i)
	{
		size += sizeof(nodes[i]) + (nodes[i].name.size() * sizeof(char));
	}*/
	uint size = sizeof(Node) * nodes.size() +
				sizeof(int); // for store num nodes

	char* buffer = new char[size];
	char* cursor = buffer;

	uint bytes = sizeof(int);
	int qty_n = nodes.size();

	memcpy(cursor, &qty_n, bytes);

	cursor += bytes;
	bytes = sizeof(Node);

	// TODO: change the way this for works, and advance more manual the cursor ptr to copy exact strings on standard char with null termination
	// to remove the hack on loading the resource.
	// save this nodes[].name like string with null termination, and adapt the loading too
	for (uint i = 0; i < nodes.size(); ++i)
	{
		//uint this_size = sizeof(nodes[i]) + nodes[i].name.size() * sizeof(char);
		memcpy(cursor, &nodes[i], bytes);
		cursor += bytes;
	}

	App->file_sys->Save(std::string(LIBRARY_MODEL_FOLDER + filename).c_str(), buffer, size);

	//LoadFromFile(filename.c_str());

	return true;
}

bool R_Model::LoadNodesFromFile()
{
	// if we have nodes, delete
	nodes.clear();

	char* buffer = nullptr;

	uint size = App->file_sys->Load(std::string(LIBRARY_MODEL_FOLDER + GetNameFromUID()).c_str(), &buffer);

	if (buffer != nullptr)
	{
		char* cursor = buffer;

		int num = 0;
		uint bytes = sizeof(int);
		memcpy(&num, cursor, bytes);
		cursor += bytes;

		bytes = sizeof(Node);
		nodes.reserve(num);

		for (int i = 0; i < num; i++)
		{
			Node node;
			// TODO: only works to max 15 characters (predefined max size of std::string)
			// std::string is not a trivial thing to memcpy
			// we pick our current Node node, and make a clone of this string data to temporal buffer
			char* hack = new char[sizeof(node.name)];
			memcpy(hack, &node.name, sizeof(std::string));
			// copy all data from buffer, this invalide our string internal data allocators
			memcpy(&node, cursor, bytes);
			// push our node and makes a valid copy
			nodes.push_back(node);
			// restore our previous string data to correctly let std::string do its things when deallocates
			memcpy(&node.name, hack, sizeof(std::string));

			cursor += bytes;
		}
	}
	else return false;

	return true;
}

bool R_Model::LoadInMemory()
{
	
	if (LoadNodesFromFile())
	{
		// load meshes and materials resources

		for (uint i = 0; i < nodes.size(); ++i)
		{
			if (nodes[i].mesh > 0)
			{
				// if this a fresh init (on engine start when loads all resources), 
				// we must to load all associated resources, no load to memory
				Resource* r = App->resources->Get(nodes[i].mesh);
				if (r)
				{
					r->LoadToMemory();
				}
			}

			if (nodes[i].material > 0)
			{
				Resource* r = App->resources->Get(nodes[i].material);
				if (r)
				{
					r->LoadToMemory();
				}
			}
		}
	}
	else
		return false;

	return true;
}

void R_Model::LoadDependencies()
{

	if (LoadNodesFromFile())
	{
		for (uint i = 0; i < nodes.size(); ++i)
		{
			if (nodes[i].mesh > 0)
			{
				// if this a fresh init (on engine start when loads all resources), 
				// we must to load all associated resources, no load to memory
				// check if the resource doesnt exist and create
				Resource* r = App->resources->Get(nodes[i].mesh);
				if (r == nullptr)
				{
					r = App->resources->CreateNewResource(Resource::Type::MESH, nodes[i].mesh);
					// TODO0: load mesh data and get the mesh from its own name, not tree node name, same for material below
					r->GetName().assign(nodes[i].name);
				}
			}

			if (nodes[i].material > 0)
			{
				Resource* r = App->resources->Get(nodes[i].material);
				if (r == nullptr)
				{
					r = App->resources->CreateNewResource(Resource::Type::MATERIAL, nodes[i].material);
					// TODO1
					r->GetName().assign(nodes[i].name);
				}
			}
		}

		nodes.clear();
	}

}

void R_Model::GenerateMeshes(const aiScene* scene, const char* file, std::vector <UID>& meshes)
{
	meshes.reserve(scene->mNumMeshes);

	for (uint i = 0; i < scene->mNumMeshes; ++i)
	{
		meshes.push_back(R_Mesh::Import(scene->mMeshes[i], file));
	}
}

void R_Model::GenerateMaterials(const aiScene* scene, const char* filepath, std::vector<UID>& materials, std::string from_path)
{
	materials.reserve(scene->mNumMaterials);

	for (uint i = 0; i < scene->mNumMaterials; ++i)
	{
		materials.push_back(R_Material::Import(scene->mMaterials[i], scene->mMaterials[i]->GetName().C_Str(), from_path));
	}

}

void R_Model::GenerateNodes(const aiScene* scene, const aiNode* node, uint parent, const std::vector<UID>& meshes, const std::vector<UID>& materials)
{
	Node parent_node;

	parent_node.parent = parent;
	parent_node.name.assign(node->mName.C_Str());
	parent_node.name.resize(15); // TODO: changes the way we save the own file format for evade the hack on there
	parent_node.transform = reinterpret_cast<const float4x4&>(node->mTransformation);

	if (node->mNumMeshes == 1)
	{
		parent_node.mesh = meshes[node->mMeshes[0]];
		parent_node.material = materials[scene->mMeshes[node->mMeshes[0]]->mMaterialIndex];
	}
	else
	{
		for (uint i = 0; i < node->mNumMeshes; ++i)
		{
			Node new_node;

			new_node.parent = parent + node->mNumMeshes;

			new_node.mesh = meshes[node->mMeshes[i]];
			new_node.material = materials[scene->mMeshes[node->mMeshes[i]]->mMaterialIndex];
			new_node.name = scene->mMeshes[node->mMeshes[i]]->mName.C_Str();
			//TODO
			new_node.name.resize(15);

			nodes.push_back(new_node);
		}
	}

	uint parent_id = nodes.size();
	nodes.push_back(parent_node);

	for (uint i = 0; i < node->mNumChildren; ++i)
	{
		GenerateNodes(scene, node->mChildren[i], parent_id, meshes, materials);
	}
	
}

void R_Model::ReleaseFromMem()
{

	for (uint i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i].mesh != 0)
		{
			App->resources->Get(nodes[i].mesh)->Release();
		}

		if (nodes[i].material != 0)
		{
			App->resources->Get(nodes[i].material)->Release();
		}
	}

	nodes.clear();
}