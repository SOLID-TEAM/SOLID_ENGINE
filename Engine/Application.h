#ifndef _APPLICATION_H__
#define _APPLICATION_H__

#include "Globals.h"
#include "Timer.h"
#include <list>
#include <string>
// Modules -------------------
#include "Module.h"
#include "ModuleTime.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleTest.h"
#include "ModuleRenderer3D.h"
#include "ModuleImporter.h"
#include "ModuleEditor.h"
#include "ModuleFileSystem.h"
#include "ModuleTextures.h"
#include "ModulePhysics.h"
#include "ModuleScene.h"
#include "ModuleResources.h"

// Tools ---------------------
#include "Config.h"
#include "HardwareInfo.h"
#include "external/MathGeoLib/include/Algorithm/Random/LCG.h"

// Panels --------------------
#include "W_Config.h"
#include "W_Console.h"

struct Event;

class Application
{
public:

	Application();

	~Application();

	bool Init();

	update_status Update();

	bool CleanUp();

	void RequestBrowser(const char* b_path) const;

	void Log(const char* new_entry);

	void SaveLogToFile() const;

	void BroadcastEvent(const Event& event);

	bool WantToSave(bool cleanInit = false);

	void WantToLoad(bool restoreDefault = false);

private:

	void AddModule(Module* mod);

	bool SaveConfig(Config& config);

	bool LoadConfig(Config& config);

public:

	// Tools ----------------------------------
	HardwareInfo* hardware = nullptr;
	Config* config = nullptr;
	std::string config_filename;
	LCG* random = nullptr;

	// Modules --------------------------------

	ModuleTime* time = nullptr;
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleTest* test = nullptr;
	ModuleRenderer3D* renderer3D = nullptr;
	ModuleTextures* textures = nullptr;
	ModuleImporter* importer = nullptr;
	ModuleEditor* editor = nullptr;
	ModuleFileSystem* file_sys = nullptr;
	ModulePhysics* physics = nullptr;
	ModuleScene* scene = nullptr;
	ModuleResources* resources = nullptr;

	// Buffers ------------------------------------
	ImVector<char*> console_log_aux;
	std::string log_buffer;
	std::string app_name;
	std::string organization_name;

private:

	// General ----------------------------------
	std::list<Module*> list_modules;
};

extern Application* App;

#endif // !_APPLICATION_H__
