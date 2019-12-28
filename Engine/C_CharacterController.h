#ifndef _C_CHARACTER_CONTROLLER_H__
#define  _C_CHARACTER_CONTROLLER_H__

#include "Component.h"
#include "external/MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class C_Collider;
class C_RigidBody;

class C_CharacterController : public Component
{	 
public:

	C_CharacterController(GameObject* go);

	bool CleanUp();

	bool Save(Config& config);

	bool Load(Config& config);

	bool Update();

	bool DrawPanelInfo();

private:

	bool is_loaded = false;
	C_RigidBody* rigid_body = nullptr;
	C_Collider*  collider = nullptr;
};

#endif // !_C_CHARACTER_CONTROLLER_H__



