#include "GL/glew.h"
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
#include "W_Inspector.h"

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
	style.DefaultColumnWidth = DFT_COLUMN;
	style.WindowRounding = 0.0f;// <- Set this on init or use ImGui::PushStyleVar()
	style.ChildRounding = 0.0f;
	style.FrameRounding = 0.0f;
	style.GrabRounding = 0.0f;
	style.PopupRounding = 0.0f;
	style.ScrollbarRounding = 0.0f;

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
	w_inspector = new W_Inspector("Inspector", true);

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
	w_inspector = nullptr;

	// debug data mesh ---
	if (ddmesh != nullptr)
		ddmesh->Clean();

	delete ddmesh;
	// -------------------

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
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);

	ImGui::NewFrame();

	return UPDATE_CONTINUE;
}

// Update
update_status ModuleEditor::Update(float dt)
{
	// check if we need to show debug normals on selected go
	if (viewport_options.debug_vertex_normals || viewport_options.debug_face_normals)
	{
	
		if (last_go_precalc != selected_go)
		{
			// check if the gameobject has meshes
			bool hasmeshes = false;
			ModelData* smesh = selected_go->GetMeshes();
			if (smesh != nullptr)
				hasmeshes = true;

			// we calc the two modes at once for the selected go
			// but we filter what to print from bools cols
			if (hasmeshes)
			{
				LOG("[Init] Info: hey, i need to recompute the normals!");
				
				// delete previous data and free opengl buffers
				if (ddmesh != nullptr)
				{
					ddmesh->Clean();
					// re-assign num v and i
					ddmesh->SetSizes(smesh->_v_size, smesh->_idx_size);
				}
				else // if not exists, create one
					ddmesh = new DebugDataMesh(smesh->_v_size, smesh->_idx_size);
				
				// recompute data from selected_go
				ddmesh->ComputeVertexNormals(smesh, viewport_options.v_n_line_length);
				ddmesh->ComputeFacesNormals(smesh, viewport_options.f_n_line_length);

				// generate opengl buffers and fill -------------------------

				ddmesh->GenAndFillBuffers();
			}
			else
				LOG("[Error] Warning: gameobject without meshes to debug");
			// -----------------------------------------------------------
			// assign new precalc
			last_go_precalc = selected_go;
		}
	}
	else if (!viewport_options.debug_face_normals && !viewport_options.debug_vertex_normals) // when the two debug options are off and still have a ddmesh
	{
		if (ddmesh != nullptr)
		{
			ddmesh->Clean();
			delete ddmesh;
			ddmesh = nullptr;
			last_go_precalc = nullptr;
		}
	}

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

	if (!DrawMainMenuBar())
		return update_status::UPDATE_STOP;

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

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}

	// -----------------------------------------------------

	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Draw()
{
	if (viewport_options.debug_vertex_normals || viewport_options.debug_face_normals)
	{
		if (ddmesh != nullptr)
		{
			if (viewport_options.debug_vertex_normals)
			{
				glColor4fv((float*)&viewport_options.d_vertex_p_color);
				ddmesh->DebugRenderVertex(viewport_options.v_point_size);
				glColor4fv((float*)&viewport_options.d_vertex_l_color);
				ddmesh->DebugRenderVertexNormals(viewport_options.v_n_line_width);
			}
			//glColor3f(1.0f, 1.0f, 0.0f);
			if (viewport_options.debug_face_normals)
			{
				glColor4fv((float*)&viewport_options.d_vertex_face_color);
				ddmesh->DebugRenderFacesVertex(viewport_options.f_v_point_size);
				glColor4fv((float*)&viewport_options.d_vertex_face_n_color);
				ddmesh->DebugRenderFacesNormals(viewport_options.f_n_line_width);
			}
		}
	}

	return UPDATE_CONTINUE;
}

// TODO: improve this methods when we get the windows class helper --------------

//bool ModuleEditor::LoadEditorConfig(const char* path)
//{
//	bool ret = true;
//
//	JSON_Value* editor_config = json_parse_file(path);
//
//	if (editor_config == NULL)
//	{
//		LOG("Editor file configuration %s not found", path);
//	}
//	else
//	{
//		LOG("Found");
//		// TODO: iterate all members and save its states
//		show_demo_imgui = json_object_get_boolean(json_object(editor_config), "show_demo_imgui");
//		show_about_popup = json_object_get_boolean(json_object(editor_config), "show_about_popup");
//	}
//
//	return ret;
//}

