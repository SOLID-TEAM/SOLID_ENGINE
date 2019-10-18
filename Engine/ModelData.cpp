#include "ModelData.h"
#include "GL/glew.h"
// TODO: for testing lenna wip
#include "Application.h"
#include "ModuleTextures.h"


ModelData::ModelData() {}

ModelData::ModelData(float* vertices, uint* indices, float* normals, float* uvs, int nVertices, int nIndices)
{
	this->vertices = new float[nVertices * 3];
	this->indices = new uint[nIndices * 3];
	this->normals = new float[nVertices * 3];
	

	memcpy(this->vertices, vertices, sizeof(float) * nVertices * 3);
	memcpy(this->indices, indices, sizeof(uint) * nIndices * 3);
	memcpy(this->normals, normals, sizeof(float) * nVertices * 3);

	if(uvs != nullptr)
	{
		this->uvs = new float[nVertices * 2];
		memcpy(this->uvs, uvs, sizeof(float) * nVertices * 2);
	}
		

	_v_size = nVertices;
	_idx_size = nIndices * 3;

	Start();
}

bool ModelData::Start()
{
	GenerateBuffers();
	UpdateBuffers();

	return true;
}

ModelData::~ModelData() 
{
	CleanUp();
}

bool ModelData::GenerateBuffers()
{
	bool ret = true;
	// TODO: maybe is a good idea to use pointers to generate all buffers at once
	glGenBuffers(1, &vertices_gl_id);
	glGenBuffers(1, &indices_gl_id);
	glGenBuffers(1, &uv_gl_id);
	glGenBuffers(1, &normals_gl_id);
	if(uvs != nullptr)
		glGenBuffers(1, &uv_gl_id);
	
	// TODO: move to moduletextures
	// texture buffer
	/*glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &texture_gl_id);*/

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

	// uv's
	if (uvs != nullptr)
	{
		glBindBuffer(GL_ARRAY_BUFFER, uv_gl_id);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * _uv_num_components * _v_size, &uvs[0], GL_STATIC_DRAW);
	}

	// elements index
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_gl_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * _idx_size, &indices[0], GL_STATIC_DRAW);

	//texture_gl_id = App->textures->GenerateCheckerTexture(512,512);

	return ret;
}


bool ModelData::Render()
{
	bool ret = true;

	// TODO: implements this check on a more universal place for all render functions
	// if we delete this linked buffer on another part and enters here without check
	// "There is no guarantee that the names form a contiguous set of integers; however, 
	// it is guaranteed that none of the returned names was in use immediately before the call to glGenTextures. "
	// checks if the texture id is a valid texture id and prevents creation of no dimensionality with new binding before render
	if (texture_gl_id != 0)
	{
		if (!glIsTexture(texture_gl_id))
			texture_gl_id = 0;
	}

	// general color
	
	// enable client side individual capabilities
	glEnableClientState(GL_VERTEX_ARRAY);
	if (_uv_num_components > 0)
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	//glClientActiveTexture(GL_TEXTURE0);
	//glEnableClientState(GL_COLOR_ARRAY); to colorize each vertex with an array of colors

	// vertices
	glBindBuffer(GL_ARRAY_BUFFER, vertices_gl_id);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);
	// uvs / texture
	if (uvs != nullptr)
	{
		glBindTexture(GL_TEXTURE_2D, texture_gl_id);

		// uv coords
		glBindBuffer(GL_ARRAY_BUFFER, uv_gl_id);
		glTexCoordPointer(_uv_num_components, GL_FLOAT, 0, (void*)0);
	}
	// normals
	glBindBuffer(GL_ARRAY_BUFFER, normals_gl_id);
	glNormalPointer(GL_FLOAT, 0, (void*)0);
	// each vertex colors
	//glBindBuffer(GL_ARRAY_BUFFER, colors_gl_id);
	//glColorPointer(3, GL_FLOAT, 0, (void*)0);
	// indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_gl_id);
	// draw
	glDrawElements(GL_TRIANGLES, _idx_size, GL_UNSIGNED_INT, (void*)0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	
	// unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
	//glDisableClientState(GL_TEXTURE0);
	//glDisableClientState(GL_COLOR_ARRAY);

	return ret;
}


bool ModelData::RenderWireframe() // need very few operations
{
	bool ret = true;
	
	glEnableClientState(GL_VERTEX_ARRAY);
	// bind and define data type vertices
	glBindBuffer(GL_ARRAY_BUFFER, vertices_gl_id);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);
	// bind indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_gl_id);
	// draw
	glDrawElements(GL_TRIANGLES, _idx_size, GL_UNSIGNED_INT, (void*)0);

	glDisableClientState(GL_VERTEX_ARRAY);

	return ret;
}

bool ModelData::RenderOutline()
{


	return true;
}


bool ModelData::CleanUp()
{
	bool ret = true;

	glDeleteBuffers(1, &vertices_gl_id);
	glDeleteBuffers(1, &indices_gl_id);
	glDeleteBuffers(1, &uv_gl_id);
	glDeleteBuffers(1, &normals_gl_id);

	// delete all stored data

	delete[] vertices;
	delete[] indices;
	delete[] uvs;
	delete[] normals;

	return ret;
}


//const char* ModelData::GetName() const
//{
//	return name.data();
//}