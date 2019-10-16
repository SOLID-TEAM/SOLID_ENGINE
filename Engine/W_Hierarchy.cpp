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

		//// get all the gameObjects
		//std::vector<ModelData*> vgo = App->importer->GetModels();
		//std::vector<ModelData*>::iterator go = vgo.begin();

		//for (; go != vgo.end(); ++go)
		//{
		//	ImGui::PushID((int)*go); // to correctly "link" each element without same problems with identical name when we manage buttons etc

		//	if (ImGui::TreeNode((*go)->name.data()))
		//	{
		//		ImGui::Text("blabla");

		//		if (ImGui::TreeNode("Renderer Options"))
		//		{
		//			ImGui::Text("wip");
		//			ImGui::TreePop();
		//		}

		//		ImGui::TreePop();
		//	}

		//	ImGui::PopID();
		//}
	}

	ImGui::End();
}

void W_Hierarchy::DrawAll(GameObject* go)
{
	ImGuiTreeNodeFlags node_flags = 0;
	/*const bool is_selected = (selection_mask & (1 << i)) != 0;
	if (is_selected)
		node_flags |= ImGuiTreeNodeFlags_Selected;*/

	if (go->childs.size() == 0)
		node_flags |= ImGuiTreeNodeFlags_Leaf;

	ImGui::PushID(go);
	if (ImGui::TreeNodeEx(go->GetName(), node_flags))
	{
		if (ImGui::IsItemClicked(0))
		{
			LOG("");
		}

		for (std::vector<GameObject*>::iterator it = go->childs.begin(); it != go->childs.end(); ++it)
			DrawAll((*it));
		
		ImGui::TreePop();
	}

	if (ImGui::IsItemClicked(0))
	{
		LOG("[Init] Clicked %s", go->GetName());
	}

	ImGui::PopID();
}
