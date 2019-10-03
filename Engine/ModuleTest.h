#pragma once
#include "Module.h"
#include "Globals.h"

class ModuleTest : public Module
{
public:
	ModuleTest(bool start_enabled = true);
	~ModuleTest();

	bool Start(Config& config);
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

	uint my_id = 0;
	uint my_indices = 2;
	/*par_shapes_mesh* cube;*/
};
