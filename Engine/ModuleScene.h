#pragma once

#include "Globals.h"
#include "Module.h"
#include "GameObject.h"

class ModuleScene : public Module
{
public:
	ModuleScene();
	~ModuleScene();

	bool Init(Config& config);
	bool Start(Config& config);
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	GameObject* CreateGameObject(std::string name = "no_name", GameObject* parent = nullptr);
	
	bool CleanUp();

public:

	GameObject* root_go = nullptr;

private:

	/*std::vector<GameObject*> game_objects;*/

};