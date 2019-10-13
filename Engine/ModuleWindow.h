#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Globals.h"
#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init(Config& config);
	bool CleanUp();
	bool Save(Config& config);
	void Load(Config& config);

	// EDITOR needs --------------
	void SetTitle(const char* title);
	float& SetBrightness(float* br) const;
	void SetWindowSize(int w, int h);
	void SetWindowFullscreen(float fullscreen, bool desktop = false);
	void SetWindowResizable(bool resizable);
	void SetWindowBorderless(bool borderless);

public:
	//// TODO: Delete Multiviewport Test
	//std::vector< SDL_Window*> sub_windows;

	//The window we'll be rendering to
	SDL_Window* window = nullptr;

	//The surface contained by the window
	SDL_Surface* screen_surface = nullptr;

	// Editor needs
public:
	// default values, brightness doesnt need, we understand that default is full brightness 1.0f
	float max_width = 1920;
	float max_height = 1080;

	uint current_w = 1920;
	uint current_h = 1080;

	bool fullscreen = WIN_FULLSCREEN;
	bool resizable = WIN_RESIZABLE;
	bool fullscreen_desktop = WIN_FULLSCREEN_DESKTOP;
	bool borderless = WIN_BORDERLESS;

};

#endif // __ModuleWindow_H__