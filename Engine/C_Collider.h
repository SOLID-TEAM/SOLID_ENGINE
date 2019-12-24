#ifndef _C_COLLIDER_H__
#define  _C_COLLIDER_H__

#include "Component.h"
#include "external/MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;

class C_Collider : public Component
{
public:

	C_Collider(GameObject* go);

	bool CleanUp();

	bool Save(Config& config);
	bool Load(Config& config);

	bool Update();

	bool Render();

	bool DrawPanelInfo();

	 // Set/Get functions ----------------------------

	void SetIsTrigger(bool is_trigger);

	bool GetIsTrigger();

private:

	void LoadCollider();

	float3 CheckInvalidCollider(float3 size);

private:

	bool is_trigger = false;

	float3 center;
	float3 size;

	btBoxShape* shape = nullptr;
	btDefaultMotionState* motion_state = nullptr;
	btRigidBody* body = nullptr;


	bool fit_mesh = false;
};

#endif // !_C_COLLIDER_H__