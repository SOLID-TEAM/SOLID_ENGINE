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

private:

	bool isTrigger = false;
	float3 center;
	btBoxShape* shape = nullptr;
	btDefaultMotionState* motion_state = nullptr;
	btRigidBody* body = nullptr;

};

#endif // !_C_COLLIDER_H__