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
	update_status Draw();

	// TODO: improve this hierarchy draw if needed
	// current in-out recursively (draw at return)
	void DrawAll(GameObject* go);
	void DrawAllComponentsPanelInfo();

	GameObject* CreateGameObject(std::string name = "no_name", GameObject* parent = nullptr);
	
	bool CleanUp();

public:

	GameObject* root_go = nullptr;

private:

	/*std::vector<GameObject*> game_objects;*/

};