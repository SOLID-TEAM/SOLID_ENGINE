#include "R_Mesh.h"
#include "GL/glew.h"
#include "glmath.h"
#include "external/MathGeoLib/include/MathGeoLib.h"

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
	Unload();
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
void R_Mesh::Unload()
{
	glDeleteBuffers(4, buffers_id);

	// Delete all stored data

	delete[] vertices;
	delete[] indices;
	delete[] normals;
	delete[] uvs;
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
				sizeof(uint)   * uv_num_components;

	char* data = new char[size];
	char* cursor = data;

	// TODO: on for loop picking the type size of the first element
	uint bytes = sizeof(ranges);
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


	std::string full_name(LIBRARY_MESH_FOLDER + std::string(name) + std::string(".solidmesh"));

	exported_file.assign(name + std::string(".solidmesh"));

	return App->file_sys->Save(full_name.c_str(), data, size) != -1 ? true:false;
}


bool R_Mesh::LoadFromFile(const char* name)
{
	bool ret = true;

	char* buffer = nullptr;

	App->file_sys->Load(LIBRARY_MESH_FOLDER,name, &buffer);

	if (buffer != nullptr)
	{

		char* cursor = buffer;

		uint ranges[BufferType::MAX + 1];
		uint bytes = sizeof(ranges);
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

bool R_Mesh::LoadInMemory()
{
	return true;
}