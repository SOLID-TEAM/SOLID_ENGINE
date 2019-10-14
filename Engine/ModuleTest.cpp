//#define PAR_SHAPES_IMPLEMENTATION
//#include "external/par_shapes.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleTest.h"
// MathGeoLib ---------------------------------------------
#include "external/MathGeoLib/include/MathBuildConfig.h"
#include "external/MathGeoLib/include/MathGeoLib.h"

#ifdef _DEBUG
#pragma comment (lib, "external/MathGeoLib/libx86/_Debug/MathGeoLib.lib") 
#else
#pragma comment (lib, "external/MathGeoLib/libx86/_Release/MathGeoLib.lib") 
#endif
#include "GL/glew.h"


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

	main_grid = new Grid(15);

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
	

	return UPDATE_CONTINUE;
}

void ModuleTest::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{

}

Grid::Grid(int units) : units(units)
{
	glGenBuffers(1, &id);
	ComputeLines();
}

void Grid::ComputeLines()
{
	if (vertices != nullptr)
	{
		delete[] vertices;
	}

	if (units < 1)
	{
		units = 1;
	}

	n_vertices =  4 + units * 8; // main axis + added perpendicular axis * units
	this->vertices = new float[n_vertices * 3];
	int j = 0;

	for (int i = -units; i <= units; ++i)
	{
		vertices[j] = i; vertices[j + 1] = 0; vertices[j + 2] = units;
		vertices[j +3] = i; vertices[j + 4] = 0; vertices[j + 5] = -units;

		vertices[j + 6] = - units; vertices[j + 7] = 0; vertices[j + 8] = -i;
		vertices[j + 9] = units; vertices[j + 10] = 0; vertices[j + 11] = -i;

		j += 12;
	}

	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * n_vertices, &vertices[0], GL_STATIC_DRAW);
}

void Grid::SetUnits(int units) 
{
	if (units != this->units)
	{
		this->units = units;
		ComputeLines();
	}
}
int Grid::GetUnits() 
{
	return units;
}

void Grid::Render()
{
	if (active == false)
	{
		return;
	}

	glLineWidth(width);
	glColor4fv((float*)&color);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, id);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);

	glDrawArrays(GL_LINES, 0, n_vertices * 2);

	glDisableClientState(GL_VERTEX_ARRAY);

	glLineWidth(1.0f);
}

void Grid::Destroy()
{
	glDeleteBuffers(1, &id);
	
	// delete all stored data

	if (vertices != nullptr) 
	{
		delete[] vertices;
	}
}


float ModuleTest::map(float value, float s1, float stop1, float s2, float stop2) const
{
	return s2 + (stop2 - s2) * ((value - s1) / (stop1 - s1));
}