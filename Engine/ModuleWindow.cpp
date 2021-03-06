#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleWindow::ModuleWindow(bool start_enabled) : Module(start_enabled)
{
	name.assign("Window");

	window = NULL;
	screen_surface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init(Config& config)
{
	LOG("[Init] SDL window & surface");
	bool ret = true;

	// load configuration
	Load(config);

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) < 0)
	{
		LOG("[Error] SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		int width = current_w * SCREEN_SIZE;
		int height = current_h * SCREEN_SIZE;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI;

		SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

		if(fullscreen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(resizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(borderless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(fullscreen_desktop == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		if (maximized == true)
		{
			flags |= SDL_WINDOW_MAXIMIZED;
		}

		// GL 3.0 + GLSL 130
		const char* glsl_version = "#version 130";
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3.2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
		
		// Create window with graphics context
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		//Create context
		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, flags);
		App->renderer3D->context = SDL_GL_CreateContext(App->window->window);
		SDL_GL_MakeCurrent(window, App->renderer3D->context);
		SDL_GL_SetSwapInterval(1);

		if(window == NULL)
		{
			LOG("[Error] Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			SetTitle("Solid Engine v.2.0");
		}
	}

	return ret;
}



// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("[CleanUp] Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

// testing fancy way to set window brightness
// TODO: maybe we need a more standard approach like store brightness on this module
float& ModuleWindow::SetBrightness(float* br) const
{
	float brightness = *br;
	if (brightness < 0.0f) brightness = 0.0f;
	if (brightness > 1.0f) brightness = 1.0f;
	
	SDL_SetWindowBrightness(window, brightness);
	*br = brightness;
	
	return *br;
}

// TODO: maybe we clean this destroying and creating new windows with required flags instead of bunch of functions?
void ModuleWindow::SetWindowSize(int w, int h)
{	// TODO: LOAD FROM JSON
	// for setting unwanted values that user can introduce with CTRL+CLICK on the slider
	if (w > 1920 || w < 320) w = 1280; 
	if (h > 1200 || h < 240) h = 1024;  

	current_w = (uint)w;
	current_h = (uint)h;

	SDL_SetWindowSize(window, current_w, current_h);
}

void ModuleWindow::SetWindowFullscreen(float fullscreen, bool desktop)
{
	Uint32 flags;

	if (fullscreen || desktop)
		flags = desktop ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_FULLSCREEN;
	else
		flags = SDL_WINDOW_MAXIMIZED;

	this->fullscreen = fullscreen;
	this->fullscreen_desktop = desktop;

	SDL_SetWindowFullscreen(window, flags);
}

void ModuleWindow::SetWindowMaximized(bool maximized)
{
	if (maximized)
		SDL_MaximizeWindow(window);
	else
		SDL_RestoreWindow(window);

	this->maximized = maximized;
}

void ModuleWindow::SetWindowResizable(bool resizable)
{
	this->resizable = resizable;
	SDL_SetWindowResizable(window, (SDL_bool)resizable);
}

void ModuleWindow::SetWindowBorderless(bool borderless)
{
	this->borderless = borderless;
	SDL_SetWindowBordered(window, (SDL_bool)!borderless);
}

bool ModuleWindow::Save(Config& config)
{
	bool ret = true;

	ret = config.AddInt("window_width", current_w);
	ret = config.AddInt("window_height", current_h);

	ret = config.AddBool("fullscreen", fullscreen);
	ret = config.AddBool("fullscreen_desktop", fullscreen_desktop);
	ret = config.AddBool("resizable", resizable);
	ret = config.AddBool("borderless", borderless);
	ret = config.AddBool("maximized", maximized);

	return true;
}

void ModuleWindow::Load(Config& config)
{
	// window size ----------------------
	SetWindowSize(config.GetInt("window_width", current_w), config.GetInt("window_height", current_h));
	// ----------------------------------
	// window flags
	SetWindowFullscreen(config.GetBool("fullscreen", fullscreen), config.GetBool("fullscreen_desktop", fullscreen_desktop));
	SetWindowResizable(config.GetBool("resizable", resizable));
	SetWindowBorderless(config.GetBool("borderless", borderless));
	SetWindowMaximized(config.GetBool("maximized", maximized));
}