#include "Application.h"
#include "GL/glew.h"
#include "ImGui/imgui.h"

#include "C_Transform.h"
#include "C_MeshRenderer.h"
#include "C_Mesh.h"
#include "C_Material.h"

#include "D_Mesh.h"
#include "D_Material.h"

C_MeshRenderer::C_MeshRenderer(GameObject* parent): Component(parent, ComponentType::MESH_RENDERER)
{
	name.assign("Mesh Renderer");
	flags |= ImGuiTreeNodeFlags_AllowItemOverlap;
}

C_MeshRenderer::~C_MeshRenderer()
{

}

bool C_MeshRenderer::Render()
{
	// Get Data from components ----------------------------

	C_Mesh* c_mesh = (C_Mesh*)linked_go->GetComponentsByType(ComponentType::MESH);           	// TODO: Link it once time ------------------
	C_Material* c_mat = (C_Material*)linked_go->GetComponentsByType(ComponentType::MATERIAL);   // TODO: Link it once time ------------------

	if (c_mesh == nullptr)
	{
		LOG("[Error] no component mesh attached to this mesh renderer");
		return false;
	}
	if (c_mat == nullptr)
	{
		LOG("[Error] no component material attached to this mesh renderer");
		return false;
	}

	d_mesh = c_mesh->data;
	d_material = c_mat->data;

	uint custom_tex_id = 0;

	if (c_mat->view_checker)
	{
		// check if checker still is valid (not deleted gl buffer on the way)
		if (glIsTexture(c_mat->checker_gl_id))
		{
			custom_tex_id = c_mat->checker_gl_id;
		}
		else
		{
			if (c_mat->checker_gl_id != 0)
				c_mat->checker_gl_id = 0;
		}
	}

	// CHECK if textured for apply or not default albedo color for mesh data
	float4 albedo_color = { 1.0f,1.0f,1.0f,1.0f }; // TODO: change variable type if needed

	if (!c_mat->textured)
	{
		albedo_color = d_material->diffuse_color;
	}

	// Push matrix ---------------------------------------------------------
	glPushMatrix();
	glMultMatrixf((float*)&linked_go->transform->global_transform.Transposed());

	// Flip faces enable ----------------------------------------------------
	if (linked_go->transform->HasNegativeScale())
	{
		glFrontFace(GL_CW);
	}

	// Start rendering ------------------------------------------------------

	ViewportOptions& vp = App->editor->viewport_options;

	if (vp.debug_bounding_box)
	{
		glBegin(GL_LINES);
		float4 f;

		GLfloat emission_new[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat emission_default[] = { 0, 0, 0, 1 };

		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission_new);

		for (int i = 0; i < d_mesh->aabb.NumEdges(); ++i)
		{
			glColor4f(1.f, 1.f, 1.f, 1.f);
			math::LineSegment line_segment = d_mesh->aabb.Edge(i);
			glVertex3f(line_segment.a.x, line_segment.a.y, line_segment.a.z);
			glVertex3f(line_segment.b.x, line_segment.b.y, line_segment.b.z);
		}

		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission_default);
		glEnd();
	}

	if (vp.mode == V_MODE_SHADED)
	{
		glColor4fv(albedo_color.ptr());
		RenderMesh(custom_tex_id, c_mat->textured);
	}

	else if (vp.mode == V_MODE_WIREFRIME)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		glLineWidth(vp.wire_line_width);
		glColor4fv((float*)&vp.wire_color);

		RenderWireframe();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	else if (1)
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

	// Flip faces enable ----------------------------------------------------
	glFrontFace(GL_CCW);

	// Pop matrix -----------------------------------------------------------
	glPopMatrix();

	return true;
}


// TODO: implements this check on a more universal place for all render functions
// if we delete this linked buffer on another part and enters here without check
// "There is no guarantee that the names form a contiguous set of integers; however, 
// it is guaranteed that none of the returned names was in use immediately before the call to glGenTextures. "
// checks if the texture id is a valid texture id and prevents creation of no dimensionality with new binding before render

bool C_MeshRenderer::RenderMesh(uint custom_tex_id, bool textured)
{
	bool ret = true;
	uint texture_id = 0;

	if (d_material->textures[D_Material::DIFFUSE] != nullptr) // TODO: clean this and perform for possible another textures
	{
		if (d_material->textures[D_Material::DIFFUSE]->buffer_id != 0)
		{
			if (glIsTexture(d_material->textures[D_Material::DIFFUSE]->buffer_id))
			{
				texture_id = d_material->textures[D_Material::DIFFUSE]->buffer_id;
			}
			else
			{
				d_material->textures[D_Material::DIFFUSE]->buffer_id = 0;
			}
		}
	}

	// Enable client ==============================================
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	if(d_mesh->buffers_size[D_Mesh::UVS] != 0) glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	if(texture_id != 0) glClientActiveTexture(GL_TEXTURE0);

	// Bind buffers ===============================================

	// Vertices --------------------------
	glBindBuffer(GL_ARRAY_BUFFER, d_mesh->buffers_id[D_Mesh::VERTICES]);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);

	// UV's & Texture --------------------
	if (d_mesh->buffers_size[D_Mesh::UVS] != 0 && d_material != nullptr)
	{
		if (textured)
		{
			if (custom_tex_id != 0)
				glBindTexture(GL_TEXTURE_2D, custom_tex_id);
			else
			{
				if (d_material->textures[D_Material::DIFFUSE] != nullptr)
					glBindTexture(GL_TEXTURE_2D, d_material->textures[D_Material::DIFFUSE]->buffer_id);
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, d_mesh->buffers_id[D_Mesh::UVS]);
		glTexCoordPointer(d_mesh->uv_num_components, GL_FLOAT, 0, (void*)0);
	}
	// Nomrals -----------------------------

	glBindBuffer(GL_ARRAY_BUFFER, d_mesh->buffers_id[D_Mesh::NORMALS]);
	glNormalPointer(GL_FLOAT, 0, (void*)0);

	// Draw & Indices ----------------------

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d_mesh->buffers_id[D_Mesh::INDICES]);
	glDrawElements(GL_TRIANGLES, d_mesh->buffers_size[D_Mesh::INDICES], GL_UNSIGNED_INT, (void*)0);

	// Unbind buffers ===========================================

	// unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisableClientState(GL_TEXTURE0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

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