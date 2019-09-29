#pragma once
#include "Module.h"
#include "Globals.h"

#include "ImGui/imgui.h"
#include "ImGui/Impl/imgui_impl_sdl.h"
#include "ImGui/Impl/imgui_impl_opengl3.h"

#include "external/Parson/parson.h"
#include <string>
#include <vector>

// TODO: change this/remove define
#define MAX_STORED_FPS 100

class ModuleEditor : public Module
{
public:
	ModuleEditor(bool start_enabled = true);
	~ModuleEditor();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	bool SaveEditorConfig(const char* path);
	bool LoadEditorConfig(const char* path);

	void AddLastFps(const float fps, const float ms);
	void AddConsoleLog(const char* new_entry);

	// temporaly utils for imgui prefab functions
	void HelpMarker(const char* desc) const;


// IMGUI ----------------
private:
	std::string editor_filename;
	// wip menu
	// menu bar HELP ------
	bool showcase = true;
	bool about = false;
	bool show_console = true;
	bool show_configuration = true;
// ----------------------
// FPS
private:
	std::vector<float> stored_fps;
	std::vector<float> stored_ms;
	//ImGuiTextBuffer console_buffer; // used for raw text (without widgets or custom colors)
	ImVector<char*> console_log;

};