//bool ModuleEditor::SaveEditorConfig(const char* path)
//{
//	bool ret = true;
//
//	// testing how array works ----
//
//	JSON_Array* arr = nullptr;
//	JSON_Value* va = json_value_init_object();
//	JSON_Object* obj = nullptr;
//	obj = json_value_get_object(va);
//
//	JSON_Value* vaa = json_value_init_array();
//	json_object_set_value(obj, "blabla", vaa);
//
//	//va = json_value_init_array();
//	//arr = json_value_get_array(vaa);
//
//	/*json_array_append_boolean(arr, true);
//	json_array_append_boolean(arr, true);
//	json_array_append_boolean(arr, false);
//	json_array_append_boolean(arr, true);
//	json_array_append_boolean(arr, true);*/
//
//	json_array_append_boolean(json_value_get_array(vaa), true);
//	json_array_append_boolean(json_value_get_array(vaa), true);
//	json_array_append_boolean(json_value_get_array(vaa), true);
//	json_array_append_boolean(json_value_get_array(vaa), true);
//
//	json_serialize_to_file_pretty(va, "testingArray.json");
//
//	return ret;
//}

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

	// DEBUG VIEWPORT OPTIONS
	ret = config.AddBool("wireframe", viewport_options.wireframe);
	ret = config.AddBool("fill_mode", viewport_options.fill_faces);
	ret = config.AddBool("debug_vertex_normals", viewport_options.debug_vertex_normals);
	ret = config.AddBool("debug_face_normals", viewport_options.debug_face_normals);

	ret = config.AddFloatArray("fill_color", (float*)&viewport_options.fill_color, 4);
	ret = config.AddFloatArray("wire_color", (float*)&viewport_options.wire_color, 4);
	ret = config.AddFloatArray("d_vertex_p_color", (float*)&viewport_options.d_vertex_p_color, 4);
	ret = config.AddFloatArray("d_vertex_l_color", (float*)&viewport_options.d_vertex_l_color, 4);
	ret = config.AddFloatArray("d_vertex_face_color", (float*)&viewport_options.d_vertex_face_color, 4);
	ret = config.AddFloatArray("d_vertex_face_n_color", (float*)&viewport_options.d_vertex_face_n_color, 4);

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
	if (w_console != nullptr)
		w_console->active = config.GetBool("show_console", w_console->active);
	else
		show_console = config.GetBool("show_console", show_console);
	if (w_config != nullptr)
		w_config->active = config.GetBool("show_configuration", w_config->active);
	else
		show_configuration = config.GetBool("show_configuration", show_configuration);

	// viewport options
	
	viewport_options.wireframe = config.GetBool("wireframe", viewport_options.wireframe);
	viewport_options.fill_faces = config.GetBool("fill_mode", viewport_options.fill_faces);
	viewport_options.debug_vertex_normals = config.GetBool("debug_vertex_normals", viewport_options.debug_vertex_normals);
	viewport_options.debug_face_normals = config.GetBool("debug_face_normals", viewport_options.debug_face_normals);
	
	// load colors ----------------------------------------------------------------------------------
	viewport_options.fill_color.x = config.GetFloat("fill_color", viewport_options.fill_color.x, 0);
	viewport_options.fill_color.y = config.GetFloat("fill_color", viewport_options.fill_color.y, 1);
	viewport_options.fill_color.z = config.GetFloat("fill_color", viewport_options.fill_color.z, 2);
	viewport_options.fill_color.w = config.GetFloat("fill_color", viewport_options.fill_color.w, 3);

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
	viewport_options.v_point_size = config.GetFloat("v_point_size", viewport_options.v_point_size);
	viewport_options.f_v_point_size = config.GetFloat("f_v_point_size", viewport_options.f_v_point_size);
	viewport_options.v_n_line_width = config.GetFloat("v_n_line_width", viewport_options.v_n_line_width);
	viewport_options.f_n_line_width = config.GetFloat("f_n_line_width", viewport_options.f_n_line_width);

	// TODO!!!!
	// if we have meshes, and line length doesnt match, recompute normals ----
	/*float temp_v = viewport_options.v_n_line_length;
	viewport_options.v_n_line_length = config.GetFloat("v_n_line_length", viewport_options.v_n_line_length);
	if (temp_v != viewport_options.v_n_line_length)
		ReComputeVertexNormals(viewport_options.v_n_line_length);

	float temp_f = viewport_options.f_n_line_length;
	viewport_options.f_n_line_length = config.GetFloat("f_n_line_length", viewport_options.f_n_line_length);
	if (temp_f != f_n_line_length)
		ReComputeFacesNormals(viewport_options.f_n_line_length);*/
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

		if (ImGui::MenuItem("Restore editor to default config"))
		{
			show_restore_popup = true;
		}

		if (ImGui::MenuItem("Quit", "ESC"))
			ret = false;
	

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

	return ret;
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

// DEBUG DATA MESH CLASS HELPER ----------------------------------------------------------------------------------------------------------

//DebugDataMesh::DebugDataMesh() {}

DebugDataMesh::DebugDataMesh(uint n_v, uint n_i) : n_vertices(n_v), n_idx(n_i) {}

DebugDataMesh::~DebugDataMesh() {}

void DebugDataMesh::GenAndFillBuffers()
{
	// gen --------------
	glGenBuffers(1, &debug_v_normals_gl_id);
	// debug draw faces normals buffers
	glGenBuffers(1, &debug_f_vertices_gl_id);
	glGenBuffers(1, &debug_f_normals_gl_id);

	// FILL -------------
	// WARNING: FOR NORMALS DEBUG ONLY, the normals for other calculations remains on normals float pointer ----
	glBindBuffer(GL_ARRAY_BUFFER, debug_v_normals_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * n_vertices, &debug_v_normals[0], GL_STATIC_DRAW);

	// TODO: 2nd warning, be careful, since we only load and print triangulated faces this is good
	glBindBuffer(GL_ARRAY_BUFFER, debug_f_vertices_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * n_idx, &debug_f_vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, debug_f_normals_gl_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * n_idx * 2, &debug_f_normals[0], GL_STATIC_DRAW);
}

