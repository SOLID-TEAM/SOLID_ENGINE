#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "SDL/include/SDL_opengl.h"

ModuleEditor::ModuleEditor(bool start_enabled) : Module(start_enabled)
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
	LOG("[Start] Loading Editor");
	bool ret = true;

	// TODO: remove this
	LOG("[Error] blabla");

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

	if (ImGui::BeginMenu("View"))
	{
		ImGui::MenuItem("Configuration", NULL, &show_configuration);
		ImGui::MenuItem("Console", NULL, &show_console);

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

	// TODO: Uncomment about, too annoying for testing xD

	//if (about)
	//{
	//	//{
	//	//	ImGui::Begin(("About SOLID Engine"), &about);
	//	//
	//	//	// TODO, load from json and add all required fields --------
	//	//	/*
	//	//	Name of your Engine
	//	//	One line description
	//	//	Name of the Author with link to github page
	//	//	Libraries (with versions queried in real time) used with links to their web
	//	//	Full text of the license
	//	//	*/
	//	ImGui::OpenPopup("About SOLID Engine");

	//	if (ImGui::BeginPopupModal("About SOLID Engine", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	//	{
	//		ImGui::Text("v0.1 - september 2019\n");
	//		ImGui::Separator();
	//		ImGui::BulletText("SOLID ENGINE is a project with the purpose of creating \n"
	//						  "a fully functional video game engine with its own innovations and features\n"
	//						  "implemented by SOLID TEAM. We are 2 Video Game Design and Development Degree students.");
	//		ImGui::Separator();
	//		ImGui::Text("3rd Party libs");
	//		SDL_version sdl_version;
	//		SDL_GetVersion(&sdl_version);
	//		ImGui::BulletText("SDL   v%d.%d.%d", sdl_version.major, sdl_version.minor, sdl_version.patch);
	//		ImGui::BulletText("glew  v%s", App->renderer3D->GetGlewVersionString().data());
	//		ImGui::BulletText("ImGui v%s", ImGui::GetVersion());
	//		ImGui::BulletText("Parson - JSON library parser");

	//		ImVec2 buttonSize = { 120.0f, 0.f };
	//		ImGuiStyle& style = ImGui::GetStyle();
	//		ImGui::Indent(ImGui::GetWindowWidth() * 0.5f - (buttonSize.x * 0.5f) - style.WindowPadding.x);
	//		if (ImGui::Button("OK", buttonSize)) { ImGui::CloseCurrentPopup(); about = false; }

	//	}
	//	ImGui::EndPopup();

	//}

	if (show_configuration)
	{
		if (ImGui::Begin("Configuration",&show_configuration))
		{
			if (ImGui::CollapsingHeader("Application"))
			{
				ImGui::Text("Testing");

				static int test = 60;

				if (ImGui::SliderInt("#Framerate cap", &test, 0, 144, "FramerateCap = %.3f"))
				{
					App->AdjustCappedMs(test);
				}
				ImGui::SameLine(); HelpMarker("Adjust to 0 to unlock cap");

				ImGui::Separator();

				ImGui::PushStyleColor(ImGuiCol_PlotHistogram, (ImVec4)ImColor(255,0,255));
				char title[25];
				sprintf_s(title, 25, "Framerate %.1f", stored_fps[stored_fps.size() - 1]);
				ImGui::PlotHistogram("##Framerate", &stored_fps[0], stored_fps.size(), 0, title, 0.0f, 100.0f, ImVec2(ImGui::GetWindowContentRegionWidth(), 100));
				
				ImGui::Separator();

				sprintf_s(title, 25, "ms %.1f", stored_ms[stored_ms.size() - 1]);
				ImGui::PlotHistogram("##Framerate", &stored_ms[0], stored_ms.size(), 0, title, 0.0f, 50.0f, ImVec2(ImGui::GetWindowContentRegionWidth(), 100));

				ImGui::PopStyleColor();
			}

			if (ImGui::CollapsingHeader("Window"))
			{
				// ----------------------------------------------------------------------
				// TODO:
				// bool active?
				// icon "file explorer"
				// TODO: loads from JSON
				static float brightness = 1.0f;
				static int width = SCREEN_WIDTH;
				static int height = SCREEN_HEIGHT;
				// fullscreen, resizable, borderless, fulldesktop || bools cols
				static bool fullscreen = WIN_FULLSCREEN;
				static bool fullscreen_desktop = WIN_FULLSCREEN_DESKTOP;
				static bool resizable = WIN_RESIZABLE;
				static bool borderless = WIN_BORDERLESS;
				// -----------------------------------------------------------------------

				if (ImGui::SliderFloat("Screen Brightness", &brightness, 0.0f, 1.0f))
				{   // only calls when slider is clicked
					&App->window->SetBrightness(&brightness);
				}

				if (ImGui::SliderInt("Window Width", &width, 320, 1920))
				{   // only calls when slider is clicked
					App->window->SetWindowSize(width, height);
				}
				if (ImGui::SliderInt("Window Height", &height, 240, 1080))
				{   // only calls when slider is clicked
					App->window->SetWindowSize(width, height);
				}

				ImGui::Separator();

				// GET DISPLAY data for every monitor
				SDL_DisplayMode dpm;
				// store color for highlighteds words
				ImColor hl_color = { 255,0,255 };
				for (int i = 0; i < SDL_GetNumVideoDisplays(); ++i)
				{
					// instead of pushing and poping styles, we can directly call ImGui::TextColored()
					SDL_GetCurrentDisplayMode(i, &dpm);
					ImGui::Text("Monitor:");
					ImGui::SameLine();
					ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)hl_color);
					ImGui::Text("%i", i);
					ImGui::PopStyleColor(1);
					ImGui::SameLine();
					ImGui::Text("Res:");
					ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)hl_color);
					ImGui::SameLine();
					ImGui::Text("%dx%dpx %ibpp", dpm.w, dpm.h, SDL_BITSPERPIXEL(dpm.format));
					ImGui::PopStyleColor(1);
					ImGui::Text("Refresh rate:");
					ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)hl_color);
					ImGui::SameLine();
					ImGui::Text("%dHz", dpm.refresh_rate);
					ImGui::PopStyleColor(1);
					ImGui::Separator();
				}

				if (ImGui::Checkbox("Fullscreen", &fullscreen))
				{
					App->window->SetWindowFullscreen(fullscreen);
				}
				ImGui::SameLine(); HelpMarker("Sets fullscreen with current window resolution");
				ImGui::SameLine();
				if (ImGui::Checkbox("Fullscreen Desktop", &fullscreen_desktop))
				{
					App->window->SetWindowFullscreen(fullscreen_desktop, true);
				}
				ImGui::SameLine(); HelpMarker("Sets fullscreen with current desktop resolution");

				// TODO: when the window is changed throught this, doesnt update current size on previous sliders (obviously)
				if (ImGui::Checkbox("Resizable ", &resizable))
				{
					App->window->SetWindowResizable(resizable);
				}
				ImGui::SameLine(); HelpMarker("Allow to resize the window manually");
				ImGui::SameLine();
				//ImGui::SameLine(test * stringLength);
				if (ImGui::Checkbox("Borderless", &borderless))
				{
					App->window->SetWindowBorderless(borderless);
				}
			}

			if (ImGui::CollapsingHeader("File System"))
			{
			}

			if (ImGui::CollapsingHeader("Input"))
			{
				static bool auto_scroll = true;

				ImGui::Text("Mouse Position:");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4( 255, 0, 0 ,255), "%i,%i", App->input->GetMouseX(), App->input->GetMouseY());

				ImGui::Text("Mouse Motion:");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(255, 0, 0, 255), "%i,%i", App->input->GetMouseXMotion(), App->input->GetMouseYMotion());

				ImGui::Text("Mouse Wheel:");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(255, 0, 0, 255), "%i", App->input->GetMouseZ());

				ImGui::Separator();

				ImGui::Checkbox("Auto scroll", &auto_scroll);
				ImGui::SameLine();
				if (ImGui::Button("Clear console"))
				{
					App->input->GetInputBuffer()->clear();
				}
				
				ImGui::BeginChild("Input Log");
				ImGui::TextUnformatted( App->input->GetInputBuffer()->begin());
				
				if (auto_scroll)
				{
					ImGui::SetScrollHere(1.0f);
				}

				ImGui::EndChild();
			}

			if (ImGui::CollapsingHeader("Hardware"))
			{
				Hardware_Info info;
				
				GetHardWareInfo(&info);
				std::string amount;

				ImGui::Text("CPU:");
				ImGui::SameLine();
				amount = std::to_string(info.cpu_count) + " (Cache: " + std::to_string(info.cpu_cache) + "Kb)";
				ImGui::TextColored({ 0, 1.0f, 1.0f, 1.0f }, amount.c_str());

				ImGui::Text("System RAM:");
				ImGui::SameLine();
				amount = std::to_string(info.sys_ram) + "Mb";
				ImGui::TextColored({ 0, 1.0f, 1.0f, 1.0f }, amount.c_str());

				ImGui::Text("Caps:");
				ImGui::SameLine();
				ImGui::TextColored({ 0, 1.0f, 1.0f, 1.0f }, info.caps.c_str());

				ImGui::Separator();

				ImGui::Text("GPU Brand:");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), info.gpu_vendor.c_str());

				ImGui::Text("Device: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), info.gpu_device.c_str());

				ImGui::Text("VRAM Dedicated: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%2.f Mb", info.vram_dedicated);

				ImGui::Text("VRAM Available: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%2.f Mb", info.vram_available);

				ImGui::Text("VRAM Current: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%2.f Mb", info.vram_current);

				ImGui::Text("VRAM Evicted: ");
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%2.f Mb", info.vram_evicted);

			}

		}
		ImGui::End();
	}

	if (show_console)
	{
		ImGui::SetNextWindowSize(ImVec2(380, 600), ImGuiCond_Once);
		static bool auto_scroll = true;
		if (ImGui::Begin("Console Log", &show_console))
		{

			ImGui::Checkbox("Auto scroll", &auto_scroll);
			ImGui::SameLine();
			if (ImGui::Button("Clear console"))
			{
				for (int i = 0; i < console_log.Size; i++)
					delete(console_log[i]);
				console_log.clear();
			}

			ImGui::SameLine();
			if (ImGui::Button("Save History Log"))
			{
				App->SaveLogToFile();
				LOG("[Info] Log File saved to hard disk");
			}
			ImGui::SameLine(); HelpMarker("Save all time history, doesn't affect console clearing");

			ImGui::Separator();

			ImGui::BeginChild("ScrollingRegion");

			// TODO: if we need thousands of lines of log, implement clipping (see on imgui_demo.cpp commentary)
			for (int i = 0; i < console_log.size(); i++)
			{
				const char* item = console_log[i];

				// Normally you would store more information in your item (e.g. make Items[](console_log) an array of structure, store color/type etc.)
				bool pop_color = false;

				//if (strstr(item, "[Error]")) { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); pop_color = true; }
				// strstr compare full string searching for match

				// change color of the line respect prefix tag
				// on [Error] | [Init] | [CleanUp] etc, only we need to compare the first 2 + \n from log 
				//(if we need we can delete for every line the jump) characters to perform a good match
				// update: deleted from editor log for now
				if (strncmp(item, "[Init]", 4) == 0) {       ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(100, 255, 97, 255)); pop_color = true;}
				else if (strncmp(item, "[Start]", 2) == 0) { ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(94, 156, 255, 255)); pop_color = true;}
				else if (strncmp(item, "[Info]", 4) == 0) {  ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(255, 230, 70, 255)); pop_color = true;}
				else if (strncmp(item, "[Error]", 2) == 0) { ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f)); pop_color = true;}

				ImGui::TextUnformatted(item);
				if (pop_color)
					ImGui::PopStyleColor();
			}

			/*ImGui::TextUnformatted(console_buffer.begin());*/
			if (auto_scroll)
				ImGui::SetScrollHereY(1.0f);
			ImGui::EndChild();
		}
		ImGui::End();
	}


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

