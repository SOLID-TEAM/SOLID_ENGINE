#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleTime.h"
#include "Config.h"

#include "GL/glew.h"
#include "SDL/include/SDL_opengl.h"

#include "ImGui/imgui.h"
#include "IconFontAwesome/IconsFontAwesome5.h"
#include "ImGuizmo/ImGuizmo.h"
#include "ImGui/Impl/imgui_impl_sdl.h"
#include "ImGui/Impl/imgui_impl_opengl3.h"
#include "ImGui/misc/cpp/imgui_stdlib.h" // input text wrapper for std::string

// Windows include 

#include "W_Config.h"
#include "W_Console.h"
#include "W_Hierarchy.h"
#include "W_Rendering.h"
#include "W_Scene.h"
#include "W_Game.h"
#include "W_Inspector.h"
#include "W_Primitives.h"
#include "W_DeleteHistory.h"

#include "C_Transform.h"


ModuleEditor::ModuleEditor(bool start_enabled) : Module(start_enabled)
{
	name.assign("Editor");
}

ModuleEditor::~ModuleEditor()
{
	ImGui_ImplSDL2_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
}

bool ModuleEditor::Init(Config& config)
{
	// Load values from json
	Load(config);

	// DEAR IMGUI SETUP ---------------------------------------------------------- 
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	// ImGui Style ----------------------------------------

	ImGuiStyle& style = ImGui::GetStyle();

	// Panels Style ----------------------------------
	style.MaxColumnSeparation = DFT_COLUMN_SEP;
	style.TitleSeparation = DFT_TITLE_SEP;
	style.SubTitleSeparation = DFT_SUB_TITLE_SEP;
	style.SeparationType = ImGuiSeparationType::ImGui_WindowSeparation;
	// -----------------------------------------------
	// TODO: rework individual window size constraints
	//style.WindowMinSize = ImVec2(240.f, 278.f);//ImVec2(278.f, 278.f);
	style.WindowRounding = 0.0f;// <- Set this on init or use ImGui::PushStyleVar()
	style.ChildRounding = 0.0f;
	style.FrameRounding = 0.0f;
	style.GrabRounding = 0.0f;
	style.PopupRounding = 0.0f;
	style.ScrollbarRounding = 0.0f;
	style.Colors[ImGuiCol_::ImGuiCol_Separator] = ImVec4(0.1f, 0.1f, 0.1f, 1.f);

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
	io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	return true;
}

// Load assets
bool ModuleEditor::Start(Config& config)
{
	LOG("[Start] Loading Editor");
	bool ret = true;

	// Initial Windows -------------------------------------

	w_config =			new W_Config("Configuration",			false);
	w_console =			new W_Console("Console",				false);
	w_hierarchy =		new W_Hierarchy("Hierarchy",			false);
	w_rendering =		new W_Rendering("Rendering Settings",	false);
	w_scene =			new W_Scene("Scene",					false);
	w_game =			new W_Game("Game",						false);
	w_inspector =		new W_Inspector("Inspector",			false);
	w_primitives =		new W_Primitives("Primitives",			false);
	w_delete_history =	new W_DeleteHistory("Delete History",	false);

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontDefault();
	
	ImFontConfig configu;
	configu.MergeMode = true;
	configu.GlyphMinAdvanceX = 13.f;
	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	icons_font = io.Fonts->AddFontFromFileTTF("Assets/Fonts/solid_icons.ttf", 13.0f, &configu, icon_ranges);

	Load(config);

	return ret;
}

// Load assets
bool ModuleEditor::CleanUp()
{
	LOG("[CleanUp] Unloading Editor");

	// Windows ------------------------------------------------------

	for (std::vector<Window*>::iterator itr = windows.begin(); itr != windows.end(); ++itr)
	{
		(*itr)->CleanUp();
		RELEASE((*itr));			// Delete object 
	}

	windows.clear();

	w_config = nullptr;
	w_console = nullptr;
	w_hierarchy = nullptr;
	w_rendering = nullptr;
	w_scene = nullptr;
	w_game = nullptr;
	w_inspector = nullptr;
	w_primitives = nullptr;
	w_delete_history = nullptr;

	// Debug Data ---------------------------------------------------

	if (ddmesh != nullptr)
	{
		ddmesh->Clean();
		delete ddmesh;
	}

	return true;
}

void ModuleEditor::AddWindow(Window* window_to_add)
{
	windows.push_back(window_to_add);
}

void ModuleEditor::DestroyWindow(Window* panel_to_destroy)
{
	std::vector<Window*>::iterator itr = std::find(windows.begin(), windows.end(), panel_to_destroy);

	if (itr != windows.end())
	{
		(*itr)->CleanUp();
		RELEASE((*itr));			// Delete object 
		windows.erase(itr);         // Delete iterator inside windows vector
		panel_to_destroy = nullptr;
	}
	else
	{
		LOG("[Error] Editor::DestroyWindow(): Window not found");
	}
}


