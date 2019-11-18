#pragma once

#include "Globals.h"
#include "Module.h"
#include "GameObject.h"
#include <queue>
#include <map>
#include <list>
#include <stack>

#include "Config.h"

// one action include any child tree inside object
#define MAX_UNDO_ACTIONS 20 // TODO: add a max saved actions buffer qty configuration on W_Config panel to allow user configure its ctrl-z buffer

class CameraEditor;
class Viewport;

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

	bool CleanUp();

	void UpdateAll(float dt, GameObject*);

	void RenderAll(GameObject* go);

	bool ToSaveScene(const char* scene_name);
	bool ToLoadScene(const char* scene_name);

	std::string GetSceneName() const;

	
	
	// Game Objects functions ----------------------------------------------

	GameObject* CreateGameObject(std::string name = "no_name", GameObject* parent = nullptr);
	GameObject* CreateGameObjectFromModel(UID uid);

	void DeleteGameObject(GameObject* go);

	GameObject* Find(std::string name);

	// Commands -------------------------------------------------------------

	std::deque<GameObject*>& GetUndoDeque();

	void AddGoToHierarchyChange(GameObject* target_go, GameObject* source_go);

	void UndoLastDelete(); // put gameobject to undo buffer

private:
	bool SaveScene(Config& config, GameObject* go);
	bool LoadScene(Config& config);

	AABB EncloseAllStaticGo();

	void AddGOToUndoDeque(GameObject* gameObject);

public:

	// Game Objects -------------------------------

	std::list<GameObject*>	static_go_list;
	std::list<GameObject*>	dynamic_go_list;

	GameObject*				root_go = nullptr;
	GameObject*				selected_go = nullptr;
	GameObject*				main_camera = nullptr;
	CameraEditor*			editor_camera = nullptr;

	// Viewports ---------------------------------

	Viewport* scene_viewport = nullptr;
	Viewport* game_viewport = nullptr;


private:

	std::string scene_name;
	std::deque<GameObject*> to_undo_buffer_go;
	//std::queue<GameObject*> to_delete_buffer_go;

	std::vector<GameObject*> temp_to_undo_go;

	// store childrens we must to move
	// TODO: think is this the better place, the idea is any hierarchy change is made outside any loop except preupdate
	// and the more direct way to access and do the changes
	//std::queue<GameObject*> childrens_to_move;
	std::map<GameObject*, GameObject*> childrens_to_move;


};