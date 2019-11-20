#include "Application.h"
#include "ModuleRenderer3D.h"
#include "GL/glew.h"
#include "ImGui/imgui.h"

#include "C_Transform.h"
#include "C_MeshRenderer.h"
#include "C_Mesh.h"
#include "C_Material.h"

#include "R_Mesh.h"
#include "R_Material.h"

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

bool C_MeshRenderer::Update(float dt)
{
	//ViewportOptions& vp = App->editor->viewport_options;

	//if (vp.debug_bounding_box)
	//{
	//	DebugRender render_bb;
	//	render_bb.Set(&linked_go->bounding_box, vp.bb_color , 1.2f);
	//	App->scene->PushDebugRender(render_bb);
	//	DebugRender render_obb;
	//	render_obb.Set(&linked_go->obb, float4(1.f, .9f, 0.1f, 1.f), 1.2f);
	//	App->scene->PushDebugRender(render_obb);
	//}

	return true;
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

	r_mesh = (R_Mesh*)App->resources->Get(c_mesh->resource);

	if (r_mesh == nullptr)
	{
		LOG("[Error] Bad mesh resource reference on component mesh renderer");
		return false;
	}

	r_mat = (R_Material*)App->resources->Get(c_mat->resource);

	if (r_mat == nullptr)
	{
		LOG("[Error] Bad material resource reference on component mesh renderer");
		return false;
	}

	//r_mesh = c_mesh->data;
	//r_material = c_mat->data;

	uint custom_tex_id = 0;

	//if (c_mat->view_checker)
	//{
	//	// check if checker still is valid (not deleted gl buffer on the way)
	//	if (glIsTexture(c_mat->checker_gl_id))
	//	{
	//		custom_tex_id = c_mat->checker_gl_id;
	//	}
	//	else
	//	{
	//		if (c_mat->checker_gl_id != 0)
	//			c_mat->checker_gl_id = 0;
	//	}
	//}

	// CHECK if textured for apply or not default albedo color for mesh data
	float4 albedo_color = { 1.0f,1.0f,1.0f,1.0f }; // TODO: change variable type if needed

	/*if (!c_mat->textured)
	{
		albedo_color = r_material->diffuse_color;
	}*/

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

	if (vp.mode == V_MODE_SHADED || App->scene->editor_mode == false)
	{
		RenderMesh(albedo_color.ptr(), custom_tex_id, true);// c_mat->textured);
	}
	else if (vp.mode == V_MODE_WIREFRAME)
	{
		RenderWireframe(vp.wire_line_width,vp.wire_color);
	}
	else if (vp.mode == V_MODE_SHADED_WIREFRAME)
	{
		RenderMesh(albedo_color.ptr(), custom_tex_id, true);// c_mat->textured);
		RenderWireframe(vp.wire_line_width, vp.wire_color);
	}

	// Editor & Debug Rendering --------------------------------------------

	if (App->scene->selected_go == linked_go && App->scene->editor_mode)
	{
		RenderOutline(5.f, float4(1.f ,0.6f , 0.f, 1.f));
	}

	glPopMatrix();

	glPushMatrix();
	glMultMatrixf(float4x4::identity.ptr());

	if (vp.debug_bounding_box && App->scene->editor_mode)
	{
		App->renderer3D->RenderAABB(linked_go->bounding_box, 1.2f , vp.bb_color);
		App->renderer3D->RenderOBB(linked_go->obb, 1.2f, float4(1.f, .9f, 0.1f, 1.f));
	}

	// Pop matrix -----------------------------------------------------------
	glPopMatrix();

	// Flip faces enable ----------------------------------------------------
	glFrontFace(GL_CCW);

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

	glDepthRange(0.1, 1.0);

	// Settings ====================================================
	glColor4fv(color);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	//if (r_mat->textures[R_Material::DIFFUSE] != 0) // TODO: clean this and perform for possible another textures
	//{
	//	if (r_mat->textures[R_Material::DIFFUSE]->buffer_id != 0)
	//	{
	//		if (glIsTexture(r_mat->textures[R_Material::DIFFUSE]->buffer_id))
	//		{
	//			texture_id = r_material->textures[R_Material::DIFFUSE]->buffer_id;
	//		}
	//		else
	//		{
	//			r_material->textures[R_Material::DIFFUSE]->buffer_id = 0;
	//		}
	//	}
	//}

	glEnable(GL_STENCIL_TEST);

	glStencilFunc(GL_ALWAYS, 1, -1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	// Enable client ==============================================
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	if(r_mesh->buffers_size[R_Mesh::UVS] != 0) 
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	if(texture_id != 0) 
		glClientActiveTexture(GL_TEXTURE0);

	// Bind buffers ===============================================

	// Vertices ------------------------------
	glBindBuffer(GL_ARRAY_BUFFER, r_mesh->buffers_id[R_Mesh::VERTICES]);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);

	// UV's & Texture ------------------------
	if (r_mesh->buffers_size[R_Mesh::UVS] != 0 && r_mat != nullptr)
	{
		if (textured)
		{
			if (custom_tex_id != 0)
				glBindTexture(GL_TEXTURE_2D, custom_tex_id);
			else
			{
				R_Texture* r_tex = (R_Texture*)App->resources->Get(r_mat->textures[R_Material::DIFFUSE]);
				if (r_tex != nullptr)
					glBindTexture(GL_TEXTURE_2D, r_tex->buffer_id);
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, r_mesh->buffers_id[R_Mesh::UVS]);
		glTexCoordPointer(r_mesh->uv_num_components, GL_FLOAT, 0, (void*)0);
	}

	// Nomrals -----------------------------
	glBindBuffer(GL_ARRAY_BUFFER, r_mesh->buffers_id[R_Mesh::NORMALS]);
	glNormalPointer(GL_FLOAT, 0, (void*)0);

	// Indices ------------------------------
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r_mesh->buffers_id[R_Mesh::INDICES]);

	// Draw =====================================================
	glDrawElements(GL_TRIANGLES, r_mesh->buffers_size[R_Mesh::INDICES], GL_UNSIGNED_INT, (void*)0);

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


void C_MeshRenderer::RenderWireframe(float width, float4& color) // need very few operations
{
	// Custom Settings ============================================
	ModuleRenderer3D::BeginDebugDraw(color);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(width);

	// Enable client ==============================================
	glEnableClientState(GL_VERTEX_ARRAY);

	// Bind buffers ===============================================

	// Vertices ---------------
	glBindBuffer(GL_ARRAY_BUFFER, r_mesh->buffers_id[R_Mesh::VERTICES]);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);
	// Indices ----------------
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r_mesh->buffers_id[R_Mesh::INDICES]);

	// Draw =======================================================
	glDrawElements(GL_TRIANGLES, r_mesh->buffers_size[R_Mesh::INDICES] , GL_UNSIGNED_INT, (void*)0);

	// Bind buffers ===============================================
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// Disable client =============================================
	glDisableClientState(GL_VERTEX_ARRAY);

	// Custom Settings Default ====================================
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glLineWidth(1.f);

	ModuleRenderer3D::EndDebugDraw();
}

void C_MeshRenderer::RenderOutline(float width, float4& color)
{
	glStencilFunc(GL_NOTEQUAL, 1, -1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	RenderWireframe(width, color);

	glStencilFunc(GL_ALWAYS, 1, -1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
}



