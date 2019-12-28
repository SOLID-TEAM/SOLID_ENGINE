#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleRenderer3D.h"
#include "ModuleTime.h"
#include "C_Collider.h"
#include "C_Transform.h"
#include "C_Mesh.h"
#include "GameObject.h"

C_Collider::C_Collider(GameObject* go) : Component(go, ComponentType::NO_TYPE)
{
	// Default values ---------------------------

	bouncing = 0.1f;
	friction = 0.5f;
	angular_friction = 0.1f;

	center = float3::zero;

	// Create aux body --------------------------

	btRigidBody::btRigidBodyConstructionInfo rbInfo(0.F, nullptr, nullptr);
	aux_body = new btRigidBody(rbInfo);
	aux_body->setUserPointer(linked_go);
}

bool C_Collider::CleanUp()
{
	for (int i = 0; i < aux_body->getNumConstraintRefs(); ++i)
		aux_body->removeConstraintRef(aux_body->getConstraintRef(i));

	App->physics->RemoveBody(aux_body);

	delete shape;
	delete aux_motion_state;
	delete aux_body;

	return true;
}

bool C_Collider::Save(Config& config)
{
	config.AddFloat("bouncing", bouncing);
	config.AddFloat("friction", friction);
	config.AddFloat("angular_friction", angular_friction);

	config.AddFloatArray("center", (float*)&center, 3);
	config.AddBool("is_trigger", is_trigger);

	SaveCollider(config);
	return true;
}

bool C_Collider::Load(Config& config)
{
	is_loaded = true;

	bouncing = config.GetFloat("bouncing", bouncing);
	friction = config.GetFloat("friction", friction);
	angular_friction = config.GetFloat("angular_friction", angular_friction);

	center = config.GetFloat3("center", { 0.f ,0.f, 0.f });
	is_trigger = config.GetBool("is_trigger", false);
	SetIsTrigger(is_trigger);
	LoadCollider(config);

	return true;
}

bool C_Collider::Update()
{
	// Load Collider on pdate -------------------------------

	if (shape == nullptr)
	{
		C_Mesh* mesh = linked_go->GetComponent<C_Mesh>();
		
		CreateShape(mesh);
		aux_body->setCollisionShape(shape);
	}

	// Match Size Scalling ----------------------------------

	AdjustShape();

	// Body Logic -------------------------------------------

	float3 world_center = GetWorldCenter();
	btTransform transform = ToBtTransform(linked_go->transform->GetPosition() + world_center, linked_go->transform->GetQuatRotation());
	aux_body->setWorldTransform(transform);

	if (App->time->GetGameState() == GameState::STOP) return true;

	if (SearchRigidBody())
	{
		if (body_added == true)
		{
			App->physics->RemoveBody(aux_body);
			body_added = false;
		}
	}
	else
	{
		if (body_added == false)
		{
			App->physics->AddBody(aux_body);
			body_added = true;
		}

		SetBouncing(bouncing);
		SetFriction(friction);
		SetAngularFriction(angular_friction);
	}

	return true;
}

bool C_Collider::Render()
{
	if (shape == nullptr) return true;

	if (App->scene->selected_go == linked_go && App->scene->editor_mode)
	{
		App->physics->RenderCollider(this);
	}

	return true;
}

bool C_Collider::DrawPanelInfo()
{
	bool last_is_trigger = is_trigger;

	ImGui::Spacing();

	ImGui::Title("Is Trigger", 1);		ImGui::Checkbox("##is_trigger", &last_is_trigger);
	ImGui::Title("Center", 1);			ImGui::DragFloat3("##center", center.ptr(), 0.1f);

	// Draw Collider Adiional Info ---------------

	DrawInfoCollider();


	ImGui::Spacing();
	ImGui::Title("Physic Material", 1); ImGui::Text("");
	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Title("Bouncing", 2);		ImGui::DragFloat("##bouncing", &bouncing, 0.01f, 0.00f, 1.f);
	ImGui::Title("Linear Fric.", 2);	ImGui::DragFloat("##friction", &friction, 0.01f, 0.00f, FLT_MAX);
	ImGui::Title("Angular Fric.", 2);	ImGui::DragFloat("##angular_friction", &angular_friction, 0.01f, 0.00f, FLT_MAX);

	ImGui::Spacing();

	if (last_is_trigger != is_trigger)
	{
		SetIsTrigger(last_is_trigger);
	}

	return true;
}

void C_Collider::SetIsTrigger(bool value)
{
	if (value == true)
	{
		aux_body->setCollisionFlags(aux_body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
	else
	{
		aux_body->setCollisionFlags(aux_body->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}

	is_trigger = value;
}


void C_Collider::SetBouncing(float& bouncing)
{
	if (bouncing != aux_body->getRestitution())
	{
		float min = 0.f, max = 1.f;
		math::Clamp(&bouncing, &min, &max);
		aux_body->setRestitution(bouncing);
	}
}

void C_Collider::SetFriction(float& friction)
{
	if (friction != aux_body->getFriction())
	{
		aux_body->setFriction(friction);
	}
}

void C_Collider::SetAngularFriction(float& angular_drag)
{
	if (angular_drag != aux_body->getRollingFriction())
	{
		aux_body->setRollingFriction(angular_drag);
	}
}

bool C_Collider::GetIsTrigger()
{
	return is_trigger;
}

float3 C_Collider::GetWorldCenter()
{
	return linked_go->transform->GetGlobalTransform().RotatePart().MulPos(scaled_center);
}

bool C_Collider::SearchRigidBody()
{
	return (linked_go->GetComponent<C_RigidBody>() == nullptr) ? false : true;
}
