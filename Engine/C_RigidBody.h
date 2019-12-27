#ifndef _C_RIGID_BODY_H__
#define  _C_RIGID_BODY_H__

#include "Component.h"
#include "external/MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class C_Collider;

class C_RigidBody : public Component
{
public:

	C_RigidBody(GameObject* go);

	bool CleanUp();

	bool Save(Config& config);
	bool Load(Config& config);

	bool Update();

	bool Render();

	bool DrawPanelInfo();

	// User Functions ----------------------

	void SetMass(float v);

	void SetBouncing(float v);

	void SetDrag(float v);

	void SetAngularDrag(float v);

	void SetFriction(float v);

	void SetAngularFriction(float v);

	void AddForce(const float3& force);

	void AddTorque(const float3& force);

private:

	void SearchCollider();

private:

	float mass = 0.0f;
	float bouncing = 0.f;
	float drag = 0.f;
	float angular_drag = 0.f;
	float friction = 0.f;
	float angular_friction = 0.f;
	
	bool use_gravity = true;
	bool is_kinematic = false;
	bool freeze_position[3] = {false, false, false};
	bool freeze_rotation[3] = {false, false, false};

	float3 velocity;
	C_Collider* collider = nullptr;
	btRigidBody* body = nullptr;
};

#endif // !_C_RIGID_BODY_H__


