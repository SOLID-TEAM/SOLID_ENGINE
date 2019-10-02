#pragma once
#include "Module.h"
#include "Globals.h"

#include "external/Parson/parson.h"
#include <string>
#include <vector>

class PanelConfig;
class PanelConsole;

// TODO: change this/remove define
#define MAX_STORED_FPS 100

class ModuleEditor : public Module
{
public:
	ModuleEditor(bool start_enabled = true);
	~ModuleEditor();

	bool Init(Config& config);
	bool Start(Config& config);
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	bool SaveEditorConfig(const char* path);
	bool LoadEditorConfig(const char* path);

	// temporaly utils for imgui prefab functions
	void HelpMarker(const char* desc) const;

private:

	// General editor info ---------

	std::string editor_filename;
	bool showcase = true;
	bool about = false;

public:

	// Panels -----------------------

	PanelConfig* config = nullptr;
	PanelConsole* console = nullptr;
};

