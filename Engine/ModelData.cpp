#include "ModelData.h"
#include "GL/glew.h"

ModelData::ModelData() {}

ModelData::~ModelData() {}

bool ModelData::GenerateBuffers()
{
	bool ret = true;
	// TODO: maybe is a good idea to use pointers to generate all buffers at once
	glGenBuffers(1, &vertices_gl_id);
	glGenBuffers(1, &indices_gl_id);
	glGenBuffers(1, &uv_gl_id);
	glGenBuffers(1, &normals_gl_id);
	glGenBuffers(1, &debug_v_normals_gl_id);
	glGenBuffers(1, &debug_f_normals_gl_id);

	return ret;
}

bool ModelData::UpdateBuffers()
{
	bool ret = true;

	glBindBuffer(GL_ARRAY_BUFFER, vertices_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * _v_size, &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_gl_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * _idx_size, &indices[0], GL_STATIC_DRAW);

	// WARNING: FOR NORMALS DEBUG ONLY, the normals for other calculations remains on normals float pointer ----
	glBindBuffer(GL_ARRAY_BUFFER, debug_v_normals_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * _v_size, &debug_v_normals[0], GL_STATIC_DRAW);

	// TODO: 2nd warning, be careful, since we only load and print triangulated faces this is good
	glBindBuffer(GL_ARRAY_BUFFER, debug_f_normals_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _idx_size, &debug_f_normals[0], GL_STATIC_DRAW);

	// ---------------------------------------------------------------------------------------------------------

	/*glBindBuffer(GL_ARRAY_BUFFER, normals_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * _v_size, &normals[0], GL_STATIC_DRAW);*/

	/*glBindBuffer(GL_ARRAY_BUFFER, uv_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _uv_size * 2, &uvs[0], GL_STATIC_DRAW);*/

	return ret;
}



bool ModelData::Render()
{
	bool ret = true;

	// general color
	//glColor3f();
	// enable client side individual capabilities
	glEnableClientState(GL_VERTEX_ARRAY);
	/*glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);*/

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
	//glBindBuffer(GL_ARRAY_BUFFER, normals_gl_id);
	//glNormalPointer(GL_FLOAT, 0, (void*)0);
	// each vertex colors // "without shaders"
	//glBindBuffer(GL_ARRAY_BUFFER, colors_gl_id);
	//glColorPointer(3, GL_FLOAT, 0, (void*)0);
	// indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_gl_id);
	// draw
	glDrawElements(GL_TRIANGLES, _idx_size, GL_UNSIGNED_INT, (void*)0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	//glDisableClientState(GL_COLOR_ARRAY);

	return ret;

}

bool ModelData::DebugRenderVertexNormals()
{
	bool ret = false;
	
	float pointSize = 5.0f;
	float lineWidth = 1;

	// draw points
	glColor3f(1.0f, 1.0f, 0.0f);
	
	glPointSize(pointSize);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, vertices_gl_id);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);

	glDrawArrays(GL_POINTS, 0, _v_size);

	//glEnableClientState(GL_VERTEX_ARRAY);

	glLineWidth(lineWidth);
	// draw normals lines
	glColor3f(0.2f, 1.0f, 0.0f);
	glBindBuffer(GL_ARRAY_BUFFER, debug_v_normals_gl_id);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);

	glDrawArrays(GL_LINES, 0, _v_size * 2);

	glDisableClientState(GL_VERTEX_ARRAY);

	glLineWidth(1.0f);

	return ret;
}

bool ModelData::DebugRenderFacesNormals()
{
	bool ret = true;

	float pointSize = 5.0f;
	float lineWidth = 1;

	// draw points
	glColor3f(1.0f, 1.0f, 0.0f);

	glPointSize(pointSize);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, debug_f_normals_gl_id);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);
	
	glDrawArrays(GL_POINTS, 0, _f_size);

	glDisableClientState(GL_VERTEX_ARRAY);



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
	glDeleteBuffers(1, &debug_f_normals_gl_id);

	return ret;
}

void ModelData::ComputeVertexNormals()
{
	// to draw lines, we need an array ready to what expects gldrawarrays
	// start point and finish point
	// TODO: improve this, thinking in deepth if this is possible with memcpy

	float length = 0.3f;

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

bool ModelData::ComputeFacesNormals()
{
	if (_idx_size % 3 != 0)
	{
		LOG("[Error] Could not compute face normals, faces with != 3 vertex");
		return false;
	}

	if (debug_f_normals != nullptr) delete debug_f_normals;

	debug_f_normals = new float[_idx_size]; // for the moment only vertex for test

	//memset(debug_f_normals, 0, _idx_size);

	//uint v_count = 0;
	uint i = 0;
	for (; i < _idx_size; i += 3)
	{
		// find the midface averaged vertex

		float* v1 = &vertices[indices[i]];     // get first face vertex
		float* v2 = &vertices[indices[i + 1]]; // get second face vertex
		float* v3 = &vertices[indices[i + 2]]; // get third face vertex

		LOG("First vertex: %f %f %f", v1[0], v1[1], v1[2]);
		LOG("2nd vertex: %f %f %f", v2[0], v2[1], v2[2]);
		LOG("Third vertex: %f %f %f", v3[0], v3[1], v3[2]);

		debug_f_normals[i]     = (v1[0] + v2[0] + v3[0]) / 3; // x coord
		debug_f_normals[i + 1] = (v1[1] + v2[1] + v3[1]) / 3; // y coord
		debug_f_normals[i + 2] = (v1[2] + v2[2] + v3[2]) / 3; // z coord
	}

	return true;
}