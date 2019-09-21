#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"

#include "Primitive.h"

#include "external/MathGeoLib/include/MathBuildConfig.h"
#include "external/MathGeoLib/include/MathGeoLib.h"

#ifdef _DEBUG
#pragma comment (lib, "external/MathGeoLib/libx86/_Debug/MathGeoLib.lib") 
#else
#pragma comment (lib, "external/MathGeoLib/libx86/_Release/MathGeoLib.lib") 
#endif


ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	
	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
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

	LOG("S2 pos: %f,%f,%f", s2.pos.x, s2.pos.y, s2.pos.z);
	LOG("S2 radius: %f", s2.r);

	if (s1.Intersects(s2))
	{
		LOG("Spheres intersecting");
	}
	else
	{
		LOG("Spheres not intersecting");
	}
	
	// ----------------------------------------------------------
	

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
}

