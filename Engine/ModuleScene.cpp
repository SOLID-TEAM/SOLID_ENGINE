#include "ModuleScene.h"
#include "GL/glew.h"
#include "Application.h"

#include "C_Transform.h"
#include "Viewport.h"
#include "CameraEditor.h"

#include "ImGuizmo/ImGuizmo.h"

#include "external/MathGeoLib/include/Math/MathAll.h"

ModuleScene::ModuleScene() 
{
	name.assign("ModuleScene");
}

ModuleScene::~ModuleScene() {}

bool ModuleScene::Init(Config& config)
{
	return true;
}

bool ModuleScene::Start(Config& config)
{
	// GameObjects ---------------------------

	// Root ----------------------
	root_go = new GameObject("Scene Root"); 
	root_go->is_static = true;
	root_go->ignore = true;

	// Main camera ---------------
	main_camera = new GameObject("Main Camera", root_go); 
	main_camera->CreateComponent<C_Camera>();
	main_camera->ignore = true;

	// Editor camera ---------------

	editor_camera = new CameraEditor(); 
	editor_camera->ignore = true;

	// Viewports -----------------------------
	scene_viewport = new Viewport(editor_camera);
	game_viewport = new Viewport(main_camera);

	Load(config);

	return true;
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

update_status ModuleScene::PreUpdate(float dt)	// TODO: SHORTCUTS

{
	// TESTING SAVE SCENE
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		ToSaveScene();
	}
	// check CTRL + Z
	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
	{
		UndoLastDelete();
	}

	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt)
{
	// Update hierarchy -----------------------------------------

	UpdateHierarchy();

	// Update Game Objects Lists -------------------------------

	UpdateGoLists();

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		FillGoLists();
	}

	// Update All GameObjects -----------------------------------

	if (root_go != nullptr)
	{
		std::stack<GameObject*> go_stack;

		go_stack.push(root_go);

		while (!go_stack.empty())
		{
			GameObject* go = go_stack.top();
			go_stack.pop();

			go->DoUpdate(dt);

			for (GameObject* child : go->childs)
			{
				go_stack.push(child);
			}
		}
	}

	editor_camera->DoUpdate(dt);

	// Update Gizmo --------------------------------------------

	UpdateGizmo();

	// Update Space partitioning ------------------------------

	UpdateSpacePartitioning();

	// --------------------------------------------------------

	return UPDATE_CONTINUE;
}

