#pragma once

#include "Globals.h"
#include "Timer.h"
#include <list>
#include <string>

#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleTest.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "PanelConfig.h"
#include "PanelConsole.h"

#include "Config.h"

class Application
{
public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void RequestBrowser(const char* b_path) const;
	void AdjustCappedMs(int max_frames);
	void Log(const char* new_entry);
	void SaveLogToFile() const;

	bool WantToSave();
	bool WantToLoad();

private:
	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

	//
	bool SaveConfig(Config& config);
	bool LoadConfig(Config& config);

public:
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;

	ModuleTest* test = nullptr;
	ModuleRenderer3D* renderer3D = nullptr;
	ModuleCamera3D* camera = nullptr;
	
	ModuleEditor* editor = nullptr;

	// TODO: Improve this aux 
	ImVector<char*> console_log_aux;

private:

	Timer	ms_timer;
	Timer fps_timer;
	Uint32 frames; // stores total life application looped frames
	float	dt;
	int fps_counter;
	int capped_ms;
	int max_frames = 60; // for default value
	Uint32 last_frame_ms; // stores amount of ms of last frame cycle
	int last_fps; // stores last second total frames

	std::list<Module*> list_modules;

	std::string log_buffer;
	std::string app_name;
	std::string organization_name;
public:
	// testing persistence config
	Config* config = nullptr;
	std::string config_filename;
};

extern Application* App;