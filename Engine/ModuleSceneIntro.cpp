#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"

#include "Primitive.h"

#include "external/MathGeoLib/include/MathBuildConfig.h"
#include "external/MathGeoLib/include/MathGeoLib.h"

#ifdef _DEBUG
#pragma comment (lib, "external/MathGeoLib/libx86/Debug/MathGeoLib.lib") 
#else
#pragma comment (lib, "external/MathGeoLib/libx86/Release/MathGeoLib.lib") 
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

	math::Sphere s1;
	s1.r = 100;
	//s1.pos.Set(0.0f, 0.0f, 0.0f);

	// mathgeolib test
	//Sphere s1(float3(0.0f, 0.0f, 0.0f), 100.0f);
	//s1.r = 100.0f;
	//s1.pos.Set(0.0f, 0.0f, 0.0f);

	Sphere s2;
	s2.r = 100.0f;
	//s2.pos.Set(0.f, 0.f, 0.f);

	/*if (s1.Intersects(s2))
	{
		LOG("Spheres intersecting");
	}
	else
	{
		LOG("Spheres not intersecting");
	}*/
	

	

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
}