update_status ModuleEditor::PreUpdate()
{
	// ImGui Internal System ------------------------------

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	// Current Gizmo Operation ----------------------------

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
	{
		ImGuizmo::SetOperation(Operation::TRANSLATE);
	}
	else if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
	{
		ImGuizmo::SetOperation(Operation::ROTATE);
	}
	else if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
	{
		ImGuizmo::SetOperation(Operation::SCALE);
	}

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		ImGuizmo::SetMode(Mode::LOCAL);
	}
	else if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		ImGuizmo::SetMode(Mode::WORLD);
	}

	return UPDATE_CONTINUE;
}

// Update
update_status ModuleEditor::Update()
{
	
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Draw()
{
	static bool dock_space = true;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_::ImGuiDockNodeFlags_PassthruCentralNode;
	static ImGuiWindowFlags window_flags =  ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	// Main Menu Bar --------------------------------------

	if (!DrawMainMenuBar())
		return update_status::UPDATE_STOP;

	// ----------------------------------------------------

	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos({ viewport->Pos.x, viewport->Pos.y + 19 });
	ImGui::SetNextWindowSize({ viewport->Size.x, 15 });
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4.f, 5.f));
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_WindowBg , ImVec4(0.1f, 0.1f, 0.1f,1.f));
	
	// Controller Bar -------------------------------------------

	float line_cursor_x = 0.f;
	float button_s_w = 30.f;
	float button_l_w = 80.f;
	float button_h = 22.f;
	float padding = 3.f;

	ImGui::Begin("ControllerBar", 0, window_flags);

	ImVec2 win_size = ImGui::GetContentRegionAvail();
	float win_center = win_size.x * 0.5f;

	ImGui::NewLine();
	line_cursor_x += 15;
	ImGui::SameLine(line_cursor_x);
	ImGui::Button(ICON_FA_HAND_PAPER, { button_s_w, button_h });
	line_cursor_x += button_s_w + padding;
	ImGui::SameLine(line_cursor_x);
	ImGui::Button(ICON_FA_ARROWS_ALT, { button_s_w, button_h });
	line_cursor_x += button_s_w + padding;
	ImGui::SameLine(line_cursor_x);
	ImGui::Button(ICON_FA_SYNC, { button_s_w, button_h });
	line_cursor_x += button_s_w + padding;
	ImGui::SameLine(line_cursor_x);
	ImGui::Button(ICON_FA_EXPAND_ARROWS_ALT, { button_s_w, button_h });
	line_cursor_x += button_s_w + padding * 8;
	ImGui::SameLine(line_cursor_x);
	ImGui::Button(ICON_FA_CUBE " Local", { button_l_w, button_h });
	line_cursor_x += button_l_w + padding;
	ImGui::SameLine(line_cursor_x);
	ImGui::Button(ICON_FA_GLOBE " Global", { button_l_w, button_h });
	line_cursor_x = win_center - ( (button_s_w + padding) * 3.f ) * 0.5f;
	ImGui::SameLine(line_cursor_x);

	if (App->time->GetGameState() != GameState::STOP)
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.f));

		if (ImGui::Button(ICON_FA_STOP, { button_s_w, button_h }))
		{
			App->time->StopGame();
		}

		ImGui::PopStyleColor();
	}
	else
	{
		if (ImGui::Button(ICON_FA_PLAY, { button_s_w, button_h }))
		{
			App->time->StartGame();
		}
	}

	line_cursor_x += button_s_w + padding;
	ImGui::SameLine(line_cursor_x);

	if (App->time->GetGameState() == GameState::PAUSE)
	{
		ImGui::PushStyleColor(ImGuiCol_::ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.f));

		if (ImGui::Button(ICON_FA_PAUSE, { button_s_w, button_h }))
		{
			App->time->ResumeGame();
		}

		ImGui::PopStyleColor();
	}
	else
	{
		if (ImGui::Button(ICON_FA_PAUSE, { button_s_w, button_h }))
		{
			App->time->PauseGame();
		}
	}

	line_cursor_x += button_s_w + padding;
	ImGui::SameLine(line_cursor_x);

	if (ImGui::Button(ICON_FA_STEP_FORWARD, { button_s_w, button_h }))
	{
		App->time->StepFowrardGame();
	}

	line_cursor_x += button_s_w + padding * 8;
	ImGui::SameLine(line_cursor_x);

	ImGui::Text("Time : %f", App->time->time_since_load);

	ImGui::End();

	ImGui::PopStyleVar(4);
	ImGui::PopStyleColor();

	// DockSpace ------------------------------------------

	ImGui::SetNextWindowPos({ viewport->Pos.x, viewport->Pos.y + 50 });
	ImGui::SetNextWindowSize({ viewport->Size.x, viewport->Size.y - 50 });
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	// Begin Dock space ------------------------------------

	ImGui::Begin("DockSpaceWindow", &dock_space, window_flags);
	DrawControllerBar();
	ImGui::PopStyleVar(3);

	// DockSpace Setter -----------------------------------

	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

	// Draw all windows ----------------------------------- 

	for (Window* window : windows)
	{
		if (window->active)
		{
			window->Draw();
		}
	}

	// Draw other elemnts ---------------------------------

	DrawPopUps();

	if (show_demo_imgui) ImGui::ShowDemoWindow(&show_demo_imgui);

	// End DockSpace --------------------------------------

	ImGui::End();

	// ImGui Internal System ------------------------------

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}

	return UPDATE_CONTINUE;
}

