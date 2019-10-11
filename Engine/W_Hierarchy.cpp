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
		// get all the gameObjects
		std::vector<ModelData*> vgo = App->importer->GetModels();
		std::vector<ModelData*>::iterator go = vgo.begin();

		for (; go != vgo.end(); ++go)
		{
			ImGui::PushID((int)*go); // to correctly "link" each element without same problems with identical name when we manage buttons etc

			if (ImGui::TreeNode((*go)->name.data()))
			{
				ImGui::Text("blabla");

				if (ImGui::TreeNode("Renderer Options"))
				{
					ImGui::Text("wip");
					ImGui::TreePop();
				}

				ImGui::TreePop();
			}

			ImGui::PopID();
		}
	}

	ImGui::End();
}
