#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"


ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	// fill stored fps vector with dummy values
	// for evade ugly visualization
	// TODO: TEST
	//std::fill(stored_fps.begin(), stored_fps.end(), 0);
	stored_fps.resize(MAX_STORED_FPS);
	stored_ms.resize(MAX_STORED_FPS);
}

ModuleEditor::~ModuleEditor()
{}

// Load assets
bool ModuleEditor::Start()
{
	LOG("Loading Editor");
	bool ret = true;

	// TODO: on some moment we must to pass a config file from application
	// like we do with XML but with json data and grab from there all needed data
	editor_filename.assign("editor_config.json");

	// DEAR IMGUI SETUP ---------------------------------------------------------- 
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL3_Init();
	// ----------------------------------------------------------------------------

	// Load editor configuration if file is found
	// TODO: clean this -----------
	LoadEditorConfig(editor_filename.data());
	// ----------------------------

	return ret;
}

// Load assets
bool ModuleEditor::CleanUp()
{
	LOG("Unloading Editor");

	// TODO: save editor config on exit or application error
	//SaveEditorConf(editor_filename.data());

	ImGui_ImplSDL2_Shutdown();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui::DestroyContext();
	return true;
}

update_status ModuleEditor::PreUpdate(float dt)
{
	// Start the Dear ImGui frame
	// this should be on a very first update order to be ready
	// to call imgui methods everywhere are needed
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();


	return UPDATE_CONTINUE;
}

// Update
update_status ModuleEditor::Update(float dt)
{
	// IMGUI UPDATE ----------------------------------------------------------------------
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// TODO: create abstraction classes, for now this is a test

	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File"))
	{
		if (ImGui::MenuItem("Quit", "ESC"))
			return update_status::UPDATE_STOP;
		
		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Window"))
	{
		if (ImGui::MenuItem("Load editor windows states", "Ctrl+Alt+L"))
			LoadEditorConfig(editor_filename.data());
		if (ImGui::MenuItem("Save editor windows states", "Ctrl+Alt+S"))
			SaveEditorConfig(editor_filename.data());

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

	if (ImGui::Begin("Configuration"))
	{
		if (ImGui::CollapsingHeader("Application"))
		{
			ImGui::Text("Testing");

			static int test = 60;
			if (ImGui::SliderInt("##Framerate cap", &test, 0, 144, "FramerateCap = %.3f"))
			{
				App->AdjustCappedMs(test);
			}

			if (ImGui::IsItemHovered())
				ImGui::SetTooltip("Adjust to 0 to unlock cap");

			char title[25];
			sprintf_s(title, 25, "Framerate %.1f", stored_fps[stored_fps.size() - 1]);
			ImGui::PlotHistogram("##Framerate", &stored_fps[0], stored_fps.size(), 0, title, 0.0f, 100.0f, ImVec2(320, 100));

			sprintf_s(title, 25, "ms %.1f", stored_ms[stored_ms.size() - 1]);
			ImGui::PlotHistogram("##Framerate", &stored_ms[0], stored_ms.size(), 0, title, 0.0f, 50.0f, ImVec2(320, 100));


		}
	}
	ImGui::End();

	// Configuration window

	/*ImGui::Begin("Hello, world!");

	ImGui::Text("This is some useful text.");
	static int counter = 0;
	if (ImGui::Button("EXIT"))
		return UPDATE_STOP;
	ImGui::SameLine();
	if (ImGui::Button("TOGGLE DEMO WINDOW"))
		showcase = !showcase;*/

	/*	counter++;
	ImGui::SameLine();
	ImGui::Text("counter = %d", counter);*/

	//ImGui::End();

	// ----------------------------------------------------------------------------------

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::PostUpdate(float dt)
{
	//ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::Render();
	//glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
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

	//JSON_Value* schema = json_parse_string("{\"name\":\"\"}");
	JSON_Value* user_data = json_parse_file(path);
	//JSON_Array* arrayTest = json_value_get_array(schema);

	// create new file if file not found or validated types not match with scheme types

	//if (user_data == NULL || json_validate(schema, user_data) != JSONSuccess)
	//{
		user_data = json_value_init_object();

		json_object_set_string(json_object(user_data), "name", "Very solid");
		json_object_set_boolean(json_object(user_data), "showcase", showcase);
		json_object_set_boolean(json_object(user_data), "about", about);


		// write data to file
		json_serialize_to_file(user_data, path);
	//}

	json_value_free(user_data);

	return ret;
}

// ----------------------------------------------------------------------------

void ModuleEditor::AddLastFps(const float fps, const float ms)
{
	// stored_fps and ms should be the same
	if (stored_fps.size() >= MAX_STORED_FPS)
	{
		// iterate and shift values
		for (uint i = 0; i < MAX_STORED_FPS - 1; ++i)
		{
			stored_fps[i] = stored_fps[i + 1];
			stored_ms[i] = stored_ms[i + 1];
		}

		stored_fps[MAX_STORED_FPS - 1] = fps;
		stored_ms[MAX_STORED_FPS - 1] = ms;
	}
	else
	{
		stored_fps.push_back(fps);
		stored_ms.push_back(ms);
	}
	
}