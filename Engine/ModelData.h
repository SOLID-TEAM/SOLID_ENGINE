#pragma once

#include "Globals.h"
#include "glmath.h"
#include <vector>

class ModelData
{
public:
	ModelData();
	~ModelData();

	bool GenerateBuffers(); // generate all needed buffers at once
	bool UpdateBuffers(); // update/fill/refill all buffers
	//UpdateVertexBuffer(uint vb_id);
	//UpdateIndicesBuffer(uint fb_id);
	//UpdateTexture2DBuffer(uint tex_id);
	//UpdateTexture2DUVBuffer(uint uvs_id);
	//UseShaderID(uint shader_program_id); 

	// void Scale();
	// void Translate();
	// void ApplyRotation();
	// mat4x4 GetTransform();
	
	bool Render();

	bool CleanUp();

	
public:
	bool to_delete = false;
	// model data ---------------
	uint _v_size = -1; // vertex
	uint _idx_size = -1; // indices
	uint _uv_size = -1; // texture coords
	uint _n_size = -1; // normals
	
	float* vertices = nullptr;
	uint* indices = nullptr;
	float* uvs = nullptr;
	float* normals = nullptr;
	// --------------------------
	// gl buffer ----------------
	uint vertices_gl_id = -1;
	uint indices_gl_id = -1;
	
	uint uv_gl_id = -1;
	uint normals_gl_id = -1;
	//uint texture_gl_id = -1;
	//
	float color[3] = { 1.0f,0.0f,1.0f };
	// --------------------------
	// world --------------------
	mat4x4 transform;
	vec4 position;
	vec4 direction;

private:
	// containers
	/*std::vector<uint*> c_gl_buffers;
	std::vector<uint*> c_sizes;
	std::vector<uint*> c_uvs;
	std::vector<uint*> c_normals;*/
	
};