// In your own code you may want to display an actual icon if you are using a merged icon fonts (see misc/fonts/README.txt)
void ModuleEditor::HelpMarker(const char* desc) const
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		//ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		//ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

bool ModuleEditor::Save(Config& config)
{
	bool ret = true;

	ret = config.AddBool("show_about_popup", show_about_popup);
	ret = config.AddBool("show_demo_imgui", show_demo_imgui);

	for (std::vector<Window*>::iterator itr = windows.begin(); itr != windows.end(); ++itr)
	{
		config.AddBool( std::string("window_active_" + (*itr)->name).c_str() , (*itr)->active);
	}

	// DEBUG VIEWPORT OPTIONS

	ret = config.AddBool("debug_vertex_normals", viewport_options.debug_vertex_normals);
	ret = config.AddBool("debug_face_normals", viewport_options.debug_face_normals);
	ret = config.AddBool("debug_bounding_boxes", viewport_options.debug_bounding_box);

	ret = config.AddFloatArray("bb_color", (float*)&viewport_options.bb_color, 4);
	ret = config.AddFloatArray("wire_color", (float*)&viewport_options.wire_color, 4);
	ret = config.AddFloatArray("d_vertex_p_color", (float*)&viewport_options.d_vertex_p_color, 4);
	ret = config.AddFloatArray("d_vertex_l_color", (float*)&viewport_options.d_vertex_l_color, 4);
	ret = config.AddFloatArray("d_vertex_face_color", (float*)&viewport_options.d_vertex_face_color, 4);
	ret = config.AddFloatArray("d_vertex_face_n_color", (float*)&viewport_options.d_vertex_face_n_color, 4);

	ret = config.AddFloat("bb_line_width", viewport_options.bb_line_width);
	ret = config.AddFloat("wire_line_width", viewport_options.wire_line_width);
	ret = config.AddFloat("v_point_size", viewport_options.v_point_size);
	ret = config.AddFloat("f_v_point_size", viewport_options.f_v_point_size);
	ret = config.AddFloat("v_n_line_width", viewport_options.v_n_line_width);
	ret = config.AddFloat("f_n_line_width", viewport_options.f_n_line_width);

	ret = config.AddFloat("v_n_line_length", viewport_options.v_n_line_length);
	ret = config.AddFloat("f_n_line_length", viewport_options.f_n_line_length);

	return ret;
}

