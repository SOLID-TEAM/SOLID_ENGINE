#include "Globals.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleTime.h"
#include "Primitive.h"
#include "Bullet/include/btBulletDynamicsCommon.h"
#include "PhysBody.h"
#include "PhysVehicle.h"
#include "GL/glew.h"

ModulePhysics::ModulePhysics(bool start_enabled) : Module(start_enabled)
{

}

// Destructor
ModulePhysics::~ModulePhysics()
{
	delete solver;
	delete broad_phase;
	delete dispatcher;
	delete collision_config;
}

// Render not available yet----------------------------------
bool ModulePhysics::Init(Config& config)
{
	LOG("Creating 3D Physics simulation");

	bool ret = true;
	collision_config = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collision_config);
	broad_phase = new btDbvtBroadphase();
	solver = new btSequentialImpulseConstraintSolver();
	return ret;
}

// ---------------------------------------------------------
bool ModulePhysics::Start(Config& config)
{
	LOG("Creating Physics environment");

	world = new btDiscreteDynamicsWorld(dispatcher, broad_phase, solver, collision_config);
	world->setGravity(GRAVITY);
	vehicle_raycaster = new btDefaultVehicleRaycaster(world);

	return true;
}

// ---------------------------------------------------------
update_status ModulePhysics::PreUpdate()
{
	float dt = App->time->DeltaTime();
	if (dt != 0)
	{
		world->stepSimulation(App->time->DeltaTime(), 15);
	}

	int numManifolds = world->getDispatcher()->getNumManifolds();

	//for (int i = 0; i < numManifolds; i++)
	//{
	//	btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
	//	btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
	//	btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

	//	int numContacts = contactManifold->getNumContacts();

	//	if (numContacts > 0)
	//	{
	//		PhysBody* pbodyA = (PhysBody*)obA->getUserPointer();
	//		PhysBody* pbodyB = (PhysBody*)obB->getUserPointer();

	//		if (pbodyA && pbodyB)
	//		{

	//			for (Module* modules : pbodyA->listeners)
	//			{
	//				// TODO:: Add oncollision
	//				//modules->OnCollision(pbodyA, pbodyB);
	//			}

	//			for (Module* modules : pbodyB->listeners)
	//			{
	//				//modules->OnCollision(pbodyB, pbodyA);
	//			}
	//
	//		}
	//	}
	//}

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics::Draw()
{

	return UPDATE_CONTINUE;
}

// ---------------------------------------------------------
update_status ModulePhysics::PostUpdate()
{
	return UPDATE_CONTINUE;
}

bool ModulePhysics::CleanUp()
{
	LOG("Destroying 3D Physics simulation");

	for (int i = world->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		world->removeCollisionObject(obj);
	}

	delete vehicle_raycaster;
	delete world;

	return true;
}

void ModulePhysics::AddBody(btRigidBody* body)
{
	world->addRigidBody(body);
}

void ModulePhysics::RemoveBody(btRigidBody* body)
{
	world->removeRigidBody(body);
}

//RigidBody* ModulePhysics::AddBody(const Sphere& sphere, float mass)
//{
//	btCollisionShape* colShape = new btSphereShape(sphere.radius);
//	shapes.add(colShape);
//
//	btTransform startTransform;
//	startTransform.setFromOpenGLMatrix(&sphere.transform);
//
//	btVector3 localInertia(0, 0, 0);
//	if (mass != 0.f)
//		colShape->calculateLocalInertia(mass, localInertia);
//
//	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
//	motions.add(myMotionState);
//	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
//
//	btRigidBody* body = new btRigidBody(rbInfo);
//	RigidBody* pbody = new RigidBody(body);
//
//	body->setUserPointer(pbody);
//	world->addRigidBody(body);
//	bodies.add(pbody);
//
//	return pbody;
//}
//
//
//RigidBody* ModulePhysics::AddBody(const Cylinder& cylinder, float mass)
//{
//	btCollisionShape* colShape = new btCylinderShapeX(btVector3(cylinder.height * 0.5f, cylinder.radius, 0.0f));
//	shapes.add(colShape);
//
//	btTransform startTransform;
//	startTransform.setFromOpenGLMatrix(&cylinder.transform);
//
//	btVector3 localInertia(0, 0, 0);
//	if (mass != 0.f)
//		colShape->calculateLocalInertia(mass, localInertia);
//
//	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
//	motions.add(myMotionState);
//	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
//
//	btRigidBody* body = new btRigidBody(rbInfo);
//	RigidBody* pbody = new RigidBody(body);
//
//	body->setUserPointer(pbody);
//	world->addRigidBody(body);
//	bodies.add(pbody);
//
//	return pbody;
//}
//
//PhysVehicle* ModulePhysics::AddVehicle(const VehicleInfo& info)
//{
//	btCompoundShape* comShape = new btCompoundShape();
//	shapes.push_back(comShape);
//
//	btCollisionShape* colShape = new btBoxShape(btVector3(info.chassis_size.x * 0.5f, info.chassis_size.y * 0.5f, info.chassis_size.z * 0.5f));
//	shapes.push_back(colShape);
//
//	btTransform trans;
//	trans.setIdentity();
//	trans.setOrigin(btVector3(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z));
//
//	comShape->addChildShape(trans, colShape);
//
//	btTransform startTransform;
//	startTransform.setIdentity();
//
//	btVector3 localInertia(0, 0, 0);
//	comShape->calculateLocalInertia(info.mass, localInertia);
//
//	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
//	btRigidBody::btRigidBodyConstructionInfo rbInfo(info.mass, myMotionState, comShape, localInertia);
//
//	btRigidBody* body = new btRigidBody(rbInfo);
//	body->setContactProcessingThreshold(BT_LARGE_FLOAT);
//	body->setActivationState(DISABLE_DEACTIVATION);
//
//	world->addRigidBody(body);
//
//	btRaycastVehicle::btVehicleTuning tuning;
//	tuning.m_frictionSlip = info.frictionSlip;
//	tuning.m_maxSuspensionForce = info.maxSuspensionForce;
//	tuning.m_maxSuspensionTravelCm = info.maxSuspensionTravelCm;
//	tuning.m_suspensionCompression = info.suspensionCompression;
//	tuning.m_suspensionDamping = info.suspensionDamping;
//	tuning.m_suspensionStiffness = info.suspensionStiffness;
//
//	btRaycastVehicle* vehicle = new btRaycastVehicle(tuning, body, vehicle_raycaster);
//
//	vehicle->setCoordinateSystem(0, 1, 2);
//
//	for (int i = 0; i < info.num_wheels; ++i)
//	{
//		btVector3 conn(info.wheels[i].connection.x, info.wheels[i].connection.y, info.wheels[i].connection.z);
//		btVector3 dir(info.wheels[i].direction.x, info.wheels[i].direction.y, info.wheels[i].direction.z);
//		btVector3 axis(info.wheels[i].axis.x, info.wheels[i].axis.y, info.wheels[i].axis.z);
//
//		vehicle->addWheel(conn, dir, axis, info.wheels[i].suspensionRestLength, info.wheels[i].radius, tuning, info.wheels[i].front);
//	}
//
//	//PhysVehicle* pvehicle = new PhysVehicle(body, vehicle, info);
//	//world->addVehicle(vehicle);
//	//vehicles.push_back(pvehicle);
//
//	return pvehicle;
//}

void ModulePhysics::AddConstraintPointToPoint(PhysBody& bodyA, PhysBody& bodyB, const vec3& anchorA, const vec3& anchorB)
{
	btTypedConstraint* p2p = new btPoint2PointConstraint(
		*(bodyA.body),
		*(bodyB.body),
		btVector3(anchorA.x, anchorA.y, anchorA.z),
		btVector3(anchorB.x, anchorB.y, anchorB.z));
	world->addConstraint(p2p);
	constraints.push_back(p2p);
	p2p->setDbgDrawSize(2.0f);
}

void ModulePhysics::AddConstraintHinge(PhysBody& bodyA, PhysBody& bodyB, const vec3& anchorA, const vec3& anchorB, const vec3& axisA, const vec3& axisB, bool disable_collision)
{
	btHingeConstraint* hinge = new btHingeConstraint(
		*(bodyA.body),
		*(bodyB.body),
		btVector3(anchorA.x, anchorA.y, anchorA.z),
		btVector3(anchorB.x, anchorB.y, anchorB.z),
		btVector3(axisA.x, axisA.y, axisA.z),
		btVector3(axisB.x, axisB.y, axisB.z));

	world->addConstraint(hinge, disable_collision);
	constraints.push_back(hinge);
	hinge->setDbgDrawSize(2.0f);
}

// =============================================
void DebugRenderer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
	glColor3f(color[0], color[1], color[2]);
	glBegin(GL_LINES);
	glVertex3f(from[0], from[1], from[2]);
	glVertex3f(to[0], to[1], to[2]);
	glEnd();
}

void DebugRenderer::drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
{
	//point.transform.translate(PointOnB.getX(), PointOnB.getY(), PointOnB.getZ());
	//point.color.Set(color.getX(), color.getY(), color.getZ());
	//point.Render();
}

void DebugRenderer::reportErrorWarning(const char* warningString)
{
	LOG("Bullet warning: %s", warningString);
}

void DebugRenderer::draw3dText(const btVector3& location, const char* textString)
{
	LOG("Bullet draw text: %s", textString);
}

void DebugRenderer::setDebugMode(int debugMode)
{
	mode = (DebugDrawModes)debugMode;
}

int	 DebugRenderer::getDebugMode() const
{
	return mode;
}

btVector3 ToBtVector3(float3& vec)
{
	return btVector3(vec.x, vec.y, vec.z);
}

btQuaternion ToBtQuaternion(Quat& quat)
{
	return btQuaternion(quat.x, quat.y, quat.z, quat.w);
}
