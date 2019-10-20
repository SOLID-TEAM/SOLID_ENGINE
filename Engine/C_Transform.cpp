#include "C_Transform.h"
#include "ImGui/imgui.h"

C_Transform::C_Transform(GameObject* parent) : Component(parent, ComponentType::TRANSFORM)
{
	name.assign("Transform");
	flags |= ImGuiTreeNodeFlags_DefaultOpen;
}

C_Transform::~C_Transform()
{

}

bool C_Transform::DrawPanelInfo()
{
	//bool aux = false;

	//aux = ImGui::CollapsingHeader("Transform"); ImGui::SetItemAllowOverlap(); ImGui::SameLine(); ImGui::Button("Reset");

	//if (aux)
	//{
		ImGui::Spacing();
		ImGui::Title("Position", 1);	ImGui::DragFloat3("##position", (float*)&position );
		ImGui::Title("Rotation", 1);	ImGui::DragFloat3("##rotation", (float*)&rotation );
		ImGui::Title("Scale", 1);		ImGui::DragFloat3("##scale	 ", (float*)&scale);
		ImGui::Spacing();
	//}

	return true;
}