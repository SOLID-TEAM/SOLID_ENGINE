#include "C_RigidBody.h"
#include "Application.h"
#include "GameObject.h"
#include "ModuleInput.h"

C_RigidBody::C_RigidBody(GameObject* go) : Component(go, ComponentType::RIGID_BODY)
{
	name.assign("Rigid Body");

	mass = 1.0f;
	bouncing = 0.f;
	drag = 0.f;
	angular_drag = 0.f;
	friction = 0.5f;
	angular_friction = 0.1f;
}

bool C_RigidBody::CleanUp()
{
	return false;
}

bool C_RigidBody::Save(Config& config)
{
	config.AddFloat("mass", mass);
	config.AddFloat("bouncing", bouncing);
	config.AddFloat("drag", drag);
	config.AddFloat("angular_drag", angular_drag);
	config.AddFloat("friction", friction);
	config.AddFloat("angular_friction", angular_friction);
	return true;
}

bool C_RigidBody::Load(Config& config)
{
	mass			 = config.GetFloat("mass", mass);
	bouncing		 = config.GetFloat("bouncing", bouncing);
	drag			 = config.GetFloat("drag", drag);
	angular_drag	 = config.GetFloat("angular_drag", angular_drag);
	friction		 = config.GetFloat("friction", friction);
	angular_friction = config.GetFloat("angular_friction", angular_friction);

	return true;
}

bool C_RigidBody::Update()
{
	if (body == nullptr)
	{
		SearchCollider();
	}

	if (body == nullptr) return true;

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		AddForce(float3(linked_go->transform->forward * 10.F));

	if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
		AddTorque(float3(linked_go->transform->forward * 2.F));



	return true;
}

bool C_RigidBody::Render()
{
	return true;
}

void C_RigidBody::AddForce(const float3& force)
{
	if (body == nullptr) return;
	body->activate(true);
	body->applyCentralImpulse(btVector3(force.x, force.y, force.z));
}

void C_RigidBody::AddTorque(const float3& force)
{
	if (body == nullptr) return;
	body->activate(true);
	body->applyTorqueImpulse(btVector3(force.x, force.y, force.z));
}

bool C_RigidBody::DrawPanelInfo()
{
	float aux_mass = mass;
	float aux_bouncing = bouncing;
	float aux_drag = drag;
	float aux_angular_drag = angular_drag;
	float aux_friction = friction;
	float aux_angular_friction = angular_friction;

	ImGui::Spacing();

	ImGui::Title("Mass", 1);			ImGui::DragFloat("##mass", &aux_mass, 0.1f, 0.00f, FLT_MAX);
	ImGui::Title("Bouncing", 1);		ImGui::DragFloat("##bouncing", &aux_bouncing, 0.1f, 0.00f, FLT_MAX);
	ImGui::Title("Drag", 1);			ImGui::DragFloat("##drag", &aux_drag, 0.1f, 0.00f, FLT_MAX);
	ImGui::Title("Angular Drag", 1);	ImGui::DragFloat("##angular_drag", &aux_angular_drag, 0.1f, 0.00f, FLT_MAX);
	ImGui::Title("Friction", 1);		ImGui::DragFloat("##friction", &aux_friction, 0.1f, 0.00f, FLT_MAX);
	ImGui::Title("Angular Friction",1);	ImGui::DragFloat("##angular_friction", &aux_angular_friction, 0.1f, 0.00f, FLT_MAX);

	ImGui::Title("Freeze", 1);	ImGui::Text("");
	ImGui::Spacing();
	ImGui::PushID(freeze_position);
	ImGui::Title("Position", 2);	
	ImGui::Checkbox("X", &freeze_position[0]); ImGui::SameLine();
	ImGui::Checkbox("Y", &freeze_position[1]); ImGui::SameLine();
	ImGui::Checkbox("Z", &freeze_position[2]); 
	ImGui::PopID();
	ImGui::PushID(freeze_rotation);
	ImGui::Title("Rotation", 2);	
	ImGui::Checkbox("X", &freeze_rotation[0]); ImGui::SameLine();
	ImGui::Checkbox("Y", &freeze_rotation[1]); ImGui::SameLine();
	ImGui::Checkbox("Z", &freeze_rotation[2]);
	ImGui::PopID();

	ImGui::Spacing();

	// Vars --------------------------------------

	if (body == nullptr) return true;

	static bool test = false;

	if (test == false)
	{
		LOG("Mass				: %f", body->getMass());
		LOG("Bouncing			: %f", body->getRestitution());
		LOG("Drag				: %f", body->getLinearDamping());
		LOG("Angular Drag		: %f", body->getAngularDamping());
		LOG("Friction			: %f", body->getFriction());
		LOG("Angular Friction	: %f", body->getRollingFriction());
		test = true;
	}


	SetMass(aux_mass);
	SetBouncing(aux_bouncing);
	SetDrag(aux_drag);
	SetAngularDrag(aux_angular_drag);
	SetFriction(aux_friction);
	SetAngularFriction(aux_angular_friction);


	// Freeze ------------------------------------ 

	btVector3 freeze_p((float)!freeze_position[0], (float)!freeze_position[1], (float)!freeze_position[2]);
	btVector3 freeze_r((float)!freeze_rotation[0], (float)!freeze_rotation[1], (float)!freeze_rotation[2]);

	if (body->getLinearFactor() != freeze_p)
	{
		body->setLinearFactor(freeze_p);
	}
	if (body->getAngularFactor() != freeze_r)
	{
		body->setAngularFactor(freeze_r);
	}

	return true;
}

void C_RigidBody::SetMass(float v)
{
	if (mass != v)
	{
		mass = v;
		body->setMassProps(mass,  collider->UpdateInertia(mass));
	}
}

void C_RigidBody::SetBouncing(float v)
{
	if (bouncing != v)
	{
		float min = 0.f, max = 1.f;
		math::Clamp(&v, &min, &max);
		bouncing = v;
		body->setRestitution(bouncing);
	}
}

void C_RigidBody::SetDrag(float v)
{
	if (drag != v)
	{
		drag = v;
		body->setDamping(drag, angular_drag);
	}
}

void C_RigidBody::SetAngularDrag(float v)
{
	if (angular_drag != v)
	{
		angular_drag = v;
		body->setDamping(drag, angular_drag);
	}
}

void C_RigidBody::SetFriction(float v)
{
	if (friction != v)
	{
		friction = v;
		body->setFriction(friction);
	}
}

void C_RigidBody::SetAngularFriction(float v)
{
	if (angular_drag != v)
	{
		angular_drag = v;
		body->setRollingFriction(angular_drag);
	}
}

void C_RigidBody::SearchCollider()
{
	C_Collider* collider = linked_go->GetComponent<C_Collider>();
	
	if (collider != nullptr && collider->body != nullptr)
	{
		body = collider->body;
		this->collider = collider;

		SetMass(mass);
		SetBouncing(bouncing);
		SetDrag(drag);
		SetAngularDrag(angular_drag);
		SetFriction(friction);
		SetAngularFriction(angular_friction);

	}
		
}