#ifndef __MODULETEST_H__
#define __MODULETEST_H__

#include "Module.h"
#include "Globals.h"

class Grid 
{
public:
	Grid( int units);
	void ComputeLines();
	void SetUnits(int units);
	int GetUnits();
	void Render();
	void Destroy();

public:

	bool active = true;
	float width = 1.f;
	ImVec4 color = { 1.f, 1.f ,1.f, 1.f };

private:
	// General
	int units = 1;
	//Buffers
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

	// TODO: fit utilities on its own file
	float map(float value, float s1, float stop1, float s2, float stop2) const;
	

	Grid* main_grid = nullptr;
};

#endif // !__MODULETEST_H__