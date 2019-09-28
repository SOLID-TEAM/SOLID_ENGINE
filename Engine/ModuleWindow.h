#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	// EDITOR needs --------------
	void SetTitle(const char* title);
	float& SetBrightness(float* br) const;
	void SetWindowSize(int w, int h) const;
	void SetWindowFullscreen(float fullscreen, bool desktop = false) const;
	void SetWindowResizable(bool resizable) const;
	void SetWindowBorderless(bool borderless) const;

public:
	//The window we'll be rendering to
	SDL_Window* window = nullptr;

	//The surface contained by the window
	SDL_Surface* screen_surface = nullptr;

	// Editor needs
private:
	// default values, brightness doesnt need, we understand that default is full brightness 1.0f
	const float max_width = 1920; // TODO: load from JSON for setting the editor to default
	const float max_height = 1080;

};

#endif // __ModuleWindow_H__