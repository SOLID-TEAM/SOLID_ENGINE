#include "ModuleScene.h"
#include "GL/glew.h"
#include "Application.h"

#include "C_Transform.h"
#include "Viewport.h"
#include "CameraEditor.h"

#include "external/MathGeoLib/include/Math/MathAll.h"

ModuleScene::ModuleScene() {}

ModuleScene::~ModuleScene() {}

bool ModuleScene::Init(Config& config)
{
	return true;
}

bool ModuleScene::Start(Config& config)
{
	// GameObjects ---------------------------

	root_go = new GameObject("Scene Root"); root_go->ignore = true;
	main_camera = new GameObject("Main Camera", root_go); main_camera->ignore = true;
	main_camera->CreateComponent<C_Camera>();
	editor_camera = new CameraEditor(); editor_camera->ignore = true;

	// Viewports -----------------------------
	scene_viewport = new Viewport(editor_camera);
	game_viewport = new Viewport(main_camera);

	return true;
}

update_status ModuleScene::PreUpdate(float dt)
{
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

	if (ImGui::Begin("Space Partitioning"))
	{

		if (ImGui::Button("Recalculate", ImVec2(100, 20)))
		{
			std::stack<GameObject*> go_stack;
			std::vector<GameObject*> go_vector;

			go_stack.push(root_go);

			while (!go_stack.empty())
			{
				GameObject* go = go_stack.top();

				if (go->bounding_box.IsFinite())
				{
					go_vector.push_back(go);
				}

				go_stack.pop();

				for (GameObject* child : go->childs)
				{
					go_stack.push(child);
				}
			}

			kdtree.Fill(3, 1, EncloseAllGo(), go_vector);
		}

		if (ImGui::Button("Clear", ImVec2(100, 20)))
		{
			kdtree.Clear();
		}

		ImGui::End();
	}

	editor_camera->DoUpdate(dt);

	// TESTING SAVE SCENE
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		ToSaveScene();
	}

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
	return UPDATE_CONTINUE;
}

update_status ModuleScene::Draw()
{
	C_Camera* camera = main_camera->GetComponent<C_Camera>();

	for (Viewport* viewport : viewports)
	{
		if (!viewport->active) continue;
		editor_mode = (viewport  == scene_viewport) ? true : false;

		// Begin -----------------------------------------------

		viewport->BeginViewport();

		// Draw GameObjects / Components -----------------------

		std::stack<GameObject*> go_stack;

		go_stack.push(root_go);

		while (!go_stack.empty())
		{
			GameObject* go = go_stack.top();

			if (main_camera == go || !camera->cullling)  // Add game object flags to ignore culling or other situations 
			{
				go->DoRender();
			}
			else if ( camera->CheckCollisionAABB(go->bounding_box) )
			{
				go->DoRender();
			}

			go_stack.pop();

			for (GameObject* child : go->childs)
			{
				go_stack.push(child);
			}
		}

		// TODO:: Remove light --------------------------------

		App->renderer3D->lights[0].SetPos(viewport->GetCamera()->linked_go->transform->position);

		App->renderer3D->lights[0].Render();

		// Debug Renders --------------------------------------

		if (editor_mode)
		{
			App->test->main_grid->Render();
			App->renderer3D->RenderKDTree(kdtree, 3.f);
		}

		// End ------------------------------------------------

		viewport->EndViewport();
	}

	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	std::vector<GameObject*>::iterator game_objects = root_go->childs.begin();

	for (; game_objects != root_go->childs.end(); ++game_objects)
	{
		(*game_objects)->DoCleanUp();
	}

	// release undo buffers
	to_undo_buffer_go.clear();

	// GameObjects --------------------------------------------------

	if (editor_camera != nullptr)
	{
		delete editor_camera;
	}

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

AABB ModuleScene::EncloseAllGo()
{
	AABB global_aabb;
	global_aabb.SetNegativeInfinity();

	std::stack<GameObject*> go_stack;

	go_stack.push(root_go);

	while (!go_stack.empty())
	{
		GameObject* go = go_stack.top();

		if (go->ignore == false)
		{
			global_aabb.Enclose(go->bounding_box);
		}

		go_stack.pop();

		for (GameObject* child : go->childs)
		{
			go_stack.push(child);
		}
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
bool ModuleScene::ToSaveScene()
{
	Config new_scene_save;

	// save first other scene needed values
	// name
	// etc
	Config other = new_scene_save.AddSection("other");
	other.AddString("scene_name", "testing.solidscene");
	
	//Config GameObjects = new_scene_save.AddSection("GameObjects");
	// save all gameobjects needed values
	new_scene_save.AddArray("GameObjects");
	SaveScene(new_scene_save, root_go);

	new_scene_save.SaveConfigToFile("testing.solidscene");

	return true;
}

bool ModuleScene::SaveScene(Config& config, GameObject* go)
{
	
	go->Save(config);

	for (std::vector<GameObject*>::iterator it = go->childs.begin(); it != go->childs.end(); ++it)
	{
		SaveScene(config, (*it));
	}

	return true;
}