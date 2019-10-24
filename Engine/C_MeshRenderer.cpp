#include "Application.h"
#include "GL/glew.h"
#include "ImGui/imgui.h"

#include "C_MeshRenderer.h"
#include "C_Mesh.h"
#include "C_Material.h"

#include "D_Mesh.h"
#include "D_Material.h"

C_MeshRenderer::C_MeshRenderer(GameObject* parent): Component(parent, ComponentType::MESH_RENDERER)
{
	name.assign("Mesh Renderer");
}

C_MeshRenderer::~C_MeshRenderer()
{

}

bool C_MeshRenderer::PostUpdate(float dt)
{
	// Get Data from components ----------------------------

	// TODO: i think assign data every frame is not needed, instead, link with the parent component once ---------
	// ------
	C_Mesh* c_mesh = (C_Mesh*)linked_go->GetComponentsByType(ComponentType::MESH);
	C_Material* c_mat = (C_Material*)linked_go->GetComponentsByType(ComponentType::MATERIAL);

	if (c_mesh != nullptr)
	{
		d_mesh = c_mesh->data;
	}
	if (c_mat != nullptr)
	{
		d_mat = c_mat->data;
	}
	// -------
	// -------------------------------------------------------------------------------------------------------------

	uint custom_tex_id = 0;

	if (c_mesh != nullptr)
	{
		if (c_mat != nullptr)
		{
			if (c_mat->view_checker)
			{
				// check if checker still is valid (not deleted gl buffer on the way)
				if(glIsTexture(c_mat->checker_gl_id))
				{ 
					custom_tex_id = c_mat->checker_gl_id;
				}
				else
				{
					if (c_mat->checker_gl_id != 0)
						c_mat->checker_gl_id = 0;
				}
				
			}
				
		}

		// TODO NEXT: implement new render functionality to pass all this shit (colors, draw modes etc)

		ViewportOptions& vp = App->editor->viewport_options;

		if (vp.fill_faces && vp.wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0f, 0.375f);

			glColor4fv((float*)&vp.fill_color);

			Render(custom_tex_id);

			glLineWidth(1.0f);
			glDisable(GL_POLYGON_OFFSET_FILL);

		}
		else if (vp.fill_faces)
		{
			glColor4fv((float*)&vp.fill_color);
			Render(custom_tex_id);
		}

		if (vp.wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glLineWidth(vp.wire_line_width);
			glColor4fv((float*)&vp.wire_color);

			RenderWireframe();

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		if (vp.outline)
		{
			glStencilFunc(GL_NOTEQUAL, 1, -1);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glLineWidth(vp.wire_line_width);
			glColor4fv((float*)&vp.wire_color);

			glLineWidth(7.f);
			RenderWireframe();
			glLineWidth(1.f);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glStencilFunc(GL_ALWAYS, 1, -1);
		}
	}

	return true;
}

bool C_MeshRenderer::Render(uint custom_tex_id)
{
	bool ret = true;

	// TODO: implements this check on a more universal place for all render functions
	// if we delete this linked buffer on another part and enters here without check
	// "There is no guarantee that the names form a contiguous set of integers; however, 
	// it is guaranteed that none of the returned names was in use immediately before the call to glGenTextures. "
	// checks if the texture id is a valid texture id and prevents creation of no dimensionality with new binding before render

	if (d_mat != nullptr && d_mat->textures[D_Material::DIFFUSE]->buffer_id != 0)
	{
		if (!glIsTexture(d_mat->textures[D_Material::DIFFUSE]->buffer_id))
			d_mat->textures[D_Material::DIFFUSE]->buffer_id = 0;
	}

	// Enable client side individual capabilities
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glClientActiveTexture(GL_TEXTURE0);
	//glEnableClientState(GL_COLOR_ARRAY); to colorize each vertex with an array of colors

	// Vertices --------------------------------------
	glBindBuffer(GL_ARRAY_BUFFER, d_mesh->buffers_id[D_Mesh::VERTICES]);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);

	// UV's & Texture --------------------------------
	if (d_mesh->buffers_size[D_Mesh::UVS] != 0 && d_mat != nullptr)
	{
		if (custom_tex_id != 0)
			glBindTexture(GL_TEXTURE_2D, custom_tex_id);
		else
			glBindTexture(GL_TEXTURE_2D, d_mat->textures[D_Material::DIFFUSE]->buffer_id);

		glBindBuffer(GL_ARRAY_BUFFER, d_mesh->buffers_id[D_Mesh::UVS]);
		glTexCoordPointer(2, GL_FLOAT, 0, (void*)0);
	}
	// Nomrals ---------------------------------------

	glBindBuffer(GL_ARRAY_BUFFER, d_mesh->buffers_id[D_Mesh::NORMALS]);
	glNormalPointer(GL_FLOAT, 0, (void*)0);

	// each vertex colors
	//glBindBuffer(GL_ARRAY_BUFFER, colors_gl_id);
	//glColorPointer(3, GL_FLOAT, 0, (void*)0);
	// indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d_mesh->buffers_id[D_Mesh::INDICES]);
	// draw
	glDrawElements(GL_TRIANGLES, d_mesh->buffers_size[D_Mesh::INDICES], GL_UNSIGNED_INT, (void*)0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

	// unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_TEXTURE0);
	//glDisableClientState(GL_COLOR_ARRAY);

	return ret;
}


bool C_MeshRenderer::RenderWireframe() // need very few operations
{
	bool ret = true;

	glEnableClientState(GL_VERTEX_ARRAY);
	//bind and define data type vertices
	glBindBuffer(GL_ARRAY_BUFFER, d_mesh->buffers_id[D_Mesh::VERTICES]);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);
	//bind indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d_mesh->buffers_id[D_Mesh::INDICES]);
	//draw
	glDrawElements(GL_TRIANGLES, d_mesh->buffers_size[D_Mesh::INDICES] , GL_UNSIGNED_INT, (void*)0);

	glDisableClientState(GL_VERTEX_ARRAY);

	return ret;
}


bool C_MeshRenderer::RenderOutline()
{


	return true;
}