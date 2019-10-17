#include "Application.h"
#include "W_Hierarchy.h"
#include "ImGui/imgui.h"
#include "ModelData.h"

W_Hierarchy::W_Hierarchy(std::string name, bool active) : Window(name, active)
{
}

void W_Hierarchy::Draw()
{
	//static bool show_hierarchy = true;

	if (ImGui::Begin("Hierarchy"))
	{
		GameObject* root = App->scene->root_go;
		if (root != nullptr)
		{
			for(std::vector<GameObject*>::iterator it = root->childs.begin(); it != root->childs.end(); ++it)
				DrawAll(*it);
		}
	}

	ImGui::End();
}

void W_Hierarchy::DrawAll(GameObject* go)
{
	// TODO: BEWARE of linkeds go, when we deleting them

	ImGuiTreeNodeFlags node_flags = 0;

	if (go->childs.size() == 0)
		node_flags |= ImGuiTreeNodeFlags_Leaf;

	/*node_flags |= ImGuiTreeNodeFlags_CollapsingHeader;*/

	bool selected = App->editor->selected_go == go;
	//bool hover_check = hovered_go == go;
   
	if (selected)
	{
		node_flags |= ImGuiTreeNodeFlags_Selected;
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, (ImVec4)ImColor(220, 220, 220, 255));
		ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(50, 255, 50, 255));
	}

	/*if (hover_check)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(0, 0, 0, 255));
	}*/

	// "default" colors ----------------------
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, (ImVec4)ImColor(255, 0, 255, 255)); // when current clicked
	
	node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth; // allows click on it on full box width
	// ---------------------------------------
	// https://github.com/ocornut/imgui/issues/2077
	bool open = ImGui::TreeNodeEx(go->GetName(), node_flags);
	bool clicked = ImGui::IsItemClicked(0);
	bool hover = ImGui::IsItemHovered();

	// pop colors ------------------------
	if (selected) ImGui::PopStyleColor(2);
	//if (hover_check) ImGui::PopStyleColor(1);
	ImGui::PopStyleColor(1);
	// -----------------------------------

	ImGui::PushID(go->GetName());
	// more code
	ImGui::PopID();

	if (open)
	{
		for (std::vector<GameObject*>::iterator it = go->childs.begin(); it != go->childs.end(); ++it)
			DrawAll((*it));

		ImGui::TreePop();
	}

	if (clicked)
	{
		//LOG("Clicked: %s", go->GetName());
		App->editor->selected_go = go;
	}

	//if (hover) hovered_go = go;

}
