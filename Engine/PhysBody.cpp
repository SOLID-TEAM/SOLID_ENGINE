#include "PhysBody.h"
#include "glmath.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

// =================================================
PhysBody::PhysBody(btRigidBody* body) : body(body)
{}

// ---------------------------------------------------------
PhysBody::~PhysBody()
{
	delete body;
}

// ---------------------------------------------------------
void PhysBody::Push(float x, float y, float z)
{
	body->applyCentralImpulse(btVector3(x, y, z));
}

// ---------------------------------------------------------
void PhysBody::GetTransform(float* matrix) const
{
	if (body != NULL && matrix != NULL)
	{
		body->getWorldTransform().getOpenGLMatrix(matrix);
	}
}

// ---------------------------------------------------------
void PhysBody::SetTransform(const float* matrix) const
{
	if (body != NULL && matrix != NULL)
	{
		btTransform t;
		t.setFromOpenGLMatrix(matrix);
		body->setWorldTransform(t);
	}
}

// ---------------------------------------------------------
void PhysBody::SetPos(float x, float y, float z)
{
	btTransform t = body->getWorldTransform();
	t.setOrigin(btVector3(x, y, z));
	body->setWorldTransform(t);
}
