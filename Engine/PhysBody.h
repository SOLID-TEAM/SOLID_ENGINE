#ifndef __PhysBody_H__
#define __PhysBody_H__

#include "Bullet/include/btBulletDynamicsCommon.h"
#include <list>

class Module;

class PhysBody
{
	friend class ModulePhysics;

public:

	PhysBody(btRigidBody* body);

	~PhysBody();

	void Push(float x, float y, float z);

	void GetTransform(float* matrix) const;

	void SetTransform(const float* matrix) const;

	void SetPos(float x, float y, float z);

private:

	btRigidBody* body = nullptr;

public:

	std::list<Module*> listeners;
};

#endif // __PhysBody3D_H__
