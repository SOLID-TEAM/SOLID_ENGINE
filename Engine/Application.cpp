#include "Application.h"
#include "PanelConfig.h"
#include "PanelConsole.h"
#include <fstream>

Application::Application()
{
	LOG("-------------- Application Creation --------------");

	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	test = new ModuleTest(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	editor = new ModuleEditor(this);

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	
	// Scenes
	AddModule(test);

	// Renderer last!

	AddModule(editor);
	AddModule(renderer3D);

}

Application::~Application()
{
	std::list<Module*>::reverse_iterator item = list_modules.rbegin();

	while(item != list_modules.rend())
	{
		delete(*item);
		++item;
	}

	list_modules.clear();
}

bool Application::Init()
{
	bool ret = true;

	LOG("-------------- Application Init --------------");

	// set fps data before start/init modules
	fps_counter = 0;
	frames = 0;
	capped_ms = 1000 / 60;
	last_frame_ms = 0;
	last_fps = 0;
	
	// Call Init() in all modules
	std::list<Module*>::iterator item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->Init();
		++item;
	}

	// After all Init calls we call Start() in all modules

	LOG("-------------- Application Start --------------");
	item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->Start();
		++item;
	}
	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	// framerate calcs
	++fps_counter;
	++frames;

	if (fps_timer.Read() >= 1000)
	{
		last_fps = fps_counter;
		fps_counter = 0;
		fps_timer.Start();
	}

	// gets last frame ms before delay comes in
	last_frame_ms = ms_timer.Read();

	// limit framerate
	if (capped_ms > 0 && (last_frame_ms < capped_ms))
		SDL_Delay((Uint32)capped_ms - last_frame_ms);

	// testing after sdl delay for plot purposes too
	last_frame_ms = ms_timer.Read();

	// save last fps to module editor vector
	editor->config->AddFpsLog((float)last_fps, (float)last_frame_ms);

}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	std::list<Module*>::iterator item = list_modules.begin();
	
	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PreUpdate(dt);
		++item;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->Update(dt);
		++item;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PostUpdate(dt);
		++item;
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	std::list<Module*>::reverse_iterator item = list_modules.rbegin();

	while(item != list_modules.rend() && ret == true)
	{
		//ret = (*item)->SetActiveAndGet(false);
		ret = (*item)->CleanUp();
		++item;
	}

	SaveLogToFile();

	return ret;
}

void Application::SaveLogToFile() const
{
	// Save log to file at aplication exit
	std::ofstream out_log("Engine_log.txt");
	out_log << log_buffer << "\n";
	out_log.clear();
	out_log.close();

}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

void Application::RequestBrowser(const char* url) const
{
	ShellExecuteA(NULL, "open", url, NULL, NULL, SW_SHOWNORMAL); //SW_SHOWNA);
		// SW_SHOWNA = show window in current state
}

void Application::AdjustCappedMs(int max_frames)
{
	if (max_frames > 0)
		capped_ms = 1000 / max_frames;
	else
		capped_ms = 0;
}

void Application::Log(const char* new_entry)
{
	// combine new line + new_entry to save log to file cleanly with new lines
	// this is needed since we not include this sum on log.cpp
	// for the current used method to print on console editor entries
	// this way evades double jump

	char tmp_string[4096];

	// Save to file -------------
	sprintf_s(tmp_string, 4096, "\n%s", new_entry);
	log_buffer.append(tmp_string);

	//Console ------------------

	if (editor != nullptr && editor->console != nullptr)
	{
		editor->console->AddConsoleLog(new_entry);
	}
	else
	{
		console_log_aux.push_back(strdup(new_entry));
	}
	
}