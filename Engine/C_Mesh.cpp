#include "Application.h"
#include "ImGui/imgui.h"
#include "ModuleResources.h"
#include "GameObject.h"

#include "C_Mesh.h"
#include "C_MeshRenderer.h"
#include "C_Transform.h"

#include "R_Mesh.h"

C_Mesh::C_Mesh(GameObject* parent) : Component(parent, ComponentType::MESH)
{
	name.assign("Mesh");
}

C_Mesh::~C_Mesh() {}


bool C_Mesh::Update()
{
	return true;
}

bool C_Mesh::DrawPanelInfo()
{
	R_Mesh* r = (R_Mesh*)App->resources->Get(resource);
	if (r != nullptr)
	{
		ImGui::Spacing();
		ImGui::Title("Name", 1);		ImGui::Text("%s", r->GetName().c_str());
		ImGui::Title("Resource", 1);	ImGui::Text("%i", r->GetUID());
		ImGui::Separator();
		ImGui::Title("Triangles", 1);   ImGui::Text("%u", r->buffers_size[R_Mesh::INDICES] / 3u);
		ImGui::Title("Indices", 1);		ImGui::Text("%u", r->buffers_size[R_Mesh::INDICES]);
		ImGui::Title("Vertices", 1);    ImGui::Text("%u", r->buffers_size[R_Mesh::VERTICES]);
		ImGui::Spacing();
	}

	return true;
}

bool C_Mesh::CleanUp()
{
	DeleteMeshResource();

	return true;
}

bool C_Mesh::SetMeshResource(UID uid)
{
	bool ret = false;
	// if any resource attached
	if (resource != 0)
	{
		Resource* r = App->resources->Get(resource);
		if (r != nullptr)
			r->Release();
	}

	if (uid != 0)
	{
		Resource* new_r = App->resources->Get(uid);
		if (new_r != nullptr)
		{
			if (new_r->GetType() == Resource::Type::MESH)
			{
				R_Mesh* m = (R_Mesh*)new_r;

				if (m->LoadToMemory())
				{
					resource = uid;

					ret = true;
				}
			}
		}
		else
		{
			LOG("[Error] Resource not found");
		}
		
	}

	mesh_aabb.SetNegativeInfinity();
	R_Mesh* res = (R_Mesh*)App->resources->Get(resource);
	mesh_aabb = res->aabb;

	return ret;

}

UID C_Mesh::GetResource()
{
	return resource;
}

void C_Mesh::DeleteMeshResource()
{
	if (App->resources->Get(resource) != nullptr)
		App->resources->Get(resource)->Release();
}

bool C_Mesh::Save(Config& config)
{
	config.AddInt("Resource", resource);
	config.AddBool("Active", enable);
	return true;
}

bool C_Mesh::Load(Config& config)
{
	SetMeshResource(config.GetInt("Resource", resource));
	enable = config.GetBool("Active", enable);

	return true;
}