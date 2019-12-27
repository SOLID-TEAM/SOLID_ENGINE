#ifndef _C_CONVEX_HULL_COLLIDER_H__
#define  _C_CONVEX_HULL_COLLIDER_H__

#include "C_Collider.h"
#include "external/MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

#include "Bullet/include/BulletCollision/CollisionShapes/btShapeHull.h"

class GameObject;
class ModulePhysics;
class C_Mesh;

class C_ConvexHullCollider : public C_Collider
{
public:

	friend ModulePhysics;

public:

	C_ConvexHullCollider(GameObject* go);

	// Virtual Functions ------------------------------

	void CreateShape(C_Mesh* mesh);

	void AdjustShape();

	void SaveCollider(Config& config);

	void LoadCollider(Config& config);

	void DrawInfoCollider();

private:

	float3 CheckInvalidCollider(float3 size);


private:

	float3 size;

};

#endif // !_C_CONVEX_HULL_COLLIDER_H__
