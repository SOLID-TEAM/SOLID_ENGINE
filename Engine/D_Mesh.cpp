#include "D_Mesh.h"
#include "GL/glew.h"
#include "glmath.h"
#include "external/MathGeoLib/include/MathGeoLib.h"

#include "Application.h"
#include "ModuleTextures.h"


D_Mesh::D_Mesh() : Data( DataType::MESH )
{
	for (uint i = 0; i < BufferType::MAX ; i++)
	{
		buffers_id[i] = 0;
		buffers_size[i] = 0;
	}
}

D_Mesh::D_Mesh(float* vertices, uint* indices, float* normals, float* uvs, uint n_vertices, uint n_indices) : Data(DataType::MESH), 
vertices(vertices), indices(indices),  uvs(uvs)
{ 
	for (uint i = 0; i < BufferType::MAX; i++)
	{
		buffers_id[i] = 0;
		buffers_size[i] = 0;
	}

	buffers_size[BufferType::VERTICES] = n_vertices;
	buffers_size[BufferType::INDICES] = n_indices;

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

D_Mesh::~D_Mesh() 
{
	Unload();
}

void D_Mesh::Load()
{
	// Generate Buffers --------------------------------
	glGenBuffers(4, buffers_id);

	// Vertives ----------------
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
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * buffers_size[UVS] *2, uvs, GL_STATIC_DRAW);
	}

	CreateAABB();
}


void D_Mesh::Unload()
{
	glDeleteBuffers(4, buffers_id);

	// Delete all stored data

	delete[] vertices;
	delete[] indices;
	delete[] normals;
	delete[] uvs;
}

void D_Mesh::CreateAABB()
{
	aabb.SetNegativeInfinity();
	aabb.Enclose( (math::float3*) vertices, buffers_size[VERTICES]);
}

//texture_id = App->textures->GenerateCheckerTexture(512,512);

/*glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
glGenTextures(1, &texture_id);*/