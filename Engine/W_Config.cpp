#include "ImGui/imgui_plot.h"
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
	static char app_name[40];
	static char org_name[40];

	if (ImGui::Begin("Configuration", &active))
	{
		if (ImGui::CollapsingHeader("Aplication"))
		{
			ImGui::Spacing();
			ImGui::Title("Aplication"); 	ImGui::InputText("##Aplication Name", app_name, 40);
			ImGui::Title("Organization"); 	ImGui::InputText("##Organization Name", org_name, 40);
			ImGui::Spacing();

		}
		if (ImGui::CollapsingHeader("Diagnostic Tools"))
		{
			ImGui::Spacing();
			static int cap = 60;
			ImGui::Title("Framerate"); if (ImGui::SliderInt("##Framerate cap", &cap, 0, 144, "%.2f")) App->AdjustCappedMs(cap);
			ImGui::Spacing();

			ImGui::PlotConfig conf;
			//conf.values.xs = x_data; // this line is optional
			conf.values.ys = &stored_fps[0];
			conf.values.count = MAX_STORED_FPS;
			conf.scale.min = -1;
			conf.scale.max = 100;
			conf.tooltip.show = true;
			conf.tooltip.format = "FPS: %.2f";
			conf.grid_x.show = true;
			conf.grid_y.show = true;
			conf.frame_size = ImVec2(ImGui::GetContentRegionAvailWidth(), 100 );
			conf.line_thickness = 2.f;
			conf.overlay_text = "FPS";
			conf.grid_x.show = false;
			conf.grid_y.show = false;
			
			ImGui::Plot("FPS", conf);
			ImGui::Spacing();

			conf.tooltip.format = "Ms: %.2f";
			conf.overlay_text = "Ms";
			conf.values.ys = &stored_ms[0];

			ImGui::Plot("Ms", conf);
			ImGui::Spacing();

			conf.scale.max = 20000;
			conf.tooltip.format = "Mb: %.2f";
			conf.overlay_text = "Memory Usage";
			conf.values.ys = &stored_mem[0];

			ImGui::Plot("Memory Usage", conf);
			ImGui::Spacing();

			ImGui::PushColumnWidth(200.f);
			ImGui::Title("Total Reported:"); 		ImGui::TextColored(green, "%i",		App->hardware->ram_usage.total_reported_mem);
			ImGui::Title("Total Actual: "); 		ImGui::TextColored(green, "%i",			App->hardware->ram_usage.total_actual_mem);
			ImGui::Title("Peak Reported: ");		ImGui::TextColored(green, "%i",		App->hardware->ram_usage.peak_reported_mem);
			ImGui::Title("Peak Actual: "); 			ImGui::TextColored(green, "%i",			App->hardware->ram_usage.peak_actual_mem);
			ImGui::Title("Accumulated Reported: ");	ImGui::TextColored(green, "%i", App->hardware->ram_usage.accumulated_reported_mem);
			ImGui::Title("Accumulated Actual: ");	ImGui::TextColored(green, "%i",	App->hardware->ram_usage.accumulated_actual_mem);
			ImGui::Title("Accumulated Alloc: ");	ImGui::TextColored(green, "%i",	App->hardware->ram_usage.accumulated_alloc_unit);
			ImGui::Title("Total Alloc Unit: ");		ImGui::TextColored(green, "%i",		App->hardware->ram_usage.total_alloc_unity_count);
			ImGui::Title("PeakAlloc Unit: ");		ImGui::TextColored(green, "%i",		App->hardware->ram_usage.peak_alloc_unit_count);
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
			// -----------------------------------------------------------------------
			ImGui::Spacing();
			ImGui::Title("Fullscreen");			if (ImGui::Checkbox("##Fullscreen", &fullscreen)) App->window->SetWindowFullscreen(fullscreen);
			ImGui::Title("Desktop");			if (ImGui::Checkbox("##Fullscreen Desktop", &fullscreen_desktop)) App->window->SetWindowFullscreen(false, fullscreen_desktop);
			ImGui::Title("Resizable");			if (ImGui::Checkbox("##Resizable ", &resizable)) App->window->SetWindowResizable(resizable);
			ImGui::Title("Borderless");			if (ImGui::Checkbox("##Borderless", &borderless)) App->window->SetWindowBorderless(borderless);

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
