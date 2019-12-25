#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleRenderer3D.h"
#include "ModuleTime.h"
#include "C_Collider.h"
#include "C_Transform.h"
#include "C_Mesh.h"
#include "GameObject.h"
#include <gl\glew.h>
#include "SDL\include\SDL_opengl.h"

C_Collider::C_Collider(GameObject* go) : Component(go, ComponentType::BOX_COLLIDER)
{
	name.assign("Box Collider");

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
	return true;
}

bool C_Collider::Load(Config& config)
{
	LoadCollider();

	return true;
}

bool C_Collider::Update()
{
	// Load Collider on pdate -------------------------------

	if (body == nullptr)
	{
		LoadCollider();
	}

	if (body == nullptr || shape == nullptr) return true;

	// Static logic -----------------------------------------

	if (linked_go->is_static == true && body->getMass() != 0.f)
	{
		btVector3 innertia;
		shape->calculateLocalInertia(0.f, innertia);
		body->setMassProps(0.f, innertia);
	}

	// Match Size Scalling ----------------------------------

	float3 scaled_size = size.Mul( linked_go->transform->scale.Abs());
	float3 scaled_center = center.Mul(linked_go->transform->scale);

	scaled_size = CheckInvalidCollider(scaled_size);
	shape->setLocalScaling(btVector3(scaled_size.x, scaled_size.y, scaled_size.z));

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

bool C_Collider::DrawPanelInfo()
{
	bool last_is_trigger = is_trigger;

	ImGui::Spacing();
	ImGui::Title("Is Trigger", 1);	ImGui::Checkbox("##is_trigger", &last_is_trigger);
	ImGui::Title("Center", 1);	ImGui::DragFloat3("##center", center.ptr(), 0.1f);
	ImGui::Title("Size", 1);	ImGui::DragFloat3("##size", size.ptr(), 0.1f, 0.01f);
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


void C_Collider::LoadCollider()
{
	float3 position;
	Quat rotation;
	C_Mesh* mesh = linked_go->GetComponent<C_Mesh>();

	if (mesh != nullptr)
	{
		position = linked_go->obb.CenterPoint();
		size = mesh->mesh_aabb.Size();
		center = mesh->mesh_aabb.CenterPoint();
	}
	else
	{
		position = linked_go->transform->position;
	}

	float3 shape_size = float3::one * 0.5f;
	rotation = linked_go->transform->GetQuatRotation();

	shape = new btBoxShape(btVector3(shape_size.x, shape_size.y, shape_size.z));

	btTransform startTransform;
	startTransform.setOrigin(btVector3(position.x, position.y, position.z));
	startTransform.setRotation(btQuaternion(rotation.x, rotation.y, rotation.z, rotation.w));
	btVector3 localInertia(0, 0, 0);

	shape->calculateLocalInertia(1.0f, localInertia);

	motion_state = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(1.0f, motion_state, shape, localInertia);

	body = new btRigidBody(rbInfo);
	body->setUserPointer(linked_go);
	App->physics->AddBody(body);
}

float3 C_Collider::CheckInvalidCollider(float3 size)
{
	return size.Max(float3(0.01, 0.01, 0.01));
}

bool C_Collider::Render()
{
	//if (App->scene->selected_go != linked_go) 
	//	return true;

	if (body == nullptr || shape == nullptr) return true;;

	uint num_edges = shape->getNumEdges();
	btVector3 a, b;
	btTransform transform = body->getCenterOfMassTransform();
	GLfloat ptr[16];
	transform.getOpenGLMatrix(ptr);

	glPushMatrix();
	glMultMatrixf(ptr);
	ModuleRenderer3D::BeginDebugDraw(float4(0.f, 1.f, 0.f, 1.f));
	glBegin(GL_LINES);

	for (uint i = 0; i < num_edges; ++i)
	{
		shape->getEdge(i, a, b);
		glVertex3fv((btScalar*)a);
		glVertex3fv((btScalar*)b);
	}

	glEnd();
	ModuleRenderer3D::EndDebugDraw();
	glPopMatrix();

	//glPointSize(4.f);

	//ModuleRenderer3D::BeginDebugDraw(float4(1.f, 0.f, 0.f, 1.f));
	//glBegin(GL_POINTS);
	//glVertex3fv(&linked_go->transform->position[0]);
	//glEnd();
	//ModuleRenderer3D::EndDebugDraw();

	//glPointSize(7.f);

	//ModuleRenderer3D::BeginDebugDraw(float4(0.f, 0.f, 1.f, 1.f));
	//glBegin(GL_POINTS);
	//btVector3 c = body->getCenterOfMassPosition();
	//glVertex3fv(&c[0]);
	//glEnd();
	//ModuleRenderer3D::EndDebugDraw();

	return true;
}