void ModuleEditor::Load(Config& config)
{
	show_about_popup = config.GetBool("show_about_popup", show_about_popup);
	show_demo_imgui = config.GetBool("show_demo_imgui", show_demo_imgui);

	for (std::vector<Window*>::iterator itr = windows.begin(); itr != windows.end(); ++itr)
	{
		(*itr)->active = config.GetBool(std::string("window_active_" + (*itr)->name).c_str(), &(*itr)->active);
	}
	// viewport options
	
	viewport_options.debug_vertex_normals = config.GetBool("debug_vertex_normals", viewport_options.debug_vertex_normals);
	viewport_options.debug_face_normals = config.GetBool("debug_face_normals", viewport_options.debug_face_normals);
	viewport_options.debug_bounding_box = config.GetBool("debug_bounding_boxes", viewport_options.debug_bounding_box);


	// load colors ----------------------------------------------------------------------------------
	viewport_options.bb_color.x = config.GetFloat("bb_color", viewport_options.wire_color.x, 0);
	viewport_options.bb_color.y = config.GetFloat("bb_color", viewport_options.wire_color.y, 1);
	viewport_options.bb_color.z = config.GetFloat("bb_color", viewport_options.wire_color.z, 2);
	viewport_options.bb_color.w = config.GetFloat("bb_color", viewport_options.wire_color.w, 3);

	viewport_options.wire_color.x = config.GetFloat("wire_color", viewport_options.wire_color.x, 0);
	viewport_options.wire_color.y = config.GetFloat("wire_color", viewport_options.wire_color.y, 1);
	viewport_options.wire_color.z = config.GetFloat("wire_color", viewport_options.wire_color.z, 2);
	viewport_options.wire_color.w = config.GetFloat("wire_color", viewport_options.wire_color.w, 3);
	
	viewport_options.d_vertex_p_color.x = config.GetFloat("d_vertex_p_color", viewport_options.d_vertex_p_color.x, 0);
	viewport_options.d_vertex_p_color.y = config.GetFloat("d_vertex_p_color", viewport_options.d_vertex_p_color.y, 1);
	viewport_options.d_vertex_p_color.z = config.GetFloat("d_vertex_p_color", viewport_options.d_vertex_p_color.z, 2);
	viewport_options.d_vertex_p_color.w = config.GetFloat("d_vertex_p_color", viewport_options.d_vertex_p_color.w, 3);
	
	viewport_options.d_vertex_l_color.x = config.GetFloat("d_vertex_l_color", viewport_options.d_vertex_l_color.x, 0);
	viewport_options.d_vertex_l_color.y = config.GetFloat("d_vertex_l_color", viewport_options.d_vertex_l_color.y, 1);
	viewport_options.d_vertex_l_color.z = config.GetFloat("d_vertex_l_color", viewport_options.d_vertex_l_color.z, 2);
	viewport_options.d_vertex_l_color.w = config.GetFloat("d_vertex_l_color", viewport_options.d_vertex_l_color.w, 3);

	viewport_options.d_vertex_face_color.x = config.GetFloat("d_vertex_face_color", viewport_options.d_vertex_face_color.x, 0);
	viewport_options.d_vertex_face_color.y = config.GetFloat("d_vertex_face_color", viewport_options.d_vertex_face_color.y, 1);
	viewport_options.d_vertex_face_color.z = config.GetFloat("d_vertex_face_color", viewport_options.d_vertex_face_color.z, 2);
	viewport_options.d_vertex_face_color.w = config.GetFloat("d_vertex_face_color", viewport_options.d_vertex_face_color.w, 3);
	
	viewport_options.d_vertex_face_n_color.x = config.GetFloat("d_vertex_face_n_color", viewport_options.d_vertex_face_color.x, 0);
	viewport_options.d_vertex_face_n_color.y = config.GetFloat("d_vertex_face_n_color", viewport_options.d_vertex_face_color.y, 1);
	viewport_options.d_vertex_face_n_color.z = config.GetFloat("d_vertex_face_n_color", viewport_options.d_vertex_face_color.z, 2);
	viewport_options.d_vertex_face_n_color.w = config.GetFloat("d_vertex_face_n_color", viewport_options.d_vertex_face_color.w, 3);
	// -----------------------------------------------------------------------------------------------

	viewport_options.wire_line_width = config.GetFloat("wire_line_width", viewport_options.wire_line_width);
	viewport_options.bb_line_width = config.GetFloat("bb_line_width", viewport_options.bb_line_width);
	viewport_options.v_point_size = config.GetFloat("v_point_size", viewport_options.v_point_size);
	viewport_options.f_v_point_size = config.GetFloat("f_v_point_size", viewport_options.f_v_point_size);
	viewport_options.v_n_line_width = config.GetFloat("v_n_line_width", viewport_options.v_n_line_width);
	viewport_options.f_n_line_width = config.GetFloat("f_n_line_width", viewport_options.f_n_line_width);


	// if we have meshes, and line length doesnt match, recompute normals ----
	//float temp_v = viewport_options.v_n_line_length;
	//viewport_options.v_n_line_length = config.GetFloat("v_n_line_length", viewport_options.v_n_line_length);
	//if (temp_v != viewport_options.v_n_line_length && ddmesh != nullptr && App->scene->selected_go != nullptr)
	//{
	//	 ddmesh->ComputeVertexNormals(App->scene->selected_go->GetMeshes(),viewport_options.v_n_line_length);
	//	 ddmesh->FillVertexBuffer();
	//}

	//float temp_f = viewport_options.f_n_line_length;
	//viewport_options.f_n_line_length = config.GetFloat("f_n_line_length", viewport_options.f_n_line_length);
	//if (temp_f != viewport_options.f_n_line_length && ddmesh != nullptr && App->scene->selected_go != nullptr)
	//{
	//	ddmesh->ComputeFacesNormals(App->scene->selected_go->GetMeshes(),viewport_options.f_n_line_length);
	//	ddmesh->FillFacesBuffer();
	//}
	// -----------------------------------------------------------------------


}


