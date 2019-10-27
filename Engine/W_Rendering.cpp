#include "GL/glew.h"
#include "Application.h"
#include "W_Rendering.h"
#include "ImGui/imgui.h"
#include "ModuleRenderer3D.h"

void W_Rendering::Draw()
{
	RenderConfig& config = App->renderer3D->render_config;

	if (ImGui::Begin(name.c_str(), &active))
	{
		/*if (ImGui::CollapsingHeader("Shading Modes"))
		{
			ImGui::Title("Fill Faces", 1);	ImGui::Checkbox("##fill_mode", &App->importer->fill_faces);
			ImGui::Title("Color", 2);		ImGui::ColorEdit4("fILL Color##2f", (float*)&App->importer->fill_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
			ImGui::Title("Alpha", 2);		ImGui::SliderFloat("##alpha_slider", &App->importer->fill_color.w, config.min_alpha, config.max_alpha, "%.1f", 1.0f);

			ImGui::Separator();

			ImGui::Title("Wireframe", 1);	ImGui::Checkbox("##wireframe", &App->importer->wireframe);
			ImGui::Title("Color", 2);		ImGui::ColorEdit4("Line Color##2f", (float*)&App->importer->wire_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
			ImGui::Title("Width", 2);		ImGui::SliderFloat("##wire_line_width", &App->importer->wire_line_width, config.min_line_w, config.max_line_w, "%.1f", 1.0f);
		}*/

		/*if (ImGui::CollapsingHeader("Debug Modes"))
		{
			ImGui::Title("Vertex Normals", 1);		ImGui::Checkbox("##d_vertex_normals", &App->importer->debug_vertex_normals);
			ImGui::Title("Point Color", 2);			ImGui::ColorEdit4("Vertex Point Color##2f", (float*)&App->importer->d_vertex_p_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
			ImGui::Title("Point Size", 2);			ImGui::SliderFloat("##d_vertex_p_size", &App->importer->v_point_size, config.min_point_size, config.max_point_size, "%.1f", 1.0f);
			ImGui::Title("Line Color", 2);			ImGui::ColorEdit4("Vertex Line Color##2f", (float*)&App->importer->d_vertex_l_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
			ImGui::Title("Line Width", 2);			ImGui::SliderFloat("##d_vertex_line_width", &App->importer->v_n_line_width, config.min_line_w, config.max_line_w, "%.1f", 1.0f);
			ImGui::Title("Line Lenght", 2);

			if (ImGui::SliderFloat("##d_vertex_line_lenght", &App->importer->v_n_line_length, config.min_n_length, config.max_n_length, "%.1f", 1.0f))
			{
				App->importer->ReComputeVertexNormals(App->importer->v_n_line_length);
			}

			ImGui::Separator();

			ImGui::Title("Face Normals", 1);	ImGui::Checkbox("##d_face_normals", &App->importer->debug_face_normals);
			ImGui::Title("Point Color", 2);		ImGui::ColorEdit4("Face Point Color##2f", (float*)&App->importer->d_vertex_face_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
			ImGui::Title("Point Size", 2);		ImGui::SliderFloat("##d_face_p_size", &App->importer->f_v_point_size, config.min_point_size, config.max_point_size, "%.1f", 1.0f);
			ImGui::Title("Line Color", 2);		ImGui::ColorEdit4("Face Line Color##2f", (float*)&App->importer->d_vertex_face_n_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
			ImGui::Title("Line Width", 2);		ImGui::SliderFloat("##d_face_line_width", &App->importer->f_n_line_width, config.min_line_w, config.max_line_w, "%.1f", 1.0f);
			ImGui::Title("Line Lenght", 2);

			if (ImGui::SliderFloat("##d_face_line_lenght", &App->importer->f_n_line_length, config.min_n_length, config.max_n_length, "%.1f", 1.0f))
			{
				App->importer->ReComputeFacesNormals(App->importer->f_n_line_length);
			}
		}*/
		if (ImGui::CollapsingHeader("OpenGL Test", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Title("Color Material");	
			if (ImGui::Checkbox("##GL_COLOR_MATERIAL", &config.gl_color_material))
			{
				if (config.gl_color_material)
					glEnable(GL_COLOR_MATERIAL);
				else
				{
					App->renderer3D->SetDefaultColorMaterial();
					glDisable(GL_COLOR_MATERIAL);
				}
					
			}
			ImGui::Title("Depht Test");		
			if (ImGui::Checkbox("##GL_DEPTH_TEST", &config.gl_depth_test))
			{
				if (config.gl_depth_test)
					glEnable(GL_DEPTH_TEST);
				else
				{
					glDisable(GL_DEPTH_TEST);
				}
			}
			ImGui::Title("Cull Faces");		
			if (ImGui::Checkbox("##GL_CULL_FACE", &config.gl_cull_face))
			{
				if (config.gl_cull_face)
				{
					glEnable(GL_CULL_FACE);
				}
				else
					glDisable(GL_CULL_FACE);
			}
			ImGui::Title("Lighting");		
			if (ImGui::Checkbox("##GL_LIGHTING", &config.gl_lighting))
			{
				if (config.gl_lighting)
				{
					glEnable(GL_LIGHTING);
				}
				else
					glDisable(GL_LIGHTING);
			}
		}
	}

	ImGui::End();
}
