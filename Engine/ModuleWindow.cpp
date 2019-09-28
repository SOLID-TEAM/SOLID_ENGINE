#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleWindow::ModuleWindow(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	window = NULL;
	screen_surface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		int width = SCREEN_WIDTH * SCREEN_SIZE;
		int height = SCREEN_HEIGHT * SCREEN_SIZE;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 3.3
		/*SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);*/


		if(WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
			SetTitle("SOLID ENGINE v0.1");
		}
	}

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

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
void ModuleWindow::SetWindowSize(int w, int h) const
{	// TODO: LOAD FROM JSON
	// for setting unwanted values that user can introduce with CTRL+CLICK on the slider
	if (w > 1920 || w < 320) w = 1280; 
	if (h > 1200 || h < 240) h = 1024;  

	SDL_SetWindowSize(window, w, h);
	App->renderer3D->OnResize(w, h);
}

void ModuleWindow::SetWindowFullscreen(float fullscreen, bool desktop) const
{
	Uint32 flags;
	if (fullscreen)
		flags = desktop ? SDL_WINDOW_FULLSCREEN_DESKTOP : SDL_WINDOW_FULLSCREEN;
	else
		flags = SDL_WINDOW_MAXIMIZED;

	SDL_SetWindowFullscreen(window, flags);
}

void ModuleWindow::SetWindowResizable(bool resizable) const
{
	SDL_SetWindowResizable(window, (SDL_bool)resizable);
}

void ModuleWindow::SetWindowBorderless(bool borderless) const
{
	SDL_SetWindowBordered(window, (SDL_bool)borderless);
}