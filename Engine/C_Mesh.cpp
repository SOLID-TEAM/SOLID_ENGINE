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

		if (fill_faces && wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0f, 0.375f);

			glColor4fv((float*)&fill_color);

			mesh->Render();

			glLineWidth(1.0f);
			glDisable(GL_POLYGON_OFFSET_FILL);

		}
		else if (fill_faces)
		{
			glColor4fv((float*)&fill_color);
			mesh->Render();
		}

		if (wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glLineWidth(wire_line_width);
			glColor4fv((float*)&wire_color);

			mesh->RenderWireframe();

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}

		if (outline)
		{
			glStencilFunc(GL_NOTEQUAL, 1, -1);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			glLineWidth(wire_line_width);
			glColor4fv((float*)&wire_color);

			glLineWidth(7.f);
			mesh->RenderWireframe();
			glLineWidth(1.f);

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glStencilFunc(GL_ALWAYS, 1, -1);
		}

		if (debug_vertex_normals)
		{
			glColor4fv((float*)&d_vertex_p_color);
			mesh->DebugRenderVertex(v_point_size);
			glColor4fv((float*)&d_vertex_l_color);
			mesh->DebugRenderVertexNormals(v_n_line_width);
		}
		//glColor3f(1.0f, 1.0f, 0.0f);
		if (debug_face_normals)
		{
			glColor4fv((float*)&d_vertex_face_color);
			mesh->DebugRenderFacesVertex(f_v_point_size);
			glColor4fv((float*)&d_vertex_face_n_color);
			mesh->DebugRenderFacesNormals(f_n_line_width);
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
	if (ImGui::CollapsingHeader("Shading Modes"))
	{
		ImGui::Title("Fill Faces", 1);	ImGui::Checkbox("##fill_mode", &fill_faces);
		ImGui::Title("Color", 2);		ImGui::ColorEdit4("fILL Color##2f", (float*)&fill_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Title("Alpha", 2);		ImGui::SliderFloat("##alpha_slider", &fill_color.w, config.min_alpha, config.max_alpha, "%.1f", 1.0f);

		ImGui::Separator();

		ImGui::Title("Wireframe", 1);	ImGui::Checkbox("##wireframe", &wireframe);
		ImGui::Title("Color", 2);		ImGui::ColorEdit4("Line Color##2f", (float*)&wire_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Title("Width", 2);		ImGui::SliderFloat("##wire_line_width", &wire_line_width, config.min_line_w, config.max_line_w, "%.1f", 1.0f);
	}
	// --------------------------------------------------------

	if (ImGui::CollapsingHeader("Debug Modes"))
	{
		ImGui::Title("Vertex Normals", 1);		ImGui::Checkbox("##d_vertex_normals", &debug_vertex_normals);
		ImGui::Title("Point Color", 2);			ImGui::ColorEdit4("Vertex Point Color##2f", (float*)&d_vertex_p_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Title("Point Size", 2);			ImGui::SliderFloat("##d_vertex_p_size", &v_point_size, config.min_point_size, config.max_point_size, "%.1f", 1.0f);
		ImGui::Title("Line Color", 2);			ImGui::ColorEdit4("Vertex Line Color##2f", (float*)&d_vertex_l_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Title("Line Width", 2);			ImGui::SliderFloat("##d_vertex_line_width", &v_n_line_width, config.min_line_w, config.max_line_w, "%.1f", 1.0f);
		ImGui::Title("Line Lenght", 2);

		if (ImGui::SliderFloat("##d_vertex_line_lenght", &v_n_line_length, config.min_n_length, config.max_n_length, "%.1f", 1.0f))
		{
			App->importer->ReComputeVertexNormals(v_n_line_length);
		}

		ImGui::Separator();

		ImGui::Title("Face Normals", 1);	ImGui::Checkbox("##d_face_normals", &debug_face_normals);
		ImGui::Title("Point Color", 2);		ImGui::ColorEdit4("Face Point Color##2f", (float*)&d_vertex_face_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Title("Point Size", 2);		ImGui::SliderFloat("##d_face_p_size", &f_v_point_size, config.min_point_size, config.max_point_size, "%.1f", 1.0f);
		ImGui::Title("Line Color", 2);		ImGui::ColorEdit4("Face Line Color##2f", (float*)&d_vertex_face_n_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Title("Line Width", 2);		ImGui::SliderFloat("##d_face_line_width", &f_n_line_width, config.min_line_w, config.max_line_w, "%.1f", 1.0f);
		ImGui::Title("Line Lenght", 2);

		if (ImGui::SliderFloat("##d_face_line_lenght", &f_n_line_length, config.min_n_length, config.max_n_length, "%.1f", 1.0f))
		{
			App->importer->ReComputeFacesNormals(f_n_line_length);
		}
	}
	
	return true;
}

bool ComponentMesh::CleanUp()
{
	delete mesh;

	return true;
}