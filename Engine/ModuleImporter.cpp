#define PAR_SHAPES_IMPLEMENTATION

#include "GL/glew.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleImporter.h"
#include "GameObject.h"
#include "Random.h"

// Include External -------------------------------
#include "external/MathGeoLib/include/MathGeoLib.h"
#include "external/par_shapes.h"
#include "external/Assimp/include/cimport.h"
#include "external/Assimp/include/scene.h"
#include "external/Assimp/include/postprocess.h"
#include "external/Assimp/include/cfileio.h"
#include "external/Assimp/include/version.h"

#pragma comment (lib, "external/Assimp/libx86/assimp.lib")

// Include Components ----------------------------
#include "Component.h"
#include "C_Mesh.h"
#include "C_Material.h"
#include "C_MeshRenderer.h"

// Include Data ----------------------------------
#include "Data.h"
#include "D_Mesh.h"
#include "D_Material.h"
#include "D_Texture.h"

ModuleImporter::ModuleImporter(bool start_enabled) : Module(start_enabled)
{
	name.assign("ModuleImporter");
}

ModuleImporter::~ModuleImporter() {}

bool ModuleImporter::Init(Config& config)
{

	// Init Assimp --------------------------------------------------

	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	LOG("[Info] Using Assimp v%i.%i.%i", aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionRevision());

	Load(config);

	return true;
}

bool ModuleImporter::Start(Config& config)
{
	return true;
}

update_status ModuleImporter::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleImporter::PostUpdate(float dt)
{
	
	return UPDATE_CONTINUE;
}

bool ModuleImporter::CleanUp()
{
	
	aiDetachAllLogStreams();

	return true;
}

// returns sequentially uid's for filename naming
UID ModuleImporter::GetNewUID()
{
	return last_id++;
}


// directly drag and drop -> import directly and create own file
bool ModuleImporter::ImportModelFile(const char* path)
{
	bool ret = true;
	
	// copy model to own assets project folder ------
	App->file_sys->DuplicateFile(path, ASSETS_FOLDER, std::string("/"));
	std::string file;
	std::string ext;
	App->file_sys->SplitFilePath(path, nullptr, &file, &ext);
	LOG("[Info] Copied model file %s to %s folder", (file+ "." + ext).c_str(), ASSETS_FOLDER);
	// -----------------------------------------------

	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_GenBoundingBoxes); // not needed, already included on quality | max quality preset

	if(scene != nullptr)
	{
		// reassign relative path (for texture loading)
		App->file_sys->SplitFilePath(path, &relative_path_from_model);

		GameObject* root = nullptr;

		// If first node has more than 1 mesh ------------
		if (scene->mRootNode->mNumMeshes == 0u)
		{
			std::string name("");
			App->file_sys->SplitFilePath(path, &(std::string("")), &name);
			root = new GameObject(name, App->scene->root_go);
		}
		else
		{
			root = App->scene->root_go;
		}

		// Create separate gameobjects from nodes
		CreateGoFromNodes(scene, scene->mRootNode, root);

		// Free the imported scene
		aiReleaseImport(scene);
	}
	else
		LOG("[Error] Loading scene %s ", path);

	return ret;
}

std::string ModuleImporter::GetFormattedName(std::string name, UID id)
{
	std::string plain_name;
	std::string extension; // this is not really the extension, only the "extension name" then sometimes is nothing

	App->file_sys->SplitFilePath(name.c_str(), nullptr, &plain_name, &extension);

	return std::string(plain_name + "_" + std::to_string(id) + "_" + extension);
}

