#ifndef __C_VEHICLE__
#define __C_VEHICLE__

#include "Component.h"
#include "Bullet/include/BulletDynamics/Vehicle/btRaycastVehicle.h"


class C_Vehicle : public Component
{
public:

	C_Vehicle(GameObject* go);
	~C_Vehicle();

	bool CleanUp();

	bool Save(Config& config);

	bool Load(Config& config);

	bool Update();

	bool Render();

	bool DrawPanelInfo();

private:

	bool CreateVehicle();
	bool CheckComponentDependencies();
	void VehicleControls();


	void ApplyEngineForce(float force);
	void Turn(float degrees);
	void Brake(float force);
	float GetKmh() const;

private:

	class Cylinder
	{
	public:

		Cylinder();
		Cylinder(float radius, float height);

		void			Render() const;
		void			InnerRender() const;
		void			SetPos(float x, float y, float z);
		void			SetRotation(float angle, const vec3& u);
		void			Scale(float x, float y, float z);

	public:

		//Color color;
		mat4x4 transform;
		bool axis, wire;
		float radius;
		float height;
	};

	struct Wheel
	{
		vec3 connection; // origin of the ray. Must come from within the chassis
		vec3 direction;
		vec3 axis;
		float suspensionRestLength; // max length for suspension in meters
		float radius;
		float width;
		bool front; // is front wheel ?
		bool drive; // does this wheel received engine power ?
		bool brake; // does breakes affect this wheel ?
		bool steering; // does this wheel turns ?
	};

	struct VehicleInfo
	{
		//~VehicleInfo();

		/*vec3 chassis_size;
		vec3 chassis_offset;*/
		//float mass; // gets from rigidbody
		float suspensionStiffness; // default to 5.88 / 10.0 offroad / 50.0 sports car / 200.0 F1 car
		float suspensionCompression; // default to 0.83
		float suspensionDamping; // default to 0.88 / 0..1 0 bounces / 1 rigid / recommended 0.1...0.3
		float maxSuspensionTravelCm; // default to 500 cm suspension can be compressed
		float frictionSlip; // defaults to 10.5 / friction with the ground. 0.8 should be good but high values feels better (kart 1000.0)
		float maxSuspensionForce; // defaults to 6000 / max force to the chassis

		Wheel* wheels = nullptr;
		int num_wheels;
	};


private:
	bool component_started = false;

	VehicleInfo info;
	btRaycastVehicle* vehicle = nullptr;

	C_Collider* col = nullptr;
	C_RigidBody* r_body = nullptr;

	float turn;
	float acceleration;
	float brake;
	float inertia;

};

#endif // !__C_VEHICLE__
