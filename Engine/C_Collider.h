#ifndef _C_COLLIDER_H__
#define  _C_COLLIDER_H__

#include "Component.h"
#include "external/MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class ModulePhysics;
class C_Mesh;
class C_RigidBody;
class C_Vehicle;

class C_Collider : public Component
{
public:

	friend ModulePhysics;
	friend C_RigidBody;
	friend C_Vehicle;

public:

	C_Collider(GameObject* go);

	virtual ~C_Collider() {}

	bool CleanUp();

	bool Save(Config& config);

	bool Load(Config& config);

	bool Update();

	virtual bool Render();

	bool DrawPanelInfo();

	 // Common Functoins ------------------------------

	void SetBouncing(float& v);

	void SetFriction(float& v);

	void SetAngularFriction(float& v);

	void SetIsTrigger(bool is_trigger);

	bool GetIsTrigger();

	float3 GetWorldCenter();

	// Virtual Functions ------------------------------

	// Create specific shape
	virtual void CreateShape(C_Mesh* mesh) {};

	// Adjust shape to scale and other factors
	virtual void AdjustShape() {};

	// Draw aditional collider info
	virtual void DrawInfoCollider() {};

	virtual void SaveCollider(Config& config) {};

	virtual void LoadCollider(Config& config) {};

private:

	bool SearchRigidBody();

protected:

	bool is_trigger = false;
	bool is_loaded = false;

	float3 center;
	float3 scaled_center;

	float bouncing = 0.f;
	float friction = 0.f;
	float angular_friction = 0.f;

	// Collider shape used in collision simulation
	btCollisionShape* shape = nullptr;
	// Used when GameObject has notrigid body in run time
	btRigidBody* aux_body = nullptr; 
	// Used when GameObject has notrigid body in run time
	btDefaultMotionState* aux_motion_state = nullptr;
	// Added to world
	bool body_added = false;

};

#endif // !_C_COLLIDER_H__