void ModuleEditor::AddConsoleLog(const char* new_entry)
{
	//console_buffer.appendf(new_entry);
	console_log.push_back(strdup(new_entry));
}

void ModuleEditor::GetHardWareInfo( Hardware_Info *info)
{
	// CPU ------------------------------------
	info->cpu_count = SDL_GetCPUCount();
	info->cpu_cache = SDL_GetCPUCacheLineSize();
	// RAM ------------------------------------
	int sys_ram = SDL_GetSystemRAM();
	// CAPS -----------------------------------
	if (SDL_Has3DNow())
		info->caps.append("3DNow, ");
	if (SDL_HasAVX())
		info->caps.append("AVX, ");
	if (SDL_HasAVX2())
		info->caps.append("AVX2, ");
	if (SDL_HasMMX())
		info->caps.append("MMX, ");
	if (SDL_HasRDTSC())
		info->caps.append("RDTSC, ");
	if (SDL_HasSSE())
		info->caps.append("SSE, ");
	if (SDL_HasSSE2())
		info->caps.append("SSE2, ");
	if (SDL_HasSSE3())
		info->caps.append("SSE3, ");
	if (SDL_HasSSE41())
		info->caps.append("SSE41, ");
	if (SDL_HasSSE42())
		info->caps.append("SSE42, ");
	if (SDL_HasAltiVec)
		info->caps.append("AltiVec, ");
	// GPU ----------------------------------
	info->gpu_device.assign((const char*)glGetString(GL_RENDERER));
	info->gpu_vendor.assign((const char*)glGetString(GL_VENDOR));

	glGetFloatv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &info->vram_dedicated);
	info->vram_dedicated /= 1024.f;
	glGetFloatv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &info->vram_available);
	info->vram_available /= 1024.f;
	glGetFloatv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &info->vram_current);
	info->vram_current /= 1024.f;
	glGetFloatv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &info->vram_evicted);
	info->vram_evicted /= 1024.f;

}