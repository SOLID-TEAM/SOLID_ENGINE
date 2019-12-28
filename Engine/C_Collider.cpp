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
	config.AddFloatArray("center", (float*)&center, 3);
	config.AddBool("is_trigger", is_trigger);

	SaveCollider(config);
	return true;
}

bool C_Collider::Load(Config& config)
{
	is_loaded = true;
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

		if (is_loaded == true)
		{
			center = (mesh != nullptr) ? mesh->mesh_aabb.CenterPoint() : float3::zero;
		}
		
		CreateShape(mesh);
		aux_body->setCollisionShape(shape);
	}

	// Match Size Scalling ----------------------------------

	AdjustShape();

	// Body Logic -------------------------------------------

	scaled_center = linked_go->transform->GetGlobalTransform().RotatePart().MulPos(scaled_center);
	btTransform transform = ToBtTransform(linked_go->transform->GetPosition() + scaled_center, linked_go->transform->GetQuatRotation());
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

	ImGui::Title("Is Trigger", 1);	ImGui::Checkbox("##is_trigger", &last_is_trigger);
	ImGui::Title("Center", 1);	ImGui::DragFloat3("##center", center.ptr(), 0.1f);

	// Draw Collider Adiional Info ---------------

	DrawInfoCollider();

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

bool C_Collider::GetIsTrigger()
{
	return is_trigger;
}

bool C_Collider::SearchRigidBody()
{
	return (linked_go->GetComponent<C_RigidBody>() == nullptr) ? false : true;
}
