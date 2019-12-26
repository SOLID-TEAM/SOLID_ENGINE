#include "C_BoxCollider.h"
#include "C_Mesh.h"
#include "GameObject.h"

C_BoxCollider::C_BoxCollider(GameObject* go) : C_Collider(go)
{
	name.assign("Box Collider");
	type = ComponentType::BOX_COLLIDER;
	size = float3::one;
}

void C_BoxCollider::CreateShape(C_Mesh* mesh)
{
	if (mesh != nullptr)
	{
		size = mesh->mesh_aabb.Size();
	}
	else
	{
		size = float3::one;
	}

	float3 shape_size = float3::one * 0.5f;
	shape = new btBoxShape(btVector3(shape_size.x, shape_size.y, shape_size.z));
	shape->calculateLocalInertia(1.0f, local_inertia);
}

void C_BoxCollider::AdjustShape()
{
	float3 scaled_size = size.Mul(linked_go->transform->scale.Abs());
	scaled_size = CheckInvalidCollider(scaled_size);
	shape->setLocalScaling(btVector3(scaled_size.x, scaled_size.y, scaled_size.z));
}

void C_BoxCollider::SaveCollider(Config& config)
{

}

void C_BoxCollider::LoadCollider(Config& config)
{
}

float3 C_BoxCollider::CheckInvalidCollider(float3 size)
{
	return size.Max(float3(0.01, 0.01, 0.01));
}

void C_BoxCollider::DrawInfoCollider()
{
	ImGui::Title("Size", 1);	ImGui::DragFloat3("##size", size.ptr(), 0.1f, 0.01f, FLT_MAX);
}

