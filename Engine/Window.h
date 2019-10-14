#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "Globals.h"
#include <string>
#include <vector>
#include "SDL/include/SDL_scancode.h"

class ModuleEditor;

class Window
{
public:

	Window(std::string name, bool active);

	virtual ~Window();

	virtual void Draw() = 0;

	void SwitchActive();

	bool IsActive() const;

	void Destroy();    // External function to call anywhere

	std::vector<SDL_Scancode> GetShortCut() const;

	std::string GetName() const
	{
		return name;
	}

protected:

	virtual void CleanUp() {};  // Internal , only Destroy can call this virtual function

public:

	bool	active = false;
	int		pos_x   = 0;
	int		pos_y   = 0;
	int		width	= -1;
	int		height	= -1;

protected:

	std::string name;
	std::vector<SDL_Scancode> shortcut;

	friend ModuleEditor;
};

#endif // __WINDOW_H__
