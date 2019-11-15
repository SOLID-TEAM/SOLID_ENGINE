#include "Application.h"
#include "ImGui/imgui.h"

#include "C_Mesh.h"
#include "C_Transform.h"

#include "R_Mesh.h"

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
	ImGui::Title("Name", 1);		ImGui::Text("%s", data->GetName());
	ImGui::Title("Triangles", 1);   ImGui::Text("%u", data->buffers_size[R_Mesh::INDICES] / 3u);
	ImGui::Title("Indices", 1);		ImGui::Text("%u", data->buffers_size[R_Mesh::INDICES]);
	ImGui::Title("Vertices", 1);    ImGui::Text("%u", data->buffers_size[R_Mesh::VERTICES]);
	ImGui::Spacing();

	return true;
}

bool C_Mesh::CleanUp()
{

	delete data;
	
	return true;
}

void C_Mesh::SetMeshResource(R_Mesh* resource)
{
	if (linked_go == nullptr || resource == nullptr)
	{
		return;
	}

	data = resource;
}

void C_Mesh::DeleteMeshResource()
{
	RELEASE(data);
}

bool C_Mesh::Save(Config& config)
{
	// TODO: resources
	int resource = 3142413;
	config.AddInt("Resource", resource);
	config.AddBool("Active", active);


	return true;
}
