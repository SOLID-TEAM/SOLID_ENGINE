#include "Application.h"
#include <fstream>

Application::Application()
{
	LOG("-------------- Application Creation --------------");

	config_filename.assign("editor_config.json");
	app_name.assign("SOLID_ENGINE");
	organization_name.assign("SOLID_TEAM");

	time = new ModuleTime();
	window = new ModuleWindow();
	input = new ModuleInput();
	test = new ModuleTest();
	renderer3D = new ModuleRenderer3D();
	importer = new ModuleImporter();
	editor = new ModuleEditor();
	file_sys = new ModuleFileSystem();
	textures = new ModuleTextures();
	physics = new ModulePhysics();
	scene = new ModuleScene();
	resources = new ModuleResources();

	// Main Modules

	AddModule(time);	// Time first
	AddModule(window);
	AddModule(input);
	AddModule(resources);
	AddModule(physics);
	AddModule(scene);
	AddModule(test);
	AddModule(file_sys);
	AddModule(textures);
	AddModule(importer);
	AddModule(editor);
	AddModule(renderer3D); // Renderer last!

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

	// Tool Creation --------------------------------
	config = new Config(config_filename.data());
	hardware = new HardwareInfo();
	random = new math::LCG();

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

	// modules init --------

	item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->Init((config->GetSection((*item)->GetName())));
		++item;
	}

	// After all Init calls we call Start() in all modules

	LOG("-------------- Application Start --------------");
	item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->Start((config->GetSection((*item)->GetName())));
		++item;
	}
	
	return ret;
}

update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;

	// Update tools ---------------------------------
	hardware->UpdateDynamicVars();
	editor->w_config->AddMemoryLog(App->hardware->ram_usage.peak_actual_mem);
	editor->w_config->AddFpsLog(time->fps, time->last_frame_ms);

	// Update modules ------------------------------

	std::list<Module*>::iterator item = list_modules.begin();

	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PreUpdate();
		++item;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->Update();
		++item;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PostUpdate();
		++item;
	}

	item = list_modules.begin();

	while (item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->Draw();
		++item;
	}

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;
	std::list<Module*>::reverse_iterator item = list_modules.rbegin();

	while(item != list_modules.rend() && ret == true)
	{
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


	return ret;
}

bool Application::LoadConfig(Config& config)
{
	bool ret = true;

	app_name.assign(config.GetString("name", app_name.data()));
	organization_name.assign(config.GetString("organization", organization_name.data()));

	return ret;
}
void Application::BroadcastEvent(const Event& e)
{
	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end(); ++it)
		(*it)->ReceiveEvent(e);
}