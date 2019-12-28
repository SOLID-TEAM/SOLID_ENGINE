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

	enum class CapsuleType
	{
		X, Y, Z
	};

	C_CapsuleCollider(GameObject* go);

	// Virtual Functions ------------------------------

	void CreateShape(C_Mesh* mesh);

	void AdjustShape();

	void SaveCollider(Config& config);

	void LoadCollider(Config& config);

	void DrawInfoCollider();

private:

	float radius = 0.5f;
	float height = 1.f;
	CapsuleType capsule_type = CapsuleType::Y;

};

#endif // !_C_CAPSULE_COLLIDER_H__
