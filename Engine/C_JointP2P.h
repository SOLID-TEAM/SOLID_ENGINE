#ifndef __C_JOINTP2P__
#define __C_JOINTP2P__

#include "Component.h"
#include "Bullet/include/BulletDynamics/ConstraintSolver/btTypedConstraint.h"
#include "Bullet/include/BulletDynamics/ConstraintSolver/btPoint2PointConstraint.h"

class C_JointP2P : public Component
{
public:
	C_JointP2P(GameObject* go);
	~C_JointP2P();

	bool CleanUp();

	bool Save(Config& config);
	bool Load(Config& config);

	bool Update();

	void BodyDeleted(GameObject* go);

	void RecreateConstraint();

	bool Render();

	bool DrawPanelInfo();
private:

	void RemakeConstraint();

private:

	btTypedConstraint* constraint = nullptr;
	float3 pivotA;
	float3 pivotB;
	bool disable_collision = false;
	bool check_rigid_body_exist = true;

	GameObject* connected_body = nullptr; // both gameobjects need a physics rigidbody
	UID connected_body_id = 0;

};

#endif // !__C_JOINTP2P__
