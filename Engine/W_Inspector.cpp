#include "W_Inspector.h"
#include "ImGui/imgui.h"
#include "GameObject.h"
#include "Component.h"
#include <vector>

void W_Inspector::Draw()
{
	if (ImGui::Begin(name.c_str(), &active))
	{
		if (selected_go == nullptr)
		{
			ImGui::End();
			return;
		}
		
		for (std::vector<Component*>::iterator itr = selected_go->components.begin(); itr != selected_go->components.end(); ++itr)
		{
			(*itr)->InspectorDraw();
		}
	}
	ImGui::End();
}
