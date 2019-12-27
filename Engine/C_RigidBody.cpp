#include "C_RigidBody.h"
#include "Application.h"
#include "GameObject.h"
#include "ModuleInput.h"

C_RigidBody::C_RigidBody(GameObject* go) : Component(go, ComponentType::RIGID_BODY)
{
	name.assign("Rigid Body");
}

bool C_RigidBody::CleanUp()
{
	return false;
}

bool C_RigidBody::Save(Config& config)
{
	return false;
}

bool C_RigidBody::Load(Config& config)
{
	return false;
}

bool C_RigidBody::Update()
{
	if (body == nullptr)
	{
		SearchCollider();
	}

	if (body == nullptr) return true;

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		AddForce(float3(linked_go->transform->forward));

	return true;
}

bool C_RigidBody::Render()
{
	return false;
}

bool C_RigidBody::DrawPanelInfo()
{
	return false;
}

void C_RigidBody::AddForce(const float3& force)
{
	if (body == nullptr) return;

	body->applyCentralImpulse(btVector3(force.x, force.y, force.z));
}

void C_RigidBody::AddTorque(const float3& force)
{
	if (body == nullptr) return;

	body->applyTorqueImpulse(btVector3(force.x, force.y, force.z));
}

void C_RigidBody::SearchCollider()
{
	C_Collider* collider = linked_go->GetComponent<C_Collider>();
	
	if (collider != nullptr && collider->body != nullptr)
		body = collider->body;
}