#include "Globals.h"
#include "Application.h"
#include "ModuleTest.h"
#include "Primitive.h"
#include "par_shapes.h"

#include "external/MathGeoLib/include/MathBuildConfig.h"
#include "external/MathGeoLib/include/MathGeoLib.h"

#ifdef _DEBUG
#pragma comment (lib, "external/MathGeoLib/libx86/_Debug/MathGeoLib.lib") 
#else
#pragma comment (lib, "external/MathGeoLib/libx86/_Release/MathGeoLib.lib") 
#endif

// Random Number Generator Test ---------------------------

#include "PCG/include/pcg_random.hpp"
#include "GL/glew.h"
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

	App->camera->Move(vec3(1.0f, 1.0f, 1.0f));
	App->camera->LookAt(vec3(0, 0, 0));
	GetIntRandomValue(1.f, 4.f);
	GetFloatRandomValue(1.f, 10.f);
	GetRandomPercent();


	float vertices[] = {
		// front
		-1.0, -1.0,  1.0,
		 1.0, -1.0,  1.0,
		 1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,
		// back
		-1.0, -1.0, -1.0,
		 1.0, -1.0, -1.0,
		 1.0,  1.0, -1.0,
		-1.0,  1.0, -1.0
	};

	uint indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	my_id = 0;
	glGenBuffers(1, (GLuint*) & (my_id));
	glBindBuffer(GL_ARRAY_BUFFER, my_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 3 , vertices, GL_STATIC_DRAW);

	my_indices = 2;
	glGenBuffers(1, (GLuint*) & (my_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 36, indices, GL_STATIC_DRAW);


	//cube = par_shapes_create_cube();

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

;
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
	glBindBuffer(GL_ARRAY_BUFFER, my_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
	glDisableClientState(GL_VERTEX_ARRAY);

	return UPDATE_CONTINUE;
}

void ModuleTest::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{

}

