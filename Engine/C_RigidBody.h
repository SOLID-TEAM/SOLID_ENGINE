#ifndef _C_RIGID_BODY_H__
#define  _C_RIGID_BODY_H__

#include "Component.h"
#include "ModulePhysics.h"
#include "external/MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class C_Collider;
class C_CapsuleCollider;
class C_JointP2P;

class C_RigidBody : public Component
{
public:
	friend class C_CapsuleCollider;
	friend class C_JointP2P;

public:

	C_RigidBody(GameObject* go);

	bool CleanUp();

	bool Save(Config& config);

	bool Load(Config& config);

	bool Update();

	bool Render();

	bool DrawPanelInfo();

	// User Functions ----------------------

	void SetMass(float& v);

	void SetDrag(float& v);

	void SetAngularDrag(float& v);

	void AddForce(const float3& force, Space space = Space::Local);

	void AddTorque(const float3& force, Space space = Space::Local);

private:

	void SetBouncing(float& v);

	void SetFriction(float& v);

	void SetAngularFriction(float& v);

	void CreateBody();

	void SetBodyTranform(const float3& pos, const Quat& rot);

	void SearchCollider();

private:

	float3 force_to_apply = float3::zero;
	float3 torque_to_apply = float3::zero;

	float mass = 0.0f;
	float drag = 0.f;
	float angular_drag = 0.f;

	
	bool use_gravity = true;
	bool is_kinematic = false;
	bool freeze_position[3] = {false, false, false};
	bool freeze_rotation[3] = {false, false, false};

	float3 velocity;
	btVector3 inertia;

	bool body_added = false;
	// Body used in physics simulation
	btRigidBody* body = nullptr;
	// Used when GameObejct has not a collider
	btBoxShape* aux_shape = nullptr;
	// Collider component
	C_Collider* collider = nullptr;
};

#endif // !_C_RIGID_BODY_H__


