#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "W_Scene.h"

void W_Scene::Draw()
{
	// --------------------
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

	if (ImGui::Begin(name.c_str(), &active , ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar ))
	{
		if (ImGui::BeginMenuBar())
		{
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(10.f, 10.f));
			ImGui::SetSeparationType(ImGuiSeparationType::ImGui_MenuSeparation);

			if (ImGui::BeginMenu("Debug"))
			{
				DebugMenu();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Examples"))
			{

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Tools"))
			{
				ImGui::EndMenu();
			}

			ImGui::PopStyleVar();
			ImGui::EndMenuBar();

			ImGui::SetSeparationType(ImGuiSeparationType::ImGui_WindowSeparation);
		}

		//if (ImGui::CollapsingHeader("viewport options"))
		//{
		//	ImGui::SetCursorPosX(10);
		//	ImGui::Text("Wireframe"); ImGui::SameLine();
		//	ImGui::Checkbox("##wire", &App->editor->viewport_options.wireframe); 
		//	ImGui::SameLine();

		//	/*float cur_w_pos = ImGui::GetCursorPosX(); */
		//	bool debug_n = ImGui::Button("Debug normals");
		//	ImGui::SetNextWindowContentWidth(320);
		//
		//	if (ImGui::BeginPopupContextItem(0,0))
		//	{
		//		//ImGui::SetCursorPosX(10);
		//		ImGui::Dummy(ImVec2(0, 8));
		//		DebugMenu();
		//		ImGui::Dummy(ImVec2(0, 8));
		//		ImGui::EndPopup();
		//	}
		//}

		// Input camera ----------------------------------------------------
		App->camera->enable_keys_input = ImGui::IsWindowFocused();

		if (!App->camera->mouse_right_presed)
		{
			App->camera->enable_mouse_input = ImGui::IsMouseHoveringRect(ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + ImGui::GetContentRegionAvail());
		}

		// Attach texture to window ----------------------------------------
		ImVec2 current_viewport_size = ImGui::GetContentRegionAvail();

		ImGui::Image((ImTextureID)App->renderer3D->texture_id, ImVec2(current_viewport_size.x, current_viewport_size.y), ImVec2(0, 1), ImVec2(1, 0));

		// Resize logic ----------------------------------------------------
		if (!(current_viewport_size == viewport_size))
		{
			viewport_size = current_viewport_size;
			App->renderer3D->OnResize();
		}
	}

	ImGui::PopStyleVar();
	ImGui::End();

}

void W_Scene::DebugMenu()
{
	RenderConfig& config = App->renderer3D->render_config;
	ViewPortOptions& vp = App->editor->viewport_options;

	ImGui::Title("Vertex Normals", 1);		ImGui::Checkbox("##d_vertex_normals", &vp.debug_vertex_normals);
	ImGui::Title("Point Color", 2);			ImGui::ColorEdit4("Vertex Point Color##2f", (float*)&vp.d_vertex_p_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	ImGui::Title("Point Size", 2);			ImGui::SetNextItemWidth(150); ImGui::SliderFloat("##d_vertex_p_size", &vp.v_point_size, config.min_point_size, config.max_point_size, "%.1f", 1.0f);
	ImGui::Title("Line Color", 2);			ImGui::ColorEdit4("Vertex Line Color##2f", (float*)&vp.d_vertex_l_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	ImGui::Title("Line Width", 2);			ImGui::SetNextItemWidth(150); ImGui::SliderFloat("##d_vertex_line_width", &vp.v_n_line_width, config.min_line_w, config.max_line_w, "%.1f", 1.0f);
	ImGui::Title("Line Lenght", 2);

	ImGui::SetNextItemWidth(150);
	if (ImGui::SliderFloat("##d_vertex_line_lenght", &vp.v_n_line_length, config.min_n_length, config.max_n_length, "%.1f", 1.0f))
	{
		App->importer->ReComputeVertexNormals(vp.v_n_line_length);
	}
	ImGui::Spacing();

	ImGui::Separator();

	ImGui::Spacing();
	ImGui::Title("Face Normals", 1);	ImGui::Checkbox("##d_face_normals", &vp.debug_face_normals);
	ImGui::Title("Point Color", 2);		ImGui::ColorEdit4("Face Point Color##2f", (float*)&vp.d_vertex_face_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	ImGui::Title("Point Size", 2);		ImGui::SetNextItemWidth(150); ImGui::SliderFloat("##d_face_p_size", &vp.f_v_point_size, config.min_point_size, config.max_point_size, "%.1f", 1.0f);
	ImGui::Title("Line Color", 2);		ImGui::ColorEdit4("Face Line Color##2f", (float*)&vp.d_vertex_face_n_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
	ImGui::Title("Line Width", 2);		ImGui::SetNextItemWidth(150); ImGui::SliderFloat("##d_face_line_width", &vp.f_n_line_width, config.min_line_w, config.max_line_w, "%.1f", 1.0f);
	ImGui::Title("Line Lenght", 2);

	ImGui::SetNextItemWidth(150);
	if (ImGui::SliderFloat("##d_face_line_lenght", &vp.f_n_line_length, config.min_n_length, config.max_n_length, "%.1f", 1.0f))
	{
		App->importer->ReComputeFacesNormals(vp.f_n_line_length);
	}
}
