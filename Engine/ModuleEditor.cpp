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

	w_config 	  = nullptr;
	w_console	  = nullptr;
	w_hierarchy   = nullptr;

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
		ImGui::MenuItem("Configuration", NULL, &w_config->active);
		ImGui::MenuItem("Console Log", NULL, &w_console->active);

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

update_status ModuleEditor::PostUpdate(float dt)
{

	// Draw all windows ----------------------------------- 

	for (std::vector<Window*>::iterator itr = windows.begin(); itr != windows.end(); ++itr)
	{
		if ((*itr)->active)
		{
			(*itr)->Draw();
		}
	}

	static bool gl_depth_test		= true;
	static bool gl_cull_face		= true;
	static bool gl_lighting			= true;
	static bool gl_color_material	= true;
	static bool wireframe			= false;

	const float max_line_w = 10.0f;
	const float min_line_w = 0.0f;
	const float max_point_size = 10.0f;
	const float min_point_size = 0.0f;
	const float max_n_length = 3.0f;
	const float min_n_length = 0.0f;
	const float min_alpha = 0.f;
	const float max_alpha = 1.f;

	ImGui::Begin("Render");

	if (ImGui::CollapsingHeader("Shading Modes"))
	{
		ImGui::Title("Fill Faces", 1);	ImGui::Checkbox("##fill_mode", &App->importer->fill_faces);
		ImGui::Title("Color", 2);		ImGui::ColorEdit4("fILL Color##2f", (float*)&App->importer->fill_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Title("Alpha", 2);		ImGui::SliderFloat("##alpha_slider", &App->importer->fill_color.w, min_alpha, max_alpha, "%.1f", 1.0f);

		ImGui::Separator();

		ImGui::Title("Wireframe", 1);	ImGui::Checkbox("##wireframe", &App->importer->wireframe);
		ImGui::Title("Color", 2);		ImGui::ColorEdit4("Line Color##2f", (float*)&App->importer->wire_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Title("Width", 2);		ImGui::SliderFloat("##wire_line_width", &App->importer->wire_line_width, min_line_w, max_line_w, "%.1f", 1.0f);
	}

	if (ImGui::CollapsingHeader("Debug Modes"))
	{
		ImGui::Title("Vertex Normals", 1);		ImGui::Checkbox("##d_vertex_normals", &App->importer->debug_vertex_normals);
		ImGui::Title("Point Color", 2);			ImGui::ColorEdit4("Vertex Point Color##2f", (float*)&App->importer->d_vertex_p_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Title("Point Size", 2);			ImGui::SliderFloat("##d_vertex_p_size", &App->importer->v_point_size, min_point_size, max_point_size, "%.1f", 1.0f);
		ImGui::Title("Line Color", 2);			ImGui::ColorEdit4("Vertex Line Color##2f", (float*)&App->importer->d_vertex_l_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Title("Line Width", 2);			ImGui::SliderFloat("##d_vertex_line_width", &App->importer->v_n_line_width, min_line_w, max_line_w, "%.1f", 1.0f);
		ImGui::Title("Line Lenght", 2);

		if (ImGui::SliderFloat("##d_vertex_line_lenght", &App->importer->v_n_line_length, min_n_length, max_n_length, "%.1f", 1.0f))
		{
			App->importer->ReComputeVertexNormals(App->importer->v_n_line_length);
		}

		ImGui::Separator();

		ImGui::Title("Face Normals", 1);	ImGui::Checkbox("##d_face_normals", &App->importer->debug_face_normals);
		ImGui::Title("Point Color", 2);		ImGui::ColorEdit4("Face Point Color##2f", (float*)&App->importer->d_vertex_face_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Title("Point Size", 2);		ImGui::SliderFloat("##d_face_p_size", &App->importer->f_v_point_size, min_point_size, max_point_size, "%.1f", 1.0f);
		ImGui::Title("Line Color", 2);		ImGui::ColorEdit4("Face Line Color##2f", (float*)&App->importer->d_vertex_face_n_color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
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
		ImGui::Title("Color", 2);		ImGui::ColorEdit4("Grid Color##2f", (float*)&App->test->main_grid->color, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
		ImGui::Title("Opacity", 2);		ImGui::SliderFloat("##d_face_line_lenght", &App->test->main_grid->color.w, min_alpha, max_alpha, "%.1f", 1.0f);
		ImGui::Title("Distance", 2);     if (ImGui::SliderInt("##grid_units", &units, 1, 50)) App->test->main_grid->SetUnits(units);
		ImGui::Title("Line Width", 2);	ImGui::SliderFloat("##grid_width", &App->test->main_grid->width, min_line_w, max_line_w, "%.1f", 1.0f);
	}
	if (ImGui::CollapsingHeader("OpenGL Test"))
	{
		ImGui::Title("Color Material");	
		if (ImGui::Checkbox("##GL_COLOR_MATERIAL", &gl_color_material))
		{
			if (gl_color_material)
				glEnable(GL_COLOR_MATERIAL);
			else
			{
				// TODO: when switching the material to off, we must set another color for base, if not
				// the last active prevails
				glColor3f(1.0f, 1.0f, 1.0f);
				glDisable(GL_COLOR_MATERIAL);
			}
		}
		ImGui::Title("Depht Test");		
		if (ImGui::Checkbox("##GL_DEPTH_TEST", &gl_depth_test))
		{
			if(gl_depth_test)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
		}
		ImGui::Title("Cull Faces");		
		if (ImGui::Checkbox("##GL_CULL_FACE", &gl_cull_face))
		{
			if (gl_cull_face)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
		}
		ImGui::Title("Lighting");		
		if (ImGui::Checkbox("##GL_LIGHTING", &gl_lighting))
		{
			if (gl_lighting)
				glEnable(GL_LIGHTING);
			else
				glDisable(GL_LIGHTING);
		}
	}


	// WE dont need this every frame !!!
	//if (gl_color_material)
	//	glEnable(GL_COLOR_MATERIAL);
	//else
	//	glDisable(GL_COLOR_MATERIAL);

	/*if (gl_depth_test)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
		
	if (gl_cull_face)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
	

	if (gl_lighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);*/
		

	// TODO: checker texture temporary here
	static bool view_checker = false;
	static int val = 512, v_min = 8, v_max = 2048;
	static uint tex_id = 0;

	if (ImGui::CollapsingHeader("Checker Texture"))
	{
		if (ImGui::Checkbox("view uv checker", &view_checker))
		{
			if (view_checker)
			{
				// TODO: create checker texture with default size
				tex_id = App->textures->GenerateCheckerTexture(val, val);
			}
			else
			{
				// TODO: unload texture id
			}
		}

		if (view_checker)
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

						// TODO: delete and reload procedural checker

					}
					ImGui::PopID();
				}

				ImGui::EndCombo();
			}

			ImGui::Image((ImTextureID)tex_id, ImVec2(map(val, 0, 2048, 128, 512), map(val, 0, 2048, 128, 512)));
		}
	}

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

	if(w_config != nullptr)
		ret = config.AddBool("show_configuration", w_config->active);
	else
		ret = config.AddBool("show_configuration", show_configuration);

	if(w_console != nullptr)
		ret = config.AddBool("show_console", w_console->active);
	else
		ret = config.AddBool("show_console", show_console);
	

	return ret;
}

void ModuleEditor::Load(Config& config)
{
	about = config.GetBool("about", about);
	showcase = config.GetBool("showcase", showcase);
	if(w_console != nullptr)
		w_console->active = config.GetBool("show_console", w_console->active);
	else
		show_console = config.GetBool("show_console", show_console);
	if (w_config != nullptr)
		w_config->active = config.GetBool("show_configuration", w_config->active);
	else
		show_configuration = config.GetBool("show_configuration", show_configuration);
}

float ModuleEditor::map(float value, float s1, float stop1, float s2, float stop2) const
{
	return s2 + (stop2 - s2) * ((value - s1) / (stop1 - s1));	
}

//bool ModuleEditor::SliderIntWithSteps(const char* label, int* v, int v_min, int v_max, int v_step, const char* display_format)
//{
//	if (!display_format)
//		display_format = "%.3f";
//
//	char text_buf[64] = {};
//	snprintf(text_buf, sizeof(text_buf), "%i", *v);
//
//	// Map from [v_min,v_max] to [0,N]
//	if (v_step == 0) v_step = 1;
//	const int countValues = int((v_max - v_min) / v_step);
//	int v_i = int((*v - v_min) / v_step);
//	const bool value_changed = ImGui::SliderInt(label, &v_i, 0, countValues, text_buf);
//
//	// Remap from [0,N] to [v_min,v_max]
//	*v = v_min + float(v_i) * v_step;
//	return value_changed;
//}