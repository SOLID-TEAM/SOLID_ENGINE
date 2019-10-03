#ifndef __PANEL_H__
#define __PANEL_H__

#include "Globals.h"
#include <string>
#include <vector>
#include "SDL/include/SDL_scancode.h"

class ModuleEditor;

class Panel
{
public:

	Panel(std::string name);

	virtual ~Panel();

	virtual void Render() = 0;

	void SwitchActive();

	bool IsActive() const;

	std::vector<SDL_Scancode> GetShortCut() const;

	std::string GetName() const
	{
		return name;
	}

public:

	bool	active = false;
	int		pos_x   = 0;
	int		pos_y   = 0;
	int		width	= 0;
	int		height	= 0;

protected:

	std::string name;
	std::vector<SDL_Scancode> shortcut;
};

#endif // __PANEL_H__
