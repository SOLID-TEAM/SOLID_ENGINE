#ifndef __MODULETEST_H__
#define __MODULETEST_H__

#include "Module.h"
#include "Globals.h"
#include "external//MathGeoLib/include/Math/float4.h"
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
	float line_width = 1.f;
	float4 color = { 0.7f, 0.7f ,0.7f, 0.7f };

private:
	// General
	int units = 1;
	//Buffers
	float* vertices = nullptr;
	int n_vertices = 0;
	uint id = 1;
};

class GameObject;

class ModuleTest : public Module
{
public:
	ModuleTest(bool start_enabled = true);
	~ModuleTest();

	bool Start(Config& config);
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	bool Save(Config& config);
	void Load(Config& config);

	// TODO: fit utilities on its own file
	float map(float value, float s1, float stop1, float s2, float stop2) const;

	Grid* main_grid = nullptr;
	GameObject* test = nullptr;
};

#endif // !__MODULETEST_H__