void ModuleImporter::CreateGoFromNodes(const aiScene* scene , aiNode* node, GameObject* parent)
{
	GameObject* root_go = nullptr;
	//aiMatrix4x4 transform;

	// Set Transform Vars ------------------------------------------------

	aiVector3D		translation;
	aiVector3D		scaling;
	aiQuaternion	rotation;

	node->mTransformation.Decompose(scaling, rotation, translation);
	float3	position(translation.x, translation.y, translation.z);
	float3	scale(scaling.x, scaling.y, scaling.z);
	Quat	quad_rotation(rotation.x, rotation.y, rotation.z, rotation.w);


	GameObject* new_go;

	if (node->mNumMeshes > 0)
	{
		// Import Meshes -------------------------------

		for (uint i = 0; i < node->mNumMeshes; ++i)
		{
			aiMesh*		ai_mesh = scene->mMeshes[node->mMeshes[i]];
			aiMaterial*  ai_material = scene->mMaterials[ai_mesh->mMaterialIndex];

			// Create GameObject ---------------

			new_go = new GameObject(node->mName.data, parent);

			//  Add Component Mesh -------------

 			C_Mesh* c_mesh = new_go->CreateComponent< C_Mesh>();
			c_mesh->data = ImportMesh( ai_mesh, ai_mesh->mName.C_Str() );
			// and save own format to disk
			std::string test_name = GetFormattedName(ai_mesh->mName.C_Str(), GetNewUID()).c_str();
			c_mesh->data->SaveToFile(test_name.c_str());

			// TESTING LOAD FROM custom file - OK
			/*std::string own_filename = c_mesh->data->GetExportedName().c_str();
			delete c_mesh->data;
			c_mesh->data = nullptr;

			c_mesh->data = new D_Mesh();
			c_mesh->data->LoadFromFile(own_filename.c_str());*/
		
			// Add Component Material ----------

			C_Material* c_material = new_go->CreateComponent< C_Material>();
			c_material->data = ImportMaterial(ai_material, ai_material->GetName().C_Str());
			// check if the data contains texture data, if not, uncheck textured by default on c_mesh
			if (c_material->data->textures[0] == nullptr) // TODO: ON DIFFUSE CHANNEL FOR NOW
				c_material->textured = false;

			// TODO: NOT TESTED
			c_material->data->SaveToFile("testing.solidmat");


			// Add Component Mesh Renderer -----

			new_go->CreateComponent< C_MeshRenderer >();

			// TODO, Create ID and search ID on Library to have not cloned data
		}



		root_go = new_go;
	}
	else
	{
		//new_go  = new GameObject(node->mName.data, parent); // gameobject adds itself as child of parent
		root_go = parent;
		//transform = node->mTransformation * accumulated_transform;
	}

	// recursively continue for all child nodes
	for (uint i = 0; i < node->mNumChildren; ++i)
	{
		CreateGoFromNodes(scene, node->mChildren[i], root_go);
	}

}

// TODO: All imports need to check if they are already loaded in resources module ( now they are cloning)

