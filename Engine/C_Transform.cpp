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

bool ComponentTransform::InspectorDraw()
{

	const char* names[3] = { "X", "Y", "Z" };
	static float position[3] = { 0.f, 0.f, 0.f };
	static float rotation[3] = { 0.f, 0.f, 0.f };
	static float scale[3] = { 0.f, 0.f, 0.f };

	bool aux = false;

	aux = ImGui::CollapsingHeader("Transform"); ImGui::SetItemAllowOverlap(); ImGui::SameLine(); ImGui::Button("Reset");

	if (aux)
	{
		ImGui::Spacing();
		ImGui::Title("Position", 1);	ImGui::DragFloatNEx(names, (float*)&position, 3);
		ImGui::Title("Rotation", 1);	ImGui::DragFloatNEx(names, (float*)&rotation, 3);
		ImGui::Title("Scale", 1);		ImGui::DragFloatNEx(names, (float*)&scale, 3);
		ImGui::Spacing();
	}

	return true;
}