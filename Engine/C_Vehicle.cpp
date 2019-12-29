#include "Application.h"
#include "C_Vehicle.h"
#include "GameObject.h"
#include "C_RigidBody.h"
#include "C_Collider.h"

C_Vehicle::C_Vehicle(GameObject* go) : Component(go, ComponentType::VEHICLE)
{
	name.assign("Vehicle Controller");

	C_Collider* col = linked_go->GetComponent<C_Collider>();
	C_RigidBody* r_body = linked_go->GetComponent<C_RigidBody>();

	if (col == nullptr)
		linked_go->AddComponent<C_ConvexHullCollider>();
	if (r_body == nullptr)
		linked_go->AddComponent<C_RigidBody>();

	// TESTING TODO: pass all this to drawpanel info : ) ------

	info.suspensionStiffness = 5.88f; // default to 5.88 / 10.0 offroad / 50.0 sports car / 200.0 F1 car
	info.suspensionCompression = 0.83f; // default to 0.83
	info.suspensionDamping = 0.88f;  // default to 0.88 / 0..1 0 bounces / 1 rigid / recommended 0.1...0.3
	info.maxSuspensionTravelCm = 500.0f;// default to 500 cm suspension can be compressed
	info.frictionSlip = 10.5f; // defaults to 10.5 / friction with the ground. 0.8 should be good but high values feels better (kart 1000.0)
	info.maxSuspensionForce = 6000.0f; // defaults to 6000 / max force to the chassis

	// Wheel properties ---------------------------------------
	float front_connection_height = 0.5625f;
	float front_wheel_radius = 0.5625f;
	float front_wheel_width = 0.5325f;
	float front_suspensionRestLength = 0.5f;

	float rear_connection_height = 0.6f;
	float rear_wheel_radius = 0.6f;
	float rear_wheel_width = 0.5625f;
	float rear_suspensionRestLength = 0.6f;


	// car properties

	float half_rear_width = 1.0f; 
	float half_front_width = 1.0f; 

	float half_length = 2.0f;

	vec3 direction(0, -1, 0);
	vec3 axis(-1, 0, 0);

	info.num_wheels = 4;
	info.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	info.wheels[0].connection.Set(half_front_width - 0.3f * front_wheel_width, front_connection_height, half_length - front_wheel_radius);
	info.wheels[0].direction = direction;
	info.wheels[0].axis = axis;
	info.wheels[0].suspensionRestLength = front_suspensionRestLength;
	info.wheels[0].radius = front_wheel_radius;
	info.wheels[0].width = front_wheel_width;
	info.wheels[0].front = true;
	info.wheels[0].drive = true;
	info.wheels[0].brake = false;
	info.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	info.wheels[1].connection.Set(-half_front_width + 0.3f * front_wheel_width, front_connection_height, half_length - front_wheel_radius);
	info.wheels[1].direction = direction;
	info.wheels[1].axis = axis;
	info.wheels[1].suspensionRestLength = front_suspensionRestLength;
	info.wheels[1].radius = front_wheel_radius;
	info.wheels[1].width = front_wheel_width;
	info.wheels[1].front = true;
	info.wheels[1].drive = true;
	info.wheels[1].brake = false;
	info.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	info.wheels[2].connection.Set(half_rear_width - 0.3f * rear_wheel_width, rear_connection_height, -half_length + rear_wheel_radius);
	info.wheels[2].direction = direction;
	info.wheels[2].axis = axis;
	info.wheels[2].suspensionRestLength = rear_suspensionRestLength;
	info.wheels[2].radius = rear_wheel_radius;
	info.wheels[2].width = rear_wheel_width;
	info.wheels[2].front = false;
	info.wheels[2].drive = false;
	info.wheels[2].brake = true;
	info.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	info.wheels[3].connection.Set(-half_rear_width + 0.3f * rear_wheel_width, rear_connection_height, -half_length + rear_wheel_radius);
	info.wheels[3].direction = direction;
	info.wheels[3].axis = axis;
	info.wheels[3].suspensionRestLength = rear_suspensionRestLength;
	info.wheels[3].radius = rear_wheel_radius;
	info.wheels[3].width = rear_wheel_width;
	info.wheels[3].front = false;
	info.wheels[3].drive = false;
	info.wheels[3].brake = true;
	info.wheels[3].steering = false;

}

