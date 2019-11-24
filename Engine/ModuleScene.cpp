#include "ModuleScene.h"
#include "GL/glew.h"
#include "Application.h"

#include "C_Transform.h"
#include "Viewport.h"
#include "CameraEditor.h"
#include "W_Scene.h"

#include "ImGuizmo/ImGuizmo.h"
#include "IconFontAwesome/IconsFontAwesome5.h"
#include "R_Model.h"
#include "R_Mesh.h"

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

	// Scene ---------------------

	scene_name.assign("untitled");

	// Root ----------------------
	root_go = new GameObject("Scene Root", nullptr, true);
	root_go->uid = 0; // scene root always 0 uid
	root_go->is_static = true;
	root_go->ignore_culling = true;

	// Main camera ---------------

	main_camera = CreateGameObject("Main Camera", root_go, false);
	main_camera->CreateComponent<C_Camera>();
	main_camera->ignore_culling = true;

	// Editor camera ---------------

	editor_camera = new CameraEditor();

	// Viewports -----------------------------

	scene_viewport = new Viewport(editor_camera);
	game_viewport = new Viewport(main_camera);

	Load(config);
	
	// TODO: get the last scene saved, or simply do nothing and let the user load one
	//ToLoadScene(scene_name.c_str());
	// load default scene

	ToLoadScene("default.solidscene");

	return true;
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
	main_camera = nullptr;

	// Clear lists --------------------------------------------------

	static_go_list.clear();
	dynamic_go_list.clear();
	go_to_render.clear();

	// TODO
	/*if (editor_camera != nullptr)
	{
		delete editor_camera;
		editor_camera = nullptr;
	}*/

	return true;
}

update_status ModuleScene::PreUpdate()	// TODO: SHORTCUTS
{

	if (create_new_scene)
	{
		CleanUp();
		create_new_scene = false;
		// Main camera ---------------

		main_camera = CreateGameObject("Main Camera", root_go, false);
		main_camera->CreateComponent<C_Camera>();
		main_camera->ignore_culling = true;
		game_viewport->SetCamera(main_camera);
	}

	if (load_new_scene)
	{
		LoadSceneNow();
		load_new_scene = false;
	}

	// check CTRL + Z
	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_Z) == KEY_DOWN)
	{
		UndoLastDelete();
	}

	return UPDATE_CONTINUE;
}

update_status ModuleScene::Update()
{
	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		App->scene->ToSaveScene("backup_scene", LIBRARY_SETTINGS_FOLDER);
	}

	// Update hierarchy -----------------------------------------

	UpdateHierarchy();

	// Update Game Objects Lists -------------------------------

	UpdateGoLists();

	// Update All GameObjects -----------------------------------

	if (editor_camera) editor_camera->DoUpdate();

	if (root_go != nullptr)
	{
		std::stack<GameObject*> go_stack;

		go_stack.push(root_go);

		while (!go_stack.empty())
		{
			GameObject* go = go_stack.top();
			go_stack.pop();

			go->DoUpdate();

			for (GameObject* child : go->childs)
			{
				go_stack.push(child);
			}
		}
	}

	// Update Gizmo --------------------------------------------

	UpdateGizmo();

	// Update Space partitioning ------------------------------

	UpdateSpacePartitioning();

	// --------------------------------------------------------

	UpdateMousePicking();

	// Update render list ------------------------------------- (Pre Render/Draw Function ?)

	UpdateGoToRender();

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

		for (GameObject* go : go_to_render)
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
 			if (render_kdtree && camera->cullling && App->time->game_state != GameState::STOP)
				App->renderer3D->RenderKDTree(kdtree, 3.f);
		}

		// End ------------------------------------------------

		viewport->EndViewport();
	}

	// Always clean render list ------------------------------

	debug_renders.clear();
	go_to_render.clear();

	return UPDATE_CONTINUE;
}

