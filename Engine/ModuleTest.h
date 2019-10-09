#ifndef __MODULETEST_H__
#define __MODULETEST_H__

#include "Module.h"
#include "Globals.h"

#define CHECKERS_HEIGHT  64;
#define CHECKERS_WIDTH  64;

class ModuleTest : public Module
{
public:
	ModuleTest(bool start_enabled = true);
	~ModuleTest();

	bool Start(Config& config);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	float** CreateCubeVertices(int vertex_num, float cube_dist);
	void DrawCube(float** v);
	void CubeFace(float* v0, float* v1, float* v2, float* v3);
	bool CleanUp();

	float** v;

};

#endif // !__MODULETEST_H__