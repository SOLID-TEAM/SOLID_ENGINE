#include "Application.h"
#include "W_Scene.h"
#include "ModuleRenderer3D.h"


void W_Scene::Draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

	if (ImGui::Begin(name.c_str(), &active))
	{
		ImGui::Image((ImTextureID)App->renderer3D->render_texture_id, ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), ImVec2(0, 1), ImVec2(1, 0));
	}
	ImGui::End();

	ImGui::PopStyleVar();

}
