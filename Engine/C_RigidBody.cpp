#include "C_RigidBody.h"

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
	return false;
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
