#ifndef _MODULE_PHYSICS_H__
#define _MODULE_PHYSICS_H__

#include "Module.h"
#include "Globals.h"
#include "PhysBody.h"
#include "PhysVehicle.h"
#include "Bullet/include/btBulletDynamicsCommon.h"
#include "external/MathGeoLib/include/Math/MathAll.h"
#include <list>

// Recommended scale is 1.0f == 1 meter, no less than 0.2 objects
#define GRAVITY btVector3(0.0f, -9.8f, 0.0f) 

class Collider{};

struct Collision
{
	Collider collider;
	GameObject* go;

};

class ModulePhysics : public Module
{
public:
	ModulePhysics( bool start_enabled = true);
	~ModulePhysics();

	bool Init(Config& config);
	bool Start(Config& config);
	update_status PreUpdate();
	//update_status Update();
	update_status PostUpdate();
	update_status Draw();
	bool CleanUp();

	void AddBody(btRigidBody* body);
	void RemoveBody(btRigidBody* body);

	//RigidBody* AddBody(const Cylinder& cylinder, float mass = 1.0f);
	//PhysVehicle* AddVehicle(const VehicleInfo& info);

	void AddConstraintPointToPoint(PhysBody& bodyA, PhysBody& bodyB, const vec3& anchorA, const vec3& anchorB);
	void AddConstraintHinge(PhysBody& bodyA, PhysBody& bodyB, const vec3& anchorA, const vec3& anchorB, const vec3& axisS, const vec3& axisB, bool disable_collision = false);

private:

	bool debug;

	btDefaultCollisionConfiguration* collision_config;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* broad_phase;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* world;
	btDefaultVehicleRaycaster* vehicle_raycaster;

	std::list<btCollisionShape*> shapes;
	std::list<PhysBody*> bodies;
	std::list<btDefaultMotionState*> motions;
	std::list<btTypedConstraint*> constraints;
	std::list<PhysVehicle*> vehicles;
};


class DebugRenderer : public btIDebugDraw
{
public:
	DebugRenderer()
	{}

	void drawLine(const btVector3& from, const btVector3& to, const btVector3& color);
	void drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color);
	void reportErrorWarning(const char* warningString);
	void draw3dText(const btVector3& location, const char* textString);
	void setDebugMode(int debugMode);
	int	 getDebugMode() const;

	DebugDrawModes mode;
};

#endif // !_MODULE_PHYSICS_H__

