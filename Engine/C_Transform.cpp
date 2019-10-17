#include "C_Transform.h"
#include "ImGui/imgui.h"

ComponentTransform::ComponentTransform(GameObject* parent) : Component(parent)
{
	name.assign("Transform");
	// imgui flags
	flags |= ImGuiTreeNodeFlags_DefaultOpen;
}

ComponentTransform::~ComponentTransform()
{

}

bool ComponentTransform::DrawPanelInfo()
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