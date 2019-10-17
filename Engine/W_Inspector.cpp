#include "Application.h"
#include "W_Inspector.h"
#include "ImGui/imgui.h"
#include "GameObject.h"
#include "Component.h"
#include <vector>

void W_Inspector::Draw()
{
	//if (ImGui::Begin(name.c_str(), &active))
	//{
	//	if (selected_go == nullptr)
	//	{
	//		ImGui::End();
	//		return;
	//	}
	//	
	//	for (std::vector<Component*>::iterator itr = selected_go->components.begin(); itr != selected_go->components.end(); ++itr)
	//	{
	//		(*itr)->InspectorDraw();
	//	}
	//}
	//ImGui::End();


	if (ImGui::Begin("Inspector"))
	{
		// Print all info using components
		// each component already has a draw elements for its own data
		// here only call this
		if (App->editor->selected_go != nullptr)
		{
			GameObject* go = App->editor->selected_go;
			ImGui::TextColored((ImVec4)ImColor(255, 0, 255, 255), go->GetName());
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
