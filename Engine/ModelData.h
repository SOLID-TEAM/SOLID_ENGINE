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
	//UpdateVertexBuffer(uint vb_id);
	//UpdateIndicesBuffer(uint fb_id);
	//UpdateTexture2DBuffer(uint tex_id);
	//UpdateTexture2DUVBuffer(uint uvs_id);
	//UseShaderID(uint shader_program_id); 

	bool DebugRenderVertex(float pointSize = 5.0f);
	bool DebugRenderVertexNormals(float lineWidth = 1.0f);
	bool DebugRenderFacesVertex(float pointSize = 5.0f);
	bool DebugRenderFacesNormals(float lineWidth = 1.0f);

	void ComputeVertexNormals(float length = 0.4f);
	bool ComputeFacesNormals(float length = 0.4f);

	bool RefillDebugVertexNormalsBuffers();
	bool RefillDebugFacesNormalsBuffers();

	// void Scale();
	// void Translate();
	// void ApplyRotation();
	// mat4x4 GetTransform();

	//const char* GetName() const;

	bool Render();
	bool RenderWireframe();

	bool CleanUp();

	
public:
	std::string name;
	bool to_delete = false;
	// model data ---------------
	uint _v_size = 0; // vertex
	uint _idx_size = 0; // indices
	//uint _uv_size = 0; // texture coords
	uint _uv_num_components = 0;
	//uint _n_size = -1; // normals || not needed, assume each vertes has its normal
	//uint _f_size = -1; // triangulated faces
	float* vertices = nullptr;
	uint* indices = nullptr;
	float* uvs = nullptr;
	float* normals = nullptr;
	// debug draw normals data ------
	float* debug_v_normals = nullptr;
	// maybe we dont need this for nothing more than debug ----
	float* debug_f_vertices = nullptr; // this array should store 1 computed vertex(for draw the point on midface) for each face
	float* debug_f_normals = nullptr; // and 3 startpoint and 3 endpoint of each face
	// --------------------------
	// gl buffer ----------------
	uint vertices_gl_id = 0;
	uint indices_gl_id = 0;
	
	uint uv_gl_id = 0;
	uint normals_gl_id = 0;
	// DEBUG DRAW PURPOSES BUFFERS ids -------------------------------
	uint debug_v_normals_gl_id = 0;
	// for debug draw faces points and line normals
	// TODO: implement this with stride in one float array (debug_f_vertices and debug_f_normals float arrays)
	uint debug_f_vertices_gl_id = 0;
	uint debug_f_normals_gl_id = 0;
	// ---------------------------------------------------------------
	uint texture_gl_id = 0;
	//
	float color[3] = { 1.0f,0.0f,1.0f };
	// --------------------------
	// world --------------------
	mat4x4 transform;
	vec4 position;
	vec4 direction;

	// TODO: testing here, print points and lines for vertex and faces normals debug draw
	bool debug_vertex_normals = true;
	bool debug_faces_normals = false;

private:
	// containers
	/*std::vector<uint*> c_gl_buffers;
	std::vector<uint*> c_sizes;
	std::vector<uint*> c_uvs;
	std::vector<uint*> c_normals;*/
	
};

#endif // !__MODELDATA_H__