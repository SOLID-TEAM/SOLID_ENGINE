#include "ImGui/imgui.h"
#include "SDL/include/SDL_opengl.h"

#include "Application.h"
#include "PanelConfig.h"
#include "ModuleEditor.h"

PanelConfig::PanelConfig(std::string name, bool active): Panel(name, active)
{
	stored_fps.resize(MAX_STORED_FPS);
	stored_ms.resize(MAX_STORED_FPS);
}

PanelConfig::~PanelConfig()
{
}

void PanelConfig::Draw()
{
	if (ImGui::Begin("Configuration", &active))
	{
		if (ImGui::CollapsingHeader("Application"))
		{
			ImGui::Text("Testing");

			static int test = 60;

			if (ImGui::SliderInt("#Framerate cap", &test, 0, 144, "FramerateCap = %.3f"))
			{
				App->AdjustCappedMs(test);
			}
			ImGui::SameLine(); App->editor->HelpMarker("Adjust to 0 to unlock cap");

			ImGui::Separator();

			ImGui::PushStyleColor(ImGuiCol_PlotHistogram, (ImVec4)ImColor(255, 0, 255));
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
			static int width = (int)App->window->current_w;
			static int height = (int)App->window->current_h;
			// fullscreen, resizable, borderless, fulldesktop || bools cols
			static bool fullscreen = App->window->fullscreen;
			static bool fullscreen_desktop = App->window->fullscreen_desktop;
			static bool resizable = App->window->resizable;
			static bool borderless = App->window->borderless;
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
			ImGui::SameLine(); App->editor->HelpMarker("Sets fullscreen with current window resolution");
			ImGui::SameLine();
			if (ImGui::Checkbox("Fullscreen Desktop", &fullscreen_desktop))
			{
				App->window->SetWindowFullscreen(fullscreen_desktop, true);
			}
			ImGui::SameLine(); App->editor->HelpMarker("Sets fullscreen with current desktop resolution");

			// TODO: when the window is changed throught this, doesnt update current size on previous sliders (obviously)
			if (ImGui::Checkbox("Resizable ", &resizable))
			{
				App->window->SetWindowResizable(resizable);
			}
			ImGui::SameLine(); App->editor->HelpMarker("Allow to resize the window manually");
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
			ImGui::TextColored(ImVec4(255, 0, 0, 255), "%i,%i", App->input->GetMouseX(), App->input->GetMouseY());

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
				input_buffer.clear();
			}

			ImGui::BeginChild("Input Log");
			ImGui::TextUnformatted(input_buffer.begin());

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

void PanelConfig::GetHardWareInfo(Hardware_Info* info)
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

void PanelConfig::AddFpsLog(const float fps, const float ms)
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

// Add a input event to common buffer
// flag : 0 = keyboard , 1 = mouse
void PanelConfig::AddInputLog(uint key, KEY_STATE state, int flag)
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
