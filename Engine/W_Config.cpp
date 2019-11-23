#include "ImGui/imgui_plot.h"
#include "ImGui/misc/cpp/imgui_stdlib.h" // input text wrapper for std::string
#include "SDL/include/SDL_opengl.h"

#include "Application.h"
#include "W_Config.h"
#include "ModuleEditor.h"

W_Config::W_Config(std::string name, bool active): Window(name, active)
{
	stored_fps.resize(MAX_STORED_FPS);
	stored_ms.resize(MAX_STORED_FPS);
	stored_mem.resize(MAX_STORED_FPS);
}

W_Config::~W_Config()
{
}

void W_Config::Draw()
{
	ImVec4 green = { 20.f / 255.f, 196.f / 255.f, 55.f / 255.f , 1.f };
	std::string final_str;

	if (ImGui::Begin("Configuration", &active))
	{
		if (ImGui::CollapsingHeader("Aplication"))
		{
			ImGui::Spacing();
			ImGui::Title("Aplication"); 	ImGui::InputText("##Aplication Name", &App->app_name);
			ImGui::Title("Organization"); 	ImGui::InputText("##Organization Name", &App->organization_name);
			ImGui::Spacing();

		}
		if (ImGui::CollapsingHeader("Diagnostic Tools"))
		{
			ImGui::Spacing();
			static int cap = 60;
			ImGui::Title("Framerate"); if (ImGui::SliderInt("##Framerate cap", &cap, 0, 144, "%f")) App->time->SetCap(cap);
			ImGui::Spacing();

			ImGui::PlotConfig conf;
			//conf.values.xs = x_data; // this line is optional
			conf.values.ys = &stored_fps[0];
			conf.values.count = MAX_STORED_FPS;
			conf.scale.min = -1;
			conf.scale.max = 100;
			conf.tooltip.show = true;
			conf.tooltip.format = "%.2f";
			conf.grid_x.show = true;
			conf.grid_y.show = true;
			conf.frame_size = ImVec2(ImGui::GetContentRegionAvailWidth(), 100 ); // OBSOLETE?
			conf.line_thickness = 2.f;
			final_str.assign("FPS: " + std::to_string((int)stored_fps[stored_fps.size() - 1]));
			conf.overlay_text = final_str.c_str();
			conf.grid_x.show = false;
			conf.grid_y.show = false;
			
			ImGui::Plot("FPS", conf);
			ImGui::Spacing();

			conf.tooltip.format = "%.2f";
			final_str.assign("MS: " + std::to_string((int)stored_ms[stored_ms.size() - 1]));
			conf.overlay_text = final_str.c_str();
			conf.values.ys = &stored_ms[0];

			ImGui::Plot("Ms", conf);
			ImGui::Spacing();

			conf.scale.max = 20000;
			conf.tooltip.format = "Mb: %.2f";
			final_str.assign("Mem Usage: " + std::to_string((int)stored_mem[stored_mem.size() - 1]));
			conf.overlay_text = final_str.c_str();
			conf.values.ys = &stored_mem[0];

			ImGui::Plot("Memory Usage", conf);
			ImGui::Spacing();

			ImGui::PushColumnWidth(200.f);
			ImGui::Title("Total:", 1 ); 		ImGui::TextColored(green, "%i",		App->hardware->ram_usage.total_reported_mem);
			ImGui::Title("Actual:", 2); 		ImGui::TextColored(green, "%i",			App->hardware->ram_usage.total_actual_mem);
			ImGui::Title("Alloc:" ,2);			ImGui::TextColored(green, "%i", App->hardware->ram_usage.total_alloc_unity_count);

			ImGui::Title("Peak:", 1);			ImGui::TextColored(green, "%i",		App->hardware->ram_usage.peak_reported_mem);
			ImGui::Title("Actual:", 2); 		ImGui::TextColored(green, "%i",			App->hardware->ram_usage.peak_actual_mem);
			ImGui::Title("Alloc:" , 2);			ImGui::TextColored(green, "%i", App->hardware->ram_usage.peak_alloc_unit_count);

			ImGui::Title("Accumulated:", 1);	ImGui::TextColored(green, "%i", App->hardware->ram_usage.accumulated_reported_mem);
			ImGui::Title("Actual:", 2);			ImGui::TextColored(green, "%i",	App->hardware->ram_usage.accumulated_actual_mem);
			ImGui::Title("Alloc:", 2);			ImGui::TextColored(green, "%i",	App->hardware->ram_usage.accumulated_alloc_unit);

			ImGui::PopColumnWidth();

			ImGui::Spacing();
		}

		if (ImGui::CollapsingHeader("Window"))
		{
			// ----------------------------------------------------------------------
			// TODO:
			// bool active?
			// icon "file explorer"
			// TODO: loads from JSON
			static float brightness = 1.0f;
			int width = (int)App->window->current_w;
			static int height = (int)App->window->current_h;
			// fullscreen, resizable, borderless, fulldesktop || bools cols
			bool fullscreen = App->window->fullscreen;
			bool fullscreen_desktop = App->window->fullscreen_desktop;
			bool resizable = App->window->resizable;
			bool borderless = App->window->borderless;
			bool maximized = App->window->maximized;
			// -----------------------------------------------------------------------
			ImGui::Spacing();
			ImGui::Title("Fullscreen");			if (ImGui::Checkbox("##Fullscreen", &fullscreen)) App->window->SetWindowFullscreen(fullscreen);
			ImGui::Title("Desktop");			if (ImGui::Checkbox("##Fullscreen Desktop", &fullscreen_desktop)) App->window->SetWindowFullscreen(false, fullscreen_desktop);
			ImGui::Title("Resizable");			if (ImGui::Checkbox("##Resizable ", &resizable)) App->window->SetWindowResizable(resizable);
			ImGui::Title("Borderless");			if (ImGui::Checkbox("##Borderless", &borderless)) App->window->SetWindowBorderless(borderless);
			ImGui::Title("Maximized");			if (ImGui::Checkbox("##Maximized", &maximized)) App->window->SetWindowMaximized(maximized);

			ImGui::Spacing();
			ImGui::Title("Screen Brightness "); if (ImGui::SliderFloat("##Screen Brightness", &brightness, 0.0f, 1.0f)) &App->window->SetBrightness(&brightness);
			ImGui::Title("Window Width"); 		if (ImGui::SliderInt("##Window Width", &width, 320, 1920)) App->window->SetWindowSize(width, height);
			ImGui::Title("Window Height");		if (ImGui::SliderInt("##Window Height", &height, 240, 1080)) App->window->SetWindowSize(width, height);

			ImGui::Spacing();
		}

		if (ImGui::CollapsingHeader("Hardware Info"))
		{
			std::string amount;

			amount = std::to_string(App->hardware->system.cpu_count) + " (Cache: " + std::to_string(App->hardware->system.cpu_cache) + "Kb)";

			ImGui::Title("CPU:");				ImGui::TextColored(green, amount.c_str());

			amount = std::to_string(App->hardware->system.sys_ram) + "Mb";

			ImGui::Title("System RAM:");		ImGui::TextColored(green, amount.c_str());
			ImGui::Title("Caps:");				ImGui::TextColored(green, App->hardware->system.caps.c_str());

			ImGui::Separator();

			ImGui::Title("GPU Brand:");			ImGui::TextColored(green, App->hardware->gpu.vendor.c_str());
			ImGui::Title("Device: "); 			ImGui::TextColored(green, App->hardware->gpu.device.c_str());
			ImGui::Title("VRAM Dedicated: ");	ImGui::TextColored(green, "%2.f Mb", App->hardware->gpu.vram_dedicated);
			ImGui::Title("VRAM Available: "); 	ImGui::TextColored(green, "%2.f Mb", App->hardware->gpu.vram_available);
			ImGui::Title("VRAM Current: ");		ImGui::TextColored(green, "%2.f Mb", App->hardware->gpu.vram_current);
			ImGui::Title("VRAM Evicted: ");		ImGui::TextColored(green, "%2.f Mb", App->hardware->gpu.vram_evicted);

			ImGui::Separator();

			// GET DISPLAY data for every monitor
			SDL_DisplayMode dpm;

			for (int i = 0; i < SDL_GetNumVideoDisplays(); ++i)
			{
				// instead of pushing and poping styles, we can directly call ImGui::TextColored()
				SDL_GetCurrentDisplayMode(i, &dpm);
				ImGui::Title("Monitor:");		ImGui::TextColored(green, "%i", i);;
				ImGui::Title("Res:");			ImGui::TextColored(green, "%dx%dpx %ibpp", dpm.w, dpm.h, SDL_BITSPERPIXEL(dpm.format));
				ImGui::Title("Refresh rate:");	ImGui::TextColored(green, "%dHz", dpm.refresh_rate);

				ImGui::Separator();
			}

			if (ImGui::CollapsingHeader("View supported GL extensions"))
			{
				char* gl_extensions = (char*)glGetString(GL_EXTENSIONS);

				for (int i = 0; gl_extensions[i]; ++i)
				{
					if (gl_extensions[i] == ' ')
					{
						gl_extensions[i] = '\n';
					}
				}
				{
					ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
					ImGui::BeginChild("Child1", ImVec2(ImGui::GetWindowContentRegionWidth(), 300), true, window_flags);

					ImGui::TextColored(green, "%s", gl_extensions);
					ImGui::EndChild();
				}
			}

			ImGui::Separator();
		}

		if (ImGui::CollapsingHeader("Software Info"))
		{
			App->editor->ShowSoftwareInfo();
		}



		if (ImGui::CollapsingHeader("Input"))
		{
			static bool auto_scroll = true;

			ImGui::Title("Mouse Position:");	ImGui::TextColored(green, "%i,%i", App->input->GetMouseX(), App->input->GetMouseY());
			ImGui::Title("Mouse Motion:");		ImGui::TextColored(green, "%i,%i", App->input->GetMouseXMotion(), App->input->GetMouseYMotion());
			ImGui::Title("Mouse Wheel:");		ImGui::TextColored(green, "%i", App->input->GetMouseZ());

			ImGui::Separator();

			ImGui::Title("Auto Scroll"); ImGui::Checkbox("##Autoscroll_Input", &auto_scroll); ImGui::SameLine();  if (ImGui::Button("Clear console")) input_buffer.clear();
	
			ImGui::BeginChild("Input Log", ImVec2(0,0) , true, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysUseWindowPadding);
			ImGui::TextUnformatted(input_buffer.begin());

			if (auto_scroll)
			{
				ImGui::SetScrollHere(1.0f);
			}

			ImGui::EndChild();
		}
		if (ImGui::CollapsingHeader("File System"))
		{
		}

		// TODO: SEARCH A BETTER PLACE TO PUT ALL INFO FOR LOADED TEXTURES
		if (ImGui::CollapsingHeader("Loaded Textures VRam"))
		{
			//ImVec2 test = ImGui::GetItemRectSize();// ImGui::GetContentRegionAvailWidth();
			static bool delete_popup = false;
			static uint selected_tex_button_id = 0;

			if (delete_popup)
			{
				ImGui::OpenPopup("Are you sure?");
				if (ImGui::BeginPopupModal("Are you sure?", NULL, ImGuiWindowFlags_AlwaysAutoResize))
				{
					ImGui::Text("Delete image from vram buffer? \n");
					ImGui::Separator();
					if (ImGui::Button("YES", ImVec2(120, 0)))
					{
						delete_popup = false;
						App->textures->FreeTextureBuffer(selected_tex_button_id);
						ImGui::CloseCurrentPopup();
					}
					ImGui::SameLine();
					if (ImGui::Button("NO", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); delete_popup = false; }
					ImGui::EndPopup();
				}
			}
			
			static bool h_borders = true;
			static bool v_borders = true;
			static int columns_count = 3;
			static int box_w = 175;
			const int box_min = 64;
			const int box_max = 512;
			uint num_textures = App->textures->textures.size();
			ImVec2 texture_box(box_w, box_w);
			
			static bool flip_vertical = true;	// by default flipped (standard visor)
			static bool flip_horizontal = true; // ""
			ImVec2 uv0 = { 0,0 };
			ImVec2 uv1 = { 1,1 };
			
			// sum required coords to flip textures
			if (flip_horizontal && flip_vertical)
			{
				uv0 = { 0,1 };
				uv1 = { 1,0 };
			}
			else if (flip_vertical)
			{
				uv0 = { 1,1 };
				uv1 = { 0,0 };
			}
			else if (flip_horizontal)
			{
				uv0 = { 1,0 };
				uv1 = { 0,1 };
			}

			float avail_w = ImGui::GetColumnWidth();

			columns_count = (int)avail_w / (int)texture_box.x;

			/*if (columns_count > num_textures) // adjust columns 
				columns_count = num_textures;*/

			if (columns_count < 1)
				columns_count = 1;

			ImGui::Text("Currently loaded: %i textures", num_textures);

			ImGui::SliderInt("Miniature size", &box_w, box_min, box_max);

			ImGui::Checkbox("Flip Vertical", &flip_vertical); ImGui::SameLine();
			ImGui::Checkbox("Flip Horizontal", & flip_horizontal);

			std::map<std::string, uint>::iterator textures = App->textures->textures.begin();
			ImGui::Columns(columns_count, NULL, v_borders);
			// TODO: center at mid of avail_w all widgets
			int widget_id = 0;
			for (; textures != App->textures->textures.end(); ++textures)
			{
				ImGui::PushID(widget_id++);
				if (h_borders && ImGui::GetColumnIndex() == 0)
					ImGui::Separator();

				ImGui::Image((ImTextureID)(*textures).second, texture_box, uv0, uv1);
				ImGui::TextColored(green, (*textures).first.c_str()); 
				// get texture data
				int w = 0, h = 0;// , d = 0;
				glBindTexture(GL_TEXTURE_2D, (*textures).second);
				glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w);
				glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h);
				//glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_DEPTH, &d);
				glBindTexture(GL_TEXTURE_2D, 0);
				ImGui::Text("Width: %i", w);
				ImGui::Text("Height: %i", h);
				ImGui::Text("Buffer id: %i", (*textures).second);
				//ImGui::Text("Depth %i", d);
				//ImGui::Text("Offset %.2f", ImGui::GetColumnOffset());
				
				if (ImGui::Button("Delete" , ImVec2(box_w, 0.0f)))
				{
					// modal popup
					delete_popup = true;
					selected_tex_button_id = (*textures).second;
				}
				ImGui::NextColumn();

				ImGui::PopID();
			}
			ImGui::Columns(1);
			if (h_borders)
				ImGui::Separator();

			
		}

		if (ImGui::CollapsingHeader("Loaded Resources"))
		{
			std::map<UID, Resource*> all_map = App->resources->GetAllMapResources();
			std::map<UID, Resource*>::iterator all = all_map.begin();
			
			for (; all != all_map.end(); ++all)
			{
				Resource* r = (*all).second;
				ImGui::Separator();
				ImGui::Text("Resource UID: %s", r->GetNameFromUID().c_str());
				std::string type_str;
				switch (r->GetType())
				{
					case Resource::Type::MESH:
						type_str.assign("MESH");
						break;
					case Resource::Type::MODEL:
						type_str.assign("MODEL");
						break;
					case Resource::Type::TEXTURE:
						type_str.assign("TEXTURE");
						break;
					case Resource::Type::MATERIAL:
						type_str.assign("MATERIAL");
						break;
					default:
						type_str.assign("moron");
						break;
					break;
				}
				ImGui::Text("Type: %s", type_str.c_str());
				ImGui::Text("Name: %s", (*all).second->GetName().c_str());
				ImGui::Text("Load: %i", r->CountReferences());

			}
			
		}

		
	}
	ImGui::End();
}

void W_Config::AddFpsLog(const float fps, const float ms)
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

void W_Config::AddMemoryLog(const float mem)
{
	// stored_fps and ms should be the same
	if (stored_mem.size() >= MAX_STORED_FPS)
	{
		// iterate and shift values
		for (uint i = 0; i < MAX_STORED_FPS - 1; ++i)
		{
			stored_mem[i] = stored_mem[i + 1];
		}

		stored_mem[MAX_STORED_FPS - 1] = mem;
	}
	else
	{
		stored_mem.push_back(mem);
	}
}

// Add a input event to common buffer
// flag : 0 = keyboard , 1 = mouse
void W_Config::AddInputLog(uint key, KEY_STATE state, int flag)
{
	static char entry[512];
	static const char* states[] = { "IDLE", "DOWN", "REPEAT", "UP" };

	const char* key_name = SDL_GetScancodeName((SDL_Scancode)key);

	if (flag == 0)
	{
		sprintf_s(entry, 512, "Key: %s - %s\n", key_name, states[state]);
	}
	else
	{
		sprintf_s(entry, 512, "Mouse: %02u - %s\n", key, states[state]);
	}

	input_buffer.appendf(entry);
}
