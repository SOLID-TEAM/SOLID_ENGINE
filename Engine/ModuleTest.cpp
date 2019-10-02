#include "Globals.h"
#include "Application.h"
#include "ModuleTest.h"
#include "Primitive.h"

#include "external/MathGeoLib/include/MathBuildConfig.h"
#include "external/MathGeoLib/include/MathGeoLib.h"

#ifdef _DEBUG
#pragma comment (lib, "external/MathGeoLib/libx86/_Debug/MathGeoLib.lib") 
#else
#pragma comment (lib, "external/MathGeoLib/libx86/_Release/MathGeoLib.lib") 
#endif

// Random Number Generator Test ---------------------------

#include "PCG/include/pcg_random.hpp"
#include "Globals.h"
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

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));
	GetIntRandomValue(1.f, 4.f);
	GetFloatRandomValue(1.f, 10.f);
	GetRandomPercent();
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

	PPlane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	// mathgeolib test ------------------------------------------

	static float x, y, z;

	Sphere s1(float3(0.0f, 0.0f, 0.0f), 50.0f);
	//s1.r = 100.0f;
	//s1.pos.Set(0.0f, 0.0f, 0.0f);

	Sphere s2;

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		x -= 0.5f;
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		x += 0.5f;
	}
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		z += 0.5f;
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		z -= 0.5f;
	}

	s2.r = 50.0f + z;
	s2.pos.Set(x, 0.f, 0.0f);

	/*LOG("S2 pos: %f,%f,%f", s2.pos.x, s2.pos.y, s2.pos.z);
	LOG("S2 radius: %f", s2.r);

	if (s1.Intersects(s2))
	{
		LOG("Spheres intersecting");
	}
	else
	{
		LOG("Spheres not intersecting");
	}*/
	
	// ----------------------------------------------------------
	

	return UPDATE_CONTINUE;
}

void ModuleTest::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
}