bool ModuleEditor::DrawMainMenuBar()
{
	bool ret = true;

	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Load editor configuration", "Ctrl+L"))
		{
			App->WantToLoad();
			LOG("[Info] Succesfully loaded last valid config from memory");

		}

		if (ImGui::MenuItem("Save editor configuration", "Ctrl+S"))
		{
			if (App->WantToSave())
			{
				LOG("[Info] %s succesfully saved", App->config_filename.data());
			}
		}

		if (ImGui::MenuItem("New scene", ""))
		{
			show_confirmation_new = true;
		}

		if (ImGui::MenuItem("Save scene", ""))
		{
			show_save_scene = true;
			//App->scene->ToSaveScene();
		}

		if (ImGui::BeginMenu("Load scene", ""))
		{
			
			// TODO: let the user explore directories and select the scene file

			// get all scenes in assets (for now)
			std::vector<std::string> all_scenes;
			App->file_sys->GetAllFilesWithExtension(ASSETS_FOLDER, "solidscene", all_scenes);

			for (uint i = 0; i < all_scenes.size(); ++i)
			{
				ImGui::MenuItem(all_scenes[i].c_str());

				if (ImGui::IsItemClicked())
				{
					// SHOW CONFIRMATION TO DISCARD CURRENT SCENE, then load new scene
					scene_to_load.assign(all_scenes[i]);
					show_confirmation_load = true;
				}
			}

			
			ImGui::EndMenu();
			
			//App->scene->ToLoadScene();
		}

		if (ImGui::MenuItem("Restore editor to default config"))
		{
			show_restore_popup = true;
		}

		if (ImGui::MenuItem("Quit", "ESC"))
			ret = false;
	

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Edit"))
	{
		bool delete_enabled = true;
		bool undo_delete = false;
		if (App->scene->GetUndoDeque().size() > 0) undo_delete = true;
		if (ImGui::MenuItem("undo delete", "Ctrl + z", nullptr, undo_delete))
		{
			App->scene->UndoLastDelete();
		}
		if (App->scene->selected_go == nullptr) delete_enabled = false;
		if (ImGui::MenuItem("delete selected", nullptr, nullptr, delete_enabled))
		{
			App->scene->DeleteGameObject(App->scene->selected_go);
		}
		ImGui::MenuItem("Deleted actions history", NULL, &w_delete_history->active);

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Windows"))
	{
		for (std::vector<Window*>::iterator itr = windows.begin(); itr != windows.end(); ++itr)
		{
			ImGui::MenuItem((*itr)->name.c_str(), NULL, &(*itr)->active);
		}

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("GameObject"))
	{
		/*if (ImGui::MenuItem("Load editor windows states", "Ctrl+Alt+L"))
			LoadEditorConfig(editor_filename.data());*/
			//if (ImGui::MenuItem("Save editor windows states", "Ctrl+Alt+S"))
				//SaveEditorConfig(editor_filename.data());

		ImGui::MenuItem("Primitives", NULL, &w_primitives->active);

		ImGui::EndMenu();
	}

	/*if (ImGui::BeginMenu("View"))
	{
		ImGui::MenuItem("Configuration", NULL, &w_config->active);
		ImGui::MenuItem("Console Log", NULL, &w_console->active);

		ImGui::EndMenu();
	}*/

	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("Gui Demo"))
			show_demo_imgui = !show_demo_imgui;

		// TODO: update links
		if (ImGui::MenuItem("Documentation"))
			App->RequestBrowser("https://github.com/SOLID-TEAM/SOLID_ENGINE");

		if (ImGui::MenuItem("Download latest version"))
			App->RequestBrowser("https://github.com/SOLID-TEAM/SOLID_ENGINE");

		if (ImGui::MenuItem("Report a bug!"))
			App->RequestBrowser("https://github.com/SOLID-TEAM/SOLID_ENGINE");

		if (ImGui::MenuItem("About"))
			show_about_popup = !show_about_popup;

		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();

	return ret;
}

void ModuleEditor::DrawControllerBar()
{
	
}

