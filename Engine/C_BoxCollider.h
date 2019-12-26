#ifndef _C_BOX_COLLIDER_H__
#define  _C_BOX_COLLIDER_H__

#include "C_Collider.h"
#include "external/MathGeoLib/include/Math/MathAll.h"
#include "Bullet/include/btBulletDynamicsCommon.h"

class GameObject;
class ModulePhysics;
class C_Mesh;

class C_BoxCollider : public C_Collider
{
public:

	friend ModulePhysics;

public:

	C_BoxCollider(GameObject* go);

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

#endif // !_C_BOX_COLLIDER_H__
