#ifndef __MODELDATA_H__
#define __MODELDATA_H__

#include "Globals.h"
#include "glmath.h"
#include <vector>
#include <string>

typedef unsigned char GLubyte;

class ModelData
{
public:
	ModelData();
	~ModelData();

	ModelData(float* vertices, uint* indices, float* normals, float* uvs, int nVertices, int nIndices);

	bool GenerateBuffers(); // generate all needed buffers at once
	bool UpdateBuffers(); // update/fill/refill all buffers
	
	//const char* GetName() const;
	bool Start();
	bool Render();
	bool RenderWireframe();
	bool RenderOutline();

	bool CleanUp();

	
public:
	std::string name;
	bool to_delete = false;
	// model data ---------------
	uint _v_size = 0; // vertex
	uint _idx_size = 0; // indices
	uint _uv_num_components = 0;
	float* vertices = nullptr;
	uint* indices = nullptr;
	float* uvs = nullptr;
	float* normals = nullptr;
	// --------------------------
	// gl buffer ----------------
	uint vertices_gl_id = 0;
	uint indices_gl_id = 0;
	uint uv_gl_id = 0;
	uint normals_gl_id = 0;
	// ---------------------------------------------------------------
	// TODO: MOVE TO MATERIAL, when finished
	uint texture_gl_id = 0; 
	float color[3] = { 1.0f,0.0f,1.0f }; 
	// ---------------------------------------------------------------
	
};

#endif // !__MODELDATA_H__