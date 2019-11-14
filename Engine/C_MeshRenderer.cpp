#include "Application.h"
#include "GL/glew.h"
#include "ImGui/imgui.h"

#include "C_Transform.h"
#include "C_MeshRenderer.h"
#include "C_Mesh.h"
#include "C_Material.h"

#include "D_Mesh.h"
#include "D_Material.h"

#define EMISSION_DEFAULT {0.F, 0.F, 0.F, 1.F}
#define EMISSION_LINES   {1.F, 1.F, 1.F, 1.F}

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

	C_Mesh* c_mesh = linked_go->GetComponent<C_Mesh>();           	// TODO: Link it once time ------------------
	C_Material* c_mat = linked_go->GetComponent<C_Material>();   // TODO: Link it once time ------------------

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

	if (vp.mode == V_MODE_SHADED)
	{
		RenderMesh(albedo_color.ptr(), custom_tex_id, c_mat->textured);
	}

	else if (vp.mode == V_MODE_WIREFRAME)
	{
		RenderWireframe(vp.wire_line_width, (float*)&vp.wire_color);
	}

	else if (vp.mode == V_MODE_SHADED_WIREFRAME)
	{
		RenderMesh(albedo_color.ptr(), custom_tex_id, c_mat->textured);
		RenderWireframe(vp.wire_line_width, (float*)&vp.wire_color);
	}

	// Editor & Debug Rendering --------------------------------------------

	if (false)
	{
		RenderOutline(7.f, (float*)&vp.wire_color);
	}

	if (vp.debug_bounding_box)
	{
		RenderBoundingBox(d_mesh->aabb, vp.bb_line_width, (float*)&vp.bb_color);
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

void C_MeshRenderer::RenderMesh(float* color, uint custom_tex_id, bool textured)
{
	uint texture_id = 0;

	// Settings ====================================================
	glColor4fv(color);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
	if(d_mesh->buffers_size[D_Mesh::UVS] != 0) 
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	if(texture_id != 0) 
		glClientActiveTexture(GL_TEXTURE0);

	// Bind buffers ===============================================

	// Vertices ------------------------------
	glBindBuffer(GL_ARRAY_BUFFER, d_mesh->buffers_id[D_Mesh::VERTICES]);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);

	// UV's & Texture ------------------------
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

	// Indices ------------------------------
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d_mesh->buffers_id[D_Mesh::INDICES]);

	// Draw =====================================================
	glDrawElements(GL_TRIANGLES, d_mesh->buffers_size[D_Mesh::INDICES], GL_UNSIGNED_INT, (void*)0);

	// Unbind buffers ===========================================
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);


	glDisableClientState(GL_TEXTURE0);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	// Settings Default =========================================
	glColor4fv(float4::one.ptr());
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}


void C_MeshRenderer::RenderWireframe(float width, float* color) // need very few operations
{
	// Custom Settings ============================================
	ModuleRenderer3D::BeginDebugDraw(color);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(width);
	glColor4fv(color);

	// Enable client ==============================================
	glEnableClientState(GL_VERTEX_ARRAY);

	// Bind buffers ===============================================

	// Vertices ---------------
	glBindBuffer(GL_ARRAY_BUFFER, d_mesh->buffers_id[D_Mesh::VERTICES]);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);
	// Indices ----------------
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, d_mesh->buffers_id[D_Mesh::INDICES]);

	// Draw =======================================================
	glDrawElements(GL_TRIANGLES, d_mesh->buffers_size[D_Mesh::INDICES] , GL_UNSIGNED_INT, (void*)0);

	// Bind buffers ===============================================
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Disable client =============================================
	glDisableClientState(GL_VERTEX_ARRAY);

	// Custom Settings Default ====================================
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLineWidth(1.f);
	glColor4fv( float4::one.ptr() );
	ModuleRenderer3D::EndDebugDraw();
}

void C_MeshRenderer::RenderOutline(float width, float* color)
{
	glStencilFunc(GL_NOTEQUAL, 1, -1);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	RenderWireframe(width, color);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glStencilFunc(GL_ALWAYS, 1, -1);
}

void C_MeshRenderer::RenderBoundingBox(math::AABB& aabb, float width, float* color)
{
	ModuleRenderer3D::BeginDebugDraw(color);
	glLineWidth(width);
	glBegin(GL_LINES);


	for (int i = 0; i < aabb.NumEdges(); ++i)
	{
		math::LineSegment line_segment = aabb.Edge(i);
		glVertex3f(line_segment.a.x, line_segment.a.y, line_segment.a.z);
		glVertex3f(line_segment.b.x, line_segment.b.y, line_segment.b.z);
	}

	glEnd();
	glLineWidth(1.f);
	ModuleRenderer3D::EndDebugDraw();
}

