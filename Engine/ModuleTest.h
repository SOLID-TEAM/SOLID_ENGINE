#ifndef __MODULETEST_H__
#define __MODULETEST_H__

#include "Module.h"
#include "Globals.h"

class Grid 
{
	Grid( int units);
	void ComputeLines();
	void Render();
	void Destroy();

private:
	int units = 1;
	float* vertices = nullptr;
	int n_vertices = 0;
	uint id = 1;
};

class ModuleTest : public Module
{
public:
	ModuleTest(bool start_enabled = true);
	~ModuleTest();

	bool Start(Config& config);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);

	/*S_Cube* cube = nullptr;
	S_Plane* plane = nullptr;
	S_Sphere* sphere = nullptr;*/
};

#endif // !__MODULETEST_H__