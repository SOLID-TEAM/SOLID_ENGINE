#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "W_Scene.h"

void W_Scene::Draw()
{

	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

	if (ImGui::Begin(name.c_str(), &active))
	{
		ImVec2 current_viewport_size = ImGui::GetContentRegionAvail();
	
		// Input camera ----------------------------------------------------
		App->camera->enable_keys_input = ImGui::IsWindowFocused();

		if (!App->camera->mouse_right_presed)
		{
			App->camera->enable_mouse_input = ImGui::IsMouseHoveringRect(ImGui::GetCursorScreenPos(), ImGui::GetCursorScreenPos() + ImGui::GetContentRegionAvail());
		}

		// Attach texture to window ----------------------------------------
		ImGui::Image((ImTextureID)App->renderer3D->texture_id, ImVec2(current_viewport_size.x, current_viewport_size.y), ImVec2(0, 1), ImVec2(1, 0));

		// Resize logic ----------------------------------------------------
		if (!(current_viewport_size == viewport_size))
		{
			viewport_size = current_viewport_size;
			App->renderer3D->OnResize();
		}
	}

	ImGui::End();

	ImGui::PopStyleVar();

}