D_Mesh*  ModuleImporter::ImportMesh(const aiMesh* mesh, const char* name)
{
	D_Mesh* d_mesh = new  D_Mesh();
	d_mesh->GetName().assign(name);

	LOG("[Info] Loading mesh data : %s", d_mesh->GetName().data());

	// Load vertices data -----------------------------------------------------

	d_mesh->buffers_size[D_Mesh::VERTICES] = mesh->mNumVertices;
	d_mesh->vertices = new float[mesh->mNumVertices * 3];
	memcpy(d_mesh->vertices, mesh->mVertices, sizeof(float) * mesh->mNumVertices * 3);

	LOG("[Info] Vertices:    %u ", mesh->mNumVertices);

	// Load faces indices data ------------------------------------------------------

	if (mesh->HasFaces())
	{
		uint faces_warnings = 0;
		d_mesh->buffers_size[D_Mesh::INDICES] = mesh->mNumFaces * 3;
		d_mesh->indices = new  uint[mesh->mNumFaces * 3];

		for (uint i = 0; i < mesh->mNumFaces; ++i)
		{
			if (mesh->mFaces[i].mNumIndices != 3)
			{
				++faces_warnings;
				// TODO: rework this, we need to research how to load models with != 3 vertex per face
				// prevention for index accessor from computevertex and faces normals functions
				memset(&d_mesh->indices[i * 3], 0, sizeof(uint) * 3);
			}
			else
				memcpy(&d_mesh->indices[i * 3], mesh->mFaces[i].mIndices, 3 * sizeof(uint));
		}

		LOG("[Info] Indices:    %u", d_mesh->buffers_size[D_Mesh::INDICES]);
		LOG("[Info] Triangles:  %u", mesh->mNumFaces);

		if (faces_warnings != 0)
		{
			LOG("[Error], Mesh contains %u no triangle faces", faces_warnings);
		}
	}

	// Load normals data ------------------------------------------------------

	if (mesh->HasNormals())
	{
		d_mesh->buffers_size[D_Mesh::NORMALS] = mesh->mNumVertices;
		d_mesh->normals = new float[mesh->mNumVertices * 3];
		memcpy(d_mesh->normals, mesh->mNormals, sizeof(float) * mesh->mNumVertices * 3);
		LOG("[Info] Normals:   %u", mesh->mNumVertices);
	}

	// Load uvs data ----------------------------------------------------------
	
	uint num_uv_channels = mesh->GetNumUVChannels();
	d_mesh->uv_num_channels = num_uv_channels;

	if (num_uv_channels > 0)
	{
		if (num_uv_channels > 1)
			LOG("[Info] More than 1 uv|w channel for mesh");

		if (mesh->HasTextureCoords(0)) // TODO: support more texture coord channels if needed
		{
			d_mesh->uv_num_components = mesh->mNumUVComponents[0]; // num uv componenets for channel 0, if we expand this, we need to rework this too
			d_mesh->buffers_size[D_Mesh::UVS] = mesh->mNumVertices;
			d_mesh->uvs = new float[mesh->mNumVertices * d_mesh->uv_num_components];

			// we need a for loop if we want to improve memory performance, assimp stores its uvs in 3d vectors for uv/w 

			for (uint i = 0; i < mesh->mNumVertices; ++i)
			{
				memcpy(&d_mesh->uvs[i * d_mesh->uv_num_components], &mesh->mTextureCoords[0][i], sizeof(float) * 2);
			}
		}
	}

	// Load aabb ---------------------------------------------------------------

	d_mesh->aabb.SetNegativeInfinity();
	
	d_mesh->aabb.minPoint.x = mesh->mAABB.mMin.x;
	d_mesh->aabb.minPoint.y = mesh->mAABB.mMin.y;
	d_mesh->aabb.minPoint.z = mesh->mAABB.mMin.z;

	d_mesh->aabb.maxPoint.x = mesh->mAABB.mMax.x;
	d_mesh->aabb.maxPoint.y = mesh->mAABB.mMax.y;
	d_mesh->aabb.maxPoint.z = mesh->mAABB.mMax.z;

	d_mesh->Load();

	LOG("Created mesh data: %s", d_mesh->GetName().c_str());

	return d_mesh;
}


D_Material* ModuleImporter::ImportMaterial(const aiMaterial* material , const char* name)
{
	D_Material* d_material = new D_Material();
	d_material->GetName().assign(name);

	// Set albedo color ----------------------------------------------------------

	aiColor4D color;
	material->Get(AI_MATKEY_COLOR_DIFFUSE,color);
	d_material->diffuse_color = { color.r, color.g, color.b, color.a };

	// Get assimp path and normalize --------

	std::string file;       // file_name + file_ex
	std::string file_name;
	std::string file_ex;
	aiString	ai_path;

	material->GetTexture(aiTextureType_DIFFUSE, 0, &ai_path);  

	if (ai_path.length > 0)
	{
		App->file_sys->SplitFilePath(ai_path.C_Str(), nullptr, &file_name, &file_ex);
		file = relative_path_from_model + file_name + "." + file_ex;

		// Load material textures -------------------------------------------------  // TODO: Only diffuse currently , if we have to do shaders get more textures :)

		d_material->textures[D_Material::DIFFUSE] = ImportTexture(file.c_str());
	}
	else
	{
		LOG("[Error] NO TEXTURE ON MODEL");
	}


	return d_material;
}

D_Material* ModuleImporter::CreateDefaultMaterial(const char* name, float4 color) const
{
	return  new D_Material(name, color);
}


D_Texture* ModuleImporter::ImportTexture(const char* path)
{
	D_Texture* d_texture = new D_Texture();
	d_texture->buffer_id =  App->textures->LoadTexture(path);
	return d_texture;
}

