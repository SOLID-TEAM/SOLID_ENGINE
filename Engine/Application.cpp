#include "Application.h"
#include <fstream>

Application::Application()
{
	LOG("-------------- Application Creation --------------");

	config_filename.assign("editor_config.json");
	app_name.assign("SOLID_ENGINE");
	organization_name.assign("SOLID_TEAM");

	random = new math::LCG();

	window = new ModuleWindow();
	input = new ModuleInput();
	test = new ModuleTest();
	renderer3D = new ModuleRenderer3D();
	importer = new ModuleImporter();
	editor = new ModuleEditor();
	file_sys = new ModuleFileSystem();
	textures = new ModuleTextures();
	scene = new ModuleScene();
	resources = new ModuleResources();

	// Main Modules
	AddModule(window);
	AddModule(input);
	AddModule(resources);
	// Scenes
	AddModule(scene);
	AddModule(test);

	AddModule(file_sys);
	AddModule(textures);
	AddModule(importer);
	AddModule(editor);
	// Renderer last!
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

	// Config Tool Creation --------------------------------
	config = new Config(config_filename.data());

	// Call Init() in all modules
	std::list<Module*>::iterator item = list_modules.begin();

	// if file not found, or something goes wrong, we need to save default values on new config file ----
	if (!config->IsOk() && ret)
	{
		LOG("[Error] Config file not found");
		bool newconf = config->CreateNewRootObject(config_filename.data());

		if (newconf)
		{
			if (WantToSave(true))
			{
				LOG("[Info] Created new default editor config %s", config_filename.data());
			}
			else
				LOG("[Error] Unable to create default config file %s", config_filename.data());
		}
	}
	// ---------------------------------------------------------------------------------------------------

	// loads application config
	LoadConfig(config->GetSection("App"));

	// set fps data before start/init modules
	fps_counter = 0;
	frames = 0;
	capped_ms = 1000 / max_frames;
	last_frame_ms = 0;
	last_fps = 0;

	// modules init --------

	item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->Init((config->GetSection((*item)->GetName())));
		++item;
	}

	// Tools Creation --------------------------------
	hardware = new HardwareInfo();

	// After all Init calls we call Start() in all modules

	LOG("-------------- Application Start --------------");
	item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->Start((config->GetSection((*item)->GetName())));
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
	
	hardware->UpdateDynamicVars();

	editor->w_config->AddMemoryLog(App->hardware->ram_usage.peak_actual_mem);
	editor->w_config->AddFpsLog((float)last_fps, (float)last_frame_ms);

}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();

	/*if (input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
	{
		if(WantToSave())
			LOG("[Info] Editor config %s succesfully overwritten", config_filename.data());
	}*/
	
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

	item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->Draw();
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
	delete config;

	delete random;

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

	this->max_frames = max_frames;
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

	if (editor != nullptr && editor->w_console != nullptr)
	{
		editor->w_console->AddConsoleLog(new_entry);
	}
	else
	{
		console_log_aux.push_back(strdup(new_entry));
	}
	
}

// For re-write all modules current values -------
bool Application::WantToSave(bool cleanInit)
{
	bool ret = true;

	std::list<Module*>::iterator item = list_modules.begin();

	SaveConfig(config->AddSection("App"));

	for (; item != list_modules.end() && ret; ++item)
	{
		ret = (*item)->Save((config->AddSection((*item)->GetName())));
	}

	if (ret)
	{
		if (cleanInit) // we create one default file apart when editor starts without any configuration, for reset values purposes
			config->SaveConfigToFile("default_editor_config.json");

		config->SaveConfigToFile(config_filename.data());
	}
	else
		ret = false;

	return ret;
}

void Application::WantToLoad(bool restoreDefault)
{
	bool ret = true;
	
	std::list<Module*>::iterator item = list_modules.begin();

	if (restoreDefault)
	{
		Config default_config("default_editor_config.json");

		if (default_config.IsOk())
		{
			LoadConfig(default_config.GetSection("App"));

			for (; item != list_modules.end() && ret; ++item)
			{
				(*item)->Load((default_config.GetSection((*item)->GetName())));
			}
		}
		else
		{
			LOG("[Error] unable to restore default editor config");
		}
	}
	else
	{
		LoadConfig(config->GetSection("App"));

		for (; item != list_modules.end() && ret; ++item)
		{
			(*item)->Load((config->GetSection((*item)->GetName())));
		}
	}
}
// -------------------------------------------------

bool Application::SaveConfig(Config& config)
{
	bool ret = true;

	ret = config.AddString("name", app_name.data());
	ret = config.AddString("organization", organization_name.data());
	ret = config.AddInt("framerate_cap", max_frames);

	return ret;
}

bool Application::LoadConfig(Config& config)
{
	bool ret = true;

	app_name.assign(config.GetString("name", app_name.data()));
	organization_name.assign(config.GetString("organization", organization_name.data()));
	AdjustCappedMs(config.GetInt("framerate_cap", max_frames));

	return ret;
}
int Application::GetFrames()
{
	return frames;
}