void DebugDataMesh::ComputeVertexNormals(ModelData* goMesh, float length)
{
	// to draw lines, we need an array ready to what expects gldrawarrays
	// start point and finish point
	// TODO: improve this, thinking in deepth if this is possible with memcpy

	if (debug_v_normals != nullptr) delete[] debug_v_normals;

	debug_v_normals = new float[n_vertices * 6]; // 3 for startpoint, 3 more for endpoint

	uint n_count = 0;
	for (uint i = 0; i < n_vertices * 3; i += 3)
	{
		debug_v_normals[n_count] = goMesh->vertices[i]; // x
		debug_v_normals[n_count + 1] = goMesh->vertices[i + 1]; // y
		debug_v_normals[n_count + 2] = goMesh->vertices[i + 2]; //z

		debug_v_normals[n_count + 3] = goMesh->vertices[i] + goMesh->normals[i] * length; // x
		debug_v_normals[n_count + 4] = goMesh->vertices[i + 1] + goMesh->normals[i + 1] * length; // y
		debug_v_normals[n_count + 5] = goMesh->vertices[i + 2] + goMesh->normals[i + 2] * length; // z

		n_count += 6;
	}

}

bool DebugDataMesh::ComputeFacesNormals(ModelData* goMesh, float length)
{

	if (n_idx % 3 != 0)
	{
		LOG("[Error] Could not compute face normals, faces with != 3 vertex");
		return false;
	}

	if (debug_f_vertices != nullptr) delete[] debug_f_vertices;
	if (debug_f_normals != nullptr) delete[] debug_f_normals;

	debug_f_vertices = new float[n_idx];
	debug_f_normals = new float[n_idx * 2];

	uint i = 0;
	uint n_count = 0;
	for (; i < n_idx; i += 3)
	{
		// find the midface averaged vertex

		float* v1 = &goMesh->vertices[goMesh->indices[i] * 3];     // get first face vertex
		float* v2 = &goMesh->vertices[goMesh->indices[i + 1] * 3]; // get second face vertex
		float* v3 = &goMesh->vertices[goMesh->indices[i + 2] * 3]; // get third face vertex

		debug_f_vertices[i] = (v1[0] + v2[0] + v3[0]) / 3.0f; // x coord
		debug_f_vertices[i + 1] = (v1[1] + v2[1] + v3[1]) / 3.0f; // y coord
		debug_f_vertices[i + 2] = (v1[2] + v2[2] + v3[2]) / 3.0f; // z coord

		// compute the averaged normal of the 3 vertex of each face

		float* n1 = &goMesh->normals[goMesh->indices[i] * 3];     // get first face vertex normal
		float* n2 = &goMesh->normals[goMesh->indices[i + 1] * 3]; // get second face vertex normal
		float* n3 = &goMesh->normals[goMesh->indices[i + 2] * 3]; // get third face vertex normal

		float avg_n[3];
		avg_n[0] = (n1[0] + n2[0] + n3[0]) / 3.0f; // x coord
		avg_n[1] = (n1[1] + n2[1] + n3[1]) / 3.0f; // y coord
		avg_n[2] = (n1[2] + n2[2] + n3[2]) / 3.0f; // z coord

		debug_f_normals[n_count] = debug_f_vertices[i]; // x
		debug_f_normals[n_count + 1] = debug_f_vertices[i + 1]; // y
		debug_f_normals[n_count + 2] = debug_f_vertices[i + 2]; //z

		debug_f_normals[n_count + 3] = debug_f_vertices[i] + avg_n[0] * length; // x
		debug_f_normals[n_count + 4] = debug_f_vertices[i + 1] + avg_n[1] * length; // y
		debug_f_normals[n_count + 5] = debug_f_vertices[i + 2] + avg_n[2] * length; // z

		n_count += 6;
	}

	return true;
}

bool DebugDataMesh::Clean()
{

	glDeleteBuffers(1, &debug_v_normals_gl_id);
	glDeleteBuffers(1, &debug_f_vertices_gl_id);
	glDeleteBuffers(1, &debug_f_normals_gl_id);

	// delete all stored data

	delete[] debug_v_normals;
	delete[] debug_f_vertices;
	delete[] debug_f_normals;

	debug_v_normals = nullptr;
	debug_f_vertices = nullptr;
	debug_f_normals = nullptr;

	debug_v_normals_gl_id = 0;
	debug_f_vertices_gl_id = 0;
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

	// TODO: USE STRIDE with debug_f_normals_gl_id
	// and delete debug_f_vertices data

	glPointSize(pointSize);

	// draw points
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, debug_f_vertices_gl_id);
	glVertexPointer(3, GL_FLOAT, 0, (void*)0);

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