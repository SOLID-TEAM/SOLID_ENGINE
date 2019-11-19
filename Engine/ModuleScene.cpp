#include "ModuleScene.h"
#include "GL/glew.h"
#include "Application.h"

#include "C_Transform.h"
#include "Viewport.h"
#include "CameraEditor.h"

#include "R_Model.h"

#include "external/MathGeoLib/include/Math/MathAll.h"

ModuleScene::ModuleScene() {}

ModuleScene::~ModuleScene() {}

bool ModuleScene::Init(Config& config)
{
	return true;
}

bool ModuleScene::Start(Config& config)
{
	scene_name.assign("untitled");
	// creates a root gameobject, wich all another go are childs of it
	root_go = new GameObject("Scene Root");
	root_go->uid = 0; // scene root always 0 uid
	main_camera = new GameObject("Main Camera", root_go, true);
	main_camera->CreateComponent<C_Camera>();
	editor_camera = new CameraEditor();

	scene_viewport = new Viewport(editor_camera);
	game_viewport = new Viewport(main_camera);

	// TESTING LOADING META RESOURCES, here for module order needs for test

	//App->resources->LoadAllMetaResources();

	// TODO: get the last scene saved, or simply do nothing and let the user load one
	//ToLoadScene(scene_name.c_str());
	// load default scene
	ToLoadScene("default.solidscene");

	return true;
}

