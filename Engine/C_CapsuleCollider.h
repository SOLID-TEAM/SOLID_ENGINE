#ifndef _C_CAPSULE_COLLIDER_H__
#define  _C_CAPSULE_COLLIDER_H__

#include "C_Collider.h"
#include "external/MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class ModulePhysics;

class C_CapsuleCollider : public C_Collider
{
public:

	friend ModulePhysics;

public:

	C_CapsuleCollider(GameObject* go);

	// Virtual Functions ------------------------------

	void SaveCollider(Config& config);

	void LoadCollider(Config& config);

	void DrawInfoCollider();

};

#endif // !_C_CAPSULE_COLLIDER_H__
