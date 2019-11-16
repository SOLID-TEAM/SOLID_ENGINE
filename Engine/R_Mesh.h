#ifndef __R_MESH_H__
#define __R_MESH_H__

#include "Globals.h"
#include "Resource.h"
#include <vector>
#include <string>
#include "external/MathGeoLib/include/Geometry/AABB.h"

struct aiMesh;

class R_Mesh : public Resource
{

public:

	R_Mesh(UID uid);

	R_Mesh(UID uid, float* vertices, uint* indices, float* normals, float* uvs, uint n_vertices, uint n_indices);

	~R_Mesh();

	void GLGenBuffersAndLoad();

	void CreateAABB();

	void Unload();

	bool LoadInMemory();

	// we need static to call this function without the needs of creating new class
	static UID Import(const aiMesh* mesh, const char* file);

	bool SaveToFile(const char* name);
	bool LoadFromFile(const char* name);

public:

	enum  BufferType
	{
		VERTICES = 0,
		NORMALS,
		INDICES,
		UVS,
		MAX
	};
	
public:

	// Data ------------------------
	AABB aabb;

	uint buffers_id[BufferType::MAX];
	uint buffers_size[BufferType::MAX];

	uint*	indices = nullptr;
	float*	vertices = nullptr;
	float*	normals = nullptr;
	float*	uvs = nullptr;
	uint	uv_num_channels = 0;
	uint    uv_num_components = 0;

	// ---------------------------------------------------------------
	// TODO: MOVE TO MATERIAL, when finished
	uint texture_id = 0; 
	float color[3] = { 1.0f,0.0f,1.0f }; 
	// ---------------------------------------------------------------
};

#endif // !__R_MESH_H__