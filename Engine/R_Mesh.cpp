#include "Globals.h"
#include "R_Mesh.h"
#include "GL/glew.h"
#include "glmath.h"
#include "external/MathGeoLib/include/MathGeoLib.h"
#include "external/Assimp/include/mesh.h"

#include "Application.h"
#include "ModuleTextures.h"


R_Mesh::R_Mesh(UID uid) : Resource(uid, Type::MESH )
{
	for (uint i = 0; i < BufferType::MAX ; i++)
	{
		buffers_id[i] = 0;
		buffers_size[i] = 0;
	}
}

// TODO: update this constructor exclusively for primitives
R_Mesh::R_Mesh(UID uid,float* vertices, uint* indices, float* normals, float* uvs, uint n_vertices, uint n_indices) : Resource(uid,Type::MESH)
{ 
	// par_shapes meshes fixed num uv components per uv = 2
	this->uv_num_components = 2;

	for (uint i = 0; i < BufferType::MAX; i++)
	{
		buffers_id[i] = 0;
		buffers_size[i] = 0;
	}

	buffers_size[BufferType::VERTICES] = n_vertices;
	buffers_size[BufferType::INDICES] = n_indices * 3;
	buffers_size[BufferType::NORMALS] = n_vertices;
	buffers_size[BufferType::UVS] = n_vertices;

	this->vertices = new float[n_vertices * 3];
	this->indices = new uint[n_indices * 3];
	this->normals = new float[n_vertices * 3];


	memcpy(this->vertices, vertices, sizeof(float) * n_vertices * 3);
	memcpy(this->indices, indices, sizeof(uint) * n_indices * 3);
	memcpy(this->normals, normals, sizeof(float) * n_vertices * 3);

	if (uvs != nullptr)
	{
		this->uvs = new float[n_vertices * 2];
		memcpy(this->uvs, uvs, sizeof(float) * n_vertices * 2);
	}	
}

R_Mesh::~R_Mesh() 
{
	// ?
	Release();
}

// TODO: rename this like before "GenBuffers" / "GenBuffersAndLoad"
void R_Mesh::GLGenBuffersAndLoad()
{
	// TODO: REWORK THIS, not all meshes has all this components
	// Generate Buffers --------------------------------
	glGenBuffers(4, buffers_id);
	
	// Vertices ----------------
	glBindBuffer(GL_ARRAY_BUFFER, buffers_id[VERTICES]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * buffers_size[VERTICES], vertices, GL_STATIC_DRAW);

	// Indices -----------------
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers_id[INDICES]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * buffers_size[INDICES], indices, GL_STATIC_DRAW);

	// Normals -----------------
	if (buffers_size[NORMALS] != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffers_id[NORMALS]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * buffers_size[NORMALS], normals, GL_STATIC_DRAW);
	}

	// UV's --------------------
	if (buffers_size[UVS] != 0)
	{
		glBindBuffer(GL_ARRAY_BUFFER, buffers_id[UVS]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * buffers_size[UVS] * uv_num_components, uvs, GL_STATIC_DRAW);
	}
}

void R_Mesh::CreateAABB()
{
	aabb.SetNegativeInfinity();
	aabb.Enclose((math::float3*) vertices, buffers_size[VERTICES]);
}

// TODO: RENAME
void R_Mesh::ReleaseFromMem()
{
	glDeleteBuffers(4, buffers_id);

	// Delete all stored data

	RELEASE_ARRAY(vertices);
	RELEASE_ARRAY(indices);
	RELEASE_ARRAY(normals);
	RELEASE_ARRAY(uvs);

}

