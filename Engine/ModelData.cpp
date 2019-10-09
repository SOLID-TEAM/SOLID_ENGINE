#include "ModelData.h"
#include "GL/glew.h"

ModelData::ModelData() {}

ModelData::ModelData(float* vertices, uint* indices, float* normals, float* uvs, int nVertices, int nIndices)
{
	this->vertices = new float[nVertices * 3];
	this->indices = new uint[nIndices * 3];
	this->normals = new float[nVertices * 3];
	//this->uvs = new float[nVertices * 2];

	memcpy(this->vertices, vertices, sizeof(float) * nVertices * 3);
	memcpy(this->indices, indices, sizeof(uint) * nIndices * 3);
	memcpy(this->normals, normals, sizeof(float) * nVertices * 3);
	//memcpy(this->uvs, uvs, sizeof(float) * nVertices * 2);

	_v_size = nVertices;
	_idx_size = nIndices * 3;

	GenerateBuffers();
	ComputeVertexNormals();
	ComputeFacesNormals();
	UpdateBuffers();
}

ModelData::~ModelData() {}

bool ModelData::GenerateBuffers()
{
	bool ret = true;
	// TODO: maybe is a good idea to use pointers to generate all buffers at once
	glGenBuffers(1, &vertices_gl_id);
	glGenBuffers(1, &indices_gl_id);
	glGenBuffers(1, &uv_gl_id);
	glGenBuffers(1, &normals_gl_id);
	// debug draw vertices normals (vertices already stored and ready to be used for this in normal way)
	glGenBuffers(1, &debug_v_normals_gl_id);
	// debug draw faces normals buffers
	glGenBuffers(1, &debug_f_vertices_gl_id);
	glGenBuffers(1, &debug_f_normals_gl_id);

	return ret;
}

bool ModelData::UpdateBuffers()
{
	bool ret = true;

	// vertex
	glBindBuffer(GL_ARRAY_BUFFER, vertices_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * _v_size, &vertices[0], GL_STATIC_DRAW);

	// normals
	glBindBuffer(GL_ARRAY_BUFFER, normals_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * _v_size, &normals[0], GL_STATIC_DRAW);

	// elements index
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_gl_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * _idx_size, &indices[0], GL_STATIC_DRAW);

	// WARNING: FOR NORMALS DEBUG ONLY, the normals for other calculations remains on normals float pointer ----
	glBindBuffer(GL_ARRAY_BUFFER, debug_v_normals_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * _v_size, &debug_v_normals[0], GL_STATIC_DRAW);

	// TODO: 2nd warning, be careful, since we only load and print triangulated faces this is good
	glBindBuffer(GL_ARRAY_BUFFER, debug_f_vertices_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _idx_size, &debug_f_vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, debug_f_normals_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _idx_size * 2, &debug_f_normals[0], GL_STATIC_DRAW);

	


	// ---------------------------------------------------------------------------------------------------------

	/*glBindBuffer(GL_ARRAY_BUFFER, normals_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * _v_size, &normals[0], GL_STATIC_DRAW);*/

	/*glBindBuffer(GL_ARRAY_BUFFER, uv_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _uv_size * 2, &uvs[0], GL_STATIC_DRAW);*/

	return ret;
}

bool ModelData::RefillDebugVertexNormalsBuffers()
{
	bool ret = true;

	//glDeleteBuffers(1, &debug_v_normals_gl_id);
	glBindBuffer(GL_ARRAY_BUFFER, debug_v_normals_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * _v_size, &debug_v_normals[0], GL_STATIC_DRAW);

	return ret;
}

bool ModelData::RefillDebugFacesNormalsBuffers()
{
	bool ret = true;

	/*glDeleteBuffers(1, &debug_f_vertices_gl_id);
	glDeleteBuffers(1, &debug_f_normals_gl_id);*/
	glBindBuffer(GL_ARRAY_BUFFER, debug_f_vertices_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _idx_size, &debug_f_vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, debug_f_normals_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _idx_size * 2, &debug_f_normals[0], GL_STATIC_DRAW);

	return ret;
}


bool ModelData::Render()
{
	bool ret = true;

	// general color
	
	// enable client side individual capabilities
	glEnableClientState(GL_VERTEX_ARRAY);
	//glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	//glClientActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texture_gl_id);
	//glEnableClientState(GL_COLOR_ARRAY); to colorize each vertex with an array of colors

	// vertices
	glBindBuffer(GL_ARRAY_BUFFER, vertices_gl_id);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);
	// uv coords
	//glBindBuffer(GL_ARRAY_BUFFER, uv_gl_id);
	//glTexCoordPointer(2, GL_FLOAT, 0, (void*)0);
	//// normals
	glBindBuffer(GL_ARRAY_BUFFER, normals_gl_id);
	glNormalPointer(GL_FLOAT, 0, (void*)0);
	// each vertex colors // "without shaders"
	//glBindBuffer(GL_ARRAY_BUFFER, colors_gl_id);
	//glColorPointer(3, GL_FLOAT, 0, (void*)0);
	// indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_gl_id);
	// draw
	glDrawElements(GL_TRIANGLES, _idx_size, GL_UNSIGNED_INT, (void*)0);

	glDisableClientState(GL_VERTEX_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	//glDisableClientState(GL_COLOR_ARRAY);

	return ret;

}

bool ModelData::DebugRenderVertex(float pointSize)
{
	bool ret = true;

	// draw points
	glPointSize(pointSize);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vertices_gl_id);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);

	glDrawArrays(GL_POINTS, 0, _v_size);

	glDisableClientState(GL_VERTEX_ARRAY);

	glPointSize(1.0f);

	return ret;
}

