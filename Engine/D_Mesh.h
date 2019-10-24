#ifndef __MODELDATA_H__
#define __MODELDATA_H__

#include "Globals.h"
#include "Data.h"
#include <vector>
#include <string>
#include "external/MathGeoLib/include/Geometry/AABB.h"

class D_Mesh : public Data
{

public:

	D_Mesh();

	D_Mesh( float* vertices, uint* indices, float* normals, float* uvs, uint n_vertices, uint n_indices);

	~D_Mesh();

	void Load();

	void Unload();

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

#endif // !__MODELDATA_H__