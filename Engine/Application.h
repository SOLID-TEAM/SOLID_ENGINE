#pragma once

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"

#include "ModuleTest.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"


#include <list>

class Application
{
public:
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;

	ModuleTest* test = nullptr;
	ModuleRenderer3D* renderer3D = nullptr;
	ModuleCamera3D* camera = nullptr;
	
	ModuleEditor* editor = nullptr;

private:

	Timer	ms_timer;
	Timer fps_timer;
	Uint32 frames; // stores total life application looped frames
	float	dt;
	int fps_counter;
	int capped_ms;
	int last_frame_ms; // stores amount of ms of last frame cycle
	int last_fps; // stores last second total frames

	std::list<Module*> list_modules;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void RequestBrowser(const char* b_path) const;
	void AdjustCappedMs(int max_frames);

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};