#ifndef _MODULE_EDITOR_H__
#define _MODULE_EDITOR_H__

#include "Globals.h"
#include "Module.h"

#include "external/Parson/parson.h"
#include <string>
#include <vector>

class Window;
class W_Config;
class W_Console;
class W_Hierarchy;
class W_Rendering;
class W_Scene;
class W_Inspector;

// TODO: change this/remove define
#define MAX_STORED_FPS 100

class ModuleEditor : public Module
{
public:
	ModuleEditor(bool start_enabled = true);

	~ModuleEditor();

	bool Init(Config& config);

	bool Start(Config& config);

	bool CleanUp();

	update_status PreUpdate(float dt);

	update_status Update(float dt);

	update_status PostUpdate(float dt);

	bool Save(Config& config);

	void Load(Config& config);

	bool SaveEditorConfig(const char* path);

	bool LoadEditorConfig(const char* path);

	// temporaly utils for imgui prefab functions
	void HelpMarker(const char* desc) const;
	void ShowSoftwareInfo() const;

	//bool SliderIntWithSteps(const char* label, int* v, int v_min, int v_max, int v_step, const char* display_format);

	void AddWindow(Window* panel_to_add);

	void DestroyWindow(Window* panel_to_add);

	// TODO: move this utility out here
	float map(float value, float s1, float stop1, float s2, float stop2) const;

private:

	bool DrawMainMenuBar();

	void DrawPopUps();

private:

	std::vector<Window*> windows;

	// General editor info ---------

	std::string editor_filename;

	bool show_restore_popup = false;
	bool show_demo_imgui = false;
	bool show_about_popup = false;
	bool show_console = false;
	bool show_configuration = false;

public:

	// Windows -----------------------
	W_Config*		w_config = nullptr;
	W_Console*		w_console = nullptr;
	W_Hierarchy*	w_hierarchy = nullptr;
	W_Rendering*	w_rendering = nullptr;
	W_Scene*		w_scene = nullptr;
	W_Inspector*	w_inspector = nullptr;
};

#endif // !_MODULE_EDITOR_H__