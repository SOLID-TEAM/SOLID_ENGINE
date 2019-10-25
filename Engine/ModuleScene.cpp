#include "ModuleScene.h"
#include "GL/glew.h"
#include "Application.h"

#include "C_Transform.h"
#include "external/MathGeoLib/include/Math/MathAll.h"
ModuleScene::ModuleScene() {}

ModuleScene::~ModuleScene() {}

bool ModuleScene::Init(Config& config)
{
	// creates a root gameobject, wich all another go are childs of it
	root_go = new GameObject("scene_root_gameobject");
	return true;
}

bool ModuleScene::Start(Config& config)
{

	return true;
}

update_status ModuleScene::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{
	if (root_go != nullptr)
	{
		UpdateAll(dt, root_go);
	}
	return UPDATE_CONTINUE;
}


void ModuleScene::UpdateAll(float dt, GameObject* go)
{
	go->Update(dt);

	for (std::vector<GameObject*>::iterator child = go->childs.begin(); child != go->childs.end(); ++child)
	{
		UpdateAll(dt , (*child));
	}
}


update_status ModuleScene::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleScene::Draw()
{
	App->test->main_grid->Render();

	// draw all go's last

	if (root_go != nullptr)
	{
		RenderAll(root_go);
	}

	return UPDATE_CONTINUE;
}

void ModuleScene::RenderAll(GameObject* go)
{
	go->Render();

	for (std::vector<GameObject*>::iterator child = go->childs.begin() ; child != go->childs.end(); ++child)
	{
		RenderAll((*child));
	}
}

bool ModuleScene::CleanUp()
{
	std::vector<GameObject*>::iterator game_objects = root_go->childs.begin();

	for (; game_objects != root_go->childs.end(); ++game_objects)
	{
		(*game_objects)->CleanUp();
	}

	return true;
}

// --------------------------------------------------------------

GameObject* ModuleScene::CreateGameObject(std::string name, GameObject* parent)
{
	return new GameObject(name,parent);
}