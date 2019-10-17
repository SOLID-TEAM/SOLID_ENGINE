#include "ModuleScene.h"
#include "GL/glew.h"
#include "Application.h"

ModuleScene::ModuleScene() {}

ModuleScene::~ModuleScene() {}

bool ModuleScene::Init(Config& config)
{
	// creates a root gameobject, wich all another go are childs of it
	root_go = new GameObject("scene_root_gameobject");

	//// create one child of root go
	//GameObject* new_go = new GameObject("Child of root_go", root_go);

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
		std::vector<GameObject*>::iterator game_objects = root_go->childs.begin();
		
		for (; game_objects != root_go->childs.end(); ++game_objects)
		{
			(*game_objects)->Update(dt);
		}
	}

	return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt)
{


	return UPDATE_CONTINUE;
}

void ModuleScene::DrawAllComponentsPanelInfo()
{

}

update_status ModuleScene::Draw()
{
	App->test->main_grid->Render();

	// draw all go's last

	if (root_go != nullptr)
	{
		std::vector<GameObject*>::iterator game_objects = root_go->childs.begin();

		for (; game_objects != root_go->childs.end(); ++game_objects)
		{
			DrawAll((*game_objects));
		}
	}

	return UPDATE_CONTINUE;
}

void ModuleScene::DrawAll(GameObject* go)
{
	std::vector<GameObject*>::iterator it = go->childs.begin();

	for (; it != go->childs.end(); ++it)
	{
		DrawAll((*it));
	}

	go->Draw();
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