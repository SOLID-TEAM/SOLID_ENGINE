#include "Application.h"
#include "C_RigidBody.h"
#include "GameObject.h"
#include "ModuleInput.h"
#include "ModuleTime.h"


C_RigidBody::C_RigidBody(GameObject* go) : Component(go, ComponentType::RIGID_BODY)
{
	name.assign("Rigid Body");

	for (uint i = 0; i < (uint)ForceMode::MAX; ++i)
	{
		force_to_apply[i] = float3::zero;
		torque_to_apply[i] = float3::zero;
	}

	// Set default values --------------------------

	mass = 1.0f;
	drag = 0.f;
	angular_drag = 0.f;

	// Create empty shape --------------------------

	aux_shape = new btBoxShape(btVector3(1.f,1.f,1.f));

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, nullptr, aux_shape);
	body = new btRigidBody(rbInfo);
	body->setUserPointer(linked_go);

	SetBodyTranform(linked_go->transform->GetPosition(), linked_go->transform->GetQuatRotation() );
	App->physics->AddBody(body);
}

bool C_RigidBody::CleanUp()
{
	for (int i = 0; i < body->getNumConstraintRefs(); ++i)
	{
		C_JointP2P * joint =(C_JointP2P *) body->getConstraintRef(i)->getUserConstraintPtr();
		joint->BodyDeleted(linked_go);
		body->removeConstraintRef(body->getConstraintRef(i));
	}
		
	App->physics->RemoveBody(body);

	delete body;
	delete aux_shape;

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
	// Search Colliders --------------------------

	SearchCollider();

	// Update Local Vars  ------------------------

	btCollisionShape* current_shape = (collider != nullptr) ? collider->shape : aux_shape;  // Update Shape 
	float3 go_offset = (collider != nullptr) ? collider->GetWorldCenter() : float3::zero;     // Update Go Offset
	float current_mass = (linked_go->is_static) ? 0.f : mass;								  // Update Mass

	 // Update Inertia ---------------------------

	current_shape->calculateLocalInertia(current_mass, inertia);
	body->setMassProps(current_mass, inertia);

	// Update Rigid Body Vars ---------------------

	// Set Is Trigger 
	if (collider)
	{
		if (collider->is_trigger)
			body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		else
			body->setCollisionFlags(body->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
	else
	{
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
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

	if (App->time->GetGameState() == GameState::STOP)
	{
		SetBodyTranform(linked_go->transform->GetPosition() + go_offset, linked_go->transform->GetQuatRotation());
	}
	else
	{
		btTransform transform = body->getCenterOfMassTransform();
		btQuaternion rotation = transform.getRotation();
		btVector3 position = transform.getOrigin() - ToBtVector3(go_offset);

		body->activate(true);
		linked_go->transform->SetPosition(float3(position));
		linked_go->transform->SetRotation(math::Quat((btScalar*)rotation));

		// Apply Forces ----------------------

		for( uint i = 0; i < (uint) ForceMode::MAX ; ++i)
		{
			if (!force_to_apply[i].Equals(float3::zero))
			{
				switch ((ForceMode)i)
				{
				case ForceMode::FORCE:
					body->applyCentralForce(ToBtVector3(force_to_apply[i]));
					break;
				case ForceMode::IMPULSE:
					body->applyCentralImpulse(ToBtVector3(force_to_apply[i]));
					break;
				}
				force_to_apply[i] = float3::zero;
			}
		}
	
		for (uint i = 0; i < (uint)ForceMode::MAX; ++i)
		{
			if (!torque_to_apply[i].Equals(float3::zero))
			{
				switch ((ForceMode)i)
				{
				case ForceMode::FORCE:
					body->applyTorque(ToBtVector3(torque_to_apply[i]));
					break;
				case ForceMode::IMPULSE:
					body->applyTorqueImpulse(ToBtVector3(torque_to_apply[i]));
					break;
				}
				
				torque_to_apply[i] = float3::zero;
			}
		}

	}


	return true;
}

float3 C_RigidBody::GetVelocity()
{
	return float3(&body->getLinearVelocity()[0]);
}

void C_RigidBody::SetVelocity(float3 velocity)
{
	body->setLinearVelocity(ToBtVector3(velocity));
}

float3 C_RigidBody::GetAngularVelocity()
{
	return float3(&body->getAngularVelocity()[0]);
}

void C_RigidBody::SetAngularVelocity(float3 velocity)
{
	body->setAngularVelocity(ToBtVector3(velocity));
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

void C_RigidBody::AddForce(const float3& force, ForceMode mode, Space space)
{
	float3 final_force = force;

	if (space == Space::Local)
	{
		final_force = linked_go->transform->global_transform.RotatePart().Mul(final_force);
	}

	force_to_apply[(uint)mode] += final_force;
}

void C_RigidBody::AddTorque(const float3& force, ForceMode mode, Space space)
{
	float3 final_force = force;

	if (space == Space::Local)
	{
		final_force = linked_go->transform->global_transform.RotatePart().Mul(final_force);
	}

	torque_to_apply[(uint)mode] += final_force;
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