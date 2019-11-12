#pragma once

#include "Globals.h"
#include "Module.h"
#include "GameObject.h"
#include <queue>
#include <map>

// one action include any child tree inside object
#define MAX_UNDO_ACTIONS 20 // TODO: add a max saved actions buffer qty configuration on W_Config panel to allow user configure its ctrl-z buffer

class ModuleScene : public Module
{
public:
	ModuleScene();
	~ModuleScene();

	bool Init(Config& config);
	bool Start(Config& config);
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	void UpdateAll(float dt, GameObject*);
	update_status PostUpdate(float dt);
	update_status Draw();

	// TODO: improve this hierarchy draw if needed
	// current in-out recursively (draw at return)
	void RenderAll(GameObject* go);
	
	// put gameobject to undo buffer
	void DeleteGameObject(GameObject* go);
	void UndoLastDelete();

	GameObject* CreateGameObject(std::string name = "no_name", GameObject* parent = nullptr);
	
	bool CleanUp();

	GameObject* Find(std::string name);

	GameObject* _Find(std::string name, GameObject* go);

	std::deque<GameObject*>& GetUndoDeque();

	void AddGoToHierarchyChange(GameObject* target_go, GameObject* source_go);

private:
	void AddGOToUndoDeque(GameObject* gameObject);

public:
	GameObject* root_go = nullptr;
	GameObject* main_camera = nullptr;

private:
	std::deque<GameObject*> to_undo_buffer_go;
	//std::queue<GameObject*> to_delete_buffer_go;

	std::vector<GameObject*> temp_to_undo_go;

	// store childrens we must to move
	// TODO: think is this the better place, the idea is any hierarchy change is made outside any loop except preupdate
	// and the more direct way to access and do the changes
	//std::queue<GameObject*> childrens_to_move;
	std::map<GameObject*, GameObject*> childrens_to_move;


};