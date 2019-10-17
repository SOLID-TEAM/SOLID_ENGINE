#include "Application.h"
#include "W_Inspector.h"

W_Inspector::W_Inspector(std::string name, bool active) : Window(name, active)
{
}

void W_Inspector::Draw()
{
	if (ImGui::Begin("Inspector"))
	{
		// Print all info using components
		// each component already has a draw elements for its own data
		// here only call this
		if (App->editor->selected_go != nullptr)
		{
			GameObject* go = App->editor->selected_go;
			ImGui::TextColored((ImVec4)ImColor(255,0,255,255) ,go->GetName());
			// iterate each component and draw if it contains something to draw
			for (std::vector<Component*>::const_iterator components = go->GetComponents().begin();
				components != go->GetComponents().end(); ++components)
			{
				// TODO: find another way to store individual go opened/closed collapsingheader
				//ImGui::SetNextTreeNodeOpen(!(*components)->collapsed); 
				if (ImGui::CollapsingHeader((*components)->name.c_str(), (*components)->flags))
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