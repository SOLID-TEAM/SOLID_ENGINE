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

//TODO: MOVE HEADERS TO ITS OWN PANELS
#include "ModuleTest.h"

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

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	

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

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	ImGuiStyle* style = &ImGui::GetStyle();
	style->DefaultColumnWidth = DFT_COLUMN;

	style->WindowRounding = 0.0f;// <- Set this on init or use ImGui::PushStyleVar()
	style->ChildRounding = 0.0f;
	style->FrameRounding = 0.0f;
	style->GrabRounding = 0.0f;
	style->PopupRounding = 0.0f;
	style->ScrollbarRounding = 0.0f;

	//style->WindowPadding = ImVec2(15, 15);
	//style->WindowRounding = 5.0f;
	//style->FramePadding = ImVec2(5, 5);
	//style->FrameRounding = 4.0f;
	//style->ItemSpacing = ImVec2(12, 8);
	//style->ItemInnerSpacing = ImVec2(8, 6);
	//style->IndentSpacing = 25.0f;
	//style->ScrollbarSize = 15.0f;
	//style->ScrollbarRounding = 9.0f;
	//style->GrabMinSize = 5.0f;
	//style->GrabRounding = 3.0f;

	//style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	//style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	//style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	//style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	//style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	//style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	//style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	//style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	//style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	//style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	//style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	//style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	//style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	//style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	//style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	//style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	//style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	//style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	////style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
	//style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	//style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	//style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	//style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	//style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	//style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	//style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	//style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	//style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	////style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	////style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	////style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	//style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	//style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	//style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	////style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
	////style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	////style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	//style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	//style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	//style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	//style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	//style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	//style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
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
	static bool restore_popup = false;

	// Main Menu Bar ---------------------------------

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
			restore_popup = true;
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
		ImGui::MenuItem("Console Log", NULL, &console_panel->active);

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

	if (restore_popup)
	{
		ImGui::OpenPopup("Are you sure?");
		if (ImGui::BeginPopupModal("Are you sure?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("All editor modificable values are going to reset\n" 
						"after this, you are still capable to revert this\n" 
						"change by loading config editor before save them");
			ImGui::Separator();
			if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); restore_popup = false; App->WantToLoad(true); }
			ImGui::SameLine();
			if (ImGui::Button("OK and save", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); restore_popup = false; App->WantToLoad(true); App->WantToSave(); }
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); restore_popup = false; }
			ImGui::EndPopup();
		}
	}

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

void ModuleEditor::SetItemsSize(int mount)
{
	ImGui::PushItemWidth((ImGui::GetWindowWidth() - ImGui::GetCursorPosX())/ (float)mount);
}

