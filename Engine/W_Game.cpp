#include "W_Game.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Viewport.h"
#include "IconFontAwesome/IconsFontAwesome5.h"

void W_Game::Draw()
{
	// --------------------
	ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

	if (ImGui::Begin(" " ICON_FA_GAMEPAD " Game", &active, ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar  /*| ImGuiWindowFlags_::ImGuiWindowFlags_NoDecoration*/))
	{
		if (ImGui::BeginMenuBar())
		{
			ImGui::SetSeparationType(ImGuiSeparationType::ImGui_MenuSeparation);
			ImGui::PushStyleVar(ImGuiStyleVar_::ImGuiStyleVar_WindowPadding, ImVec2(10.f, 10.f));



			ImGui::PopStyleVar();
			ImGui::SetSeparationType(ImGuiSeparationType::ImGui_WindowSeparation);

			ImGui::EndMenuBar();
		}

		ImVec2 current_viewport_size = ImGui::GetContentRegionAvail();

		// Attach texture to window ----------------------------------------

		ImGui::Image((ImTextureID)App->scene->game_viewport->GetTexture(), ImVec2(current_viewport_size.x, current_viewport_size.y), ImVec2(0, 1), ImVec2(1, 0));

		// Set viewport values -----------------------------------

		if (!(current_viewport_size == viewport_size))
		{
			viewport_size = current_viewport_size;
			App->scene->game_viewport->SetSize(viewport_size.x, viewport_size.y);
		}
	}

	App->scene->game_viewport->active = active;

	// --------------------------------------------

	ImGui::PopStyleVar();
	ImGui::End();
}