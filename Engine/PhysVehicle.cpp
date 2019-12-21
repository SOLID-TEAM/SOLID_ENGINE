#include "btBulletDynamicsCommon.h"
#include "btBulletCollisionCommon.h"
#include "PhysBody.h"
#include "PhysVehicle.h"
// ----------------------------------------------------------------------------
VehicleInfo::~VehicleInfo()
{
	//if(wheels != NULL)
		//delete wheels;
}

// ----------------------------------------------------------------------------
PhysVehicle::PhysVehicle(btRigidBody* body, btRaycastVehicle* vehicle, const VehicleInfo& info) : PhysBody(body), vehicle(vehicle), info(info)
{
}

// ----------------------------------------------------------------------------
PhysVehicle::~PhysVehicle()
{
	delete vehicle;
}

// ----------------------------------------------------------------------------
void PhysVehicle::Render()
{
	//Cylinder wheel;

	//wheel.color = Blue;

	//for (int i = 0; i < vehicle->getNumWheels(); ++i)
	//{
	//	wheel.radius = info.wheels[0].radius;
	//	wheel.height = info.wheels[0].width;

	//	vehicle->updateWheelTransform(i);
	//	vehicle->getWheelInfo(i).m_worldTransform.getOpenGLMatrix(&wheel.transform);

	//	wheel.Render();
	//}

	//Cube chassis(info.chassis_size.x, info.chassis_size.y, info.chassis_size.z);
	//vehicle->getChassisWorldTransform().getOpenGLMatrix(&chassis.transform);
	//btQuaternion q = vehicle->getChassisWorldTransform().getRotation();
	//btVector3 offset(info.chassis_offset.x, info.chassis_offset.y, info.chassis_offset.z);
	//offset = offset.rotate(q.getAxis(), q.getAngle());

	//chassis.transform.M[12] += offset.getX();
	//chassis.transform.M[13] += offset.getY();
	//chassis.transform.M[14] += offset.getZ();


	//chassis.Render();
}

// ----------------------------------------------------------------------------
void PhysVehicle::ApplyEngineForce(float force)
{
	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if (info.wheels[i].drive == true)
		{
			vehicle->applyEngineForce((btScalar)force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle::Brake(float force)
{
	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if (info.wheels[i].brake == true)
		{
			vehicle->setBrake((btScalar)force, i);
		}
	}
}

// ----------------------------------------------------------------------------
void PhysVehicle::Turn(float degrees)
{
	for (int i = 0; i < vehicle->getNumWheels(); ++i)
	{
		if (info.wheels[i].steering == true)
		{
			vehicle->setSteeringValue((btScalar)degrees, i);
		}
	}
}

// ----------------------------------------------------------------------------
float PhysVehicle::GetKmh() const
{
	return vehicle->getCurrentSpeedKmHour();
}