// TODO: pick value for ranges from enumerator and adds and assert
// TODO: BE aware on the future if we need uv_num_channels, you have to add here too
bool R_Mesh::SaveToFile(const char* name)
{
	uint ranges[BufferType::MAX + 1] = { buffers_size[BufferType::INDICES],buffers_size[BufferType::VERTICES], buffers_size[BufferType::NORMALS], buffers_size[BufferType::UVS], uv_num_components };

	uint size = sizeof(ranges) +
				sizeof(uint)   * buffers_size[BufferType::INDICES] +
				sizeof(float)  * buffers_size[BufferType::VERTICES] * 3 +
				sizeof(float)  * buffers_size[BufferType::NORMALS] * 3 +
				sizeof(float)  * buffers_size[BufferType::UVS] * uv_num_components +
				sizeof(uint)   * uv_num_components +
				sizeof(char)   * strlen(GetName().c_str()) + 1; // +1 for the null termination

	char* data = new char[size];
	char* cursor = data;

	uint bytes = sizeof(char) * strlen(GetName().c_str());
	//strcpy(cursor, GetName().c_str());
	if(bytes > 0)
		memcpy(cursor, GetName().c_str(), bytes);

	cursor += bytes;
	bytes = sizeof(char);
	strcpy(cursor, "\0");

	// TODO: on for loop picking the type size of the first element
	cursor += bytes;
	bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);

	cursor += bytes;
	bytes = sizeof(uint) * buffers_size[BufferType::INDICES];
	memcpy(cursor, indices, bytes);

	cursor += bytes;
	bytes = sizeof(float) * buffers_size[BufferType::VERTICES] * 3;
	memcpy(cursor, vertices, bytes);

	cursor += bytes;
	bytes = sizeof(float) * buffers_size[BufferType::NORMALS] * 3;
	memcpy(cursor, normals, bytes);

	cursor += bytes;
	bytes = sizeof(float) * buffers_size[BufferType::UVS] * uv_num_components;
	memcpy(cursor, uvs, bytes);


	//std::string full_name(LIBRARY_MESH_FOLDER + std::string(name) + std::string(".solidmesh"));
	std::string full_name(LIBRARY_MESH_FOLDER + std::string(name));

	//exported_file.assign(name + std::string(".solidmesh"));

	return App->file_sys->Save(full_name.c_str(), data, size) != -1 ? true:false;
}


bool R_Mesh::LoadInMemory()
{
	bool ret = true;

	char* buffer = nullptr;

	App->file_sys->Load(LIBRARY_MESH_FOLDER, GetNameFromUID().c_str(), &buffer);

	if (buffer != nullptr)
	{
		char* cursor = buffer;

		uint ranges[BufferType::MAX + 1];

		uint bytes = strlen(buffer);

		this->name.assign(buffer);

		cursor += bytes + 1; // for the forced previous null terminated char for mesh name
		bytes = sizeof(ranges);
		memcpy(ranges, cursor, bytes);

		
		buffers_size[BufferType::INDICES]  = ranges[0];
		buffers_size[BufferType::VERTICES] = ranges[1];
		buffers_size[BufferType::NORMALS]  = ranges[2];
		buffers_size[BufferType::UVS]	   = ranges[3];
		uv_num_components = ranges[BufferType::MAX];

		cursor += bytes;

		bytes = sizeof(uint) * buffers_size[BufferType::INDICES];
		indices = new uint[buffers_size[BufferType::INDICES]];
		memcpy(indices, cursor, bytes);

		cursor += bytes;

		bytes = sizeof(float) * buffers_size[BufferType::VERTICES] * 3;
		vertices = new float[buffers_size[BufferType::VERTICES] * 3];
		memcpy(vertices, cursor, bytes);

		cursor += bytes;

		bytes = sizeof(float) * buffers_size[BufferType::NORMALS] * 3;
		normals = new float[buffers_size[BufferType::NORMALS] * 3];
		memcpy(normals, cursor, bytes);

		cursor += bytes;

		bytes = sizeof(float) * buffers_size[BufferType::UVS] * uv_num_components;
		uvs = new float[buffers_size[BufferType::UVS] * uv_num_components];
		memcpy(uvs, cursor, bytes);

		// TODO: we want always load?
		// TODO: recalc aabb/obb
		GLGenBuffersAndLoad();
	}
	else
	{
		LOG("[Error] Loading mesh from %s", name);
	}

	return ret;
}


