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

	void AddForce(const float3& force);

	void AddTorque(const float3& force);

private:

	void SearchCollider();

private:

	float mass = 0.0f;
	float3 velocity;
	float3 mass_center;
	btRigidBody* body = nullptr;
};

#endif // !_C_RIGID_BODY_H__


