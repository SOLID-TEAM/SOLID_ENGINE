//#define PAR_SHAPES_IMPLEMENTATION
//#include "external/par_shapes.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleTest.h"
//#include "Primitive.h"

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
	//GetIntRandomValue(1.f, 4.f);
	//GetFloatRandomValue(1.f, 10.f);
	//GetRandomPercent();
	glEnable(GL_TEXTURE_2D);

	v = CreateCubeVertices(8, 1.f);

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
	// TODO: place scene y0 plane in other site

	glColor3f(0.9f, 0.9f, 0.9f);
	glLineWidth(2.5f);
	glBegin(GL_LINES);

	for (float i = -100.f; i <= 100.f; i++)
	{
		glVertex3f(i, 0.f, 100.f);
		glVertex3f(i, 0.f, -100.f);

		glVertex3f(-100.f, 0.f, -i);
		glVertex3f(100.f, 0.f, -i);
	}

	glEnd();

	GLubyte checkImage[64][64][4];

	for (int i = 0; i < 64; i++) {
		for (int j = 0; j < 64; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	//glGenTextures(1, &ImageName);
	//glBindTexture(GL_TEXTURE_2D, ImageName);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
	//	0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

	DrawCube(v);

	return UPDATE_CONTINUE;
}

float** ModuleTest::CreateCubeVertices(int vertex_num, float cube_dist)
{
	float** v = new float* [vertex_num];;

	for (int i = 0; i < vertex_num; ++i)
	{
		v[i] = new float[3];
	}

	//v0
	v[0][0] = 0.f;
	v[0][1] = 0.f;
	v[0][2] = 0.f;

	//v1
	v[1][0] = 0.f;
	v[1][1] = 0.f;
	v[1][2] = cube_dist;

	//v2
	v[2][0] = cube_dist;
	v[2][1] = 0.f;
	v[2][2] = 0.f;

	//v3
	v[3][0] = cube_dist;
	v[3][1] = 0.f;
	v[3][2] = cube_dist;

	//v4
	v[4][0] = 0.f;
	v[4][1] = cube_dist;
	v[4][2] = 0.f;

	//v5
	v[5][0] = 0.f;
	v[5][1] = cube_dist;
	v[5][2] = cube_dist;

	//v6
	v[6][0] = cube_dist;
	v[6][1] = cube_dist;
	v[6][2] = 0.f;

	//v7
	v[7][0] = cube_dist;
	v[7][1] = cube_dist;
	v[7][2] = cube_dist;

	return v;
}

//Draw a cube and name each vertex with the number
//Place the vertices of each face of the cube in counterclock direction
void ModuleTest::DrawCube(float** v)
{
	CubeFace(v[2], v[3], v[1], v[0]);
	CubeFace(v[0], v[1], v[5], v[4]);
	CubeFace(v[2], v[0], v[4], v[6]);
	CubeFace(v[3], v[2], v[6], v[7]);
	CubeFace(v[1], v[3], v[7], v[5]);
	CubeFace(v[4], v[5], v[7], v[6]);
}


//Square direction
//   3 - 2
//   | / |
//   0 - 1 
void ModuleTest::CubeFace(float* v0, float* v1, float* v2, float* v3)
{
	//tri 0
	glBegin(GL_TRIANGLES);
	glVertex3fv(v0);
	glVertex3fv(v1);
	glVertex3fv(v2);
	glEnd();

	glBegin(GL_TRIANGLES);
	//tri 1
	glVertex3fv(v2);
	glVertex3fv(v3);
	glVertex3fv(v0);
	glEnd();

}

