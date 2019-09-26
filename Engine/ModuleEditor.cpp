#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"


ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled)
{
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
		{
			ImGui::Begin(("About SOLID Engine"), &about);
		
			// TODO, load from json and add all required fields --------
			/*
			Name of your Engine
			One line description
			Name of the Author with link to github page
			Libraries (with versions queried in real time) used with links to their web
			Full text of the license
			*/
			ImGui::Text("v0.1");

			ImGui::Separator();

			ImGui::BulletText("SOLID ENGINE is a project with the purpose of creating \na fully functional video game engine with its own innovations and features \nimplemented by SOLID TEAM. We are 2 Video Game Design and Development Degree students.");
			// -----------------------------
			
			ImGui::End();
		}
		
	}

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