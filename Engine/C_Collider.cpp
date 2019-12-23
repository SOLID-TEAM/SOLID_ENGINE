#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleRenderer3D.h"
#include "C_Collider.h"
#include "C_Transform.h"
#include "C_Mesh.h"
#include "GameObject.h"
#include <gl\glew.h>
#include "SDL\include\SDL_opengl.h"

C_Collider::C_Collider(GameObject* go) : Component(go, ComponentType::BOX_COLLIDER)
{
	name.assign("Box Collider");

	float3 size(1.f, 1.f, 1.f);
	C_Mesh* mesh = linked_go->GetComponent<C_Mesh>();

	if (mesh)
	{
		size = mesh->mesh_aabb.Size() * 0.5F;
	}

	shape = new btBoxShape(btVector3(size.x, size.y, size.z));

	btTransform startTransform;
	startTransform.setFromOpenGLMatrix((btScalar*)&go->transform->GetGlobalTransform().Transposed());
	btVector3 localInertia(0, 0, 0);
	
	shape->calculateLocalInertia(1.0f, localInertia);

	motion_state = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(1.0f, motion_state, shape, localInertia);

	body = new btRigidBody(rbInfo);
	body->setUserPointer(linked_go);

	App->physics->AddBody(body);
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
	
	return true;
}

bool C_Collider::Update()
{
	btTransform transform = body->getCenterOfMassTransform();
	btVector3 position = transform.getOrigin();
	btQuaternion rotation = transform.getRotation();

	linked_go->transform->SetPosition(float3((btScalar*)position));
	linked_go->transform->SetRotation(math::Quat((btScalar*)rotation));

	return true;
}

bool C_Collider::Render()
{
	//if (App->scene->selected_go != linked_go) 
	//	return true;

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

	glPointSize(3.f);

	ModuleRenderer3D::BeginDebugDraw(float4(1.f, 0.f, 0.f, 1.f));
	glBegin(GL_POINTS);
	glVertex3fv(&linked_go->transform->position[0]);
	glEnd();
	ModuleRenderer3D::EndDebugDraw();

	ModuleRenderer3D::BeginDebugDraw(float4(0.f, 0.f, 1.f, 1.f));
	glBegin(GL_POINTS);
	btVector3 c = body->getCenterOfMassPosition();
	glVertex3fv(&c[0]);
	glEnd();
	ModuleRenderer3D::EndDebugDraw();

	return true;
}

bool C_Collider::DrawPanelInfo()
{
	return true;
}
