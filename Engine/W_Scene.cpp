#include "Application.h"
#include "W_Scene.h"
#include "ModuleRenderer3D.h"


void W_Scene::Draw()
{

	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

	if (ImGui::Begin(name.c_str(), &active))
	{
		ImVec2 region_avail = ImGui::GetContentRegionAvail();
		
		ImGui::Image((ImTextureID)App->renderer3D->texture_id, ImVec2(region_avail.x, region_avail.y), ImVec2(0, 1), ImVec2(1, 0));

		if (!(size == ImGui::GetWindowSize()))
		{
			App->renderer3D->OnResize(ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
			size = ImGui::GetWindowSize();
		}
		
	}
	ImGui::End();

	ImGui::PopStyleVar();
}
