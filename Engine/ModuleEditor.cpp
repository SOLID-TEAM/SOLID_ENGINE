#include "SDL/include/SDL_opengl.h"

#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"

#include "ImGui/imgui.h"
#include "ImGui/Impl/imgui_impl_sdl.h"
#include "ImGui/Impl/imgui_impl_opengl3.h"

// Panels include 

#include "PanelConfig.h"
#include "PanelConsole.h"

ModuleEditor::ModuleEditor(bool start_enabled) : Module(start_enabled)
{
	name.assign("Editor");
}

ModuleEditor::~ModuleEditor()
{}

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

	config_panel = new PanelConfig("Configuration", show_configuration);
	console_panel = new PanelConsole("Console", show_console);

	return true;
}

// Load assets
bool ModuleEditor::Start(Config& conf)
{
	LOG("[Start] Loading Editor");
	bool ret = true;

	// Create all panels --------------------------------


	return ret;
}

// Load assets
bool ModuleEditor::CleanUp()
{
	LOG("[CleanUp] Unloading Editor");

	// TODO: save editor config on exit or application error
	//SaveEditorConf(editor_filename.data());

	//console_buffer.clear();

	ImGui_ImplSDL2_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
	return true;
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
	// Main Menu Bar ---------------------------------

	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Save editor configuration", "Ctrl+S"))
		{
			if (App->WantToSave())
			{
				LOG("[Info] %s succesfully saved", App->config_filename.data());
			}
		}

		if (ImGui::MenuItem("Quit", "ESC"))
			return update_status::UPDATE_STOP;
		
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
		ImGui::MenuItem("Configuration", NULL, &config_panel->active);
		ImGui::MenuItem("Console", NULL, &console_panel->active);

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Help"))
	{
		if (ImGui::MenuItem("Gui Demo"))
			showcase = !showcase;

		// TODO: update links
		if (ImGui::MenuItem("Documentation"))
			App->RequestBrowser("https://github.com/SOLID-TEAM/SOLID_ENGINE");

		if (ImGui::MenuItem("Download latest version"))
			App->RequestBrowser("https://github.com/SOLID-TEAM/SOLID_ENGINE");

		if (ImGui::MenuItem("Report a bug!"))
			App->RequestBrowser("https://github.com/SOLID-TEAM/SOLID_ENGINE");

		if (ImGui::MenuItem("About"))
			about = !about;

		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();

	// print active imgui elements -----------------------

	if (showcase)
		ImGui::ShowDemoWindow(&showcase);

	if (about)
	{
		//{
		//	ImGui::Begin(("About SOLID Engine"), &about);
		//
		//	// TODO, load from json and add all required fields --------
		//	/*
		//	Name of your Engine
		//	One line description
		//	Name of the Author with link to github page
		//	Libraries (with versions queried in real time) used with links to their web
		//	Full text of the license
		//	*/
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
			if (ImGui::Button("OK", buttonSize)) { ImGui::CloseCurrentPopup(); about = false; }

		}
		ImGui::EndPopup();

	}

	// Render window -------------------------------------------

	ImGui::Begin("Render");


	static bool gl_depth_test = false;
	static bool gl_cull_face = false;
	static bool gl_lighting = false;
	static bool gl_color_material = false;
	static bool wireframe = false;


	if (ImGui::Checkbox("GL_COLOR_MATERIAL", &gl_color_material))
	{
		if (glIsEnabled(GL_COLOR_MATERIAL))
			glDisable(GL_COLOR_MATERIAL);
		else
			glEnable(GL_COLOR_MATERIAL);
	}


	if (ImGui::Checkbox("GL_DEPTH_TEST", &gl_depth_test))
	{
		if (glIsEnabled(GL_DEPTH_TEST))
			glDisable(GL_DEPTH_TEST);
		else
			glEnable(GL_DEPTH_TEST);
	}


	if (ImGui::Checkbox("GL_CULL_FACE", &gl_cull_face))
	{
		if (glIsEnabled(GL_CULL_FACE))
			glDisable(GL_CULL_FACE);
		else
			glEnable(GL_CULL_FACE);
	}


	if (ImGui::Checkbox("GL_LIGHTING", &gl_lighting))
	{
		if (glIsEnabled(GL_LIGHTING))
			glDisable(GL_LIGHTING);
		else
			glEnable(GL_LIGHTING);
	}

	if (ImGui::Checkbox("WIREFRAME", &wireframe))
	{
		if (wireframe)
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	ImGui::End();

	// ----------------------------------------------------------------------------------

	//if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	//{
	//	//App->config->SaveConfigToFile("blas.json");
	//	Save(App->config->GetSection(name.data()));
	//	App->config->SaveConfigToFile(App->config_filename.data());
	//	//SaveEditorConfig(editor_filename.data());
	//}
	//if (App->input->GetKey(SDL_SCANCODE_KP_ENTER) == KEY_DOWN)
	//{
	//	//App->config->SaveConfigToFile("blasfdgfd.json");
	//	Load(App->config->GetSection(name.data()));
	//	/*Save(App->config->GetSection(name.data()));*/
	//	//SaveEditorConfig(editor_filename.data());
	//}

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate(float dt)
{

	if (console_panel->active)
	console_panel->Draw();
	if (config_panel->active)
	config_panel->Draw();

	// ImGui Internal System ------------------------------

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	return UPDATE_CONTINUE;
}

// TODO: improve this methods when we get the panels class helper --------------

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
		showcase = json_object_get_boolean(json_object(editor_config), "showcase");
		about = json_object_get_boolean(json_object(editor_config), "about");
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

	ret = config.AddBool("about", about);
	ret = config.AddBool("showcase", showcase);
	ret = config.AddFloatArray("sdfgd", blabla, 10);

	if(config_panel != nullptr)
		ret = config.AddBool("show_configuration", config_panel->active);
	else
		ret = config.AddBool("show_configuration", show_configuration);
	if(console_panel != nullptr)
		ret = config.AddBool("show_console", console_panel->active);
	else
		ret = config.AddBool("show_console", show_console);
	

	return ret;
}

void ModuleEditor::Load(Config& config)
{
	about = config.GetBool("about", about);
	showcase = config.GetBool("showcase", showcase);
	show_console = config.GetBool("show_console", show_console);
	show_configuration = config.GetBool("show_configuration", show_configuration);
}