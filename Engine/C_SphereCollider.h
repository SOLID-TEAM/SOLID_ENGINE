#ifndef _C_SPHERE_COLLIDER_H__
#define  _C_SPHERE_COLLIDER_H__

#include "C_Collider.h"
#include "external/MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class ModulePhysics;
class C_Mesh;

class C_SphereCollider : public C_Collider
{
public:

	friend ModulePhysics;

public:

	C_SphereCollider(GameObject* go);

	// Virtual Functions ------------------------------

	void CreateShape(C_Mesh* mesh);

	void AdjustShape();

	void SaveCollider(Config& config);

	void LoadCollider(Config& config);

	void DrawInfoCollider();

private:

	float radius;
};

#endif // !_C_SPHERE_COLLIDER_H__

