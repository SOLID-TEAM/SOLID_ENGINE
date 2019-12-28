#include "Application.h"
#include "C_RigidBody.h"
#include "GameObject.h"
#include "ModuleInput.h"
#include "ModuleTime.h"


C_RigidBody::C_RigidBody(GameObject* go) : Component(go, ComponentType::RIGID_BODY)
{
	name.assign("Rigid Body");

	// Set default values --------------------------

	mass = 1.0f;
	drag = 0.f;
	angular_drag = 0.f;

	// Create empty shape --------------------------

	empty_shape = new btEmptyShape();

	// Create Body  --------------------------------

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,  nullptr, empty_shape);
	body = new btRigidBody(rbInfo);
	body->setUserPointer(linked_go);
}

bool C_RigidBody::CleanUp()
{
	if (body_added == true)
	{
		body_added = false;
		App->physics->RemoveBody(body);
	}

	delete body;
	delete empty_shape;

	return true;
}

bool C_RigidBody::Save(Config& config)
{
	config.AddFloat("mass", mass);
	config.AddFloat("drag", drag);
	config.AddFloat("angular_drag", angular_drag);

	config.AddBool("freeze_pos_x", freeze_position[0]);
	config.AddBool("freeze_pos_y", freeze_position[1]);
	config.AddBool("freeze_pos_z", freeze_position[2]);

	config.AddBool("freeze_rot_x", freeze_rotation[0]);
	config.AddBool("freeze_rot_y", freeze_rotation[1]);
	config.AddBool("freeze_rot_z", freeze_rotation[2]); 

	return true;
}

bool C_RigidBody::Load(Config& config)
{
	mass			   = config.GetFloat("mass", mass);

	drag			   = config.GetFloat("drag", drag);
	angular_drag	   = config.GetFloat("angular_drag", angular_drag);

	freeze_position[0] = config.GetBool("freeze_pos_x", freeze_position[0]);
	freeze_position[1] = config.GetBool("freeze_pos_y", freeze_position[1]);
	freeze_position[2] = config.GetBool("freeze_pos_z", freeze_position[2]);

	freeze_rotation[0] = config.GetBool("freeze_rot_x", freeze_rotation[0]);
	freeze_rotation[1] = config.GetBool("freeze_rot_y", freeze_rotation[1]);
	freeze_rotation[2] = config.GetBool("freeze_rot_z", freeze_rotation[2]);

	return true;
}