update_status ModuleScene::Draw()
{
	for (Viewport* viewport : viewports)
	{
		if (!viewport->active) continue;
		editor_mode = (viewport  == scene_viewport) ? true : false;

		// Begin -----------------------------------------------

		viewport->BeginViewport();

		// Draw GameObjects / Components -----------------------
		
		go_render_list.push_back(main_camera);

		for (GameObject* go : go_render_list)
		{
			go->DoRender();
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

	// Always clean render list ------------------------------

	go_render_list.clear();

	return UPDATE_CONTINUE;
}

void ModuleScene::UpdateHierarchy()
{
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
}

void ModuleScene::UpdateGoLists()
{
	EventGo event_go;
	GameObject* go = nullptr;

	while (!events_go_stack.empty())
	{
		event_go = events_go_stack.top();
		events_go_stack.pop();

		go = event_go.go;

		if (go == nullptr) continue;

		if (event_go.type == EventGoType::DYNAMIC_TO_STATIC || event_go.type == EventGoType::STATIC_TO_DYNAMIC)
		{
			std::list<GameObject*>& list_from = (event_go.type == EventGoType::DYNAMIC_TO_STATIC) ? (dynamic_go_list) : (static_go_list);
			std::list<GameObject*>& list_to = (event_go.type == EventGoType::DYNAMIC_TO_STATIC)   ? (static_go_list) :  (dynamic_go_list);

			list_from.remove(event_go.go);
			list_to.push_back(event_go.go);
		}
		else if (event_go.type == EventGoType::DELETE_FROM_STATIC)
		{
			static_go_list.remove(event_go.go);
		}
		else if (event_go.type == EventGoType::DELETE_FROM_DYNAMIC)
		{
			dynamic_go_list.remove(event_go.go);
		}
	}
}

void ModuleScene::UpdateGizmo()
{
	if (selected_go != nullptr)
	{
		float4x4 global_transform = selected_go->transform->global_transform.Transposed();
		ImGuizmo::Manipulate(editor_camera->GetViewMatrix().Transposed().ptr(), editor_camera->GetProjectionMatrix().Transposed().ptr(), global_transform.ptr());

		if (!global_transform.Equals(selected_go->transform->global_transform.Transposed()))
		{
			selected_go->transform->SetGlobalTransform(global_transform.Transposed());
		}
	}
}

void ModuleScene::UpdateSpacePartitioning()
{
	uint kdtree_collisions = 0u;
	uint dyntree_collisions = 0u;
	uint frustum_collisions = 0u;

	C_Camera* camera = main_camera->GetComponent<C_Camera>();


	if (camera->cullling)
	{
		// Add dynamic go to render list -----------------------------

		for (GameObject* go : dynamic_go_list)
		{
			if (go->bounding_box.IsFinite())
			{
				camera->CheckCollisionAABB(go->bounding_box) ? go_render_list.push_back(go) : false;
				++frustum_collisions;
			}
		}

		// Add static game objects -----------------------------------

		kdtree.GetIntersections<C_Camera>(*camera, go_render_list, kdtree_collisions);
		frustum_collisions += kdtree_collisions;

	}
	else
	{
		go_render_list.insert(std::end(go_render_list), std::begin(static_go_list), std::end(static_go_list));
		go_render_list.insert(std::end(go_render_list), std::begin(dynamic_go_list), std::end(dynamic_go_list));

	}

	// Test Window --------------------------------------------

	if (ImGui::Begin("Space Partitioning"))
	{
		if (ImGui::Button("Recalculate", ImVec2(100, 20)))
		{
			std::vector<GameObject*> go_vector;

			for (GameObject* go : static_go_list)
			{
				if (go->bounding_box.IsFinite())
				{
					go_vector.push_back(go);
				}

			}

			kdtree.Fill(6, 1, EncloseAllGo(), go_vector);
		}

		ImGui::SameLine();

		if (ImGui::Button("Clear", ImVec2(100, 20)))
		{
			kdtree.Clear();
		}

		if (camera->cullling)
		{
			int static_size = static_go_list.size() - 1;
			static_size = static_size < 0 ? 0 : static_size;
			ImGui::Title("Game Objects"); ImGui::Text("");
			ImGui::Title("Static",2); ImGui::Text("%i", static_size); // -1 Less root go
			ImGui::Title("Dynamic",2); ImGui::Text("%i" , dynamic_go_list.size());
			ImGui::Title("KDTree");	ImGui::Text(( kdtree.Active()) ? "ON" : "OFF");
			ImGui::Title("Checked Collisions");  ImGui::Text("");
			ImGui::Title("Frustum",2);  ImGui::Text("%i", frustum_collisions);
		}
		else
		{
			ImGui::Text("Active Main Camera Culling");
		}

		ImGui::End();
	}
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

void ModuleScene::PushEvent(GameObject* go, EventGoType type)
{
	events_go_stack.push(EventGo(go, type));
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

void ModuleScene::FillGoLists()
{
	std::stack<GameObject*> go_stack;
	go_stack.push(root_go);

	// Clean lists ----------------------------------------------------------
	events_go_stack = std::stack<EventGo>();
	static_go_list.clear();
	dynamic_go_list.clear();

	// Fill lists ----------------------------------------------------------------

	while (!go_stack.empty())
	{
		GameObject* go = go_stack.top();
		go_stack.pop();

		(go->is_static == true) ? static_go_list.push_back(go) : dynamic_go_list.push_back(go);

		for (GameObject* child : go->childs)
		{
			go_stack.push(child);
		}
	}
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

bool ModuleScene::Save(Config& config)
{
	if (editor_camera != nullptr)
	{
		config.AddFloatArray("Camera editor position", editor_camera->transform->position.ptr(), 3);
		config.AddFloatArray("Camera editor rotation", editor_camera->transform->rotation.ptr(), 3);
	}
	else
	{
		config.AddFloatArray("Camera editor position", float3(0.f, 0.f, 0.f).ptr(), 3);
		config.AddFloatArray("Camera editor rotation", float3(0.f, 0.f, 0.f).ptr(), 3);
	}

	return true;
}

void ModuleScene::Load(Config& config)
{
	if (editor_camera != nullptr)
	{
		editor_camera->SetPosition(float3(
			config.GetFloat("Camera editor position", 0.f, 0),
			config.GetFloat("Camera editor position", 0.f, 1),
			config.GetFloat("Camera editor position", 0.f, 2)));

		editor_camera->SetRotation(float3(
			config.GetFloat("Camera editor rotation", 0.f, 0),
			config.GetFloat("Camera editor rotation", 0.f, 1),
			config.GetFloat("Camera editor rotation", 0.f, 2)));
	}
}
