#include "C_Mesh.h"
#include "Application.h"
#include "GL/glew.h"
#include "ImGui/imgui.h"

ComponentMesh::ComponentMesh(GameObject* parent) : Component(parent)
{
	name.assign("Mesh");
	mesh = new ModelData();
}

ComponentMesh::~ComponentMesh() {}

bool ComponentMesh::Update(float dt)
{
	return true;
}

bool ComponentMesh::PostUpdate(float dt)
{
	return true;
}

bool ComponentMesh::Draw()
{
	if (mesh != nullptr)
	{
		// TODO NEXT: implement new render functionality to pass all this shit (colors, draw modes etc)

		ViewPortOptions& vp = App->editor->viewport_options;

		if (vp.fill_faces && vp.wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0f, 0.375f);

			glColor4fv((float*)&vp.fill_color);

			mesh->Render();

			glLineWidth(1.0f);
			glDisable(GL_POLYGON_OFFSET_FILL);

		}
		else if (vp.fill_faces)
		{
			glColor4fv((float*)&vp.fill_color);
			mesh->Render();
		}

		if (vp.wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glLineWidth(vp.wire_line_width);
			glColor4fv((float*)&vp.wire_color);

			mesh->RenderWireframe();

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		if (vp.outline)
		{
			glStencilFunc(GL_NOTEQUAL, 1, -1);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glLineWidth(vp.wire_line_width);
			glColor4fv((float*)&vp.wire_color);

			glLineWidth(7.f);
			mesh->RenderWireframe();
			glLineWidth(1.f);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glStencilFunc(GL_ALWAYS, 1, -1);
		}

		if (vp.debug_vertex_normals)
		{
			glColor4fv((float*)&vp.d_vertex_p_color);
			mesh->DebugRenderVertex(vp.v_point_size);
			glColor4fv((float*)&vp.d_vertex_l_color);
			mesh->DebugRenderVertexNormals(vp.v_n_line_width);
		}
		//glColor3f(1.0f, 1.0f, 0.0f);
		if (vp.debug_face_normals)
		{
			glColor4fv((float*)&vp.d_vertex_face_color);
			mesh->DebugRenderFacesVertex(vp.f_v_point_size);
			glColor4fv((float*)&vp.d_vertex_face_n_color);
			mesh->DebugRenderFacesNormals(vp.f_n_line_width);
		}
	}

	return true;
}

bool ComponentMesh::DrawPanelInfo()
{
	ImGui::Text("Triangles: %i", mesh->_idx_size / 3);
	ImGui::Text("Indices: %i", mesh->_idx_size);
	ImGui::Text("Vertices: %i", mesh->_v_size);

	// TODO: individual data for each gameobject/mesh
	RenderConfig& config = App->renderer3D->render_config;

	// TODO: MOVE THIS TO MATERIAL PANEL --------------------
	/*if (ImGui::CollapsingHeader("Shading Modes"))
	{
		ImGui::Title("Fill Faces", 1);	ImGui::Checkbox("##fill_mode", &fill_faces);
		ImGui::Title("Color", 2);		ImGui::ColorEdit4("fILL Color##2f", (float*)&fill_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Title("Alpha", 2);		ImGui::SliderFloat("##alpha_slider", &fill_color.w, config.min_alpha, config.max_alpha, "%.1f", 1.0f);

		ImGui::Separator();

		ImGui::Title("Wireframe", 1);	ImGui::Checkbox("##wireframe", &wireframe);
		ImGui::Title("Color", 2);		ImGui::ColorEdit4("Line Color##2f", (float*)&wire_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Title("Width", 2);		ImGui::SliderFloat("##wire_line_width", &wire_line_width, config.min_line_w, config.max_line_w, "%.1f", 1.0f);
	}*/
	// --------------------------------------------------------
	
	return true;
}

bool ComponentMesh::CleanUp()
{
	delete mesh;

	return true;
}