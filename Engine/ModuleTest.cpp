//#define PAR_SHAPES_IMPLEMENTATION
//#include "external/par_shapes.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleTest.h"
#include "ModuleImporter.h"
#include "C_Transform.h"


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
{
}

// Load assets
bool ModuleTest::Start(Config& config)
{
	LOG("Loading Test assets");
	bool ret = true;

	App->camera->Move({ 1.0f, 1.0f, 1.0f });
	App->camera->LookAt({ 0, 0, 0 });

	main_grid = new Grid(config.GetInt("grid_units", 10));
	Load(config);

	App->importer->ImportModelFile("Assets/Models/baker_house/BakerHouse.fbx");
	test = App->scene->Find("BakerHouse");

	return ret;
}

// Load assets
bool ModuleTest::CleanUp()
{
	LOG("Unloading Test scene");

	return true;
}

bool ModuleTest::Save(Config& config)
{
	if (main_grid != nullptr)
	{
		config.AddInt("grid_units", main_grid->GetUnits());
		config.AddFloat("grid_line_width", main_grid->line_width);
		config.AddFloatArray("grid_color", main_grid->color.ptr(), 4);
	}
	else
	{
		float4 color = { 1.f, 1.f ,1.f ,5.f };
		config.AddInt("grid_units", 10);
		config.AddFloat("grid_line_width", 1.f);
		config.AddFloatArray("grid_color", color.ptr(), 4);
	}

	return true;
}

void ModuleTest::Load(Config& config)
{
	main_grid->SetUnits(config.GetInt("grid_units", 10));
	main_grid->line_width = config.GetFloat("grid_line_width", 1.f);
	main_grid->color.x = config.GetFloat("grid_color", 1.f, 0);
	main_grid->color.y = config.GetFloat("grid_color", 1.f, 1);
	main_grid->color.z = config.GetFloat("grid_color", 1.f, 2);
	main_grid->color.w = config.GetFloat("grid_color", 1.f, 3);
}

// Update
update_status ModuleTest::Update(float dt)
{
	test->transform->LookAt(float3(0.f, 0.f, 0.f));

	return UPDATE_CONTINUE;
}

update_status ModuleTest::PostUpdate(float dt)
{
	

	return UPDATE_CONTINUE;
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

	ModuleRenderer3D::BeginDebugDraw(color.ptr());
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnableClientState(GL_VERTEX_ARRAY);

	glLineWidth(line_width);
	glColor4fv(color.ptr());

	glBindBuffer(GL_ARRAY_BUFFER, id);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);

	glDrawArrays(GL_LINES, 0, n_vertices * 3);

	ModuleRenderer3D::EndDebugDraw();

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