#include "C_Transform.h"
#include "ImGui/imgui.h"

ComponentTransform::ComponentTransform(GameObject* parent) : Component(parent)
{
	name.assign("Transform");
	// imgui flags
	flags |= ImGuiTreeNodeFlags_DefaultOpen;
}

ComponentTransform::~ComponentTransform()
{}

bool ComponentTransform::DrawPanelInfo()
{

	ImGui::DragFloat3("Position", (float*)&position, 0.01f, -math::inf, math::inf);
	// TODO: improve visualization --
	ImGui::DragFloat3("Rotation", (float*)&rotation, 0.01f, -math::inf, math::inf);
	// ------------------------------
	ImGui::DragFloat3("Scale", (float*)&scale, 0.01f, -math::inf, math::inf);

	return true;
}