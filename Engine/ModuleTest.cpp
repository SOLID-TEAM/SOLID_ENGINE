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
	//GetIntRandomValue(1.f, 4.f);
	//GetFloatRandomValue(1.f, 10.f);
	//GetRandomPercent();

	//cube = new S_Cube(1, 0, 0);
	////cube->Scale(2, 1, 1);
	//plane = new S_Plane(4, 0, 0);
	//sphere = new S_Sphere(-2, 0, 0, 4);

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

	/*cube->Render();*/

	return UPDATE_CONTINUE;
}

void ModuleTest::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{

}

