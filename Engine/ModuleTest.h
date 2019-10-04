#ifndef __MODULETEST_H__
#define __MODULETEST_H__

#include "Module.h"
#include "Globals.h"

#include "external/par_shapes.h"
#include "Primitive.h"

class ModuleTest : public Module
{
public:
	ModuleTest(bool start_enabled = true);
	~ModuleTest();

	bool Start(Config& config);
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

	S_Cube* cube = nullptr;
	S_Plane* plane = nullptr;
	S_Sphere* sphere = nullptr;
};

#endif // !__MODULETEST_H__