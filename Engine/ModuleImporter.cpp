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

	//ImportModelFile("child_test999.fbx");
	//ImportModelFile("child_test_definitive_edition_remaster.fbx");

	ImportModelFile("Assets/Models/BakerHouse.fbx");

	//ImportModelFile("Assets/Models/hammer_low.fbx");
	//ImportModelFile("Assets/Models/suzanne.solid");
	////ImportModelFile("Assets/Models/warrior.FBX");

	/*CreatePrimitive(CUBE, { 2,0,0 }, { 1,1,1 });
	CreatePrimitive(SPHERE, { -3,0,0 }, { 1,1,1 });
	CreatePrimitive(TETRAHEDRON, { 0,1,0 }, { 1,1.2f,1 });
	CreatePrimitive(ICOSAHEDRON, { 4.5f,0,0 }, { 1,1,1 });*/

	Random::GetRandomPercent();

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

bool ModuleImporter::ImportModelFile(const char* path)
{
	bool ret = true;

	// TODO: duplicate model file whatever we want on internal files

	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate  | aiProcess_GenSmoothNormals | aiProcess_GenBoundingBoxes);

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
	//Quat	rotation(rotation.x, rotation.y, rotation.z, rotation.w);


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

 			C_Mesh* c_mesh = (C_Mesh*)new_go->CreateComponent(ComponentType::MESH);
			c_mesh->data = ImportMesh( ai_mesh, ai_mesh->mName.C_Str() );

			// Add Component Material ----------

			C_Material* c_material = (C_Material*)new_go->CreateComponent(ComponentType::MATERIAL);
			c_material->data = ImportMaterial(ai_material, ai_material->GetName().C_Str());

			// Add Component Mesh Renderer -----

			new_go->CreateComponent(ComponentType::MESH_RENDERER);

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
			LOG("[Error] More than 1 uv|w channel for mesh");

		if (mesh->HasTextureCoords(0)) // TODO: support more texture coord channels if needed
		{
			d_mesh->buffers_size[D_Mesh::UVS] = mesh->mNumVertices;
			d_mesh->uvs = new float[mesh->mNumVertices * 2];

			// we need a for loop if we want to improve memory performance, assimp stores its uvs in 3d vectors for uv/w 

			for (uint i = 0; i < mesh->mNumVertices; ++i)
			{
				memcpy(&d_mesh->uvs[i * 2], &mesh->mTextureCoords[0][i], sizeof(float) * 2);
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
	d_material->albedo_color = Color(color.r, color.g, color.b, color.a);

	// Get assimp path and normalize --------

	std::string file;       // file_name + file_ex
	std::string file_name;
	std::string file_ex;
	aiString	ai_path;

	material->GetTexture(aiTextureType_DIFFUSE, 0, &ai_path);   
	App->file_sys->SplitFilePath(ai_path.C_Str(), nullptr, &file_name, &file_ex);
	file = relative_path_from_model + file_name + "." + file_ex;

	// Load material textures -------------------------------------------------  // TODO: Only diffuse currently , if we have to do shaders get more textures :)

	d_material->textures[D_Material::DIFFUSE] = ImportTexture(file.c_str());

	return d_material;
}


D_Texture* ModuleImporter::ImportTexture(const char* path)
{
	D_Texture* d_texture = new D_Texture();
	d_texture->buffer_id =  App->textures->LoadTexture(path);
	return d_texture;
}

void ModuleImporter::CreatePrimitive(PrimitiveType type, vec3 position, vec3 size)
{
	bool isPlatonic = false; // par_shapes weld its vertices on creation and needs to be unwelded to compute normals
	std::string name;
	par_shapes_mesh* p_mesh = nullptr;

	// TODO: from the panel decide the division slices and stacks of all parametric primitives
	int slices = 20;
	int stacks = 20;

	switch (type)
	{
	case CUBE:
		p_mesh = par_shapes_create_cube();
		isPlatonic = true;
		name.assign("Cube");
		break;
	case DODECAHEDRON:
		p_mesh = par_shapes_create_dodecahedron();
		isPlatonic = true;
		name.assign("Dodecahedron");
		break;
	case TETRAHEDRON:
		p_mesh = par_shapes_create_tetrahedron();
		isPlatonic = true;
		name.assign("Tetrahedron");
		break;
	case OCTOHEDRON:
		p_mesh = par_shapes_create_octahedron();
		isPlatonic = true;
		name.assign("Octohedron");
		break;
	case ICOSAHEDRON:
		p_mesh = par_shapes_create_icosahedron();
		isPlatonic = true;
		name.assign("Icosahedron");
		break;
	case PLANE:
		p_mesh = par_shapes_create_plane(slices, stacks);
		name.assign("Plane");
		break;
	case SPHERE:
		p_mesh = par_shapes_create_parametric_sphere(slices, stacks);
		name.assign("Sphere");
		break;
	case CYLINDER:
		p_mesh = par_shapes_create_cylinder(slices, stacks);
		name.assign("Cylinder");
		break;
	case CONE:
		p_mesh = par_shapes_create_cone(slices, stacks);
		name.assign("Cone");
		break;
	case TORUS:
		p_mesh = par_shapes_create_torus(slices, stacks, size.x * 0.5f); // Gets the radius from the size x component
		name.assign("Torus");
		break;
	case MAX:
		break;
	default:
		break;
	}

	if (p_mesh == nullptr)
	{
		return;
	}
	

	if (isPlatonic)
	{
		par_shapes_unweld(p_mesh, true);
		par_shapes_compute_normals(p_mesh);
	}

	par_shapes_scale(p_mesh, size.x, size.y, size.z);
	par_shapes_translate(p_mesh, position.x, position.y, position.z);

	// Create GameObject -------------------------------------

	GameObject* gameobject = new GameObject(name, App->scene->root_go);

	// Components --------------------------------------------
	C_Mesh* c_mesh = (C_Mesh*)gameobject->CreateComponent(ComponentType::MESH);
	c_mesh->data = new D_Mesh(p_mesh->points, p_mesh->triangles, p_mesh->normals, p_mesh->tcoords, p_mesh->npoints, p_mesh->ntriangles);
	c_mesh->data->name.assign(name.data());
	c_mesh->data->Load();

	C_MeshRenderer* c_renderer = (C_MeshRenderer*)gameobject->CreateComponent(ComponentType::MESH_RENDERER);

	par_shapes_free_mesh(p_mesh);
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