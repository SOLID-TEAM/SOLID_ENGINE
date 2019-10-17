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
		//if (selected_go == nullptr)
		//{
		//	ImGui::End();
		//	return;
		//}

		//for (std::vector<Component*>::iterator itr = selected_go->components.begin(); itr != selected_go->components.end(); ++itr)
		//{
		//	(*itr)->InspectorDraw();
		//}

		static char go_name[40] = "GameObject";

		ImGui::Spacing();
		ImGui::Checkbox("##active", /*&selected_go->active*/&active); ImGui::SameLine(); ImGui::InputText("##go_name", go_name, 40);
		ImGui::Spacing();

		const char* names[3] = { "X", "Y", "Z" };
		static float position[3] = { 0.f, 0.f, 0.f };
		static float rotation[3]= { 0.f, 0.f, 0.f };
		static float scale[3] = { 0.f, 0.f, 0.f };

		bool aux = false;

		aux = ImGui::CollapsingHeader("Transform"); ImGui::SetItemAllowOverlap(); ImGui::SameLine(); ImGui::Button("Reset");

		if (aux)
		{
			ImGui::Spacing();
			ImGui::Title("Position", 1);	ImGui::DragFloatNEx(names, position, 3);
			ImGui::Title("Rotation", 1);	ImGui::DragFloatNEx(names, rotation, 3);
			ImGui::Title("Scale", 1);		ImGui::DragFloatNEx(names, scale, 3);
			ImGui::Spacing();
		}
	}

	ImGui::End();
}