void ModuleScene::UpdateMousePicking()
{
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && IsGizmoActived() == false && App->editor->w_scene->is_focused )
	{
 		std::vector< GameObject*> intersections;
		float2 screen_point(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
		C_Camera* camera = editor_camera->GetComponent<C_Camera>();

		if (!scene_viewport->ScreenPointToViewport(screen_point)) 
		{
 			return; // Point not contained in viewport pixel size
		}

		LineSegment& ray = camera->ViewportPointToRay(screen_point);
		uint check = 0;

		// Get all static ray intersections -------------------------------------
	
		kdtree.GetIntersections(ray, intersections, check);

		// Get all dynamic ray intersections ------------------------------------

		for (GameObject* go : dynamic_go_list)
		{
			if (go->bounding_box.IsFinite() && go->bounding_box.Intersects(ray))
			{
				intersections.push_back(go);
			}
		}

		// Check all meshes -----------------------------------------------------

		LineSegment local_ray;
		Triangle triangle;
		float* vertices = nullptr;
		uint* indices = nullptr;
		uint indx = 0u;
		R_Mesh* res = nullptr;
		C_Mesh* mesh = nullptr;
		float triangle_dist = 100.f;
		float near_triangle_dist = triangle_dist;
		GameObject* near_go = nullptr;

		for (GameObject* go : intersections)
		{
			mesh = go->GetComponent<C_Mesh>();
			if (mesh != nullptr) res = (R_Mesh*)App->resources->Get(mesh->GetResource());
			if (res == nullptr) 
				continue;

			local_ray = ray;
			local_ray.Transform(go->transform->GetGlobalTransform().Inverted());
			vertices = res->vertices;
			indices = res->indices;

			for (uint i = 0u; i < res->buffers_size[R_Mesh::INDICES]; i += 3u)
			{
				// Set triangle -------------------------------------------------
				indx = indices[i];
				triangle.a = (float3)&vertices[indx *3];
				indx = indices[i + 1u];
				triangle.b = (float3)&vertices[indx * 3];
				indx = indices[i + 2u];
				triangle.c = (float3)&vertices[indx * 3];

				// Check intersection -------------------------------------------
				float3 hit_point;

				if (local_ray.Intersects(triangle, &triangle_dist, &hit_point))
				{
					hit_point = go->transform->GetGlobalTransform().MulPos(hit_point);

					if (triangle_dist < near_triangle_dist)
					{
						near_go = go;
						near_triangle_dist = triangle_dist;
					}
				}
			}
		}

		selected_go = near_go;
	}
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

		if (event_go.go == nullptr) continue;

		if (event_go.type == EventGoType::ADD_TO_STATIC)
		{
			static_go_list.push_back(event_go.go);
			update_kdtree = true;
		}
		else if (event_go.type == EventGoType::ADD_TO_DYNAMIC)
		{
			dynamic_go_list.push_back(event_go.go);
		}
		else if (event_go.type == EventGoType::DELETE_FROM_STATIC)
		{
			static_go_list.remove(event_go.go);
			update_kdtree = true;
		}
		else if (event_go.type == EventGoType::DELETE_FROM_DYNAMIC)
		{
			dynamic_go_list.remove(event_go.go);
		}
		else if (event_go.type == EventGoType::DYNAMIC_TO_STATIC || event_go.type == EventGoType::STATIC_TO_DYNAMIC)
		{
			std::list<GameObject*>& list_from = (event_go.type == EventGoType::DYNAMIC_TO_STATIC) ? (dynamic_go_list) : (static_go_list);
			std::list<GameObject*>& list_to = (event_go.type == EventGoType::DYNAMIC_TO_STATIC) ? (static_go_list) : (dynamic_go_list);

			list_from.remove(event_go.go);
			list_to.push_back(event_go.go);
			update_kdtree = true;
		}
	}

	dynamic_go_list.unique();
	static_go_list.unique();

}

void ModuleScene::UpdateGizmo()
{
	if (selected_go != nullptr)
	{
		bool enable_gizmo;
		
		if (App->time->game_state != GameState::STOP)
		{
			enable_gizmo = !selected_go->is_static;
		}
		else
		{
			enable_gizmo = true;
		}
	
		ImGuizmo::Enable(enable_gizmo);

		float4x4 global_transform = selected_go->transform->global_transform.Transposed();
		ImGuizmo::Manipulate(editor_camera->GetViewMatrix().Transposed().ptr(), editor_camera->GetProjectionMatrix().Transposed().ptr(), global_transform.ptr());

		if (enable_gizmo && !global_transform.Equals(selected_go->transform->global_transform.Transposed()))
		{
			selected_go->transform->SetGlobalTransform(global_transform.Transposed());
		}
	}
}

void ModuleScene::UpdateSpacePartitioning()
{
	kdtree_collisions = 0u;
	dyntree_collisions = 0u;
	frustum_collisions = 0u;

	// Update kdtree for static game objects --------------------------

	if (update_kdtree == true)
	{
		go_ignore_culling.clear();

		std::vector<GameObject*> go_vector;

		for (GameObject* go : static_go_list)
		{
			if (go->ignore_culling)
			{
				go_ignore_culling.push_back(go);
			}
			else if (go->bounding_box.IsFinite())
			{
				go_vector.push_back(go);
			}
		}

		kdtree.Fill(4, 2, EncloseAllStaticGo(), go_vector);
		update_kdtree = false;
	}
}