update_status ModuleScene::PreUpdate(float dt)
{
	if (create_new_scene)
	{
		CleanUp();
		create_new_scene = false;
	}

	if (load_new_scene)
	{
		LoadSceneNow();
		load_new_scene = false;
		scene_to_load.clear();
	}

	//// delete go from parent, if any | game object to undo --------------------

	// iterates once the list, if any goes wrong inform to user
	for (std::vector<GameObject*>::iterator gotu = temp_to_undo_go.begin(); gotu != temp_to_undo_go.end(); ++gotu)
	{
		if ((*gotu)->parent != nullptr)
		{
			std::vector<GameObject*>::iterator it = std::find((*gotu)->parent->childs.begin(), (*gotu)->parent->childs.end(), (*gotu));

			if (it != (*gotu)->parent->childs.end())
			{
				(*gotu)->parent->childs.erase(it);
				LOG(" ------------------------------------------------------- ");
				LOG("[Info] Succesfully erased child %s from parent %s", (*gotu)->GetName(), (*gotu)->parent->GetName());
				// add to undo buffer
				AddGOToUndoDeque(*gotu);
				LOG(" ------------------------------------------------------- ");
				// "unselect from hierarchy"
				selected_go = nullptr;
			}
			else
				LOG("child not found on parent");
		}
		else
			LOG("[Info] Parent not found");
	}
	if (temp_to_undo_go.size() > 0) temp_to_undo_go.clear();

	// RE-parent all needed childrens if needed --------------
	// map is done for when we are able to move on hierarchy more than 1 go at once WIP
	std::map<GameObject*, GameObject*>::iterator it = childrens_to_move.begin();

	for (; it != childrens_to_move.end(); ++it)
		(*it).first->AddChild((*it).second);

	if (childrens_to_move.size() > 0)
		childrens_to_move.clear();

	// ---------------------------------------------------------------------------

	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{
	if(editor_camera)
		editor_camera->DoUpdate(dt);

	// TESTING SAVE SCENE
	//if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	//{
	//	//ToSaveScene();
	//	LoadScene();
	//}

	// TODO: SHORTCUTS
	// check CTRL + Z
	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
	{
		UndoLastDelete();
	}

	if (root_go != nullptr)
	{
		UpdateAll(dt, root_go);
	}

	return UPDATE_CONTINUE;
}


void ModuleScene::UpdateAll(float dt, GameObject* go)
{
	go->DoUpdate(dt);

	for (std::vector<GameObject*>::iterator child = go->childs.begin(); child != go->childs.end(); ++child)
	{
		UpdateAll(dt , (*child));
	}
}


update_status ModuleScene::PostUpdate(float dt)
{
	if(editor_camera != nullptr)
		App->renderer3D->lights[0].SetPos(editor_camera->transform->position);

	return UPDATE_CONTINUE;
}

update_status ModuleScene::Draw()
{

	scene_viewport->BeginViewport();

	App->renderer3D->lights[0].Render();
	App->test->main_grid->Render();

	// draw all go's last

	if (root_go != nullptr)
	{
		RenderAll(root_go);
	}

	scene_viewport->EndViewport();

	return UPDATE_CONTINUE;
}

void ModuleScene::RenderAll(GameObject* go)
{
	go->DoRender();

	for (std::vector<GameObject*>::iterator child = go->childs.begin() ; child != go->childs.end(); ++child)
	{
		RenderAll((*child));
	}
}

bool ModuleScene::CleanUp()
{
	// release undo buffers
	while (to_undo_buffer_go.size() > 0)
	{
		to_undo_buffer_go.front()->DoCleanUp();
		delete to_undo_buffer_go.front();
		to_undo_buffer_go.pop_front();
	}

	to_undo_buffer_go.clear();

	std::vector<GameObject*>::iterator game_objects = root_go->childs.begin();
	// TODO: search why dont work normal delete
	std::queue<GameObject*> delete_forever;
	for (; game_objects != root_go->childs.end();)
	{
		(*game_objects)->DoCleanUp();
		delete_forever.push((*game_objects));
		game_objects = root_go->childs.erase(game_objects);
	}

	while (!delete_forever.empty())
	{
		delete delete_forever.front();
		delete_forever.front() = nullptr;
		delete_forever.pop();
	}

	selected_go = nullptr;

	// GameObjects --------------------------------------------------

	// TODO
	/*if (editor_camera != nullptr)
	{
		delete editor_camera;
		editor_camera = nullptr;
	}*/

	return true;
}

GameObject* ModuleScene::Find(std::string name)
{
	std::stack<GameObject*> go_stack;

	go_stack.push(root_go);

	while (!go_stack.empty())
	{
		GameObject* go = go_stack.top();

		if (go->name == name)
		{
			return go;
		}


		go_stack.pop();

		for (GameObject * child : go->childs)
		{
			go_stack.push(child);
		}
	}
	return nullptr;
}

GameObject* ModuleScene::CreateGameObject(std::string name, GameObject* parent)
{
	return new GameObject(name,parent);
}

void ModuleScene::DeleteGameObject(GameObject* go)
{
	temp_to_undo_go.push_back(go);
}

void ModuleScene::UndoLastDelete()
{
	if (to_undo_buffer_go.size() > 0)
	{
		// re-add gameobject to its parent
		// TODO: for advanced delete history recovery, we must to search:
		// 1 -if the parent exists in the scene
		// 1.2 - if not, search if exists on undo buffer
		// 1.3 - if not, assign to scene root object

		// if the go previosly has parent
		if (to_undo_buffer_go.back()->parent != nullptr)
		{
			// re-add child to its parent
			to_undo_buffer_go.back()->parent->childs.push_back(to_undo_buffer_go.back());
			// TODO: if the parent is already deleted the object doesn't re-arrange on scene (not tested) || currently this never gonna happen
			LOG("[Info] Succesfully re-attached child %s to its parent %s", to_undo_buffer_go.back()->GetName(), to_undo_buffer_go.back()->parent->GetName());
			// restore hierarchy selection
			selected_go = to_undo_buffer_go.back();
			LOG("[Info] %s deleted from undo buffer", to_undo_buffer_go.back()->GetName());
			to_undo_buffer_go.pop_back();
		}
	}
	else
		LOG("no more gameobjects to restore");
}


AABB ModuleScene::EncloseAllStaticGo()
{
	AABB global_aabb;
	global_aabb.SetNegativeInfinity();

	for (GameObject* go : static_go_list)
	{
		global_aabb.Enclose(go->bounding_box);
	}

	return global_aabb;
}

void ModuleScene::AddGOToUndoDeque(GameObject* go)
{
	if (to_undo_buffer_go.size() >= MAX_UNDO_ACTIONS)
	{
		LOG("[Error] Max %i undo buffer reached, permanently deleted %s gameObject", to_undo_buffer_go.size(), to_undo_buffer_go.front()->GetName())
		to_undo_buffer_go.front()->DoCleanUp();
		delete to_undo_buffer_go.front();
		to_undo_buffer_go.pop_front();
	}

	to_undo_buffer_go.push_back(go);
	LOG("[Info] Added %s to undo buffers", go->GetName());
}


std::deque<GameObject*>& ModuleScene::GetUndoDeque()
{
	return to_undo_buffer_go;
}

void ModuleScene::AddGoToHierarchyChange(GameObject* target_go, GameObject* source_go)
{
	childrens_to_move.insert({ target_go, source_go });
}

// testing function to launch the process of save
bool ModuleScene::ToSaveScene(const char* name)
{
	Config new_scene_save;
	this->scene_name.assign(name);

	std::string scene_name(name + std::string(".solidscene"));
	// save first other scene needed values
	// name
	// etc
	Config other = new_scene_save.AddSection("general");
	other.AddString("scene_name", name);
	
	//Config GameObjects = new_scene_save.AddSection("GameObjects");
	// save all gameobjects needed values
	new_scene_save.AddArray("GameObjects");
	SaveScene(new_scene_save, root_go);

	new_scene_save.SaveConfigToFile((ASSETS_FOLDER + scene_name).c_str());

	

	return true;
}

bool ModuleScene::SaveScene(Config& config, GameObject* go)
{
	//go->Save(config);

	for (std::vector<GameObject*>::iterator it = go->childs.begin(); it != go->childs.end(); ++it)
	{
		(*it)->Save(config);
		SaveScene(config, (*it));
	}

	return true;
}

bool ModuleScene::LoadScene(Config& scene)
{
	Config general = scene.GetSection("general");

	scene_name.assign(general.GetString("scene_name", scene_name.c_str()));

	std::map<GameObject*, uint> relations;

	int go_count = scene.GetArrayCount("GameObjects");

	if (go_count <= 0) return false;

	/*std::vector<GameObject*> to_load_gos;
	to_load_gos.reserve(go_count);*/

	for (int i = 0; i < go_count; ++i)
	{
		GameObject* new_go = new GameObject();
		new_go->Load(scene.GetArray("GameObjects", i), relations);

		//to_load_gos.push_back(new_go);
	}

	for (std::map<GameObject*, uint>::iterator it = relations.begin(); it != relations.end(); ++it)
	{
		GameObject* go = (*it).first;
		uint parent_id = (*it).second;

		if (parent_id == 0) // 0 is root scene
		{
			if (go->parent != root_go)
			{
				go->SetNewParent(root_go);
			}
			continue;
		}
		
		GameObject* parent_match = FindByUID(parent_id, root_go);

		go->SetNewParent(parent_match);

		//LOG("");
	}


	return true;
}

GameObject* ModuleScene::FindByUID(UID uid, GameObject* go)
{
	GameObject* ret = nullptr;

	if (uid == go->uid)
		return go;
	
	for (std::vector<GameObject*>::iterator it = go->childs.begin(); it != go->childs.end(); ++it)
	{
		ret = FindByUID(uid, (*it));
		if (ret) break;
	}

	return ret;
}

bool ModuleScene::LoadSceneNow()
{
	// TODO: maybe the scene is not on assets folder, but for now we dont let decide, scene are saved on assets folder
	Config to_load(std::string(ASSETS_FOLDER + scene_to_load).c_str());

	return LoadScene(to_load);;
}

// TODO: pass this on finishupdate on app.cpp
bool ModuleScene::ToLoadScene(const char* name)
{
	bool ret = false;

	if (name != nullptr)
	{
		// TODO: make VFS for all project files on runtime
		// re-search if this filename exists
		/*if (App->file_sys->Exists(name))
		{
			LOG("");
		}*/
		/*std::vector<std::string> file_list, dir_list;
		file_list.push_back(name);
		App->file_sys->DiscoverFiles(ASSETS_FOLDER, file_list, dir_list);*/
		std::vector<std::string> file_list;
		App->file_sys->GetAllFilesWithExtension(ASSETS_FOLDER, "solidscene", file_list);

		for (uint i = 0; i < file_list.size(); ++i)
		{
			if (file_list[i].compare(name) == 0)
			{
				scene_to_load.assign(name);
				create_new_scene = true;
				load_new_scene = true;

				ret = true;
			}
		}
	}

	return ret;
}

GameObject* ModuleScene::CreateGameObjectFromModel(UID uid)
{
	Resource* r = App->resources->Get(uid);

	if (r->GetType() != Resource::Type::MODEL)
	{
		LOG("[Error] bad resource type to create gameobjects from");
		return nullptr;
	}

	R_Model* model =  (R_Model*)r;

	model->LoadToMemory();

	std::vector<GameObject*> all_go;
	uint num_nodes = model->GetNumNodes();

	all_go.reserve(num_nodes);

	for (uint i = 0; i < num_nodes; ++i)
	{
		const R_Model::Node& node = model->GetNode(i);

		GameObject* parent = root_go;
		if (i > 0)
			parent = all_go[node.parent];

		GameObject* new_go = CreateGameObject(node.name.c_str(), parent);

		//TODO: SET TRANSFORM

		if (node.mesh > 0)
		{
			C_Mesh* c_mesh = new_go->CreateComponent< C_Mesh>();
			
			if (c_mesh->SetMeshResource(node.mesh))
			{
				new_go->CreateComponent<C_MeshRenderer>();
			}
		}



		all_go.push_back(new_go);
	}

	// TODO: improve models to be a prefab, currently is only used as container of hierarchy and meshes/materials
	// when we are done creating a usable gameobjects classes from model data, unload model
	model->Release();

	return nullptr;
}

std::string ModuleScene::GetSceneName() const
{
	return scene_name;
}

void ModuleScene::NewScene()
{
	create_new_scene = true;
}