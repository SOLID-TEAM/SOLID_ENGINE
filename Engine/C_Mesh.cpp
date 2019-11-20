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


bool C_Mesh::Update(float dt)
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

void C_Mesh::DeleteMeshResource()
{
	if (App->resources->Get(resource) != nullptr)
		App->resources->Get(resource)->Release();
}

// TODO: Create hit struct to save triangle , point, normal, etc.

bool C_Mesh::Intersects(const LineSegment& ray, float& near_distance, math::float3& hit_point) 
{
	// Operation vars -------------------------
	LineSegment local_ray;

	Triangle triangle;
	math::float3 local_hit_point;
	bool intersects = false;
	float curr_distance;
	// Resource vars ---------------------------
	float* vertices = nullptr;
	uint* indices = nullptr;
	uint indx = 0u;
	R_Mesh* res = (R_Mesh*)App->resources->Get(resource);


	if (res == nullptr) 
		return false; // If resource is null return false

	// Set vars -------------------------------
	
	local_ray = ray;  
	local_ray.Transform(linked_go->transform->GetGlobalTransform().Inverted()); // Inverted transformation to get local ray
	vertices = res->vertices;
	indices = res->indices;

	near_distance = FLOAT_INF;

	for (uint i = 0u; i < res->buffers_size[R_Mesh::INDICES]; i += 3u)
	{
		// Set triangle -------------------------------------------------
		indx = indices[i];
		triangle.a = { vertices[indx], vertices[indx + 1u], vertices[indx + 2u] };
		indx = indices[i + 1u];
		triangle.b = { vertices[indx], vertices[indx + 1u], vertices[indx + 2u] };
		indx = indices[i + 2u];
		triangle.c = { vertices[indx], vertices[indx + 1u], vertices[indx + 2u] };
	
		// Check intersection -------------------------------------------

		if (local_ray.Intersects(triangle, &curr_distance, &local_hit_point))
		{
			intersects = true;

			if (curr_distance < near_distance)
			{
				near_distance = curr_distance;
				hit_point = local_hit_point;
			}
		}
	}

	if (intersects == true)
	{
		// Transform local point to global point -----------------------

		hit_point = linked_go->transform->GetGlobalTransform().MulPos(local_hit_point);
	}

	return intersects;
}

bool C_Mesh::Save(Config& config)
{
	//// TODO: resources
	//int resource = 3142413;
	config.AddInt("Resource", resource);
	config.AddBool("Active", active);
	return true;
}

bool C_Mesh::Load(Config& config)
{
	SetMeshResource(config.GetInt("Resource", resource));
	active = config.GetBool("Active", active);

	return true;
}