GameObject* ModuleImporter::CreatePrimitive(PrimitiveType type, float3 position, float3 size, float2 slicesStacks, float4 color)
{
	bool isPlatonic = false; // par_shapes weld its vertices on creation and needs to be unwelded to compute normals
	std::string name;
	par_shapes_mesh* p_mesh = nullptr;

	switch (type)
	{
	case CUBE:
		p_mesh = par_shapes_create_cube();
		isPlatonic = true;
		name.assign("Cube");
		break;
	case ICOSPHERE:
		/*p_mesh = par_shapes_create_icosahedron();*/
		p_mesh = par_shapes_create_subdivided_sphere(1);
		//isPlatonic = true;
		name.assign("Icosphere");
		break;
	case PLANE:
		p_mesh = par_shapes_create_plane(slicesStacks.x, slicesStacks.y);
		name.assign("Plane");
		break;
	case SPHERE:
		p_mesh = par_shapes_create_parametric_sphere(slicesStacks.x, slicesStacks.y);
		name.assign("Sphere");
		break;
	case CYLINDER:
		p_mesh = par_shapes_create_cylinder(slicesStacks.x, slicesStacks.y);
		name.assign("Cylinder");
		break;
	case CONE:
		p_mesh = par_shapes_create_cone(slicesStacks.x, slicesStacks.y);
		name.assign("Cone");
		break;
	case TORUS:
		p_mesh = par_shapes_create_torus(slicesStacks.x, slicesStacks.y, size.x * 0.5f); // Gets the radius from the size x component
		name.assign("Torus");
		break;
	case MAX:
		break;
	default:
		break;
	}

	/*if (p_mesh == nullptr)
	{
		return;
	}*/
	

	if (isPlatonic)
	{
		par_shapes_unweld(p_mesh, true);
		par_shapes_compute_normals(p_mesh);
	}

	par_shapes_scale(p_mesh, size.x, size.y, size.z);
	par_shapes_translate(p_mesh, position.x, position.y, position.z);

	// Create GameObject -------------------------------------

	GameObject* gameobject;


	if (App->scene->selected_go != nullptr)
	{
		gameobject = new GameObject(name, App->scene->selected_go);
	}
	else
	{
		gameobject = new GameObject(name, App->scene->root_go);
	}
	

	// Components --------------------------------------------
	C_Mesh* c_mesh = gameobject->CreateComponent<C_Mesh>();
	c_mesh->data = new D_Mesh(p_mesh->points, p_mesh->triangles, p_mesh->normals, p_mesh->tcoords, p_mesh->npoints, p_mesh->ntriangles);
	c_mesh->data->name.assign(name.data());
	c_mesh->data->Load();
	c_mesh->data->CreateAABB();

	C_Material* c_material = gameobject->CreateComponent< C_Material>();
	c_material->data = CreateDefaultMaterial("Default material", color);
	c_material->textured = false;

	C_MeshRenderer* c_renderer = gameobject->CreateComponent< C_MeshRenderer>();

	par_shapes_free_mesh(p_mesh);

	return gameobject;
}

bool ModuleImporter::Save(Config& config)
{
	bool ret = true;


	return ret;
}

void ModuleImporter::Load(Config& config)
{

}

std::vector<D_Mesh*>& ModuleImporter::GetModels()
{
	return meshes;
}

//void ModuleImporter::ImportFileFromPath(const char* path)
//{
//	std::string normalized_path = App->file_sys->NormalizePath(path);
//	std::string final_path;
//
//	// Duplicate file to the indicated internal path 
//	if (App->file_sys->DuplicateFile(normalized_path.c_str(), "Project/Assets", final_path))
//	{
//		// Load asset and create meta data 
//		// If is a data import and add to scene 
//		ImportModelFile(normalized_path.c_str()); // Model test
//	}
//}

void ModuleImporter::ReloadTextureForAllModels(uint texture_id)
{
	std::vector<D_Mesh*>::iterator models = meshes.begin();
	for (; models != meshes.end(); ++models)
	{
		// if the model has the same texture id, not free the buffer
		if ((*models)->texture_id == texture_id)
		{
			LOG("[Info] Texure id of the model already loaded");
			continue;
		}

		// TODO: this is not the functionality we want, just for test
		// free all the others and re assign new id
		if (App->textures->FreeTextureBuffer((*models)->texture_id))
		{
			LOG("[Info] freed texture buffer of %s model", (*models)->name.data());
			
		}
		else
		{
			LOG("[Info] texture not found on texture map or previously freed");
		}

		// re-assign new id
		(*models)->texture_id = texture_id;
		
	}
}