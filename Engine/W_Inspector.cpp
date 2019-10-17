#include "Application.h"
#include "W_Inspector.h"
#include "ImGui/imgui.h"
#include "GameObject.h"
#include "Component.h"
#include <vector>

void W_Inspector::Draw()
{

	if (ImGui::Begin(name.c_str(), &active))
	{
		// Print all info using components
		// each component already has a draw elements for its own data
		// here only call this
		if (App->editor->selected_go != nullptr)
		{
			GameObject* go = App->editor->selected_go;

			char* go_name = new char[200 + 1];
			std::copy(go->name.begin(), go->name.end(), go_name);
			go_name[200] = '\0';


			ImGui::Spacing();
			ImGui::Checkbox("##active", &go->active); ImGui::SameLine(); ImGui::InputText("##go_name", go_name, 200);
			ImGui::Spacing();

			go->name.clear();
			go->name.assign(go_name);

			delete[]go_name;

			// iterate each component and draw if it contains something to draw
			for (std::vector<Component*>::const_iterator components = go->GetComponents().begin();
				components != go->GetComponents().end(); ++components)
			{
				// TODO: find another way to store individual go opened/closed collapsingheader
				//ImGui::SetNextTreeNodeOpen(!(*components)->collapsed); 
				//if (ImGui::CollapsingHeader((*components)->name.c_str(), (*components)->flags))
				//{
					(*components)->InspectorDraw();
					//(*components)->collapsed = false;
				//}
				/*else
					(*components)->collapsed = true;*/
			}
		}
	}

	ImGui::End();
}
