#include "Application.h"
#include "ImGui/imgui.h"

#include "GameObject.h"

#include "C_Mesh.h"
#include "C_MeshRenderer.h"
#include "C_Transform.h"

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
	ImGui::Title("Name", 1);		ImGui::Text("%s", data->GetName());
	ImGui::Title("Triangles", 1);   ImGui::Text("%u", data->buffers_size[D_Mesh::INDICES] / 3u);
	ImGui::Title("Indices", 1);		ImGui::Text("%u", data->buffers_size[D_Mesh::INDICES]);
	ImGui::Title("Vertices", 1);    ImGui::Text("%u", data->buffers_size[D_Mesh::VERTICES]);
	ImGui::Spacing();

	return true;
}

bool C_Mesh::CleanUp()
{

	delete data;
	
	return true;
}

void C_Mesh::SetMeshResource(D_Mesh* resource)
{
	if (linked_go == nullptr || resource == nullptr)
	{
		return;
	}

	data = resource;

	mesh_aabb.SetNegativeInfinity();
	mesh_aabb = resource->aabb;
}

void C_Mesh::DeleteMeshResource()
{
	//RELEASE(data); // Not delete , resource managment
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

	if (data == nullptr) return false; // If resource is null return false

	// Set vars -------------------------------
	
	local_ray = ray;  
	local_ray.Transform(linked_go->transform->GetGlobalTransform().Inverted()); // Inverted transformation to get local ray
	vertices = data->vertices;
	indices = data->indices;

	near_distance = FLOAT_INF;

	for (uint i = 0u; i < data->buffers_size[D_Mesh::INDICES]; i += 3u)
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
	// TODO: resources
	int resource = 3142413;
	config.AddInt("Resource", resource);
	config.AddBool("Active", active);
	return true;
}
