#include "SDL/include/SDL_opengl.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "Config.h"

#include "ImGui/imgui.h"
#include "ImGui/Impl/imgui_impl_sdl.h"
#include "ImGui/Impl/imgui_impl_opengl3.h"


// Windows include 

#include "W_Config.h"
#include "W_Console.h"
#include "W_Hierarchy.h"
#include "W_Rendering.h"
#include "W_Scene.h"

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

	// load values from json
	Load(config);

	// DEAR IMGUI SETUP ---------------------------------------------------------- 
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	// ImGui Style ----------------------------------------

	ImGuiStyle* style = &ImGui::GetStyle();
	style->DefaultColumnWidth = DFT_COLUMN;
	style->WindowRounding = 0.0f;// <- Set this on init or use ImGui::PushStyleVar()
	style->ChildRounding = 0.0f;
	style->FrameRounding = 0.0f;
	style->GrabRounding = 0.0f;
	style->PopupRounding = 0.0f;
	style->ScrollbarRounding = 0.0f;
	return true;
}

// Load assets
bool ModuleEditor::Start(Config& conf)
{
	LOG("[Start] Loading Editor");
	bool ret = true;

	// Initial Windows -------------------------------------

	w_config = new W_Config("Configuration", show_configuration);
	w_console = new W_Console("Console", show_console);
	w_hierarchy = new W_Hierarchy("Hierarchy", true);
	w_rendering = new W_Rendering("Rendering Settings", true);
	w_scene = new W_Scene("Scene", true);

	return ret;
}

// Load assets
bool ModuleEditor::CleanUp()
{
	LOG("[CleanUp] Unloading Editor");

	// TODO: save editor config on exit or application error
	//SaveEditorConf(editor_filename.data());

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


update_status ModuleEditor::PreUpdate(float dt)
{
	// ImGui Internal System ------------------------------

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return UPDATE_CONTINUE;
}

// Update
update_status ModuleEditor::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate(float dt)
{
	// DockSpace ------------------------------------------
	static bool dock_space = true;
	static bool opt_fullscreen_persistant = true;
	bool opt_fullscreen = opt_fullscreen_persistant;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	if (opt_fullscreen)
	{
		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	}

	if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
		window_flags |= ImGuiWindowFlags_NoBackground;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", &dock_space, window_flags);
	ImGui::PopStyleVar();

	if (opt_fullscreen)
		ImGui::PopStyleVar(2);

	// DockSpace

	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

	// Main Menu Bar --------------------------------------

	DrawMainMenuBar();

	// Draw all windows ----------------------------------- 

	for (std::vector<Window*>::iterator itr = windows.begin(); itr != windows.end(); ++itr)
	{
		if ((*itr)->active)
		{
			(*itr)->Draw();
		}
	}

	// Draw other elemnts ---------------------------------

	DrawPopUps();

	if (show_demo_imgui) ImGui::ShowDemoWindow(&show_demo_imgui);

	ImGui::End();

	// ImGui Internal System ------------------------------

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
	SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
	SDL_GL_MakeCurrent(backup_current_window, backup_current_context);

	// -----------------------------------------------------

	return UPDATE_CONTINUE;
}

// TODO: improve this methods when we get the windows class helper --------------

bool ModuleEditor::LoadEditorConfig(const char* path)
{
	bool ret = true;

	JSON_Value* editor_config = json_parse_file(path);

	if (editor_config == NULL)
	{
		LOG("Editor file configuration %s not found", path);
	}
	else
	{
		LOG("Found");
		// TODO: iterate all members and save its states
		show_demo_imgui = json_object_get_boolean(json_object(editor_config), "show_demo_imgui");
		show_about_popup = json_object_get_boolean(json_object(editor_config), "show_about_popup");
	}

	return ret;
}

bool ModuleEditor::SaveEditorConfig(const char* path)
{
	bool ret = true;

	// testing how array works ----

	JSON_Array* arr = nullptr;
	JSON_Value* va = json_value_init_object();
	JSON_Object* obj = nullptr;
	obj = json_value_get_object(va);

	JSON_Value* vaa = json_value_init_array();
	json_object_set_value(obj, "blabla", vaa);

	//va = json_value_init_array();
	//arr = json_value_get_array(vaa);

	/*json_array_append_boolean(arr, true);
	json_array_append_boolean(arr, true);
	json_array_append_boolean(arr, false);
	json_array_append_boolean(arr, true);
	json_array_append_boolean(arr, true);*/

	json_array_append_boolean(json_value_get_array(vaa), true);
	json_array_append_boolean(json_value_get_array(vaa), true);
	json_array_append_boolean(json_value_get_array(vaa), true);
	json_array_append_boolean(json_value_get_array(vaa), true);

	json_serialize_to_file_pretty(va, "testingArray.json");

	return ret;
}

// ----------------------------------------------------------------------------

//--------------------------------------------------------------------------
// TODO: temporaly methods helpers for imgui here --------------------------

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

	float blabla[10] = { 0,2,0,0,0,0,4,5,5,10 };

	ret = config.AddBool("show_about_popup", show_about_popup);
	ret = config.AddBool("show_demo_imgui", show_demo_imgui);
	ret = config.AddFloatArray("sdfgd", blabla, 10);

	if (w_config != nullptr)
		ret = config.AddBool("show_configuration", w_config->active);
	else
		ret = config.AddBool("show_configuration", show_configuration);

	if (w_console != nullptr)
		ret = config.AddBool("show_console", w_console->active);
	else
		ret = config.AddBool("show_console", show_console);


	return ret;
}

