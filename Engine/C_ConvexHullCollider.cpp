#include "Application.h"

#include "C_ConvexHullCollider.h"
#include "C_Mesh.h"
#include "R_Mesh.h"
#include "GameObject.h"

C_ConvexHullCollider::C_ConvexHullCollider(GameObject* go) : C_Collider(go)
{
	name.assign("Convex Hull Collider");
	type = ComponentType::CONVEX_HULL_COLLIDER;
	size = float3::one;
}

void C_ConvexHullCollider::CreateShape(C_Mesh* mesh)
{
	if (mesh == nullptr)
		return;

	R_Mesh* r_mesh = dynamic_cast<R_Mesh*>(App->resources->Get(mesh->GetResource()));

	if (r_mesh == nullptr)
		return;
	
	btConvexHullShape* new_shape = new btConvexHullShape();
	uint num_indices = r_mesh->buffers_size[2]; // buffertype::indices
	for (uint i = 0; i < num_indices; ++i)
	{
		float* v = &r_mesh->vertices[r_mesh->indices[i] * 3];

		btVector3 vertex = { v[0],v[1],v[2] };
		new_shape->addPoint(vertex);
	}

	bool optimize = false;

	//if (optimize)
	//{
	//	// TODO: use btshapeHull to trim vertices size
	//	btShapeHull* hull = new btShapeHull(new_shape);
	//	btScalar margin = new_shape->getMargin();
	//	hull->buildHull(margin);
	//}

	shape = new_shape;
}

void C_ConvexHullCollider::AdjustShape()
{
	// TODO: 
	scaled_center = center.Mul(linked_go->transform->scale);

	float3 scaled_size = size.Mul(linked_go->transform->scale.Abs());
	scaled_size = CheckInvalidCollider(scaled_size);
	shape->setLocalScaling(btVector3(scaled_size.x, scaled_size.y, scaled_size.z));
	//shape->setLocalScaling(btVector3(1.0f, 1.0f, 1.0f));
}

void C_ConvexHullCollider::SaveCollider(Config& config)
{

}

void C_ConvexHullCollider::LoadCollider(Config& config)
{
}

float3 C_ConvexHullCollider::CheckInvalidCollider(float3 size)
{
	return size.Max(float3(0.01, 0.01, 0.01));
}

void C_ConvexHullCollider::DrawInfoCollider()
{
	//ImGui::Title("Size", 1);	ImGui::DragFloat3("##size", size.ptr(), 0.1f, 0.01f, FLT_MAX);
}

