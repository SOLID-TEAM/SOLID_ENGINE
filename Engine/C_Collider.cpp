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
}

bool C_Collider::CleanUp()
{
	App->physics->RemoveBody(body);
	delete shape;
	delete motion_state;
	delete body;

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
	CreateCollider();

	center = config.GetFloat3("center", { 0.f ,0.f, 0.f });
	is_trigger = config.GetBool("is_trigger", false);

	LoadCollider(config);
	return true;
}

bool C_Collider::Update()
{
	// Load Collider on pdate -------------------------------

	if (body == nullptr)
	{
		CreateCollider();
	}

	if (body == nullptr || shape == nullptr) return true;

	// Static logic -----------------------------------------

	if (linked_go->is_static == true && body->getMass() != 0.f)
	{
		shape->calculateLocalInertia(0.f, local_inertia);
		body->setMassProps(0.f, local_inertia);
	}

	// Match Size Scalling ----------------------------------

	AdjustShape();

	// Set Transform ------------------------------------------

	if (App->time->GetGameState() == GameState::RUN)
	{
		btTransform transform = body->getCenterOfMassTransform();
		btVector3 position = transform.getOrigin();
		btMatrix3x3 rotation = transform.getBasis();
		btQuaternion quat_rotation = transform.getRotation();
	
		linked_go->transform->SetPosition(float3(position - rotation * ToBtVector3(scaled_center)));
		linked_go->transform->SetRotation(math::Quat((btScalar*)quat_rotation));
	}
	else
	{
		btTransform transform;
		scaled_center = linked_go->transform->GetGlobalTransform().RotatePart().MulPos(scaled_center);
		transform.setOrigin(ToBtVector3(linked_go->transform->GetPosition() + scaled_center));
		transform.setRotation(ToBtQuaternion(linked_go->transform->GetQuatRotation()));
		body->setWorldTransform(transform);
	}

	return true;
}

bool C_Collider::Render()
{
	if (body == nullptr || shape == nullptr) return true;

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

btVector3 C_Collider::UpdateInertia(float mass)
{
	if (body == nullptr || shape == nullptr) return btVector3(0.f, 0.f, 0.f);
	
	shape->calculateLocalInertia(mass, local_inertia);
	return local_inertia;
}

void C_Collider::SetIsTrigger(bool value)
{
	if (value == true)
	{
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}
	else
	{
		body->setCollisionFlags(body->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
	}

	is_trigger = value;
}

bool C_Collider::GetIsTrigger()
{
	return is_trigger;
}

void C_Collider::CreateCollider()
{
	float3 position;
	Quat rotation;
	C_Mesh* mesh = linked_go->GetComponent<C_Mesh>();

	if (mesh != nullptr)
	{
		position = linked_go->obb.CenterPoint();
		center = mesh->mesh_aabb.CenterPoint();
	}
	else
	{
		position = linked_go->transform->position;		
		center = float3::zero;
	}

	rotation = linked_go->transform->GetQuatRotation();

	// Create specific shape -------------------------------------

	CreateShape(mesh);
	shape->calculateLocalInertia(1.0f, local_inertia);

	// Create Body & Motion State --------------------------------

	btTransform startTransform;
	startTransform.setOrigin(btVector3(position.x, position.y, position.z));
	startTransform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
	motion_state = new btDefaultMotionState(startTransform);

	btRigidBody::btRigidBodyConstructionInfo rbInfo(1.0f, motion_state, shape, local_inertia);

	body = new btRigidBody(rbInfo);
	body->setUserPointer(linked_go);
	App->physics->AddBody(body);
}