void ModuleEditor::DrawPopUps()
{
	if (show_confirmation_new)
	{
		ImGui::OpenPopup("confirmation_new");
		if (ImGui::BeginPopupModal("confirmation_new", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("All your changes to current scene will be deleted");
			ImGui::Text("Are you sure?");
			if (ImGui::Button("Yes", ImVec2(120, 0)))
				{
					show_confirmation_new = false;
					App->scene->NewScene();
					ImGui::CloseCurrentPopup();
				}
			ImGui::SameLine();
			if (ImGui::Button("No", ImVec2(120, 0)))
			{
				show_confirmation_new = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	if (show_confirmation_load)
	{
		ImGui::OpenPopup("confirmation");
		if(ImGui::BeginPopupModal("confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			//ImGui::TextWrapped("Are you sure you want to load %s ?, your changes on current scene will be discarded", scene_to_load.c_str());
			ImGui::Text("Are you sure you want to load %s ?", scene_to_load.c_str()); 
			ImGui::TextColored({ 1.0f,0.2f,0.2f,1.0f }, "Your changes on current scene '%s' will be discarded", App->scene->GetSceneName().c_str());
			ImGui::Separator();
			if (ImGui::Button("Yes, load", ImVec2(120, 0))) 
			{
				show_confirmation_load = false;
				// TODO: delete scene, then load
				App->scene->ToLoadScene(scene_to_load.c_str());
				scene_to_load.clear();
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("No", ImVec2(120, 0))) 
			{ 
				show_confirmation_load = false; 
				scene_to_load.clear(); 
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	if (show_save_scene)
	{
		ImGui::OpenPopup("Save scene");
		if(ImGui::BeginPopupModal("Save scene", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			std::string input_name;
			input_name.assign(App->scene->GetSceneName()); // TODO: fix this, name static
			static std::string temp_name = input_name;
			ImGui::InputText("name for .solidscene", &temp_name);
			ImGui::Separator();

			if (ImGui::Button("Save", ImVec2(120, 0))) 
			{ 
				// TODO: check for any possible name collision and re-popup are you sure
				App->scene->ToSaveScene(temp_name.c_str());
				ImGui::CloseCurrentPopup(); 
				show_save_scene = false;  
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); show_save_scene = false; temp_name = input_name; }
			ImGui::SameLine();

			ImGui::EndPopup();
		}

	}

	if (show_restore_popup)
	{
		ImGui::OpenPopup("Are you sure?");
		if (ImGui::BeginPopupModal("Are you sure?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("All editor modificable values are going to reset\n"
				"after this, you are still capable to revert this\n"
				"change by loading config editor before save them");
			ImGui::Separator();
			if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); show_restore_popup = false; App->WantToLoad(true); }
			ImGui::SameLine();
			if (ImGui::Button("OK and save", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); show_restore_popup = false; App->WantToLoad(true); App->WantToSave(); }
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); show_restore_popup = false; }
			ImGui::EndPopup();
		}
	}

	if (show_about_popup)
	{
		ImGui::OpenPopup("About SOLID Engine");

		if (ImGui::BeginPopupModal("About SOLID Engine", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("v0.1 - september 2019\n");
			ImGui::Separator();
			ImGui::BulletText("SOLID ENGINE is a project with the purpose of creating \n"
				"a fully functional video game engine with its own innovations and features\n"
				"implemented by SOLID TEAM. We are 2 Video Game Design and Development Degree students.");
			ImGui::Separator();
			
			ShowSoftwareInfo();

			if(ImGui::CollapsingHeader("Show Application License"))
			{
				ImGui::TextColored({ 1.0f,0.0f,1.0f,1.0f },
					"MIT License\n\n"

					"Copyright(c) 2019 SOLID TEAM\n\n"

					"Permission is hereby granted, free of charge, to any person obtaining a copy\n"
					"of this softwareand associated documentation files(the 'Software'), to deal\n"
					"in the Software without restriction, including without limitation the rights\n"
					"to use, copy, modify, merge, publish, distribute, sublicense, and /or sell\n"
					"copies of the Software, and to permit persons to whom the Software is\n"
					"furnished to do so, subject to the following conditions :\n\n"

					"The above copyright noticeand this permission notice shall be included in all\n"
					"copies or substantial portions of the Software.\n\n"

					"THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
					"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
					"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE\n"
					"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
					"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
					"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
					"SOFTWARE.");
			}

			ImVec2 buttonSize = { 120.0f, 0.f };
			ImGuiStyle& style = ImGui::GetStyle();
			ImGui::Indent(ImGui::GetWindowWidth() * 0.5f - (buttonSize.x * 0.5f) - style.WindowPadding.x);
			if (ImGui::Button("OK", buttonSize)) { ImGui::CloseCurrentPopup(); show_about_popup = false; }

		}
		ImGui::EndPopup();
	}
}

void ModuleEditor::ShowSoftwareInfo() const
{
	ImGui::Separator();
	ImGui::Text("3rd Party libs");
	SDL_version sdl_version;
	SDL_GetVersion(&sdl_version);
	ImGui::BulletText("SDL   v%d.%d.%d", sdl_version.major, sdl_version.minor, sdl_version.patch);
	ImGui::BulletText("glew  v%s", App->renderer3D->GetGlewVersionString().data());
	ImGui::BulletText("ImGui v%s", ImGui::GetVersion());
	ImGui::BulletText(ilGetString(IL_VERSION_NUM));
	ImGui::BulletText("OpenGL (supported) v%s", glGetString(GL_VERSION));
	ImGui::BulletText("Parson - JSON library parser");
	ImGui::BulletText("par_shapes.h for Primitives creation");
	// TODO: search how to query on runtime for mathgeolib, if is possible
	ImGui::BulletText("MathGeoLib v1.5");
	ImGui::Separator();
}

void ModuleEditor::ShowCheckerTexture(uint& checker_id, bool& active) const
{

	static int val = 256, v_min = 16, v_max = 2048;

	/*if (ImGui::CollapsingHeader("Checker Texture"))
	{*/

		ImGui::Title("View UV checker", 1);
		if (ImGui::Checkbox("##view uv checker", &active))
		{
			if (active)
			{
				checker_id = App->textures->GenerateCheckerTexture(val, val);
			}
			else
			{
				App->textures->FreeTextureBuffer(checker_id);
				checker_id = 0;
			}
		}

		if (active)
		{
			if (ImGui::BeginCombo("Resolution", std::to_string(val).data()))
			{
				for (uint i = v_min; i < v_max * 2; i = i > 0 ? i * 2 : ++i)
				{
					ImGui::PushID(i);
					bool is_selected;
					if (ImGui::Selectable(std::to_string(i).data(), val == i))
					{
						val = i;

						App->textures->FreeTextureBuffer(checker_id);
						checker_id = App->textures->GenerateCheckerTexture(val, val);

					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			if (glIsTexture(checker_id))
				ImGui::Image((ImTextureID)checker_id, ImVec2(256, 256));
			else
			{
				active = false;
				checker_id = 0; // for rare circumstances
			}
		}
	//}
}

// DEBUG DATA MESH CLASS HELPER ----------------------------------------------------------------------------------------------------------

//DebugDataMesh::DebugDataMesh() {}

DebugDataMesh::DebugDataMesh(uint n_v, uint n_i) : n_vertices(n_v), n_idx(n_i) {}

DebugDataMesh::~DebugDataMesh() {}

void DebugDataMesh::GenBuffers()
{
	// gen --------------
	glGenBuffers(1, &debug_v_normals_gl_id);
	// debug draw faces normals buffers
	//glGenBuffers(1, &debug_f_vertices_gl_id);
	glGenBuffers(1, &debug_f_normals_gl_id);
}

void DebugDataMesh::FillVertexBuffer()
{
	// FILL -------------
	// WARNING: FOR NORMALS DEBUG ONLY, the normals for other calculations remains on normals float pointer ----
	glBindBuffer(GL_ARRAY_BUFFER, debug_v_normals_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * n_vertices, &debug_v_normals[0], GL_STATIC_DRAW);
}

void DebugDataMesh::FillFacesBuffer()
{
	// TODO: 2nd warning, be careful, since we only load and print triangulated faces this is good
	////glBindBuffer(GL_ARRAY_BUFFER, debug_f_vertices_gl_id);
	////glBufferData(GL_ARRAY_BUFFER, sizeof(float) * n_idx, &debug_f_vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, debug_f_normals_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * n_idx * 2, &debug_f_normals[0], GL_STATIC_DRAW);
}

void DebugDataMesh::ComputeVertexNormals(R_Mesh* goMesh, float length)
{
	// to draw lines, we need an array ready to what expects gldrawarrays
	// start point and finish point
	// TODO: improve this, thinking in deepth if this is possible with memcpy

	//if (debug_v_normals != nullptr) delete[] debug_v_normals;

	//debug_v_normals = new float[n_vertices * 6]; // 3 for startpoint, 3 more for endpoint

	//uint n_count = 0;
	//for (uint i = 0; i < n_vertices * 3; i += 3)
	//{
	//	debug_v_normals[n_count] = goMesh->vertices[i]; // x
	//	debug_v_normals[n_count + 1] = goMesh->vertices[i + 1]; // y
	//	debug_v_normals[n_count + 2] = goMesh->vertices[i + 2]; //z

	//	debug_v_normals[n_count + 3] = goMesh->vertices[i] + goMesh->normals[i] * length; // x
	//	debug_v_normals[n_count + 4] = goMesh->vertices[i + 1] + goMesh->normals[i + 1] * length; // y
	//	debug_v_normals[n_count + 5] = goMesh->vertices[i + 2] + goMesh->normals[i + 2] * length; // z

	//	n_count += 6;
	//}

	//// updates internal line length
	//v_last_ll = length;

}

bool DebugDataMesh::ComputeFacesNormals(R_Mesh* goMesh, float length)
{

	if (n_idx % 3 != 0)
	{
		LOG("[Error] Could not compute face normals, faces with != 3 vertex");
		return false;
	}

	/*if (debug_f_vertices != nullptr) delete[] debug_f_vertices;*/
	if (debug_f_normals != nullptr) delete[] debug_f_normals;

	//debug_f_vertices = new float[n_idx];
	debug_f_normals = new float[n_idx * 2];

	uint i = 0;
	uint n_count = 0;
	for (; i < n_idx; i += 3)
	{
		// find the midface averaged vertex

		//float* v1 = &goMesh->vertices[goMesh->indices[i] * 3];     // get first face vertex
		//float* v2 = &goMesh->vertices[goMesh->indices[i + 1] * 3]; // get second face vertex
		//float* v3 = &goMesh->vertices[goMesh->indices[i + 2] * 3]; // get third face vertex

		//float debug_f_vertices[3];
		//debug_f_vertices[0] = (v1[0] + v2[0] + v3[0]) / 3.0f; // x coord
		//debug_f_vertices[1] = (v1[1] + v2[1] + v3[1]) / 3.0f; // y coord
		//debug_f_vertices[2] = (v1[2] + v2[2] + v3[2]) / 3.0f; // z coord

		//// compute the averaged normal of the 3 vertex of each face

		//float* n1 = &goMesh->normals[goMesh->indices[i] * 3];     // get first face vertex normal
		//float* n2 = &goMesh->normals[goMesh->indices[i + 1] * 3]; // get second face vertex normal
		//float* n3 = &goMesh->normals[goMesh->indices[i + 2] * 3]; // get third face vertex normal

		//float avg_n[3];
		//avg_n[0] = (n1[0] + n2[0] + n3[0]) / 3.0f; // x coord
		//avg_n[1] = (n1[1] + n2[1] + n3[1]) / 3.0f; // y coord
		//avg_n[2] = (n1[2] + n2[2] + n3[2]) / 3.0f; // z coord

		//debug_f_normals[n_count] = debug_f_vertices[0]; // x
		//debug_f_normals[n_count + 1] = debug_f_vertices[1]; // y
		//debug_f_normals[n_count + 2] = debug_f_vertices[2]; //z

		//debug_f_normals[n_count + 3] = debug_f_vertices[0] + avg_n[0] * length; // x
		//debug_f_normals[n_count + 4] = debug_f_vertices[1] + avg_n[1] * length; // y
		//debug_f_normals[n_count + 5] = debug_f_vertices[2] + avg_n[2] * length; // z

		//n_count += 6;
	}

	// updates internal line length
	f_last_ll = length;

	return true;
}

bool DebugDataMesh::Clean()
{

	glDeleteBuffers(1, &debug_v_normals_gl_id);
	//glDeleteBuffers(1, &debug_f_vertices_gl_id);
	glDeleteBuffers(1, &debug_f_normals_gl_id);

	// delete all stored data

	delete[] debug_v_normals;
	//delete[] debug_f_vertices;
	delete[] debug_f_normals;

	debug_v_normals = nullptr;
	//debug_f_vertices = nullptr;
	debug_f_normals = nullptr;

	debug_v_normals_gl_id = 0;
	//debug_f_vertices_gl_id = 0;
	debug_f_normals_gl_id = 0;

	return true;
}

bool DebugDataMesh::DebugRenderVertex(float pointSize)
{
	bool ret = true;

	// draw points
	glPointSize(pointSize);

	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, debug_v_normals_gl_id);
	glVertexPointer(3, GL_FLOAT, 24, (void*)0); // stride: 4 bytes * 6 floats between each "vertex start point"

	glDrawArrays(GL_POINTS, 0, n_vertices);

	glDisableClientState(GL_VERTEX_ARRAY);

	glPointSize(1.0f);

	return ret;
}

bool DebugDataMesh::DebugRenderVertexNormals(float lineWidth)
{
	bool ret = true;

	glLineWidth(lineWidth);

	// draw normals lines
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, debug_v_normals_gl_id);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);

	glDrawArrays(GL_LINES, 0, n_vertices * 2);

	glDisableClientState(GL_VERTEX_ARRAY);

	glLineWidth(1.0f);

	return ret;
}

bool DebugDataMesh::DebugRenderFacesVertex(float pointSize)
{
	bool ret = true;

	glPointSize(pointSize);

	// draw points
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, debug_f_normals_gl_id);
	glVertexPointer(3, GL_FLOAT, 24, (void*)0);

	glDrawArrays(GL_POINTS, 0, n_idx);

	glDisableClientState(GL_VERTEX_ARRAY);

	glPointSize(1.0f);

	return ret;
}

bool DebugDataMesh::DebugRenderFacesNormals(float lineWidth)
{
	bool ret = true;

	glLineWidth(lineWidth);

	glEnableClientState(GL_VERTEX_ARRAY);

	// draw lines
	glBindBuffer(GL_ARRAY_BUFFER, debug_f_normals_gl_id);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);

	glDrawArrays(GL_LINES, 0, n_idx * 2);

	glDisableClientState(GL_VERTEX_ARRAY);

	glLineWidth(1.0f); // returns to default line width

	return ret;
}

void DebugDataMesh::SetSizes(uint n_vx, uint n_indices)
{
	n_vertices = n_vx;
	n_idx = n_indices;
}