void ModuleEditor::Load(Config& config)
{
	show_about_popup = config.GetBool("show_about_popup", show_about_popup);
	show_demo_imgui = config.GetBool("show_demo_imgui", show_demo_imgui);
	if (w_console != nullptr)
		w_console->active = config.GetBool("show_console", w_console->active);
	else
		show_console = config.GetBool("show_console", show_console);
	if (w_config != nullptr)
		w_config->active = config.GetBool("show_configuration", w_config->active);
	else
		show_configuration = config.GetBool("show_configuration", show_configuration);
}

bool ModuleEditor::DrawMainMenuBar()
{
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

		if (ImGui::MenuItem("Restore editor to default config"))
		{
			show_restore_popup = true;
		}

		if (ImGui::MenuItem("Quit", "ESC"))
			return false;

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Window"))
	{
		/*if (ImGui::MenuItem("Load editor windows states", "Ctrl+Alt+L"))
			LoadEditorConfig(editor_filename.data());*/
			//if (ImGui::MenuItem("Save editor windows states", "Ctrl+Alt+S"))
				//SaveEditorConfig(editor_filename.data());

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("View"))
	{
		ImGui::MenuItem("Configuration", NULL, &w_config->active);
		ImGui::MenuItem("Console Log", NULL, &w_console->active);

		ImGui::EndMenu();
	}

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

	return true;
}

void ModuleEditor::DrawPopUps()
{
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
			ImGui::Text("3rd Party libs");
			SDL_version sdl_version;
			SDL_GetVersion(&sdl_version);
			ImGui::BulletText("SDL   v%d.%d.%d", sdl_version.major, sdl_version.minor, sdl_version.patch);
			ImGui::BulletText("glew  v%s", App->renderer3D->GetGlewVersionString().data());
			ImGui::BulletText("ImGui v%s", ImGui::GetVersion());
			ImGui::BulletText("Parson - JSON library parser");

			ImVec2 buttonSize = { 120.0f, 0.f };
			ImGuiStyle& style = ImGui::GetStyle();
			ImGui::Indent(ImGui::GetWindowWidth() * 0.5f - (buttonSize.x * 0.5f) - style.WindowPadding.x);
			if (ImGui::Button("OK", buttonSize)) { ImGui::CloseCurrentPopup(); show_about_popup = false; }

		}
		ImGui::EndPopup();
	}
}