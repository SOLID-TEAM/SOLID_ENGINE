#include "Application.h"
#include "W_Hierarchy.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "R_Mesh.h"

W_Hierarchy::W_Hierarchy(std::string name, bool active) : Window(name, active)
{
}

void W_Hierarchy::Draw()
{
	//static bool show_hierarchy = true;

	if (ImGui::Begin("Hierarchy", &active))
	{
		GameObject* root = App->scene->root_go;
		if (root != nullptr)
		{
			for (std::vector<GameObject*>::iterator it = root->childs.begin(); it != root->childs.end(); ++it)
				DrawAll(*it);
		}
	}

	ImGui::End();
}

void W_Hierarchy::DrawAll(GameObject* go)
{
	// TODO: BEWARE of linkeds go, when we deleting them

	ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen; // allows click on it on full box width | allows open only by arrow click

	if (go->childs.size() == 0)
		node_flags |= ImGuiTreeNodeFlags_Leaf;

	/*node_flags |= ImGuiTreeNodeFlags_CollapsingHeader;*/

	bool selected = App->scene->selected_go == go;
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

	ImGui::PushID(go);
	// ---------------------------------------
	// https://github.com/ocornut/imgui/issues/2077
	bool open = ImGui::TreeNodeEx(go->GetName(), node_flags);
	bool clicked = ImGui::IsItemClicked(0);
	//bool right_clicked = ImGui::IsItemClicked(1);
	bool hover = ImGui::IsItemHovered();

	// pop colors ------------------------
	if (selected) ImGui::PopStyleColor(2);
	//if (hover_check) ImGui::PopStyleColor(1);
	ImGui::PopStyleColor(1);
	// -----------------------------------

	
	

	// RIGHT CLICK BEHAVIOUR ---
	
	if (ImGui::BeginPopupContextItem(go->GetName()))
	{
		App->scene->selected_go = go;

		if (ImGui::Button("Delete"))
		{
			//LOG("[Error] TODO: deleting %s gameobject", go->GetName());
			App->scene->DeleteGameObject(go);

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
	ImGui::PopID();

	if (clicked)
	{
		//LOG("Clicked: %s", go->GetName());
		App->scene->selected_go = go;
	}

	// DRAG AND DROP
	 // Our buttons are both drag sources and drag targets here!
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload("gameobject_object", &go, sizeof(GameObject*));    // Set payload to carry the index of our item (could be anything)
	   // Display preview (could be anything, e.g. when dragging an image we could decide to display the filename and a small preview of the image, etc.)
		ImGui::Text("Move %s", go->GetName());

		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTargetCustom(ImGui::GetCurrentWindow()->Rect(), ImGui::GetID("Hierarchy")))
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("gameobject_object"))
		{
			IM_ASSERT(payload->DataSize == sizeof(GameObject*));
			GameObject** source_go = (GameObject**)payload->Data;

			App->scene->selected_go = (*source_go);

			App->scene->AddGoToHierarchyChange(App->scene->root_go, (*source_go));

			//LOG("[Info] Moved %s to %s", (*source_go)->GetName(), go->GetName());
		}

		ImGui::EndDragDropTarget();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("gameobject_object"))
		{
			IM_ASSERT(payload->DataSize == sizeof(GameObject*));
			GameObject** source_go = (GameObject**)payload->Data;

			//LOG("%s",(*source_go)->GetName());
			
			/*go->AddChildren((*source_go));*/
			App->scene->AddGoToHierarchyChange(go, (*source_go));

			App->scene->selected_go = (*source_go);

			//LOG("[Info] Moved %s to %s", (*source_go)->GetName(), go->GetName());
		}
		ImGui::EndDragDropTarget();
	}

	if (open)
	{
		for (std::vector<GameObject*>::iterator it = go->childs.begin(); it != go->childs.end(); ++it)
			DrawAll((*it));

		ImGui::TreePop();
	}
	
	
	//if (hover) hovered_go = go;

}
