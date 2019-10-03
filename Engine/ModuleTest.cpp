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

	/*float cube[36 * 3] = {
	-.5f, .5f, .5f,
	-.5f, -.5f, .5f,
	.5f, -.5f, .5f,

	.5f, -.5f, .5f,
	.5f, .5f, .5f,
	-.5f, .5f, .5f,

	.5f, .5f, .5f,
	.5f, -.5f, .5f,
	.5f, -.5f, -.5f,

	.5f, -.5f, -.5f,
	.5f, .5f, -.5f,
	.5f, .5f, .5f,

	.5f, .5f, -.5f,
	.5f, -.5f, -.5f,
	-.5f, -.5f, -.5f,

	-.5f, -.5f, -.5f,
	-.5f, .5f, -.5f,
	.5f, .5f, -.5f,

	-.5f, -.5f, -.5f,
	-.5f, -.5f, .5f,
	-.5f, .5f, .5f,

	-.5f, .5f, .5f,
	-.5f, .5f, -.5f,
	-.5f, -.5f, -.5f,

	-.5f, .5f, .5f,
	.5f, .5f, .5f,
	.5f, .5f, -.5f,

	-.5f, .5f, .5f,
	.5f, .5f, -.5f,
	-.5f, .5f, -.5f,

	.5f, -.5f, -.5f,
	.5f, -.5f, .5f,
	-.5f, -.5f, .5f,

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

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, cube, GL_STATIC_DRAW);*/


	// -----------------------------------------------------
	// shapes from par_shapes.h
	s_cube = par_shapes_create_cube();
	par_shapes_translate(s_cube, -2, 0, 0);
	s_sphere = par_shapes_create_parametric_sphere(40, 20);
	par_shapes_translate(s_sphere, 1, 0, 0);

	LOG("%i points", s_cube->npoints); // store vertices indexed
	LOG("%i tris", s_cube->ntriangles); // store indices of each point / each triangle index to a point || each index points to 3 values

	for (int i = 0; i < s_cube->ntriangles * 3; ++i) // each face/tri has 3 index to some vertex/points
	{
		LOG("triangle %i", s_cube->triangles[i]);
	}
	for (int i = 0; i < s_cube->npoints; ++i)
	{
		float* v = &s_cube->points[i];
		LOG("point %i - %f,%f,%f", i,v[0], v[1], v[2]);
	}
	
	//generate buffer for vertices
	glGenBuffers(1, &s_cube_v_id);
	glBindBuffer(GL_ARRAY_BUFFER, s_cube_v_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * s_cube->npoints, &s_cube->points[0], GL_STATIC_DRAW);

	// generate buffer for indices
	glGenBuffers(1, &s_cube_elements_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_cube_elements_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * s_cube->ntriangles * 3, &s_cube->triangles[0], GL_STATIC_DRAW);

	// sphere
	//generate buffer for vertices
	glGenBuffers(1, &s_sphere_v_id);
	glBindBuffer(GL_ARRAY_BUFFER, s_sphere_v_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * s_sphere->npoints, &s_sphere->points[0], GL_STATIC_DRAW);

	// generate buffer for indices
	glGenBuffers(1, &s_sphere_elements_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_sphere_elements_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * s_sphere->ntriangles * 3, &s_sphere->triangles[0], GL_STATIC_DRAW);
//=======
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 3 , vertices, GL_STATIC_DRAW);
//>>>>>>> 3f51289fb7c610bdf779fccba2570d648eac60da
//
//	my_indices = 2;
//	glGenBuffers(1, (GLuint*) & (my_indices));
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, my_indices);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * 36, indices, GL_STATIC_DRAW);


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

	// ----------------------------------------------------------

	//glEnableClientState(GL_VERTEX_ARRAY);
	//glBindBuffer(GL_ARRAY_BUFFER, my_id);
	//glVertexPointer(3, GL_FLOAT, 0, NULL);
	//// … draw other buffers
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	//glDisableClientState(GL_VERTEX_ARRAY);

	//par_shapes_translate(s_cube, 0 +(xAmount), 0, 0);
	//par_shapes_scale(s_cube, 1.001f, 1.0f, 1.0f);
	// s_cube draw --------------------
	glEnableClientState(GL_VERTEX_ARRAY);
	// cube
	glBindBuffer(GL_ARRAY_BUFFER, s_cube_v_id);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * s_cube->npoints, &s_cube->points[0], GL_STATIC_DRAW); // re-send for translate

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_cube_elements_id);
	glDrawElements(GL_TRIANGLES, s_cube->ntriangles * 3, GL_UNSIGNED_SHORT, (void*)0);
	
	// sphere
	glBindBuffer(GL_ARRAY_BUFFER, s_sphere_v_id);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_sphere_elements_id);
	glDrawElements(GL_TRIANGLES, s_sphere->ntriangles * 3, GL_UNSIGNED_SHORT, (void*)0);
	// --------------------------------

	glDisableClientState(GL_VERTEX_ARRAY);

	return UPDATE_CONTINUE;
}

void ModuleTest::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{

}

