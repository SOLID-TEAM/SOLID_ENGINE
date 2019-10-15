#ifndef _APPLICATION_H__
#define _APPLICATION_H__

#include "Globals.h"
#include "Timer.h"
#include <list>
#include <string>
// Modules -------------------
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleTest.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleImporter.h"
#include "ModuleEditor.h"
#include "ModuleFileSystem.h"
#include "ModuleTextures.h"
#include "ModuleScene.h"

// Tools ---------------------
#include "Config.h"
#include "HardwareInfo.h"

// Panels --------------------
#include "W_Config.h"
#include "W_Console.h"

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
	int GetFrames();
	void SaveLogToFile() const;

	bool WantToSave(bool cleanInit = false);
	void WantToLoad(bool restoreDefault = false);

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

	bool SaveConfig(Config& config);
	bool LoadConfig(Config& config);

public:

	// Tools ----------------------------------
	HardwareInfo* hardware = nullptr;
	Config* config = nullptr;
	std::string config_filename;
	int max_frames = 60;

	// Modules --------------------------------
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleTest* test = nullptr;
	ModuleRenderer3D* renderer3D = nullptr;
	ModuleCamera3D* camera = nullptr;
	ModuleTextures* textures = nullptr;
	ModuleImporter* importer = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleFileSystem* file_sys = nullptr;
	ModuleScene* scene = nullptr;

	// Buffers ------------------------------------
	ImVector<char*> console_log_aux;
	std::string log_buffer;

	std::string app_name;
	std::string organization_name;
private:

	// General ----------------------------------
	std::list<Module*> list_modules;

	// Framerate --------------------------------
	Timer	ms_timer;
	Timer fps_timer;
	Uint32 frames; // stores total life application looped frames
	float	dt;
	int fps_counter;
	int capped_ms;
	Uint32 last_frame_ms; // stores amount of ms of last frame cycle
	int last_fps; // stores last second total frames
};

extern Application* App;

#endif // !_APPLICATION_H__