UID R_Mesh::Import(const aiMesh* mesh, const char* file)
{
	R_Mesh* r_mesh = static_cast<R_Mesh*>(App->resources->CreateNewResource(Resource::Type::MESH));

	r_mesh->GetName().assign(mesh->mName.C_Str());
	
	LOG("[Info] Loading mesh data : %s", r_mesh->GetName().c_str());
	
	// Load vertices data -----------------------------------------------------
	
	r_mesh->buffers_size[R_Mesh::VERTICES] = mesh->mNumVertices;
	r_mesh->vertices = new float[mesh->mNumVertices * 3];
	memcpy(r_mesh->vertices, mesh->mVertices, sizeof(float) * mesh->mNumVertices * 3);
	
	LOG("[Info] Vertices:    %u ", mesh->mNumVertices);
	
	// Load faces indices data ------------------------------------------------------
	
	if (mesh->HasFaces())
	{
		uint faces_warnings = 0;
		r_mesh->buffers_size[R_Mesh::INDICES] = mesh->mNumFaces * 3;
		r_mesh->indices = new  uint[mesh->mNumFaces * 3];
	
		for (uint i = 0; i < mesh->mNumFaces; ++i)
		{
			if (mesh->mFaces[i].mNumIndices != 3)
			{
				++faces_warnings;
				// TODO: rework this, we need to research how to load models with != 3 vertex per face
				// prevention for index accessor from computevertex and faces normals functions
				memset(&r_mesh->indices[i * 3], 0, sizeof(uint) * 3);
			}
			else
				memcpy(&r_mesh->indices[i * 3], mesh->mFaces[i].mIndices, 3 * sizeof(uint));
		}
	
		LOG("[Info] Indices:    %u", r_mesh->buffers_size[R_Mesh::INDICES]);
		LOG("[Info] Triangles:  %u", mesh->mNumFaces);
	
		if (faces_warnings != 0)
		{
			LOG("[Error], Mesh contains %u no triangle faces", faces_warnings);
		}
	}
	
	// Load normals data ------------------------------------------------------
	
	if (mesh->HasNormals())
	{
		r_mesh->buffers_size[R_Mesh::NORMALS] = mesh->mNumVertices;
		r_mesh->normals = new float[mesh->mNumVertices * 3];
		memcpy(r_mesh->normals, mesh->mNormals, sizeof(float) * mesh->mNumVertices * 3);
		LOG("[Info] Normals:   %u", mesh->mNumVertices);
	}
	
	// Load uvs data ----------------------------------------------------------
	
	uint num_uv_channels = mesh->GetNumUVChannels();
	r_mesh->uv_num_channels = num_uv_channels;
	
	if (num_uv_channels > 0)
	{
		if (num_uv_channels > 1)
			LOG("[Info] More than 1 uv|w channel for mesh");
	
		if (mesh->HasTextureCoords(0)) // TODO: support more texture coord channels if needed
		{
			r_mesh->uv_num_components = mesh->mNumUVComponents[0]; // num uv componenets for channel 0, if we expand this, we need to rework this too
			r_mesh->buffers_size[R_Mesh::UVS] = mesh->mNumVertices;
			r_mesh->uvs = new float[mesh->mNumVertices * r_mesh->uv_num_components];
	
			// we need a for loop if we want to improve memory performance, assimp stores its uvs in 3d vectors for uv/w 
	
			for (uint i = 0; i < mesh->mNumVertices; ++i)
			{
				memcpy(&r_mesh->uvs[i * r_mesh->uv_num_components], &mesh->mTextureCoords[0][i], sizeof(float) * 2);
			}
		}
	}
	
	// Load aabb ---------------------------------------------------------------
	
	r_mesh->aabb.SetNegativeInfinity();
	
	r_mesh->aabb.minPoint.x = mesh->mAABB.mMin.x;
	r_mesh->aabb.minPoint.y = mesh->mAABB.mMin.y;
	r_mesh->aabb.minPoint.z = mesh->mAABB.mMin.z;
	
	r_mesh->aabb.maxPoint.x = mesh->mAABB.mMax.x;
	r_mesh->aabb.maxPoint.y = mesh->mAABB.mMax.y;
	r_mesh->aabb.maxPoint.z = mesh->mAABB.mMax.z;

	// SAVE to own file format

	if (r_mesh->SaveToFile(std::to_string(r_mesh->GetUID()).c_str()))
	{
		LOG("[Info] Succesfully saved %i", r_mesh->GetUID());

		// Testing new load with mesh filename embedded - ok
		//r_mesh->LoadFromFile(std::to_string(r_mesh->GetUID()).c_str());
	}
	else
	{
		LOG("[Error] Saving %s", mesh->mName.C_Str());
	}
	
	LOG("Created mesh data: %s", r_mesh->GetName().c_str());

	// free mesh data
	r_mesh->Release();
	
	return r_mesh->uid;
}