void ModuleScene::UpdateGoToRender()
{
	// Fill render list ----------------------------------------------

	C_Camera* camera = main_camera->GetComponent<C_Camera>();

	if (camera->cullling && App->time->game_state != GameState::STOP) // Check if go is colliding with main camera frustum optimizing the game
	{
		// Add dynamic go to render list -----------------------------

		for (GameObject* go : dynamic_go_list)
		{
			if (go->ignore_culling)
			{
				go_to_render.push_back(go);
			}
			else if (go->bounding_box.IsFinite())
			{
				camera->CheckCollisionAABB(go->bounding_box) ? go_to_render.push_back(go) : false;
				++frustum_collisions;
			}
		}

		// Add static game objects -----------------------------------

		kdtree.GetIntersections<C_Camera>(*camera, go_to_render, kdtree_collisions);
		frustum_collisions += kdtree_collisions;

		go_to_render.insert(go_to_render.end(), go_ignore_culling.begin(), go_ignore_culling.end()); // Merge go ignore culling
	}
	else // Directly render all go without any optimization
	{
		go_to_render.insert(go_to_render.end(), static_go_list.begin(), static_go_list.end()); // Merge go ignore culling
		go_to_render.insert(go_to_render.end(), dynamic_go_list.begin(), dynamic_go_list.end()); // Merge go ignore culling
	}

	// Test Window --------------------------------------------

	if (ImGui::Begin("Space Partitioning"))
	{
		ImGui::Title("Game Objects"); ImGui::Text("");
		ImGui::Title("Static", 2); ImGui::Text("%i", static_go_list.size());
		ImGui::Title("Dynamic", 2); ImGui::Text("%i", dynamic_go_list.size());
		ImGui::Title("Rendering", 2); ImGui::Text("%i", go_to_render.size());

		ImGui::Spacing();


		if (camera->cullling)
		{
			ImGui::Separator();

			ImGui::Title("KDTree");	ImGui::Text((kdtree.Active()) ? "ON" : "OFF");
			ImGui::Title("Render", 2); ImGui::Checkbox("##render_kdtree", &render_kdtree);
			ImGui::Title("Checked Collisions");  ImGui::Text("");
			ImGui::Title("Frustum", 2);  ImGui::Text("%i", frustum_collisions);
		}
		else
		{
			ImGui::Title("Active Camera Culling", 2);  ImGui::Text("");
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

GameObject* ModuleScene::CreateGameObject(std::string name , GameObject* parent, bool is_static)
{
	GameObject* new_go = nullptr;

	if (parent == nullptr)
	{
		parent = root_go;
	}

	new_go = new GameObject(name , parent, is_static);
	PushEvent(new_go, (is_static) ? EventGoType::ADD_TO_STATIC : EventGoType::ADD_TO_DYNAMIC);

	return new_go;
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

void ModuleScene::PushDebugRender(DebugRender debug_render)
{
	debug_renders.push_back(debug_render);
}

AABB ModuleScene::EncloseAllStaticGo()
{
	AABB global_aabb;
	global_aabb.SetNegativeInfinity();

	for (GameObject* go : static_go_list)
	{
		if (go->bounding_box.IsFinite())
		{
			global_aabb.Enclose(go->bounding_box);
		}
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

		if (go->ignore_culling == false)
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

bool ModuleScene::IsGizmoActived()
{
	return (ImGuizmo::IsOver() || ImGuizmo::IsUsing());
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
// TODO: add a intermediate pass for wait to order module interests and then save
bool ModuleScene::ToSaveScene(const char* name, const char* destination_path)
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

	new_scene_save.SaveConfigToFile((destination_path + scene_name).c_str());

	

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


bool ModuleScene::SaveScene(Config& config, GameObject* go)
{
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
		GameObject* new_go = new GameObject("Unknown", root_go, false);
		new_go->Load(scene.GetArray("GameObjects", i), relations);
		PushEvent(new_go, (new_go->is_static) ? EventGoType::ADD_TO_STATIC : EventGoType::ADD_TO_DYNAMIC);

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
	scene_to_load.clear();

	return LoadScene(to_load);;
}

// TODO: pass this on finishupdate on app.cpp
bool ModuleScene::ToLoadScene(const char* name, const char* source_path, bool clean)
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
		App->file_sys->GetAllFilesWithExtension(source_path, "solidscene", file_list);

		for (uint i = 0; i < file_list.size(); ++i)
		{
			if (file_list[i].compare(name) == 0)
			{
				scene_to_load.assign(name);
				if(clean)
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

		// Set transform -------------------------------

		new_go->transform->SetLocalTransform(node.transform);

		if (node.mesh > 0)
		{
			C_Mesh* c_mesh = new_go->CreateComponent< C_Mesh>();
			
			if (c_mesh->SetMeshResource(node.mesh))
			{
				new_go->CreateComponent<C_MeshRenderer>();
			}
		}

		if (node.material > 0)
		{
			C_Material* c_mat = new_go->CreateComponent< C_Material>();
			c_mat->SetMaterialResource(node.material);
			/*if (c_mat->SetMaterialResource(node.material))
			{
				new_go->CreateComponent<C_MeshRenderer>();
			}*/
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

