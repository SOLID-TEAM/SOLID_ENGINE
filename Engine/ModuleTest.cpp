#define PAR_SHAPES_IMPLEMENTATION
#include "external/par_shapes.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleTest.h"
#include "Primitive.h"
#include "par_shapes.h"

// MathGeoLib ---------------------------------------------
#include "external/MathGeoLib/include/MathBuildConfig.h"
#include "external/MathGeoLib/include/MathGeoLib.h"

#ifdef _DEBUG
#pragma comment (lib, "external/MathGeoLib/libx86/_Debug/MathGeoLib.lib") 
#else
#pragma comment (lib, "external/MathGeoLib/libx86/_Release/MathGeoLib.lib") 
#endif
// --------------------------------------------------------
// Random Number Generator Test ---------------------------
#include "PCG/include/pcg_random.hpp"
#include "GL/glew.h"
#include <random>




int GetIntRandomValue(int range_i1, int range_i2)
{
	// Seed with a real random value, if available
	pcg_extras::seed_seq_from<std::random_device> seed_source;

	// Make a random number engine
	pcg32 rng(seed_source);

	// Choose a random mean 
	std::uniform_int_distribution<int> uniform_dist1(range_i1, range_i2);
	int mean = uniform_dist1(rng);
	LOG("Random number generated: %i", mean);

	return mean;
}

float GetFloatRandomValue(float range_f1, float range_f2)
{

	// Seed with a real random value, if available
	pcg_extras::seed_seq_from<std::random_device> seed_source;

	// Make a random number engine
	pcg32 rng(seed_source);

	//Choose a random mean 
	std::uniform_real_distribution<float> uniform_dist(range_f1, range_f2);
	float mean = uniform_dist(rng);

	LOG("Random number generated: %f", mean);

	return mean;
}

// Return a value between 0 and 1

float GetRandomPercent()
{
	// Seed with a real random value, if available
	pcg_extras::seed_seq_from<std::random_device> seed_source;

	// Make a random number engine
	pcg32 rng(seed_source);

	//Choose a random mean 
	std::uniform_real_distribution<float> uniform_dist(0, 1);
	float mean = uniform_dist(rng);

	LOG("Random number generated: %f", mean);

	return mean;
}



ModuleTest::ModuleTest(bool start_enabled) : Module(start_enabled)
{
	name.assign("ModuleRandom?");
}

ModuleTest::~ModuleTest()
{}

// Load assets
bool ModuleTest::Start(Config& config)
{
	LOG("Loading Test assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 1.0f));
	App->camera->LookAt(vec3(0, 0, 0));
	GetIntRandomValue(1.f, 4.f);
	GetFloatRandomValue(1.f, 10.f);
	GetRandomPercent();

	cube = new S_Cube(1, 0, 0);
	cube->Scale(2, 1, 1);
	plane = new S_Plane(4, 0, 0);
	sphere = new S_Sphere(-2, 0, 0, 4);

	return ret;
}

// Load assets
bool ModuleTest::CleanUp()
{
	LOG("Unloading Test scene");

	
	return true;
}

// Update
update_status ModuleTest::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleTest::PostUpdate(float dt)
{
	// TODO: same of the "TODO NEXT" on ModuleImporter

	/*glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(1.0f, 0.375f);

	glColor3f(1.0f, 0.0f, 1.0f);

	cube->Render();
	plane->Render();
	sphere->Render();

	glDisable(GL_POLYGON_OFFSET_FILL);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glColor3f(1.0f, 1.0f, 1.0f);

	cube->Render();
	plane->Render();
	sphere->Render();*/


	return UPDATE_CONTINUE;
}

void ModuleTest::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{

}

