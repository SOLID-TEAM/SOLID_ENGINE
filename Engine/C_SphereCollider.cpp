#include "C_SphereCollider.h"
#include "GameObject.h"
#include "C_Mesh.h"
#include "Bullet/include/BulletDynamics/Character/btKinematicCharacterController.h"
#include "Bullet/include/BulletDynamics/Character/btCharacterControllerInterface.h"

C_SphereCollider::C_SphereCollider(GameObject* go) :C_Collider(go)
{
	name.assign("Sphere Collider");
	type = ComponentType::SPHERE_COLLIDER;
	radius = 0.5f;
}

void C_SphereCollider::CreateShape(C_Mesh* mesh)
{
	if (is_loaded == false)
	{
		radius = (mesh != nullptr) ? linked_go->GetOBB().MinimalEnclosingSphere().Diameter() * 0.5f : 0.5f;
		center = (mesh != nullptr) ? mesh->mesh_aabb.CenterPoint() : float3::zero;
	}

	shape = new btSphereShape(1.f);
}

void C_SphereCollider::AdjustShape()
{
	scaled_center = center;
	float scaled_size = radius *  linked_go->transform->scale.Abs().MaxElement();
	shape->setLocalScaling(btVector3(scaled_size, scaled_size  , scaled_size));
}

void C_SphereCollider::SaveCollider(Config& config)
{
	config.AddFloat("radius", radius);
}

void C_SphereCollider::LoadCollider(Config& config)
{
	radius = config.GetFloat("radius", radius);
}

void C_SphereCollider::DrawInfoCollider()
{
	ImGui::Title("Radius", 1);	ImGui::DragFloat("##size", &radius, 0.1f, 0.01f, FLT_MAX);
}
