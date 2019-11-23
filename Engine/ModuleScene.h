#pragma once

#include "Globals.h"
#include "Module.h"
#include "GameObject.h"
#include "ModuleRenderer3D.h"
#include "KDTree.h"
#include <queue>
#include <map>
#include <list>
#include <stack>
#include <map>

#include "Config.h"

// one action include any child tree inside object
#define MAX_UNDO_ACTIONS 20 // TODO: add a max saved actions buffer qty configuration on W_Config panel to allow user configure its ctrl-z buffer

class CameraEditor;
class Viewport;

enum class EventGoType
{
	STATIC_TO_DYNAMIC,
	DYNAMIC_TO_STATIC,
	DELETE_FROM_STATIC,
	DELETE_FROM_DYNAMIC,
	UKNNOWKN
};

struct EventGo 
{
	EventGo(){}
	EventGo(GameObject* go , EventGoType type) : go (go) , type(type) {}

	GameObject* go = nullptr;
	EventGoType type = EventGoType::UKNNOWKN;

	//bool operator == (EventGo event_go)
	//{
	//	return (event_go.go == go && event_go.type == type);
	//}
};

class ModuleScene : public Module
{
public:

	ModuleScene();

	~ModuleScene();

	bool Init(Config& config);

	bool Start(Config& config);

	update_status PreUpdate();

	update_status Update();
	
	//update_status PostUpdate();

	update_status Draw();

	void UpdateMousePicking();

	bool CleanUp();

	bool ToSaveScene(const char* scene_name);

	bool ToLoadScene(const char* scene_name);

	std::string GetSceneName() const;

	void NewScene();
	
	bool Save(Config& config);

	void Load(Config& config);

	// Game Objects functions ----------------------------------------------

	GameObject* CreateGameObject(std::string name = "no_name", GameObject* parent = nullptr);

	GameObject* CreateGameObjectFromModel(UID uid);

	void DeleteGameObject(GameObject* go);

	GameObject* Find(std::string name);

	GameObject* FindByUID(UID uid, GameObject* go);

	// Commands -------------------------------------------------------------

	std::deque<GameObject*>& GetUndoDeque();

	void AddGoToHierarchyChange(GameObject* target_go, GameObject* source_go);

	void UndoLastDelete(); // put gameobject to undo buffer

	void PushEvent(GameObject* go, EventGoType type);

	// Debug ----------------------------------------------------------------

	void PushDebugRender(DebugRender debug_render);

private:

	bool SaveScene(Config& config, GameObject* go);

	bool LoadScene(Config& config);

	bool LoadSceneNow();

	void UpdateHierarchy();

	void UpdateGoLists();

	void UpdateGizmo();

	void UpdateSpacePartitioning();

	AABB EncloseAllStaticGo();

	AABB EncloseAllGo();

	void AddGOToUndoDeque(GameObject* gameObject);

	// Lists operatiions -------------------------------------

	void FillGoLists();

	// Others ------------------------------------------------

	bool IsGizmoActived();

public:

	// Mode ---------------------------------------

	bool	editor_mode = false;

	// Game Objects -------------------------------

	std::stack<EventGo>			events_go_stack;
	//std::stack<GameObject*>	to_delete_go_stack;
	std::list<GameObject*>		static_go_list;
	std::list<GameObject*>		dynamic_go_list;
	std::vector<GameObject*>	go_render_list;

	GameObject*				root_go = nullptr;
	GameObject*				selected_go = nullptr;
	GameObject*				main_camera = nullptr;
	CameraEditor*			editor_camera = nullptr;

	// Viewports ---------------------------------

	std::list<Viewport*> viewports;

	Viewport* scene_viewport = nullptr;
	Viewport* game_viewport = nullptr;

	// Debug -------------------------------------

	std::vector<DebugRender> debug_renders;

	// Trees -------------------------------------

	KDTree kdtree;

private:

	bool create_new_scene = false;
	bool load_new_scene = false;
	std::string scene_name;
	std::string scene_to_load;
	std::deque<GameObject*> to_undo_buffer_go;
	std::vector<GameObject*> temp_to_undo_go;

	// store childrens we must to move
	// TODO: think is this the better place, the idea is any hierarchy change is made outside any loop except preupdate
	// and the more direct way to access and do the changes
	//std::queue<GameObject*> childrens_to_move;
	std::map<GameObject*, GameObject*> childrens_to_move;


};