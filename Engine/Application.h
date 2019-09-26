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
	float	dt;
	std::list<Module*> list_modules;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void RequestBrowser(const char* b_path) const;

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};