update_status ModuleEditor::PostUpdate(float dt)
{


	// Render window -------------------------------------------

	// TODO: MAKE A PANEL CLASS

	ImGui::Begin("main");

	ImGui::Begin("Render");


	static bool gl_depth_test = false;
	static bool gl_cull_face = false;
	static bool gl_lighting = false;
	static bool gl_color_material = false;
	static bool wireframe = false;

	const float max_line_w = 10.0f;
	const float min_line_w = 0.0f;
	const float max_point_size = 10.0f;
	const float min_point_size = 0.0f;
	const float min_alpha = 0.f;
	const float max_alpha = 1.f;

	const float max_n_length = 3.0f;
	const float min_n_length = 0.0f;

	float v_n_line_length = App->importer->v_n_line_length;
	float f_n_line_length = App->importer->f_n_line_length;

	if (ImGui::CollapsingHeader("Shading Modes"))
	{
		ImGui::Title("Fill Faces", 1);	ImGui::Checkbox("##fill_mode", &App->importer->fill_faces);
		ImGui::Title("Color", 2);		ImGui::ColorEdit4("fILL Color##2f", (float*)& App->importer->fill_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Title("Alpha", 2);		ImGui::SliderFloat("##alpha_slider", &App->importer->fill_color.w, min_alpha, max_alpha, "%.1f", 1.0f);

		ImGui::Separator();

		ImGui::Title("Wireframe", 1);	ImGui::Checkbox("##wireframe", &App->importer->wireframe);
		ImGui::Title("Color", 2);		ImGui::ColorEdit4("Line Color##2f", (float*)& App->importer->wire_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Title("Width", 2);		ImGui::SliderFloat("##wire_line_width", &App->importer->wire_line_width, min_line_w, max_line_w, "%.1f", 1.0f);
	}

	if (ImGui::CollapsingHeader("Debug Modes"))
	{
		ImGui::Title("Vertex Normals", 1);		ImGui::Checkbox("##d_vertex_normals", &App->importer->debug_vertex_normals);
		ImGui::Title("Point Color", 2);			ImGui::ColorEdit4("Vertex Point Color##2f", (float*)& App->importer->d_vertex_p_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Title("Point Size", 2);			ImGui::SliderFloat("##d_vertex_p_size", &App->importer->v_point_size, min_point_size, max_point_size, "%.1f", 1.0f);
		ImGui::Title("Line Color", 2);			ImGui::ColorEdit4("Vertex Line Color##2f", (float*)& App->importer->d_vertex_l_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Title("Line Width", 2);			ImGui::SliderFloat("##d_vertex_line_width", &App->importer->v_n_line_width, min_line_w, max_line_w, "%.1f", 1.0f);
		ImGui::Title("Line Lenght", 2);

		if (ImGui::SliderFloat("##d_vertex_line_lenght", &App->importer->v_n_line_length, min_n_length, max_n_length, "%.1f", 1.0f))
		{
			App->importer->ReComputeVertexNormals(App->importer->v_n_line_length);
		}

		ImGui::Separator();

		ImGui::Title("Face Normals", 1);	ImGui::Checkbox("##d_face_normals", &App->importer->debug_face_normals);
		ImGui::Title("Point Color", 2);		ImGui::ColorEdit4("Face Point Color##2f", (float*)& App->importer->d_vertex_face_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Title("Point Size", 2);		ImGui::SliderFloat("##d_face_p_size", &App->importer->f_v_point_size, min_point_size, max_point_size, "%.1f", 1.0f);
		ImGui::Title("Line Color", 2);		ImGui::ColorEdit4("Face Line Color##2f", (float*)& App->importer->d_vertex_face_n_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Title("Line Width", 2);		ImGui::SliderFloat("##d_face_line_width", &App->importer->f_n_line_width, min_line_w, max_line_w, "%.1f", 1.0f);
		ImGui::Title("Line Lenght", 2);

		if (ImGui::SliderFloat("##d_face_line_lenght", &App->importer->f_n_line_length, min_n_length, max_n_length, "%.1f", 1.0f))
		{
			App->importer->ReComputeFacesNormals(App->importer->f_n_line_length);
		}
	}
	static int units = 50;

	if (ImGui::CollapsingHeader("Grid Settings"))
	{
		ImGui::Title("Active", 1);		ImGui::Checkbox("##active_grid", &App->test->main_grid->active);
		ImGui::Title("Color", 2);		ImGui::ColorEdit4("Grid Color##2f", (float*)& App->test->main_grid->color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Title("Opacity", 2);		ImGui::SliderFloat("##d_face_line_lenght", &App->test->main_grid->color.w, min_alpha, max_alpha, "%.1f", 1.0f);
		ImGui::Title("Distance", 2);     if (ImGui::SliderInt("##grid_units", &units, 1, 50)) App->test->main_grid->SetUnits(units);
		ImGui::Title("Line Width", 2);	ImGui::SliderFloat("##grid_width", &App->test->main_grid->width, min_line_w, max_line_w, "%.1f", 1.0f);
	}
	if (ImGui::CollapsingHeader("OpenGL Test"))
	{
		ImGui::Title("Color Material");	ImGui::Checkbox("##GL_COLOR_MATERIAL", &gl_color_material);
		ImGui::Title("Depht Test");		ImGui::Checkbox("##GL_DEPTH_TEST", &gl_depth_test);
		ImGui::Title("Cull Faces");		ImGui::Checkbox("##GL_CULL_FACE", &gl_cull_face);
		ImGui::Title("Lighting");			ImGui::Checkbox("##GL_LIGHTING", &gl_lighting);
	}

	if (glIsEnabled(gl_color_material))
		glDisable(GL_COLOR_MATERIAL);
	else
		glEnable(GL_COLOR_MATERIAL);

	if (glIsEnabled(gl_depth_test))
		glDisable(GL_DEPTH_TEST);
	else
		glEnable(GL_DEPTH_TEST);

	if (glIsEnabled(gl_cull_face))
		glDisable(GL_CULL_FACE);
	else
		glEnable(GL_CULL_FACE);

	if (glIsEnabled(gl_lighting))
		glDisable(GL_LIGHTING);
	else
		glEnable(GL_LIGHTING);



	ImGui::End();

	//// TODO: MAKE A HIERARCHY PANEL

	static bool hierarchy_ = true;

	//static bool show_hierarchy = true;
	if (ImGui::Begin("Hierarchy", &hierarchy_))
	{
		// get all the gameObjects
		std::vector<ModelData*> vgo = App->importer->GetModels();
		std::vector<ModelData*>::iterator go = vgo.begin();

		for (; go != vgo.end(); ++go)
		{
			ImGui::PushID((int)* go); // to correctly "link" each element without same problems with identical name when we manage buttons etc

			if (ImGui::TreeNodeEx((*go)->name.data()))
			{
				ImGui::Text("blabla");

				if (ImGui::TreeNode("Renderer Options"))
				{
					ImGui::Text("wip");
					ImGui::TreePop();
				}

				ImGui::TreePop();
			}

			ImGui::PopID();
		}
	}

	ImGui::End();

	if (console_panel->active)
	console_panel->Draw();
	if (config_panel->active)
	config_panel->Draw();

	ImGui::End();

	// ImGui Internal System ------------------------------

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	
	SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
	SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
	SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	

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
	if(console_panel != nullptr)
		console_panel->active = config.GetBool("show_console", console_panel->active);
	else
		show_console = config.GetBool("show_console", show_console);
	if (config_panel != nullptr)
		config_panel->active = config.GetBool("show_configuration", config_panel->active);
	else
		show_configuration = config.GetBool("show_configuration", show_configuration);
}