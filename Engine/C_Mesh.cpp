#include "Application.h"
#include "ImGui/imgui.h"

#include "C_Mesh.h"
#include "D_Mesh.h"

C_Mesh::C_Mesh(GameObject* parent) : Component(parent, ComponentType::MESH)
{
	name.assign("Mesh");
}

C_Mesh::~C_Mesh() {}


bool C_Mesh::Update(float dt)
{
	return true;
}

bool C_Mesh::DrawPanelInfo()
{
	ImGui::Spacing();
	ImGui::Title("Triangles", 1);   ImGui::Text("%u", data->buffers_size[D_Mesh::INDICES] / 3u);
	ImGui::Title("Indices", 1);		ImGui::Text("%u", data->buffers_size[D_Mesh::INDICES]);
	ImGui::Title("Vertices", 1);    ImGui::Text("%u", data->buffers_size[D_Mesh::VERTICES]);
	ImGui::Spacing();

	return true;
}

bool C_Mesh::CleanUp()
{
	return true;
}