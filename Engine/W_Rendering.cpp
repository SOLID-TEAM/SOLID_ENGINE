#include "Application.h"
#include "W_Rendering.h"
#include "ImGui/imgui.h"
#include "ModuleRenderer3D.h"

void W_Rendering::Draw()
{
	RenderConfig& config = App->renderer3D->render_config;

	if (ImGui::Begin(name.c_str()) )
	{
		if (ImGui::CollapsingHeader("Shading Modes"))
		{
			ImGui::Title("Fill Faces", 1);	ImGui::Checkbox("##fill_mode", &App->importer->fill_faces);
			ImGui::Title("Color", 2);		ImGui::ColorEdit4("fILL Color##2f", (float*)&App->importer->fill_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
			ImGui::Title("Alpha", 2);		ImGui::SliderFloat("##alpha_slider", &App->importer->fill_color.w, config.min_alpha, config.max_alpha, "%.1f", 1.0f);

			ImGui::Separator();

			ImGui::Title("Wireframe", 1);	ImGui::Checkbox("##wireframe", &App->importer->wireframe);
			ImGui::Title("Color", 2);		ImGui::ColorEdit4("Line Color##2f", (float*)&App->importer->wire_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
			ImGui::Title("Width", 2);		ImGui::SliderFloat("##wire_line_width", &App->importer->wire_line_width, config.min_line_w, config.max_line_w, "%.1f", 1.0f);
		}

		if (ImGui::CollapsingHeader("Debug Modes"))
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
		}

		static int units = 50;

		if (ImGui::CollapsingHeader("Grid Settings"))
		{
			ImGui::Title("Active", 1);		ImGui::Checkbox("##active_grid", &App->test->main_grid->active);
			ImGui::Title("Color", 2);		ImGui::ColorEdit4("Grid Color##2f", (float*)&App->test->main_grid->color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
			ImGui::Title("Opacity", 2);		ImGui::SliderFloat("##d_face_line_lenght", &App->test->main_grid->color.w, config.min_alpha, config.max_alpha, "%.1f", 1.0f);
			ImGui::Title("Distance", 2);     if (ImGui::SliderInt("##grid_units", &units, 1, 50)) App->test->main_grid->SetUnits(units);
			ImGui::Title("Line Width", 2);	ImGui::SliderFloat("##grid_width", &App->test->main_grid->width, config.min_line_w, config.max_line_w, "%.1f", 1.0f);
		}
		if (ImGui::CollapsingHeader("OpenGL Test"))
		{
			ImGui::Title("Color Material");	
			if (ImGui::Checkbox("##GL_COLOR_MATERIAL", &config.gl_color_material))
			{
				if (!config.gl_color_material)
					App->renderer3D->SetDefaultColorMaterial();
					
			}
			ImGui::Title("Depht Test");		ImGui::Checkbox("##GL_DEPTH_TEST", &config.gl_depth_test);
			ImGui::Title("Cull Faces");		ImGui::Checkbox("##GL_CULL_FACE", &config.gl_cull_face);
			ImGui::Title("Lighting");		ImGui::Checkbox("##GL_LIGHTING", &config.gl_lighting);
		}

		// TODO: checker texture temporary here
		static bool view_checker = false;
		static int val = 512, v_min = 8, v_max = 2048;
		static uint tex_id = 0;

		if (ImGui::CollapsingHeader("Checker Texture"))
		{
			if (ImGui::Checkbox("view uv checker", &view_checker))
			{
				if (view_checker)
				{
					// TODO: create checker texture with default size
					tex_id = App->textures->GenerateCheckerTexture(val, val);
				}
				else
				{
					// TODO: unload texture id
				}
			}

			if (view_checker)
			{
				if (ImGui::BeginCombo("Resolution", std::to_string(val).data()))
				{
					for (uint i = v_min; i < v_max * 2; i = i > 0 ? i * 2 : ++i)
					{
						ImGui::PushID(i);
						bool is_selected;
						if (ImGui::Selectable(std::to_string(i).data(), val == i))
						{
							val = i;

							// TODO: delete and reload procedural checker

						}
						ImGui::PopID();
					}

					ImGui::EndCombo();
				}

				ImGui::Image((ImTextureID)tex_id, ImVec2(App->test->map(val, 0, 2048, 128, 512), App->test->map(val, 0, 2048, 128, 512)));
			}
		}
	}

	ImGui::End();
}
