#ifndef __MODULETEST_H__
#define __MODULETEST_H__
#include "Module.h"
#include "Globals.h"

#include "external/par_shapes.h"


class ModuleTest : public Module
{
public:
	ModuleTest(bool start_enabled = true);
	~ModuleTest();

	bool Start(Config& config);
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

	// testing ----
	uint s_cube_elements_id = -1;
	uint s_cube_v_id = -1;
	uint s_sphere_elements_id = -1;
	uint s_sphere_v_id = -1;

	par_shapes_mesh* s_cube = nullptr;
	par_shapes_mesh* s_sphere = nullptr;

	uint my_id = -1;
	uint my_indices = 2;

};

#endif // !__MODULETEST_H__