bool C_RigidBody::Update()
{
	// Only run in game --------------------------

	if (App->time->GetGameState() == GameState::STOP)
		return true;

	// Search Colliders --------------------------

	SearchCollider();
	
	// Update Local Vars  ------------------------

	btCollisionShape* current_shape = (collider != nullptr) ? collider->shape : empty_shape;  // Update Shape 
	float3 go_offset = (collider != nullptr) ? collider->GetWorldCenter() : float3::zero;     // Update Go Offset
	float current_mass = (linked_go->is_static) ? 0.f : mass;							      // Update Mass

	 // Update Inertia
	if (collider != nullptr) 
	{
		current_shape->calculateLocalInertia(current_mass, inertia);
	}
	else
	{
		inertia = btVector3(0.f, 0.f, 0.f);
	}

	body->setMassProps(current_mass, inertia);

	// Add body to world ------------------------

	if (body_added == false)
	{
		SetBodyTranform(linked_go->transform->GetPosition() + go_offset, linked_go->transform->GetQuatRotation());
		App->physics->AddBody(body);
		body_added = true;
	}

	// Update Rigid Body Vars ---------------------

	// Set Is Trigger 
	if (collider != nullptr)
	{
		if (collider->is_trigger == true)
		{
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		}
		else
		{
			body->setCollisionFlags(body->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
		}
	}

	// Set Collision Shape 
	if (body->getCollisionShape() != current_shape)
	{
		body->setCollisionShape(current_shape);
	}

	SetMass(current_mass);
	SetDrag(drag);
	SetAngularDrag(angular_drag);

	if (collider != nullptr)
	{
		SetBouncing(collider->bouncing);
		SetFriction(collider->friction);
		SetAngularFriction(collider->angular_friction);
	}

	btVector3 freeze_p((float)!freeze_position[0], (float)!freeze_position[1], (float)!freeze_position[2]);
	btVector3 freeze_r((float)!freeze_rotation[0], (float)!freeze_rotation[1], (float)!freeze_rotation[2]);

	if (body->getLinearFactor() != freeze_p)
	{
		body->activate(true);
		body->setLinearFactor(freeze_p);
	}
	if (body->getAngularFactor() != freeze_r)
	{
		body->activate(true);
		body->setAngularFactor(freeze_r);
	}

	// Set Go Transform ---------------------------

	btTransform transform = body->getCenterOfMassTransform();
	btQuaternion rotation = transform.getRotation();
	btVector3 position = transform.getOrigin() - ToBtVector3(go_offset);

	linked_go->transform->SetPosition(float3(position));
	linked_go->transform->SetRotation(math::Quat((btScalar*)rotation));

	// Apply Forces ----------------------

	if (!force_to_apply.Equals(float3::zero))
	{
		body->activate(true);
		body->applyCentralImpulse(ToBtVector3(force_to_apply));
		force_to_apply = float3::zero;
	}

	if (!torque_to_apply.Equals(float3::zero))
	{
		body->activate(true);
		body->applyTorqueImpulse(ToBtVector3(torque_to_apply));
		torque_to_apply = float3::zero;
	}


	return true;
}

bool C_RigidBody::Render()
{
	return true;
}

void C_RigidBody::SetMass(float& mass)
{
	if (mass != body->getMass())
	{
		body->setMassProps(mass, inertia);
	}
}

void C_RigidBody::SetDrag(float& drag)
{
	if (drag != body->getLinearDamping())
	{
		body->setDamping(drag, angular_drag);
	}
}

void C_RigidBody::SetAngularDrag(float& angular_drag)
{
	if (angular_drag != body->getAngularDamping())
	{
		body->setDamping(drag, angular_drag);
	}
}

void C_RigidBody::AddForce(const float3& force, Space space)
{
	float3 final_force = force;

	if (space == Space::Local)
	{
		final_force = linked_go->transform->global_transform.RotatePart().Mul(final_force);
	}

	force_to_apply += final_force;
}

void C_RigidBody::AddTorque(const float3& force, Space space)
{
	float3 final_force = force;

	if (space == Space::Local)
	{
		final_force = linked_go->transform->global_transform.RotatePart().Mul(final_force);
	}

	torque_to_apply += final_force;
}

void C_RigidBody::SetBouncing(float& bouncing)
{
	if (bouncing != body->getRestitution())
	{
		float min = 0.f, max = 1.f;
		math::Clamp(&bouncing, &min, &max);
		body->setRestitution(bouncing);
	}
}

void C_RigidBody::SetFriction(float& friction)
{
	if (friction != body->getFriction())
	{
		body->setFriction(friction);
	}
}

void C_RigidBody::SetAngularFriction(float& angular_bouncing)
{
	if (angular_drag != body->getRollingFriction())
	{
		body->setRollingFriction(angular_drag);
	}
}

bool C_RigidBody::DrawPanelInfo()
{
	ImGui::Spacing();

	ImGui::Title("Mass", 1);			ImGui::DragFloat("##mass", &mass, 0.01f, 0.00f, FLT_MAX);
	ImGui::Title("Drag", 1);			ImGui::DragFloat("##drag", &drag, 0.01f, 0.00f, FLT_MAX);
	ImGui::Title("Angular Drag", 1);	ImGui::DragFloat("##angular_drag", &angular_drag, 0.01f, 0.00f, FLT_MAX);

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

	return true;
}

void C_RigidBody::CreateBody()
{
	float3 position;
	Quat rotation;
	C_Mesh* mesh = linked_go->GetComponent<C_Mesh>();

	if (mesh != nullptr)
	{
		position = linked_go->obb.CenterPoint();
	}
	else
	{
		position = linked_go->transform->position;
	}

	rotation = linked_go->transform->GetQuatRotation();


}

void C_RigidBody::SetBodyTranform(const float3& pos, const Quat& rot)
{
	body->setWorldTransform( ToBtTransform(pos, rot) );
}


void C_RigidBody::SearchCollider()
{
	C_Collider* collider = linked_go->GetComponent<C_Collider>();
	
	if (collider != nullptr && collider->shape != nullptr)
	{
		this->collider = collider;
	}
		
}