C_Vehicle::~C_Vehicle() {};

bool C_Vehicle::Update()
{
	if (!component_started)
	{
		CreateVehicle();
		component_started = true;
	}

	if (vehicle)
		VehicleControls();

	return true;
}

void C_Vehicle::VehicleControls()
{
	turn = acceleration = brake = 0.0f;

	float turn_deg = 20.0f * DEGTORAD;
	float max_accel = 5000.0f;

	inertia = (-vehicle->getRigidBody()->getMass() * vehicle->getCurrentSpeedKmHour()) / 50 + vehicle->getCurrentSpeedKmHour();
	
	// TEST
	// ACCEL
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		acceleration = max_accel;
	}
	else 
	{
		if (GetKmh() > 0)
		{
			acceleration += inertia;
		}
	}
	
	// TURN
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		
		if (turn < turn_deg)
			turn += turn_deg;
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if (turn > -turn_deg)
			turn -= turn_deg;
	}

	// BRAKE
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		if (GetKmh() > 0) 
		{                      
			brake = 3500;
		}
		else 
		{
			acceleration = -max_accel / 3.0f;
		}
	}


	ApplyEngineForce(acceleration);
	Turn(turn);
	Brake(brake);

}

bool C_Vehicle::CheckComponentDependencies()
{
	col = linked_go->GetComponent<C_Collider>();
	r_body = linked_go->GetComponent<C_RigidBody>();

	return (col != nullptr && r_body != nullptr);
}

bool C_Vehicle::CreateVehicle()
{
	float mass = 500.0f;

	if (!CheckComponentDependencies()) return false;
	// get convex hull shape
	
	btVector3 localInertia(0.0f, 0.0f, 0.0f);
	col->shape->calculateLocalInertia(mass, localInertia);//r_body->body->getMass(), localInertia);

	// TODO: check for body constraints?
	// --

	btRaycastVehicle::btVehicleTuning tuning;
	tuning.m_frictionSlip = info.frictionSlip;
	tuning.m_maxSuspensionForce = info.maxSuspensionForce;
	tuning.m_maxSuspensionTravelCm = info.maxSuspensionTravelCm;
	tuning.m_suspensionCompression = info.suspensionCompression;
	tuning.m_suspensionDamping = info.suspensionDamping;
	tuning.m_suspensionStiffness = info.suspensionStiffness;

	vehicle = new btRaycastVehicle(tuning, r_body->body, App->physics->vehicle_raycaster);

	r_body->body->setActivationState(DISABLE_DEACTIVATION);
	//r_body->body->setMassProps(mass, localInertia); // this fucks me, rb sets constantly its mass

	//vehicle->setCoordinateSystem(0, 1, 2);

	for (int i = 0; i < info.num_wheels; ++i)
	{
		btVector3 conn(info.wheels[i].connection.x, info.wheels[i].connection.y, info.wheels[i].connection.z);
		btVector3 dir(info.wheels[i].direction.x, info.wheels[i].direction.y, info.wheels[i].direction.z);
		btVector3 axis(info.wheels[i].axis.x, info.wheels[i].axis.y, info.wheels[i].axis.z);

		vehicle->addWheel(conn, dir, axis, info.wheels[i].suspensionRestLength, info.wheels[i].radius, tuning, info.wheels[i].front);
	}

	App->physics->AddVehicle(vehicle);

	return true;
}

bool C_Vehicle::Render()
{
	// test debug draw
	App->physics->world->debugDrawWorld();

	return true;
}

bool C_Vehicle::DrawPanelInfo()
{
	return true;
}

bool C_Vehicle::CleanUp()
{
	App->physics->RemoveVehicle(vehicle);
	delete vehicle;
	vehicle = nullptr;

	return true;
}

bool C_Vehicle::Save(Config& config)
{
	return true;
}

bool C_Vehicle::Load(Config& config)
{
	return true;
}

// ----------------------------------------------------------------------------
void C_Vehicle::ApplyEngineForce(float force)
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
void C_Vehicle::Brake(float force)
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
void C_Vehicle::Turn(float degrees)
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
float C_Vehicle::GetKmh() const
{
	return vehicle->getCurrentSpeedKmHour();
}