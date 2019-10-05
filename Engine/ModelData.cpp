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

	return ret;
}

bool ModelData::UpdateBuffers()
{
	bool ret = true;

	glBindBuffer(GL_ARRAY_BUFFER, vertices_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * _v_size, &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_gl_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * _idx_size, &indices[0], GL_STATIC_DRAW);

	/*glBindBuffer(GL_ARRAY_BUFFER, uv_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _uv_size * 2, &uvs[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, normals_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _uv_size * 3, &normals[0], GL_STATIC_DRAW);*/

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
	glDrawElements(GL_TRIANGLES, _idx_size * 3, GL_UNSIGNED_INT, (void*)0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	//glDisableClientState(GL_COLOR_ARRAY);

	return ret;

}
bool ModelData::CleanUp()
{
	bool ret = true;

	glDeleteBuffers(1, &vertices_gl_id);
	glDeleteBuffers(1, &indices_gl_id);
	glDeleteBuffers(1, &uv_gl_id);
	glDeleteBuffers(1, &normals_gl_id);

	return ret;
}