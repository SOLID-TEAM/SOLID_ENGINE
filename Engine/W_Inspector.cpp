#include "Application.h"
#include "W_Inspector.h"
#include "ImGui/imgui.h"
#include "GameObject.h"
#include "Component.h"
#include <vector>
#include "ImGui/misc/cpp/imgui_stdlib.h" // input text wrapper for std::string

void W_Inspector::Draw()
{

	if (ImGui::Begin(name.c_str(), &active))
	{
		// Print all info using components
		// each component already has a draw elements for its own data
		// here only call this
		if (App->scene->selected_go != nullptr)
		{
			GameObject* go = App->scene->selected_go;

			ImGui::Spacing();
			ImGui::Checkbox("##active", &go->active); ImGui::SameLine(); ImGui::SetNextItemWidth(ImGui::GetWindowContentRegionMax().x - ImGui::GetCursorPosY() - 85.F);  ImGui::InputText("##etc", &go->name); ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 75.F);
			ImGui::Checkbox("Static", &go->active); 
			ImGui::Spacing();

			// -----------------------------------------------------------------

			// iterate each component and draw if it contains something to draw
			for (std::vector<Component*>::const_iterator components = go->GetComponents().begin();
				components != go->GetComponents().end(); ++components)
			{
				// TODO: find another way to store individual go opened/closed collapsingheader
				//ImGui::SetNextTreeNodeOpen(!(*components)->collapsed); 

				

				bool aux = ImGui::CollapsingHeader(("   " + (*components)->name).c_str(), (*components)->flags);

				if ((*components)->flags & ImGuiTreeNodeFlags_AllowItemOverlap)
				{
					ImGui::SameLine(30.f);  ImGui::Checkbox(("##active" + (*components)->name).c_str(), &(*components)->active);
				}
		
				if ( aux )
				{
					(*components)->DrawPanelInfo();
					//(*components)->collapsed = false;
				}
				/*else
					(*components)->collapsed = true;*/
			}
		}
	}

	ImGui::End();
}