bool ModelData::DebugRenderVertexNormals(float lineWidth)
{
	bool ret = true;
	
	glLineWidth(lineWidth);

	// draw normals lines
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, debug_v_normals_gl_id);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);

	glDrawArrays(GL_LINES, 0, _v_size * 2);

	glDisableClientState(GL_VERTEX_ARRAY);

	glLineWidth(1.0f);

	return ret;
}

bool ModelData::DebugRenderFacesVertex(float pointSize)
{
	bool ret = true;

	glPointSize(pointSize);

	// draw points
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, debug_f_vertices_gl_id);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);

	glDrawArrays(GL_POINTS, 0, _idx_size);

	glDisableClientState(GL_VERTEX_ARRAY);

	glPointSize(1.0f);

	return ret;
}

bool ModelData::DebugRenderFacesNormals(float lineWidth)
{
	bool ret = true;

	glLineWidth(lineWidth);

	glEnableClientState(GL_VERTEX_ARRAY);
	
	// draw lines
	glBindBuffer(GL_ARRAY_BUFFER, debug_f_normals_gl_id);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);

	glDrawArrays(GL_LINES, 0, _idx_size * 2);

	glDisableClientState(GL_VERTEX_ARRAY);

	glLineWidth(1.0f); // returns to default line width

	return ret;
}

bool ModelData::CleanUp()
{
	bool ret = true;

	glDeleteBuffers(1, &vertices_gl_id);
	glDeleteBuffers(1, &indices_gl_id);
	glDeleteBuffers(1, &uv_gl_id);
	glDeleteBuffers(1, &normals_gl_id);
	glDeleteBuffers(1, &debug_v_normals_gl_id);
	glDeleteBuffers(1, &debug_f_vertices_gl_id);
	glDeleteBuffers(1, &debug_f_normals_gl_id);

	// delete all stored data

	delete[] vertices;
	delete[] indices;
	delete[] uvs;
	delete[] normals;
	delete[] debug_v_normals;
	delete[] debug_f_vertices;
	delete[] debug_f_normals;

	return ret;
}

void ModelData::ComputeVertexNormals(float length)
{
	// to draw lines, we need an array ready to what expects gldrawarrays
	// start point and finish point
	// TODO: improve this, thinking in deepth if this is possible with memcpy

	if (debug_v_normals != nullptr) delete[] debug_v_normals;

	debug_v_normals = new float[_v_size * 6]; // 3 for startpoint, 3 more for endpoint

	uint n_count = 0;
	for(uint i = 0; i < _v_size * 3; i += 3)
	{
		debug_v_normals[n_count]     = vertices[i]; // x
		debug_v_normals[n_count + 1] = vertices[i+1]; // y
		debug_v_normals[n_count + 2] = vertices[i+2]; //z

		debug_v_normals[n_count + 3] = vertices[i] + normals[i] * length; // x
		debug_v_normals[n_count + 4] = vertices[i+1] + normals[i+1] * length; // y
		debug_v_normals[n_count + 5] = vertices[i+2] + normals[i+2] * length; // z

		n_count += 6;
	}

}

bool ModelData::ComputeFacesNormals(float length)
{

	if (_idx_size % 3 != 0)
	{
		LOG("[Error] Could not compute face normals, faces with != 3 vertex");
		return false;
	}

	if (debug_f_vertices != nullptr) delete[] debug_f_vertices;
	if (debug_f_normals != nullptr) delete[] debug_f_normals;

	debug_f_vertices = new float[_idx_size];
	debug_f_normals = new float[_idx_size * 2];

	uint i = 0;
	uint n_count = 0;
	for (; i < _idx_size; i += 3)
	{
		// find the midface averaged vertex

		float* v1 = &vertices[indices[i] * 3];     // get first face vertex
		float* v2 = &vertices[indices[i + 1] * 3]; // get second face vertex
		float* v3 = &vertices[indices[i + 2] * 3]; // get third face vertex

		debug_f_vertices[i]     = (v1[0] + v2[0] + v3[0]) / 3.0f; // x coord
		debug_f_vertices[i + 1] = (v1[1] + v2[1] + v3[1]) / 3.0f; // y coord
		debug_f_vertices[i + 2] = (v1[2] + v2[2] + v3[2]) / 3.0f; // z coord

		// compute the averaged normal of the 3 vertex of each face

		float* n1 = &normals[indices[i] * 3];     // get first face vertex normal
		float* n2 = &normals[indices[i + 1] * 3]; // get second face vertex normal
		float* n3 = &normals[indices[i + 2] * 3]; // get third face vertex normal

		float avg_n[3];
		avg_n[0] = (n1[0] + n2[0] + n3[0]) / 3.0f; // x coord
		avg_n[1] = (n1[1] + n2[1] + n3[1]) / 3.0f; // y coord
		avg_n[2] = (n1[2] + n2[2] + n3[2]) / 3.0f; // z coord

		debug_f_normals[n_count]     = debug_f_vertices[i]; // x
		debug_f_normals[n_count + 1] = debug_f_vertices[i + 1]; // y
		debug_f_normals[n_count + 2] = debug_f_vertices[i + 2]; //z

		debug_f_normals[n_count + 3] = debug_f_vertices[i] + avg_n[0] * length; // x
		debug_f_normals[n_count + 4] = debug_f_vertices[i + 1] + avg_n[1] * length; // y
		debug_f_normals[n_count + 5] = debug_f_vertices[i + 2] + avg_n[2] * length; // z

		n_count += 6;
	}

	return true;
}

//const char* ModelData::GetName() const
//{
//	return name.data();
//}